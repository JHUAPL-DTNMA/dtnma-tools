/*
 * Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

int refda_exec_proc_ctrl_finish(refda_exec_item_t *item)
{
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(item->result), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution finished with result %s", string_get_cstr(buf));
        string_clear(buf);
    }
    const bool is_failure = cace_ari_is_undefined(&(item->result));

    refda_runctx_t *runctx = refda_runctx_ptr_ref(item->seq->runctx);

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

    // item will be invalidated when removed from sequence
    refda_exec_seq_t *seq = item->seq;

    if (is_failure)
    {
        // done with this whole sequence
        string_t buf;
        string_init(buf);
        cace_ari_text_encode_objpath(buf, cace_ari_cget_ref_objpath(&item->ref), CACE_ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_WARNING("execution of sequence PID %" PRIu64 " failed on %s (as %s), halting", item->seq->pid,
                         m_string_get_cstr(item->deref.obj->obj_id.name), m_string_get_cstr(buf));
        string_clear(buf);

        if (seq->status)
        {
            atomic_store(&seq->status->failed, true);
        }
        refda_exec_item_list_reset(seq->items);
    }
    else if (item->seq)
    {
        // done with this item
        refda_exec_item_list_pop_front(NULL, seq->items);
    }

    if (seq->status && refda_exec_item_list_empty_p(seq->items))
    {
        CACE_LOG_DEBUG("Agent-directed sequence finished");
        sem_post(&seq->status->finished);
    }

    return 0;
}

int refda_exec_proc_ctrl_start(refda_exec_seq_t *seq)
{
    refda_exec_item_t *item = refda_exec_item_list_front(seq->items);
    CHKERR1(item->deref.obj);

    refda_amm_ctrl_desc_t *ctrl = item->deref.obj->app_data.ptr;
    CHKERR1(ctrl);
    CHKERR1(ctrl->execute);

    if (cace_log_is_enabled_for(LOG_INFO))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(item->ref), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Execution item %s", string_get_cstr(buf));
        string_clear(buf);
    }
    if (atomic_load(&(item->execution_stage)) == REFDA_EXEC_PENDING)
    {
        refda_ctrl_exec_ctx_t ctx;
        refda_ctrl_exec_ctx_init(&ctx, item);
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

    return 0;
}

int refda_exec_proc_run(refda_exec_seq_t *seq)
{
    int retval = 0;
    while (!refda_exec_item_list_empty_p(seq->items))
    {
        if (atomic_load(&(refda_exec_item_list_front(seq->items)->execution_stage)) == REFDA_EXEC_WAITING)
        {
            // cannot complete at this time
            return 0;
        }

        retval = refda_exec_proc_ctrl_start(seq);
        if (retval)
        {
            break;
        }
    }

    return retval;
}

/** Expand any ARI target (reference or literal).
 */
static int refda_exec_proc_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *target,
                                    cace_ari_array_t invalid_items);

/** Expand an arbitrary object reference.
 */
static int refda_exec_proc_exp_ref(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *target,
                                   cace_ari_array_t invalid_items)
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
                refda_exec_item_t *item = refda_exec_item_list_push_back_new(seq->items);

                item->seq = seq;
                cace_ari_set_copy(&(item->ref), target);
                cace_amm_lookup_set_move(&(item->deref), &deref);
                cace_amm_lookup_init(&deref);
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
                    cace_ari_array_push_back(invalid_items, *target);
                }
                else
                {
                    // execute the produced value as a target
                    retval = refda_exec_proc_exp_item(runctx, seq, &(prodctx.value), invalid_items);
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
static int refda_exec_proc_exp_mac(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *ari,
                                   cace_ari_array_t invalid_items)
{
    const struct cace_ari_ac_s *inval = cace_ari_cget_ac(ari);
    CHKERR1(inval);

    int retval = 0;

    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, inval->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        const cace_ari_t *item = cace_ari_list_cref(it);

        retval = refda_exec_proc_exp_item(runctx, seq, item, invalid_items);
        if (retval)
        {
            break;
        }
    }

    return retval;
}

static int refda_exec_proc_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *target,
                                    cace_ari_array_t invalid_items)
{
    int retval = 0;
    if (target->is_ref)
    {
        CACE_LOG_DEBUG("Expanding as reference");
        retval = refda_exec_proc_exp_ref(runctx, seq, target, invalid_items);
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
            retval = refda_exec_proc_exp_mac(runctx, seq, target, invalid_items);
        }
    }

    return retval;
}

int refda_exec_proc_expand(refda_exec_seq_t *seq, refda_runctx_ptr_t runctxp, const cace_ari_t *target)
{
    CHKERR1(seq);
    CHKERR1(runctxp);
    CHKERR1(target);
    refda_runctx_t *runctx = refda_runctx_ptr_ref(runctxp);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, target, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

        string_t mgr_buf;
        string_init(mgr_buf);
        cace_ari_text_encode(mgr_buf, &runctx->mgr_ident, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

        CACE_LOG_DEBUG("Expanding PID %" PRIu64 " target %s from manager %s", seq->pid, m_string_get_cstr(buf),
                       m_string_get_cstr(mgr_buf));
        string_clear(mgr_buf);
        string_clear(buf);
    }

    refda_runctx_ptr_set(seq->runctx, runctxp);

    cace_ari_array_t invalid_items;
    cace_ari_array_init(invalid_items);

    // FIXME: lock more fine-grained level
    REFDA_AGENT_LOCK(runctx->agent, REFDA_AGENT_ERR_LOCK_FAILED);

    int retval = refda_exec_proc_exp_item(runctx, seq, target, invalid_items);

    // FIXME: lock more fine-grained level
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
