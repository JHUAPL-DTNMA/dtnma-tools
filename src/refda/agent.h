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

#ifndef REFDA_AGENT_H_
#define REFDA_AGENT_H_

#include "instr.h"
#include "msgdata.h"
#include "exec_seq.h"
#include "rpt_agg.h"
#include "timeline.h"
#include "acl.h"
#include <cace/util/daemon_run.h>
#include <cace/util/threadset.h>
#include <cace/amm/obj_ns.h>
#include <cace/amm/obj_store.h>
#include <cace/amm/msg_if.h>
#include <m-buffer.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Error result when agent locking fails
#define REFDA_AGENT_ERR_LOCK_FAILED 2

/// Size of agent hand-off queues
#define AGENT_QUEUE_SIZE 1024

/** @struct string_list_t
 * Linked list of text strings with ownership semantics.
 * Usable as a simple memory pool for runtime text allocation.
 */
/// @cond Doxygen_Suppress
M_LIST_DEF(string_list, m_string_t, M_STRING_OPLIST)
/// @endcond

/** State of a DTNMA Agent.
 */
typedef struct refda_agent_s
{
    /// Agent endpoint ID as URI text
    string_t agent_eid;

    /// Running state
    cace_daemon_run_t running;
    /// Messaging configuration
    cace_amm_msg_if_t mif;
    /// Instrumentation counters
    refda_instr_t instr;
    /// Threads associated with the agent
    cace_threadset_t threads;

    /// Access control list
    refda_acl_t acl;
    /// Mutex for the state of #acl
    pthread_mutex_t acl_mutex;

    /// Text string ownership for ODM (runtime-defined) text names
    string_list_t odm_names;
    /// Runtime AMM object store
    cace_amm_obj_store_t objs;
    /// Mutex for the state of #objs, its object sub-trees, and #odm_names
    pthread_mutex_t objs_mutex;

    /// Cached type from //ietf-amm/TYPEDEF/MAC
    const cace_amm_type_t *mac_type;
    /// Cached type from //ietf-amm/TYPEDEF/EXPR
    const cace_amm_type_t *expr_type;
    /// Cached type from //ietf-amm/TYPEDEF/RPTT
    const cace_amm_type_t *rptt_type;

    /// Ingress EXECSET queue
    refda_msgdata_queue_t execs;
    /// Startup state to disable use of #execs
    atomic_bool execs_enable;
    /// Semaphore for items in #execs and change of #execs_enable
    sem_t execs_sem;

    /// The next-to-use execution PID
    uint64_t exec_next_pid;
    /** Execution sequence state table.
     * This is owned by the refda_exec_worker() thread.
     */
    refda_exec_seq_list_t exec_state;
    /// Mutex for the state of #exec_state
    pthread_mutex_t exec_state_mutex;
    /** Execution wait timeline.
     * This is owned by the refda_exec_worker() thread.
     */
    refda_timeline_t exec_timeline;

    /// Egress RPTSET queue
    refda_msgdata_queue_t rptgs;
    /// Semaphore for items in #rptgs
    sem_t rptgs_sem;

    /// Pending aggregated RPTSETs
    // TBD
} refda_agent_t;

void refda_agent_init(refda_agent_t *agent);

void refda_agent_deinit(refda_agent_t *agent);

/** Lock the object mutex on an agent and return if failed.
 */
#define REFDA_AGENT_LOCK(agent, err)                  \
    if (pthread_mutex_lock(&(agent->objs_mutex)))     \
    {                                                 \
        CACE_LOG_ERR("failed to lock agent objects"); \
        return err;                                   \
    }
/** Unlock the object mutex on an agent and return if failed.
 */
#define REFDA_AGENT_UNLOCK(agent, err)                  \
    if (pthread_mutex_unlock(&(agent->objs_mutex)))     \
    {                                                   \
        CACE_LOG_ERR("failed to unlock agent objects"); \
        return err;                                     \
    }

/** Store the current timestamp in an ARI.
 *
 * @param[in] agent The agent context.
 * @param[in,out] val The value to set.
 * This ARI must already be initialized.
 * @return Zero if successful, or 2 if the clock is not available.
 */
int refda_agent_nowtime(refda_agent_t *agent, cace_ari_t *val);

/** Lookup a specific registered object by reference.
 *
 * @pre The agent object store must already be locked.
 *
 * @param[in] agent The agent to search within.
 * @param org_id The namespace organization enumeration.
 * @param model_id The namespace model enumeration.
 * @param type_id The object type.
 * @param obj_id The object enumeration.
 * @return The object descriptor or NULL if not found.
 */
cace_amm_obj_desc_t *refda_agent_get_object(refda_agent_t *agent, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                            cace_ari_type_t type_id, cace_ari_int_id_t obj_id);

/** Lookup a specific known TYPEDEF by reference.
 *
 * @pre The agent object store must already be locked.
 *
 * @param[in] agent The agent to search within.
 * @param org_id The namespace organization enumeration.
 * @param model_id The namespace model enumeration.
 * @param obj_id The object enumeration.
 * @return The typing object or NULL if not found.
 */
cace_amm_type_t *refda_agent_get_typedef(refda_agent_t *agent, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                         cace_ari_int_id_t obj_id);

/** After all ADMs are registered, bind cross-references between them.
 *
 * @param[in,out] agent The agent state to update.
 * @return The number of failed binds, or zero if successful.
 */
int refda_agent_bindrefs(refda_agent_t *agent);

/** After all ADMs are registered, finish object initialization.
 *
 * @param[in,out] agent The agent state to update.
 * @return The number of failed object initializations, or zero if successful.
 */
int refda_agent_init_objs(refda_agent_t *agent);

/** Called to start worker threads.
 *
 * @param[in] agent The agent state.
 * @return Zero if successful.
 */
int refda_agent_start(refda_agent_t *agent);

/** Called to join worker threads.
 * This should be called after refda_agent_start().
 *
 * @param[in] agent The agent state.
 * @return Zero if successful.
 */
int refda_agent_stop(refda_agent_t *agent);

/** Queue a startup macro to be executed after worker threads are running.
 *
 * @pre This must be called after refda_agent_start().
 * @param[in] agent The agent to send from.
 * @param[in] target The target to execute.
 * This object is moved from.
 * @return Zero if successful.
 */
int refda_agent_startup_exec(refda_agent_t *agent, cace_ari_t *target);

/** Enable ingress processing after work threads are started and all startup is completed.
 *
 * @pre This is called after all refda_agent_startup_exec() uses.
 * @param[in] agent The agent to send from.
 * @param[in] target The target to execute.
 * This object is moved from.
 */
void refda_agent_enable_exec(refda_agent_t *agent);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AGENT_H_ */
