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
#include "cace/ari/text.h"
#include "cace/amm/lookup.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_exec_ctx_init(refda_exec_ctx_t *obj, refda_runctx_t *parent, const cace_amm_lookup_t *deref)
{
    CHKVOID(obj);
    CHKVOID(deref);

    obj->parent = parent;
    obj->deref  = deref;
    ari_init(&(obj->result));
}

void refda_exec_ctx_deinit(refda_exec_ctx_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->result));
}

/** Process a top-level incoming ARI which has already been verified
 * to be an EXECSET literal.
 */
static int refda_exec_execset(refda_agent_t *agent, const ari_t *ari)
{
    CHKERR1(agent);
    CHKERR1(ari);

    refda_runctx_t runctx;
    if (refda_runctx_init(&runctx, agent, ari))
    {
        return 2;
    }

    // FIXME: lock more fine-grained level
    REFDA_AGENT_LOCK(agent)

    ari_list_t   *targets = &(ari->as_lit.value.as_execset->targets);
    ari_list_it_t tgtit;
    for (ari_list_it(tgtit, *targets); !ari_list_end_p(tgtit); ari_list_next(tgtit))
    {
        const ari_t *tgt = ari_list_cref(tgtit);

        // Even if an individual execution fails, continue on with others
        refda_exec_target(&runctx, tgt);
    }

    // FIXME: lock more fine-grained level
    REFDA_AGENT_UNLOCK(agent)

    return 0;
}

static int refda_exec_ctrl(refda_runctx_t *runctx, cace_amm_lookup_t *deref)
{
    CHKERR1(deref);
    CHKERR1(deref->obj);
    refda_amm_ctrl_desc_t *ctrl = deref->obj->app_data.ptr;
    CHKERR1(ctrl);

    CACE_LOG_DEBUG("Executing CTRL named \"%s\"", string_get_cstr(deref->obj->name));

    refda_exec_ctx_t ctx;
    refda_exec_ctx_init(&ctx, runctx, deref);

    int res = refda_amm_ctrl_desc_execute(ctrl, &ctx);

    if (ctx.parent->nonce && !ari_is_null(ctx.parent->nonce))
    {
        // generate report regardless of production
        refda_reporting_ctrl(runctx, deref->ref, &ctx.result);
    }

    refda_exec_ctx_deinit(&ctx);

    return res;
}

/** Execute an arbitrary object reference.
 */
static int refda_exec_ref(refda_runctx_t *runctx, const ari_t *target)
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
        switch (target->as_ref.objpath.ari_type)
        {
            case ARI_TYPE_CTRL:
                retval = refda_exec_ctrl(runctx, &deref);
                break;
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
                    retval = refda_exec_target(runctx, &(prodctx.value));
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
static int refda_exec_mac(refda_runctx_t *runctx, const ari_t *ari)
{
    ari_list_t *items = &(ari->as_lit.value.as_ac->items);

    ari_list_it_t it;
    for (ari_list_it(it, *items); !ari_list_end_p(it); ari_list_next(it))
    {
        const ari_t *item = ari_list_cref(it);

        // FIXME more agent state keeping needed
        refda_exec_target(runctx, item);
    }

    return 0;
}

int refda_exec_target(refda_runctx_t *runctx, const ari_t *target)
{
    CHKERR1(target);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, target, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Executing target %s", string_get_cstr(buf));
        string_clear(buf);
    }

    int retval = 0;
    if (target->is_ref)
    {
        CACE_LOG_DEBUG("Executing as reference");
        retval = refda_exec_ref(runctx, target);
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
            CACE_LOG_DEBUG("Executing as MAC");
            retval = refda_exec_mac(runctx, target);
        }
    }

    CACE_LOG_DEBUG("Execution result %d", retval);
    return retval;
}

void *refda_exec_worker(void *arg)
{
    refda_agent_t *agent = arg;
    CACE_LOG_INFO("Worker started");

    while (daemon_run_get(&agent->running))
    {
        ari_t ari;

        sem_wait(&(agent->execs_sem));
        if (!agent_ari_queue_pop(&ari, agent->execs))
        {
            // shouldn't happen
            CACE_LOG_WARNING("failed to pop from execs queue");
            continue;
        }
        // sentinel for end-of-input
        const bool at_end = ari_is_undefined(&ari);
        if (!at_end)
        {
            refda_exec_execset(agent, &ari);
        }

        ari_deinit(&ari);
        if (at_end)
        {
            CACE_LOG_INFO("Got undefined exec, stopping");

            // flush the input queue but keep the daemon running
            ari_t undef = ARI_INIT_UNDEFINED;
            agent_ari_queue_push_move(agent->rptgs, &undef);
            sem_post(&(agent->rptgs_sem));

            break;
        }
    }

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}
