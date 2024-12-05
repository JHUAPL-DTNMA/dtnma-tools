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
#include "agent.h"
#include "ingress.h"
#include "egress.h"
#include "exec.h"
#include "reporting.h"
#include "amm/typedef.h"
#include "adm/ietf.h"
#include "cace/amm/lookup.h"
#include "cace/util/threadset.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include <errno.h>

void refda_agent_init(refda_agent_t *agent)
{
    string_init(agent->agent_eid);
    daemon_run_init(&(agent->running));
    threadset_init(agent->threads);

    cace_amm_obj_store_init(&(agent->objs));
    pthread_mutex_init(&(agent->objs_mutex), NULL);

    refda_msgdata_queue_init(agent->execs, AGENT_QUEUE_SIZE);
    sem_init(&(agent->execs_sem), 0, 0);

    refda_msgdata_queue_init(agent->rptgs, AGENT_QUEUE_SIZE);
    sem_init(&(agent->rptgs_sem), 0, 0);
}

void refda_agent_deinit(refda_agent_t *agent)
{
    sem_destroy(&(agent->rptgs_sem));
    refda_msgdata_queue_clear(agent->rptgs);

    sem_destroy(&(agent->execs_sem));
    refda_msgdata_queue_clear(agent->execs);

    pthread_mutex_destroy(&(agent->objs_mutex));
    cace_amm_obj_store_deinit(&(agent->objs));

    threadset_clear(agent->threads);
    daemon_run_cleanup(&(agent->running));
    string_clear(agent->agent_eid);
}

/// Time of the DTN epoch (2000-01-01T00:00:00Z) in the POSIX clock
#define DTN_EPOCH_IN_POSIX 946702800

int refda_agent_nowtime(refda_agent_t *agent _U_, ari_t *val)
{
    CHKERR1(val);

    struct timespec time;
    int             res = clock_gettime(CLOCK_REALTIME, &time);
    if (res)
    {
        CACE_LOG_ERR("Failed in clock_gettime() with errno %d", errno);
        return 2;
    }

    // simple offset
    time.tv_sec -= DTN_EPOCH_IN_POSIX;

    ari_set_tp(val, time);
    return 0;
}

amm_type_t *refda_agent_get_typedef(refda_agent_t *agent, int64_t ns_id, int64_t obj_id)
{
    amm_type_t *found = NULL;

    ari_t ref = ARI_INIT_UNDEFINED;
    ari_set_objref_path_intid(&ref, ns_id, ARI_TYPE_TYPEDEF, obj_id);

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    if (!cace_amm_lookup_deref(&deref, &(agent->objs), &ref))
    {
        refda_amm_typedef_desc_t *typedesc = deref.obj->app_data.ptr;
        if (typedesc)
        {
            found = &(typedesc->typeobj);
        }
    }

    cace_amm_lookup_deinit(&deref);
    ari_deinit(&ref);

    return found;
}

int refda_agent_bindrefs(refda_agent_t *agent)
{
    REFDA_AGENT_LOCK(agent);
    int failcnt = 0;

    agent->mac_type = refda_agent_get_typedef(agent, REFDA_ADM_IETF_AMM_ENUM, 21);
    if (!agent->mac_type)
    {
        ++failcnt;
    }

    REFDA_AGENT_UNLOCK(agent);

    return failcnt;
}

int refda_agent_start(refda_agent_t *agent)
{
    CHKERR1(agent);
    CHKERR1(agent->mif.recv);
    CHKERR1(agent->mif.send);
    CACE_LOG_INFO("Work threads starting...");

    // clang-format off
    threadinfo_t threadinfo[] = {
        { &refda_ingress_worker, "ingress" },
        { &refda_egress_worker, "egress" },
        { &refda_exec_worker, "exec" },
        //        { &rda_reports, "rda_reports" },
        //        { &rda_rules, "rda_rules" },
    };
    // clang-format on
    int res = threadset_start(agent->threads, threadinfo, sizeof(threadinfo) / sizeof(threadinfo_t), agent);
    if (res)
    {
        CACE_LOG_ERR("Failed to start work threads: %d", res);
        return 2;
    }

    CACE_LOG_INFO("Work threads started");
    return 0;
}

int refda_agent_stop(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_INFO("Work threads stopping...");

    /* Notify threads */
    daemon_run_stop(&agent->running);

    threadset_join(agent->threads);

    CACE_LOG_INFO("Work threads stopped");
    return 0;
}

int refda_agent_send_hello(refda_agent_t *agent)
{
    ari_t ref = ARI_INIT_UNDEFINED;
    // ari:/ietf-dtnma-agent/CONST/hello
    ari_set_objref_path_intid(&ref, REFDA_ADM_IETF_DTNMA_AGENT_ENUM, ARI_TYPE_CONST, 0);

    // dummy message source
    refda_msgdata_t msg;
    refda_msgdata_init(&msg);
    // FIXME how to indicate this destination..?
    static const char *src = "any";
    cace_data_copy_from(&(msg.ident), strlen(src) - 1, (cace_data_ptr_t)src);

    refda_runctx_t runctx;
    int            retval = 0;
    int            res    = refda_runctx_init(&runctx, agent, &msg);
    if (res)
    {
        retval = 2;
    }

    if (!retval)
    {
        res = refda_reporting_target(&runctx, &ref);
        if (res)
        {
            retval = 3;
        }
    }

    // no deinit for runctx
    refda_msgdata_deinit(&msg);

    return retval;
}
