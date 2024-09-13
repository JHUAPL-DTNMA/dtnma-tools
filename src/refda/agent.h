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

#ifndef REFDA_AGENT_H_
#define REFDA_AGENT_H_

#include <cace/util/daemon_run.h>
#include <cace/util/threadset.h>
#include <cace/amm/obj_ns.h>
#include <cace/amm/obj_store.h>
#include <cace/amm/msg_if.h>
#include <m-buffer.h>
#include <pthread.h>

/// Error result when agent locking fails
#define REFDA_AGENT_ERR_LOCK_FAILED 2

/// Size of agent handoff queues
#define AGENT_QUEUE_SIZE 1024

/// @cond Doxygen_Suppress
QUEUE_SPSC_DEF(agent_ari_queue, ari_t, BUFFER_QUEUE | BUFFER_PUSH_INIT_POP_MOVE)
/// @endcond

/** State of a DTNMA Agent.
 */
typedef struct
{
    /// Agent endpoint ID
    string_t agent_eid;

    /// Running state
    daemon_run_t running;
    /// Messaging configuration
    cace_amm_msg_if_t mif;
    /// Threads associated with the agent
    threadset_t threads;

    /// Runtime AMM object store
    cace_amm_obj_store_t objs;
    /// Mutex for the state of #objs
    pthread_mutex_t objs_mutex;

    /// Cached type from //ietf-amm/TYPEDEF/MAC
    const amm_type_t *mac_type;

    /// Ingress EXECSET queue
    agent_ari_queue_t execs;
    sem_t             execs_sem;

    // TBD execution state table

    /// Egress RPTSET queue
    agent_ari_queue_t rptgs;
    sem_t             rptgs_sem;

    /// Pending aggregated RPTSETs
    // TBD
} refda_agent_t;

void refda_agent_init(refda_agent_t *agent);

void refda_agent_deinit(refda_agent_t *agent);

/** Lock the object mutex on an agent and return if failed.
 */
#define REFDA_AGENT_LOCK(agent)                   \
    if (pthread_mutex_lock(&(agent->objs_mutex))) \
    {                                             \
        return REFDA_AGENT_ERR_LOCK_FAILED;       \
    }
/** Unlock the object mutex on an agent and return if failed.
 */
#define REFDA_AGENT_UNLOCK(agent)                   \
    if (pthread_mutex_unlock(&(agent->objs_mutex))) \
    {                                               \
        return REFDA_AGENT_ERR_LOCK_FAILED;         \
    }

/** Store the current timestamp in an ARI.
 *
 * @param[in] agent The agent context.
 * @param[in,out] val The value to set.
 * This ARI must already be initialized.
 * @return Zero if successful, or 2 if the clock is not available.
 */
int refda_agent_nowtime(refda_agent_t *agent, ari_t *val);

/** Lookup a specific known TYPEDEF by reference.
 *
 * @pre The agent object store must already be locked.
 *
 * @param[in] agent The agent to search within.
 * @param ns_id The namespace enumeration.
 * @param obj_id The object enumeration.
 */
amm_type_t *refda_agent_get_typedef(refda_agent_t *agent, int64_t ns_id, int64_t obj_id);

/** After all ADMs are registered, bind cross-references between them.
 *
 * @param[in,out] agent The agent state to update.
 * @return The number of failed binds, or zero if successful.
 */
int refda_agent_bindrefs(refda_agent_t *agent);

int refda_agent_start(refda_agent_t *agent);

int refda_agent_stop(refda_agent_t *agent);

int refda_agent_send_hello(refda_agent_t *agent);

#endif /* REFDA_AGENT_H_ */
