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
#include "ingress.h"
#include "agent.h"
#include "cace/util/logging.h"

void *refda_ingress_worker(void *arg)
{
    refda_agent_t *agent = arg;
    CACE_LOG_INFO("Worker started");

    ari_list_t items;
    ari_list_init(items);
    cace_amm_msg_if_metadata_t meta;
    cace_amm_msg_if_metadata_init(&meta);

    /*
     * agent->running controls the overall execution of threads in the
     * Agent.
     */
    while (daemon_run_get(&agent->running))
    {
        ari_list_reset(items);
        int recv_res = agent->mif.recv(items, &meta, &agent->running, agent->mif.ctx);
        // process received items even if failed status

        if (!ari_list_empty_p(items))
        {
            CACE_LOG_INFO("Message has %d ARIs", ari_list_size(items));

            ari_list_it_t iit;
            /* For each received ARI, validate it */
            for (ari_list_it(iit, items); !ari_list_end_p(iit); ari_list_next(iit))
            {
                ari_t *item = ari_list_ref(iit);
                if (item->is_ref)
                {
                    CACE_LOG_ERR("Ignoring input ARI that is not an EXECSET");
                    continue;
                }
                if (!item->as_lit.has_ari_type || (item->as_lit.ari_type != ARI_TYPE_EXECSET))
                {
                    CACE_LOG_ERR("Ignoring input ARI that is not an EXECSET");
                    continue;
                }

                agent_ari_queue_push_move(agent->execs, item);
                sem_post(&(agent->execs_sem));
            }
        }

        if (recv_res)
        {
            CACE_LOG_INFO("Got mif.recv result=%d, stopping", recv_res);

            // flush the input queue but keep the daemon running
            ari_t undef = ARI_INIT_UNDEFINED;
            agent_ari_queue_push_move(agent->execs, &undef);
            sem_post(&(agent->execs_sem));

            break;
        }
    }

    cace_amm_msg_if_metadata_deinit(&meta);
    ari_list_clear(items);

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}
