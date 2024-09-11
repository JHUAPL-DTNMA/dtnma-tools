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
#include "cace/util/threadset.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_agent_init(refda_agent_t *agent)
{
    string_init(agent->agent_eid);
    daemon_run_init(&(agent->running));
    threadset_init(agent->threads);
    cace_amm_obj_store_init(&(agent->objs));
    pthread_mutex_init(&(agent->objs_mutex), NULL);
    agent_ari_queue_init(agent->execs, AGENT_QUEUE_SIZE);
    sem_init(&(agent->execs_sem), 0, 0);
    agent_ari_queue_init(agent->egress, AGENT_QUEUE_SIZE);
}

void refda_agent_deinit(refda_agent_t *agent)
{
    agent_ari_queue_clear(agent->egress);
    sem_destroy(&(agent->execs_sem));
    agent_ari_queue_clear(agent->execs);
    pthread_mutex_destroy(&(agent->objs_mutex));
    cace_amm_obj_store_deinit(&(agent->objs));
    threadset_clear(agent->threads);
    daemon_run_cleanup(&(agent->running));
    string_clear(agent->agent_eid);
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
    return 0;
}
