/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
 * Laboratory LLC.
 *
 * This file is part of the Delay-Tolerant Networking Management
 * Architecture (DTNMA) Tools package.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "exec_proc.h"
#include "exec.h"
#include "ctrl_exec_ctx.h"
#include "valprod.h"
#include "reporting.h"
#include "amm/ctrl.h"
#include <cace/ari/text.h>
#include <cace/ari/text_util.h>
#include <cace/amm/lookup.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <timespec.h>

/** Pop the front execution item after it has finished successfully.
 *
 * @param[in,out] seq The sequence to pop.
 * @post After this the front item will be removed, and if necessary
 * any status will be marked.
 */
static void refda_exec_proc_pop_front(refda_exec_seq_t *seq)
{
    CHKVOID(seq);

    if (pthread_mutex_lock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return;
    }
    // decouple front item from the sequence
    refda_exec_item_ptr_t *item_ptr;
    refda_exec_item_list_pop_at(&item_ptr, seq->items, 0);
    if (item_ptr)
    {
        refda_exec_item_ptr_ref(item_ptr)->seq = NULL;
    }
    refda_exec_item_ptr_release(item_ptr);

    bool is_empty = refda_exec_item_list_empty_p(seq->items);
    if (pthread_mutex_unlock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
    }

    // report after the entire sequence is finished
    if (seq->status && is_empty)
    {
        refda_exec_status_post(seq->status, false);
    }
}

/** Clear the entire execution sequence item after an item has failed.
 *
 * @param[in,out] seq The sequence to clear.
 * @post After this the items will be empty and any status will be marked.
 */
static void refda_exec_proc_clear(refda_exec_seq_t *seq)
{
    CHKVOID(seq);

    if (pthread_mutex_lock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return;
    }
    // decouple and release all items from the sequence
    refda_exec_item_list_it_t item_it;
    for (refda_exec_item_list_it(item_it, seq->items); !refda_exec_item_list_end_p(item_it);)
    {
        refda_exec_item_ptr_t **item_ptr = refda_exec_item_list_ref(item_it);

        // clear parent reference
        refda_exec_item_ptr_ref(*item_ptr)->seq = NULL;

        refda_exec_item_list_remove(seq->items, item_it);
    }
    if (pthread_mutex_unlock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
    }

    if (seq->status)
    {
        refda_exec_status_post(seq->status, true);
    }
}

int refda_exec_proc_ctrl_finish(refda_exec_item_t *item)
{
    if (!item->seq)
    {
        // item is already terminated
        return 0;
    }
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(item->result), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution finished with result %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    const bool is_failure = cace_ari_is_undefined(&(item->result));

    // item will be invalidated when removed from sequence
    refda_exec_seq_t *seq = item->seq;

    refda_runctx_t *runctx = refda_runctx_ptr_ref(seq->runctx);

    // Track number of successes/failures
    refda_agent_t *agent = runctx->agent;
    if (is_failure)
    {
        atomic_fetch_add(&agent->instr.num_ctrls_failed, 1);
    }
    else
    {
        atomic_fetch_add(&agent->instr.num_ctrls_succeeded, 1);
    }

    if (!cace_ari_is_null(&(runctx->nonce)))
    {
        // generate report regardless of success or failure
        CACE_LOG_DEBUG("Pushing execution result");
        // this moves the result value
        refda_reporting_ctrl(runctx, &(item->ref), &(item->result));
    }

    if (is_failure && cace_log_is_enabled_for(LOG_WARNING))
    {
        // done with this whole sequence
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode_objpath(buf, cace_ari_cget_ref_objpath(&item->ref), CACE_ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_WARNING("execution of sequence PID %" PRIu64 " (at %p) failed on %s (as %s), halting", seq->pid, seq,
                         m_string_get_cstr(item->deref.obj->obj_id.name), m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    if (is_failure)
    {
        // done with this whole sequence
        refda_exec_proc_clear(seq);
    }
    else if (seq)
    {
        // done with this item
        refda_exec_proc_pop_front(seq);
    }

    return 0;
}

int refda_exec_proc_ctrl_start(refda_exec_seq_t *seq)
{
    if (pthread_mutex_lock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
    }
    refda_exec_item_ptr_t **front_ptr = refda_exec_item_list_front(seq->items);
    refda_exec_item_ptr_t  *item_ptr  = refda_exec_item_ptr_acquire(*front_ptr);
    if (pthread_mutex_unlock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
    }

    refda_exec_item_t *item = refda_exec_item_ptr_ref(item_ptr);
    CHKERR1(item->deref.obj);

    refda_amm_ctrl_desc_t *ctrl = item->deref.obj->app_data.ptr;
    CHKERR1(ctrl);
    CHKERR1(ctrl->execute);

    if (cace_log_is_enabled_for(LOG_INFO))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(item->ref), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Execution item %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    if (atomic_load(&(item->execution_stage)) == REFDA_EXEC_PENDING)
    {
        refda_ctrl_exec_ctx_t ctx;
        refda_ctrl_exec_ctx_init(&ctx, item_ptr);
        atomic_fetch_add(&ctx.runctx->agent->instr.num_ctrls_run, 1);
        (ctrl->execute)(&ctx);
        refda_ctrl_exec_ctx_deinit(&ctx);
        CACE_LOG_DEBUG("execution callback returned");
    }

    if (atomic_load(&(item->execution_stage)) == REFDA_EXEC_WAITING)
    {
        CACE_LOG_INFO("Control is still waiting to finish");
    }
    else
    {
        refda_exec_proc_ctrl_finish(item);
    }
    refda_exec_item_ptr_release(item_ptr);

    return 0;
}

int refda_exec_proc_run(refda_exec_seq_t *seq)
{
    int retval = 0;
    while (true)
    {
        refda_exec_item_status_t front_status;
        if (refda_exec_proc_front_status(&front_status, seq))
        {
            // nothing to do
            break;
        }
        if (front_status == REFDA_EXEC_WAITING)
        {
            // cannot complete at this time
            break;
        }

        retval = refda_exec_proc_ctrl_start(seq);
        if (retval)
        {
            // something went wrong
            break;
        }
    }

    if (retval)
    {
        CACE_LOG_WARNING("execution of sequence PID %" PRIu64 " (at %p) failed", seq->pid, seq);
    }
    return retval;
}

int refda_exec_proc_front_status(refda_exec_item_status_t *status, refda_exec_seq_t *seq)
{
    CHKERR1(status);
    CHKERR1(seq);

    int retval = 0;
    if (pthread_mutex_lock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return 2;
    }
    if (!refda_exec_item_list_empty_p(seq->items))
    {
        refda_exec_item_ptr_t **front_ptr = refda_exec_item_list_front(seq->items);
        // safe during mutex lock
        refda_exec_item_t *item = refda_exec_item_ptr_ref(*front_ptr);

        *status = atomic_load(&(item->execution_stage));
    }
    else
    {
        retval = 2;
    }
    if (pthread_mutex_unlock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
        return 2;
    }
    return retval;
}

void refda_exec_proc_terminate(refda_exec_seq_t *seq)
{
    CHKVOID(seq);
    CACE_LOG_DEBUG("execution of sequence PID %" PRIu64 " (at %p) terminating", seq->pid, seq);

    // mark the front item as failed if already waiting
    refda_exec_item_ptr_t *item_ptr = NULL;

    if (pthread_mutex_lock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return;
    }
    if (!refda_exec_item_list_empty_p(seq->items))
    {
        item_ptr = refda_exec_item_ptr_acquire(*refda_exec_item_list_front(seq->items));
    }
    if (pthread_mutex_unlock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
    }

    if (item_ptr)
    {
        refda_exec_item_t *item = refda_exec_item_ptr_ref(item_ptr);

        if (atomic_load(&(item->execution_stage)) == REFDA_EXEC_WAITING)
        {
            // leave undefined but mark as finished (i.e. failed)
            refda_exec_proc_ctrl_finish(item);
        }

        refda_exec_item_ptr_release(item_ptr);
    }
    else
    {
        refda_exec_proc_clear(seq);
    }
}

/** Expand any ARI target (reference or literal).
 */
static int refda_exec_proc_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, size_t *seq_ix,
                                    const cace_ari_t *target, cace_ari_array_t invalid_items);

/** Expand an arbitrary object reference.
 */
static int refda_exec_proc_exp_ref(refda_runctx_t *runctx, refda_exec_seq_t *seq, size_t *seq_ix,
                                   const cace_ari_t *target, cace_ari_array_t invalid_items)
{
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(runctx->agent->objs), target);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        cace_ari_array_push_back(invalid_items, *target);
        retval = REFDA_EXEC_ERR_DEREF_FAILED;
    }

    if (!retval)
    {
        switch (deref.obj_type)
        {
            case CACE_ARI_TYPE_CTRL:
            {
                // expansion finished, execution comes later
                refda_exec_item_ptr_t *ptr = refda_exec_item_ptr_new();
                refda_exec_item_list_push_at(seq->items, *seq_ix, ptr);
                ++(*seq_ix);

                {
                    refda_exec_item_t *item = refda_exec_item_ptr_ref(ptr);
                    // back reference to parent
                    item->seq = seq;
                    cace_ari_set_copy(&(item->ref), target);
                    cace_amm_lookup_set_move(&(item->deref), &deref);
                    cace_amm_lookup_init(&deref);

                    // access check, this permission has no parameters
                    bool acl_found =
                        refda_acl_search_one_permission(runctx->agent, runctx->acl_groups, &item->ref, &item->deref,
                                                        runctx->agent->acl.permissions.execute, NULL);
                    if (!acl_found)
                    {
                        cace_ari_array_push_back(invalid_items, *target);
                        retval = REFDA_EXEC_ERR_NO_ACCESS;
                    }
                }
                refda_exec_item_ptr_clear(ptr);
                break;
            }
            case CACE_ARI_TYPE_CONST:
            case CACE_ARI_TYPE_VAR:
            case CACE_ARI_TYPE_EDD:
            {
                refda_valprod_ctx_t prodctx;
                refda_valprod_ctx_init(&prodctx, runctx, target, &deref);
                retval = refda_valprod_run(&prodctx);
                if (retval)
                {
                    if (retval == 3)
                    {
                        retval = REFDA_EXEC_ERR_NO_ACCESS;
                    }
                    cace_ari_array_push_back(invalid_items, *target);
                }
                else
                {
                    // execute the produced value as a target
                    retval = refda_exec_proc_exp_item(runctx, seq, seq_ix, &(prodctx.value), invalid_items);
                }
                refda_valprod_ctx_deinit(&prodctx);
                break;
            }
            default:
                cace_ari_array_push_back(invalid_items, *target);
                retval = REFDA_EXEC_ERR_BAD_TYPE;
                break;
        }
    }

    cace_amm_lookup_deinit(&deref);

    return retval;
}

/** Expand a MAC-typed literal value.
 */
static int refda_exec_proc_exp_mac(refda_runctx_t *runctx, refda_exec_seq_t *seq, size_t *seq_ix, const cace_ari_t *ari,
                                   cace_ari_array_t invalid_items)
{
    const struct cace_ari_ac_s *inval = cace_ari_cget_ac(ari);
    CHKERR1(inval);

    int retval = 0;

    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, inval->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        const cace_ari_t *item = cace_ari_list_cref(it);

        retval = refda_exec_proc_exp_item(runctx, seq, seq_ix, item, invalid_items);
        if (retval)
        {
            break;
        }
    }

    return retval;
}

static int refda_exec_proc_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, size_t *seq_ix,
                                    const cace_ari_t *target, cace_ari_array_t invalid_items)
{
    int retval = 0;
    if (target->is_ref)
    {
        CACE_LOG_DEBUG("Expanding as reference");
        retval = refda_exec_proc_exp_ref(runctx, seq, seq_ix, target, invalid_items);
    }
    else
    {
        const bool valid = (CACE_AMM_TYPE_MATCH_POSITIVE == cace_amm_type_match(runctx->agent->mac_type, target));
        if (!valid)
        {
            CACE_LOG_WARNING("Attempt to execute a non-MAC literal");
            cace_ari_array_push_back(invalid_items, *target);
            retval = REFDA_EXEC_ERR_BAD_TYPE;
        }
        else
        {
            CACE_LOG_DEBUG("Expanding as MAC");
            retval = refda_exec_proc_exp_mac(runctx, seq, seq_ix, target, invalid_items);
        }
    }

    return retval;
}

int refda_exec_proc_expand(refda_exec_seq_t *seq, size_t *seq_ix, const cace_ari_t *target)
{
    CHKERR1(seq);
    CHKERR1(target);
    refda_runctx_t *runctx = refda_runctx_ptr_ref(seq->runctx);
    CHKERR1(runctx);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, target, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

        m_string_t mgr_buf;
        m_string_init(mgr_buf);
        cace_ari_text_encode(mgr_buf, &runctx->mgr_ident, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

        CACE_LOG_DEBUG("Expanding sequence PID %" PRIu64 " (at %p) target %s from manager %s", seq->pid, seq,
                       m_string_get_cstr(buf), m_string_get_cstr(mgr_buf));
        m_string_clear(mgr_buf);
        m_string_clear(buf);
    }

    cace_ari_array_t invalid_items;
    cace_ari_array_init(invalid_items);

    REFDA_AGENT_LOCK(runctx->agent, REFDA_AGENT_ERR_LOCK_FAILED);
    if (pthread_mutex_lock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return REFDA_AGENT_ERR_LOCK_FAILED;
    }
    int retval = refda_exec_proc_exp_item(runctx, seq, seq_ix, target, invalid_items);
    if (pthread_mutex_unlock(&seq->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
        return REFDA_AGENT_ERR_LOCK_FAILED;
    }
    REFDA_AGENT_UNLOCK(runctx->agent, REFDA_AGENT_ERR_LOCK_FAILED);

    if (!cace_ari_array_empty_p(invalid_items))
    {
        CACE_LOG_ERR("Expansion failed with %zu invalid items", cace_ari_array_size(invalid_items));
    }
    else
    {
        CACE_LOG_DEBUG("Expansion succeeded");
    }

    if (!cace_ari_is_null(&(runctx->nonce)))
    {
        // report on any failed expansions
        cace_ari_array_it_t inval_it;
        for (cace_ari_array_it(inval_it, invalid_items); !cace_ari_array_end_p(inval_it); cace_ari_array_next(inval_it))
        {
            const cace_ari_t *item = cace_ari_array_cref(inval_it);

            cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
            // this moves the result value
            refda_reporting_ctrl(runctx, item, &result);
        }
    }
    cace_ari_array_clear(invalid_items);

    return retval;
}
