/*
 * Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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
#include "exec.h"
#include "valprod.h"
#include "reporting.h"
#include "amm/ctrl.h"
#include <cace/ari/text.h>
#include <cace/ari/text_util.h>
#include <cace/amm/lookup.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <timespec.h>

/** Finish the execution of an item.
 * Also report on a result if requested.
 */
static int refda_exec_ctrl_finish(refda_exec_item_t *item)
{
    refda_runctx_t *runctx = refda_runctx_ptr_ref(item->seq->runctx);

    if (!ari_is_null(&(runctx->nonce)))
    {
        // generate report regardless of production
        CACE_LOG_DEBUG("Pushing execution result");
        refda_reporting_ctrl(runctx, &(item->ref), &(item->result));
    }

    // done with this item
    if (item->seq)
    {
        refda_exec_item_list_pop_front(NULL, item->seq->items);
    }

    return 0;
}

/** Execute a single CTRL, possibly deferring its finish.
 */
static int refda_exec_ctrl_start(refda_exec_seq_t *seq)
{
    refda_exec_item_t *item = refda_exec_item_list_front(seq->items);
    CHKERR1(item->deref.obj);
    refda_amm_ctrl_desc_t *ctrl = item->deref.obj->app_data.ptr;
    CHKERR1(ctrl);

    if (cace_log_is_enabled_for(LOG_INFO))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &(item->ref), ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Execution item %s", string_get_cstr(buf));
        string_clear(buf);
    }

    refda_exec_ctx_t ctx;
    refda_exec_ctx_init(&ctx, item);

    int res = refda_amm_ctrl_desc_execute(ctrl, &ctx);
    CACE_LOG_INFO("Execution callback returned with status %d", res);
    if (atomic_load(&(item->waiting)))
    {
        CACE_LOG_INFO("Control is still waiting to finish");
    }
    else
    {
        refda_exec_ctrl_finish(item);
    }

    refda_exec_ctx_deinit(&ctx);

    return res;
}

int refda_exec_run_seq(refda_exec_seq_t *seq)
{
    int retval = 0;
    while (!refda_exec_item_list_empty_p(seq->items))
    {
        if (atomic_load(&(refda_exec_item_list_front(seq->items)->waiting)))
        {
            // cannot complete at this time
            return 0;
        }

        retval = refda_exec_ctrl_start(seq);
        if (retval)
        {
            break;
        }
    }

    return retval;
}

/** Execute any ARI target (reference or literal).
 */
static int refda_exec_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, const ari_t *target);

/** Execute an arbitrary object reference.
 */
static int refda_exec_exp_ref(refda_runctx_t *runctx, refda_exec_seq_t *seq, const ari_t *target)
{
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(runctx->agent->objs), target);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        retval = REFDA_EXEC_ERR_DEREF_FAILED;
    }

    if (!retval)
    {
        switch (deref.obj_type)
        {
            case ARI_TYPE_CTRL:
            {
                // expansion finished, execution comes later
                refda_exec_item_t *item = refda_exec_item_list_push_back_new(seq->items);
                item->seq               = seq;
                ari_set_copy(&(item->ref), target);
                cace_amm_lookup_set_move(&(item->deref), &deref);
                cace_amm_lookup_init(&deref);
                break;
            }
            case ARI_TYPE_CONST:
            case ARI_TYPE_VAR:
            case ARI_TYPE_EDD:
            {
                refda_valprod_ctx_t prodctx;
                refda_valprod_ctx_init(&prodctx, runctx, &deref);
                retval = refda_valprod_run(&prodctx);
                if (!retval)
                {
                    // execute the produced value as a target
                    retval = refda_exec_exp_item(runctx, seq, &(prodctx.value));
                }
                refda_valprod_ctx_deinit(&prodctx);
                break;
            }
            default:
                retval = REFDA_EXEC_ERR_BAD_TYPE;
                break;
        }
    }

    cace_amm_lookup_deinit(&deref);

    return retval;
}

/** Execute a MAC-typed literal value.
 */
static int refda_exec_exp_mac(refda_runctx_t *runctx, refda_exec_seq_t *seq, const ari_t *ari)
{
    ari_list_t *items = &(ari->as_lit.value.as_ac->items);

    ari_list_it_t it;
    for (ari_list_it(it, *items); !ari_list_end_p(it); ari_list_next(it))
    {
        const ari_t *item = ari_list_cref(it);

        refda_exec_exp_item(runctx, seq, item);
    }

    return 0;
}

static int refda_exec_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, const ari_t *target)
{
    int retval = 0;
    if (target->is_ref)
    {
        CACE_LOG_DEBUG("Expanding as reference");
        retval = refda_exec_exp_ref(runctx, seq, target);
    }
    else
    {
        if (!amm_type_match(runctx->agent->mac_type, target) && false) // FIXME need to implement typedefs
        {
            CACE_LOG_WARNING("Attempt to execute a non-MAC literal");
            retval = REFDA_EXEC_ERR_BAD_TYPE;
        }
        else
        {
            CACE_LOG_DEBUG("Expanding as MAC");
            retval = refda_exec_exp_mac(runctx, seq, target);
        }
    }

    return retval;
}

int refda_exec_exp_target(refda_exec_seq_t *seq, refda_runctx_ptr_t runctxp, const ari_t *target)
{
    CHKERR1(target);
    refda_runctx_t *runctx = refda_runctx_ptr_ref(runctxp);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, target, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Expanding target %s", string_get_cstr(buf));
        string_clear(buf);
    }

    refda_runctx_ptr_set(seq->runctx, runctxp);

    // FIXME: lock more fine-grained level
    REFDA_AGENT_LOCK(runctx->agent)

    int retval = refda_exec_exp_item(runctx, seq, target);

    // FIXME: lock more fine-grained level
    REFDA_AGENT_UNLOCK(runctx->agent)

    return retval;
}

static int refda_exec_waiting(refda_agent_t *agent)
{
    refda_exec_seq_list_it_t seq_it;
    for (refda_exec_seq_list_it(seq_it, agent->exec_state); !refda_exec_seq_list_end_p(seq_it);)
    {
        refda_exec_seq_t *seq = refda_exec_seq_list_ref(seq_it);

        if (atomic_load(&(refda_exec_item_list_front(seq->items)->waiting)))
        {
            // still waiting
            continue;
        }

        int res = refda_exec_run_seq(seq);
        if (res)
        {
            CACE_LOG_WARNING("execution of sequence failed, continuing");
        }

        if (refda_exec_item_list_empty_p(seq->items))
        {
            // no need to keep around
            refda_exec_seq_list_remove(agent->exec_state, seq_it);
        }
        else
        {
            refda_exec_seq_list_next(seq_it);
        }
    }
    return 0;
}

/** Process a top-level incoming ARI which has already been verified
 * to be an EXECSET literal.
 */
static int refda_exec_exp_execset(refda_agent_t *agent, const refda_msgdata_t *msg)
{
    CHKERR1(agent);
    CHKERR1(msg);

    refda_runctx_ptr_t ctxptr;
    refda_runctx_ptr_init_new(ctxptr);

    if (refda_runctx_from(refda_runctx_ptr_ref(ctxptr), agent, msg))
    {
        return 2;
    }

    ari_list_t *targets = &(msg->value.as_lit.value.as_execset->targets);

    ari_list_it_t tgtit;
    for (ari_list_it(tgtit, *targets); !ari_list_end_p(tgtit); ari_list_next(tgtit))
    {
        const ari_t *tgt = ari_list_cref(tgtit);

        refda_exec_seq_t *seq = refda_exec_seq_list_push_back_new(agent->exec_state);
        // Even if an individual execution fails, continue on with others
        int res = refda_exec_exp_target(seq, ctxptr, tgt);
        if (res)
        {
            // clean up useless sequence
            refda_exec_seq_list_pop_back(NULL, agent->exec_state);
        }
    }

    return 0;
}

void *refda_exec_worker(void *arg)
{
    refda_agent_t *agent = arg;
    CACE_LOG_INFO("Worker started");

    // run until explicitly told to stop via refda_agent_t::execs
    while (true)
    {
        refda_msgdata_t item;

        refda_timeline_it_t tl_it;
        refda_timeline_it(tl_it, agent->exec_timeline);
        if (!refda_timeline_end_p(tl_it))
        {
            const refda_timeline_event_t *next = refda_timeline_cref(tl_it);
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                struct timespec nowtime;
                clock_gettime(CLOCK_REALTIME, &nowtime);

                struct timespec diff = timespec_sub(next->ts, nowtime);

                string_t buf;
                string_init(buf);
                timeperiod_encode(buf, &diff);
                CACE_LOG_DEBUG("waiting for exec event or %s", string_get_cstr(buf));
                string_clear(buf);
            }

            sem_timedwait(&(agent->execs_sem), &(next->ts));

            struct timespec nowtime;
            clock_gettime(CLOCK_REALTIME, &nowtime);

            // execute appropriate callbacks now
            while (next && timespec_le(next->ts, nowtime))
            {
                CACE_LOG_DEBUG("running deferred callback");
                bool finished = (next->callback)(next->item);
                if (finished)
                {
                    refda_exec_ctrl_finish(next->item);
                }

                refda_timeline_remove(agent->exec_timeline, tl_it);
                // try to advance
                next = refda_timeline_end_p(tl_it) ? NULL : refda_timeline_cref(tl_it);
            }
        }
        else
        {
            CACE_LOG_DEBUG("waiting for exec event");
            sem_wait(&(agent->execs_sem));
        }

        // execs queue may still be empty if deferred callbacks were run
        if (refda_msgdata_queue_pop(&item, agent->execs))
        {
            // sentinel for end-of-input
            const bool at_end = ari_is_undefined(&(item.value));
            if (!at_end)
            {
                refda_exec_exp_execset(agent, &item);
            }
            refda_msgdata_deinit(&item);
            if (at_end && refda_timeline_empty_p(agent->exec_timeline))
            {
                CACE_LOG_INFO("Got undefined exec, stopping");

                // flush the input queue but keep the daemon running
                refda_msgdata_t undef;
                refda_msgdata_init(&undef);
                refda_msgdata_queue_push_move(agent->rptgs, &undef);
                sem_post(&(agent->rptgs_sem));

                break;
            }
        }

        // execute any waiting sequences
        refda_exec_waiting(agent);
    }

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}
