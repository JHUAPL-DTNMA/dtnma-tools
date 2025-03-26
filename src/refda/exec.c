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

/** Finish the execution of an item.
 * Also report on a result if requested.
 */
static int refda_exec_ctrl_finish(refda_exec_item_t *item)
{
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(item->result), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution finished with result %s", string_get_cstr(buf));
        string_clear(buf);
    }

    refda_runctx_t *runctx = refda_runctx_ptr_ref(item->seq->runctx);

    if (!cace_ari_is_null(&(runctx->nonce)))
    {
        // generate report regardless of success or failure
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
    CHKERR1(ctrl->execute);

    if (cace_log_is_enabled_for(LOG_INFO))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(item->ref), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Execution item %s", string_get_cstr(buf));
        string_clear(buf);
    }
    if (atomic_load(&(item->execution_stage)) == EXEC_PENDING)
    {
        refda_ctrl_exec_ctx_t ctx;
        refda_ctrl_exec_ctx_init(&ctx, item);
        (ctrl->execute)(&ctx);
        refda_ctrl_exec_ctx_deinit(&ctx);
        CACE_LOG_DEBUG("execution callback returned");
    }

    if (atomic_load(&(item->execution_stage)) == EXEC_WAITING)
    {
        CACE_LOG_INFO("Control is still waiting to finish");
    }
    else if(atomic_load(&(item->execution_stage)) == EXEC_COMPLETE)
    {
        refda_exec_ctrl_finish(item);
    }
    else
    {
        CACE_LOG_INFO("Improper execution_stage value");
    }

    return 0;
}

int refda_exec_run_seq(refda_exec_seq_t *seq)
{
    int retval = 0;
    while (!refda_exec_item_list_empty_p(seq->items))
    {
        if (atomic_load(&(refda_exec_item_list_front(seq->items)->execution_stage)) == EXEC_WAITING)
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
static int refda_exec_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *target);

/** Execute an arbitrary object reference.
 */
static int refda_exec_exp_ref(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *target)
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
            case CACE_ARI_TYPE_CTRL:
            {
                // expansion finished, execution comes later
                refda_exec_item_t *item = refda_exec_item_list_push_back_new(seq->items);
                item->seq               = seq;
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
static int refda_exec_exp_mac(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *ari)
{
    cace_ari_list_t *items = &(ari->as_lit.value.as_ac->items);

    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, *items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        const cace_ari_t *item = cace_ari_list_cref(it);

        refda_exec_exp_item(runctx, seq, item);
    }

    return 0;
}

static int refda_exec_exp_item(refda_runctx_t *runctx, refda_exec_seq_t *seq, const cace_ari_t *target)
{
    int retval = 0;
    if (target->is_ref)
    {
        CACE_LOG_DEBUG("Expanding as reference");
        retval = refda_exec_exp_ref(runctx, seq, target);
    }
    else
    {
        if (!cace_amm_type_match(runctx->agent->mac_type, target))
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

int refda_exec_exp_target(refda_exec_seq_t *seq, refda_runctx_ptr_t runctxp, const cace_ari_t *target)
{
    CHKERR1(target);
    refda_runctx_t *runctx = refda_runctx_ptr_ref(runctxp);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, target, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Expanding PID %" PRIu64 " target %s from source %s", seq->pid, m_string_get_cstr(buf),
                       m_string_get_cstr(runctx->mgr_ident));
        string_clear(buf);
    }

    refda_runctx_ptr_set(seq->runctx, runctxp);

    // FIXME: lock more fine-grained level
    REFDA_AGENT_LOCK(runctx->agent, REFDA_AGENT_ERR_LOCK_FAILED);

    int retval = refda_exec_exp_item(runctx, seq, target);

    // FIXME: lock more fine-grained level
    REFDA_AGENT_UNLOCK(runctx->agent, REFDA_AGENT_ERR_LOCK_FAILED);

    return retval;
}

static int refda_exec_waiting(refda_agent_t *agent)
{

    refda_exec_seq_ptr_list_t ready;
    refda_exec_seq_ptr_list_init(ready);

    // lock only to collect the ready sequences
    // the sequences themselves will not be touched outside of this worker
    if (pthread_mutex_lock(&(agent->exec_state_mutex)))
    {
        CACE_LOG_ERR("failed to lock exec_state_mutex");
        return 2;
    }

    refda_exec_seq_list_it_t seq_it;
    for (refda_exec_seq_list_it(seq_it, agent->exec_state); !refda_exec_seq_list_end_p(seq_it);)
    {
        refda_exec_seq_t *seq = refda_exec_seq_list_ref(seq_it);

        if (refda_exec_item_list_empty_p(seq->items))
        {
            // Skip completed exec item
            //
            // Do not remove now because it will relocate seq in memory and cause
            // problems with pointers within items. We clean up after iterating.
            refda_exec_seq_list_next(seq_it);
            continue;
        }

        if (atomic_load(&(refda_exec_item_list_front(seq->items)->execution_stage)) == EXEC_WAITING)
        {
            // still waiting
            continue;
        }

        CACE_LOG_DEBUG("pushing to ready");
        refda_exec_seq_ptr_list_push_back(ready, seq);
        refda_exec_seq_list_next(seq_it);
    }

    // Safely clear any completed sequences from the front of the queue
    while (!refda_exec_seq_list_empty_p(agent->exec_state)
           && refda_exec_item_list_empty_p(refda_exec_seq_list_front(agent->exec_state)->items))
    {
        refda_exec_seq_list_pop_front(NULL, agent->exec_state);
        CACE_LOG_DEBUG("Removed completed item from agent exec_state queue");
    }

    if (pthread_mutex_unlock(&(agent->exec_state_mutex)))
    {
        CACE_LOG_ERR("failed to unlock exec_state_mutex");
        return 2;
    }

    refda_exec_seq_ptr_list_it_t ready_it;
    for (refda_exec_seq_ptr_list_it(ready_it, ready); !refda_exec_seq_ptr_list_end_p(ready_it);
         refda_exec_seq_ptr_list_next(ready_it))
    {
        refda_exec_seq_t *seq = *refda_exec_seq_ptr_list_ref(ready_it);

        int res = refda_exec_run_seq(seq);
        if (res)
        {
            CACE_LOG_WARNING("execution of sequence PID %" PRIu64 " failed, continuing", seq->pid);
        }
    }
    refda_exec_seq_ptr_list_clear(ready);

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

    cace_ari_list_t *targets = &(msg->value.as_lit.value.as_execset->targets);

    cace_ari_list_it_t tgtit;
    for (cace_ari_list_it(tgtit, *targets); !cace_ari_list_end_p(tgtit); cace_ari_list_next(tgtit))
    {
        const cace_ari_t *tgt = cace_ari_list_cref(tgtit);

        if (pthread_mutex_lock(&(agent->exec_state_mutex)))
        {
            CACE_LOG_ERR("failed to lock exec_state_mutex");
            continue;
        }

        refda_exec_seq_t *seq = refda_exec_seq_list_push_back_new(agent->exec_state);

        seq->pid = agent->exec_next_pid++;
        // Even if an individual execution fails, continue on with others
        int res = refda_exec_exp_target(seq, ctxptr, tgt);
        if (res)
        {
            // clean up useless sequence
            refda_exec_seq_list_pop_back(NULL, agent->exec_state);
        }

        if (pthread_mutex_unlock(&(agent->exec_state_mutex)))
        {
            CACE_LOG_ERR("failed to unlock exec_state_mutex");
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
                cace_timeperiod_encode(buf, &diff);
                CACE_LOG_DEBUG("waiting for exec event or %s", string_get_cstr(buf));
                string_clear(buf);
            }

            sem_timedwait(&(agent->execs_sem), &(next->ts));

            struct timespec nowtime;
            clock_gettime(CLOCK_REALTIME, &nowtime);

            // execute appropriate callbacks (up to and including nowtime)
            refda_timeline_it(tl_it, agent->exec_timeline);
            while (!refda_timeline_end_p(tl_it))
            {
                const refda_timeline_event_t *next = refda_timeline_cref(tl_it);
                if (timespec_gt(next->ts, nowtime))
                {
                    break;
                }

                CACE_LOG_DEBUG("running deferred callback");
                {
                    refda_ctrl_exec_ctx_t ctx;
                    refda_ctrl_exec_ctx_init(&ctx, next->item);
                    (next->callback)(&ctx);
                    refda_ctrl_exec_ctx_deinit(&ctx);
                }
                if (!(atomic_load(&(next->item->execution_stage)) == EXEC_WAITING))
                {
                    refda_exec_ctrl_finish(next->item);
                }

                refda_timeline_remove(agent->exec_timeline, tl_it);
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
            const bool at_end = cace_ari_is_undefined(&(item.value));
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
