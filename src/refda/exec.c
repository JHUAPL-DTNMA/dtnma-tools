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
#include "cace/util/logging.h"

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
            CACE_LOG_INFO("popped exec!");
        }

        ari_deinit(&ari);
        if (at_end)
        {
            break;
        }
    }

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}
