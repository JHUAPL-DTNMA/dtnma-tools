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
#include "ingress.h"
#include "agent.h"
#include <cace/util/daemon_run.h>
#include <cace/util/logging.h>

void *refda_ingress_worker(void *arg)
{
    refda_agent_t *agent = arg;
    CACE_LOG_INFO("Worker started");

    cace_ari_list_t values;
    cace_ari_list_init(values);
    cace_amm_msg_if_metadata_t meta;
    cace_amm_msg_if_metadata_init(&meta);

    /*
     * agent->running controls the overall execution of threads in the
     * Agent.
     */
    while (cace_daemon_run_get(&agent->running))
    {
        cace_ari_list_reset(values);
        int recv_res = agent->mif.recv(values, &meta, &agent->running, agent->mif.ctx);

        // process received items even if failed status
        if (!cace_ari_list_empty_p(values))
        {
            CACE_LOG_INFO("Message has %d ARIs", cace_ari_list_size(values));

            cace_ari_list_it_t val_it;
            /* For each received ARI, validate it */
            for (cace_ari_list_it(val_it, values); !cace_ari_list_end_p(val_it); cace_ari_list_next(val_it))
            {
                cace_ari_t *val = cace_ari_list_ref(val_it);
                if (!cace_ari_get_execset(val))
                {
                    CACE_LOG_ERR("Ignoring input ARI that is not an EXECSET");
                    // item is left in list for later deinit
                    continue;
                }

                refda_msgdata_t exec_item;
                refda_msgdata_init(&exec_item);
                cace_ari_set_copy(&exec_item.ident, &meta.src);
                cace_ari_set_move(&exec_item.value, val);

                refda_msgdata_queue_push_move(agent->execs, &exec_item);
                sem_post(&(agent->execs_sem));

                atomic_fetch_add(&agent->instr.num_execset_recv, 1);
            }

            // Update the (mutex proctected) agent.instr.last_time_recv with the message's meta timestamp
            if (pthread_mutex_lock(&agent->instr.mutex) != 0)
            {
                CACE_LOG_ERR(REFDA_INSTR_MSG_FAIL_MUTEX_ACQUIRE);
                continue;
            }
            cace_ari_set_copy(&agent->instr.last_time_recv, &meta.timestamp);
            if (pthread_mutex_unlock(&agent->instr.mutex) != 0)
            {
                CACE_LOG_ERR(REFDA_INSTR_MSG_FAIL_MUTEX_RELEASE);
                continue;
            }
        }

        if (recv_res)
        {
            CACE_LOG_INFO("Got mif.recv result=%d, stopping", recv_res);
            atomic_fetch_add(&agent->instr.num_execset_recv_failure, 1);

            // flush the input queue but keep the daemon running
            refda_msgdata_t undef;
            refda_msgdata_init(&undef);
            refda_msgdata_queue_push_move(agent->execs, &undef);
            sem_post(&(agent->execs_sem));

            break;
        }
    }

    cace_amm_msg_if_metadata_deinit(&meta);
    cace_ari_list_clear(values);

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}

void refda_ingress_push_move(refda_agent_t *agent, const cace_amm_msg_if_metadata_t *meta, cace_ari_t *ari)
{
    if (cace_ari_get_execset(ari) == NULL)
    {
        CACE_LOG_ERR("Ignoring input ARI that is not an EXECSET");
        return;
    }

    refda_msgdata_t execItem;
    refda_msgdata_init(&execItem);
    cace_ari_set_copy(&execItem.ident, &meta->src);
    cace_ari_set_move(&execItem.value, ari);

    refda_msgdata_queue_push_move(agent->execs, &execItem);
    sem_post(&(agent->execs_sem));

    atomic_fetch_add(&agent->instr.num_execset_recv, 1);
    CACE_LOG_DEBUG("Pushed an item into the exec thread");
}
