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
#include "egress.h"
#include "agent.h"
#include "cace/util/logging.h"

void *refda_egress_worker(void *arg)
{
    refda_agent_t *agent = arg;
    CACE_LOG_INFO("Worker started");

    while (daemon_run_get(&agent->running))
    {
        ari_t ari;

        sem_wait(&(agent->rptgs_sem));
        if (!agent_ari_queue_pop(&ari, agent->rptgs))
        {
            // shouldn't happen
            CACE_LOG_WARNING("failed to pop from rptgs queue");
            continue;
        }
        // sentinel for end-of-input
        const bool at_end = ari_is_undefined(&ari);
        if (!at_end)
        {
            ari_list_t data;
            ari_list_init(data);
            cace_amm_msg_if_metadata_t meta;
            cace_amm_msg_if_metadata_init(&meta);

            ari_list_push_back_move(data, &ari);

            int send_res = (agent->mif.send)(data, &meta, agent->mif.ctx);
            if (send_res)
            {
                CACE_LOG_WARNING("Got mif.send result=%d", send_res);
            }

            ari_list_clear(data);
            cace_amm_msg_if_metadata_deinit(&meta);
        }

        ari_deinit(&ari);
        if (at_end)
        {
            // No more reports possible
            daemon_run_stop(&agent->running); // FIXME move farther down chain

            break;
        }
    }

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}
