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

#ifndef REFDA_EXEC_H_
#define REFDA_EXEC_H_

#include "exec_status.h"
#include "agent.h"
#include "runctx.h"
#include "amm/sbr.h"
#include "amm/tbr.h"
#include <cace/ari.h>
#include <m-atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Error result when an invalid type is present
#define REFDA_EXEC_ERR_BAD_TYPE 3
/// Error result when dereferencing fails
#define REFDA_EXEC_ERR_DEREF_FAILED 4
/// Error result when value production fails
#define REFDA_EXEC_ERR_PROD_FAILED 5

/** From outside the exec worker thread, inject a single execution target
 * and allow its sequence to be tracked to its finish.
 *
 * This function is thread safe on the agent.
 *
 * @param[in] runctxp The run context, which includes the agent pointer.
 * @param[in] target The execution target (CTRL ref or macro).
 * @param[in] finish An optional execution-finished state for the whole target,
 * or a null pointer to not track this state.
 */
int refda_exec_add_target(refda_runctx_ptr_t runctxp, const cace_ari_t *target, refda_exec_status_t *finish);

/** Work thread function for the Agent execution manager.
 *
 * @param[in] arg The context ::refda_agent_t pointer.
 * @return Always NULL pointer.
 */
void *refda_exec_worker(void *arg);

/** Execute any waiting sequences as part of normal worker iteration.
 *
 * @note This is for unit testing only, not used by agent directly.
 *
 * @param[in] agent The agent state.
 * @return Zero if successful.
 */
int refda_exec_waiting(refda_agent_t *agent);

/** Helper function to run a single iteration of the exec worker thread.
 *
 * @note This is for unit testing only, not used by agent directly.
 *
 * @param[in] agent The agent state.
 * @return True to continue the loop, false to stop
 */
bool refda_exec_worker_iteration(refda_agent_t *agent);

/**
 * Begin periodic execution of a time based rule
 * @param[in] agent The agent context pointer
 * @param[in] tbr The rule to execute
 * @return Non-zero if the rule could not be started
 */
int refda_exec_tbr_enable(refda_agent_t *agent, refda_amm_tbr_desc_t *tbr);

/**
 * Cease periodic execution of a time based rule
 * @param[in] agent The agent context pointer
 * @param[in] tbr The rule to disable
 * @return Non-zero if the rule could not be stopped
 */
int refda_exec_tbr_disable(refda_agent_t *agent, refda_amm_tbr_desc_t *tbr);

/**
 * Begin periodic execution of a state based rule
 * @param[in] agent The agent context pointer
 * @param[in] sbr The rule to execute
 * @return Non-zero if the rule could not be started
 */
int refda_exec_sbr_enable(refda_agent_t *agent, refda_amm_sbr_desc_t *sbr);

/**
 * Cease periodic execution of a state based rule
 * @param[in] agent The agent context pointer
 * @param[in] sbr The rule to disable
 * @return Non-zero if the rule could not be stopped
 */
int refda_exec_sbr_disable(refda_agent_t *agent, refda_amm_sbr_desc_t *sbr);

/**
 * Setup an ARI to execute next in the sequence
 * @param[in] agent    The agent context pointer
 * @param[in] seq      The sequence within which to inject the ARI target(s)
 * @param[in] target   The ARI to execute
 * @return Non-zero if the ARI could not be queued for execution
 */
int refda_exec_next(refda_agent_t *agent, refda_exec_seq_t *seq, const cace_ari_t *target);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_H_ */
