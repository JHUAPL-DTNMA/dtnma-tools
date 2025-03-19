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

#ifndef REFDA_EXEC_H_
#define REFDA_EXEC_H_

#include "agent.h"
#include "runctx.h"
#include "amm/sbr.h"
#include "amm/tbr.h"
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Error result when an invalid type is present
#define REFDA_EXEC_ERR_BAD_TYPE 3
/// Error result when dereferencing fails
#define REFDA_EXEC_ERR_DEREF_FAILED 4
/// Error result when value production fails
#define REFDA_EXEC_ERR_PROD_FAILED 5

/** Implement the expansion procedure from Section TBD of @cite ietf-dtn-amm-01.
 *
 * @param[in,out] seq A freshly initialized sequence to expand into.
 * @param[in] runctxp The agent state for ARI lookup.
 * @param[in] ari The ARI to dereference, if necessary, and execute.
 * @return Zero if successful.
 */
int refda_exec_exp_target(refda_exec_seq_t *seq, refda_runctx_ptr_t runctxp, const cace_ari_t *ari);

/** Implement the running procedure from Section TBD of @cite ietf-dtn-amm-01.
 * This executes items in a sequence until the first deferred completion.
 *
 * @param[in,out] seq The sequence which will be popped as items are executed.
 * @return Zero if this sequence executed without error (so far).
 */
int refda_exec_run_seq(refda_exec_seq_t *seq);

/** Work thread function for the Agent execution manager.
 *
 * @param[in] arg The context ::refda_agent_t pointer.
 */
void *refda_exec_worker(void *arg);

/** Helper function to run a single iteration of the exec worker thread
 *
 * @param[in] arg The context ::refda_agent_t pointer.
 */
void refda_exec_worker_iteration(refda_agent_t *agent);

int refda_exec_waiting(refda_agent_t *agent);

/**
 * Begin periodic execution of a time based rule
 * @param[in] The agent context pointer
 * @param[in] The rule to execute
 * @return Non-zero if the rule could not be started
 */
int refda_exec_tbr_enable(refda_agent_t *agent, refda_amm_tbr_desc_t *tbr);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_H_ */
