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
#include "agent.h"
#include "amm/ctrl.h"
#include "cace/ari/text.h"
#include "cace/amm/lookup.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

static int refda_exec_target(refda_agent_t *agent, const ari_t *ari);

/** Process a top-level incoming ARI which has already been verified
 * to be an EXECSET literal.
 */
static int refda_exec_process(refda_agent_t *agent, const ari_t *ari)
{
    CHKERR1(agent);
    CHKERR1(ari);
    CHKERR1(!(ari->is_ref));
    CHKERR1(ari->as_lit.has_ari_type && ari->as_lit.ari_type == ARI_TYPE_EXECSET);

    ari_list_t   *targets = &(ari->as_lit.value.as_execset->targets);
    ari_list_it_t tgtit;
    for (ari_list_it(tgtit, *targets); !ari_list_end_p(tgtit); ari_list_next(tgtit))
    {
        const ari_t *tgt = ari_list_cref(tgtit);
        refda_exec_target(agent, tgt);
    }

    return 0;
}

static int refda_exec_ctrl(refda_agent_t *agent, cace_amm_lookup_t *deref)
{
    CHKERR1(deref);
    refda_amm_ctrl_desc_t *ctrl = deref->obj->app_data.ptr;
    CHKERR1(ctrl);

    refda_amm_exec_ctx_t ctx;
    refda_amm_exec_ctx_init(&ctx, agent, deref);

    int res = refda_amm_ctrl_desc_execute(ctrl, &ctx);

    refda_amm_exec_ctx_deinit(&ctx);

    return res;
}

/** Implement the execution procedure from Section TBD of @cite ietf-dtn-amm-01.
 */
static int refda_exec_target(refda_agent_t *agent, const ari_t *ari)
{
    CHKERR1(ari);
    CHKERR1(ari->is_ref);
    CHKERR1(ari->as_ref.objpath.has_ari_type); // must be one we understand

    if (pthread_mutex_lock(&(agent->objs_mutex)))
    {
        return 2;
    }

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int retval = 0;

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode_objpath(buf, &(ari->as_ref.objpath), ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_DEBUG("Executing reference to %s", string_get_cstr(buf));
        string_clear(buf);
    }
    int res = cace_amm_lookup_deref(&deref, &(agent->objs), ari);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        retval = 3;
    }

    if (!retval)
    {
        switch (ari->as_ref.objpath.ari_type)
        {
            case ARI_TYPE_CTRL:
                retval = refda_exec_ctrl(agent, &deref);
                break;
            default:
                break;
        }
    }

    cace_amm_lookup_deinit(&deref);

    if (pthread_mutex_unlock(&(agent->objs_mutex)))
    {
        return 2;
    }
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
            refda_exec_process(agent, &ari);
        }

        ari_deinit(&ari);
        if (at_end)
        {
            break;
        }
    }

    CACE_LOG_INFO("Worker stopped");
    daemon_run_stop(&agent->running); // FIXME move farther down chain
    return NULL;
}
