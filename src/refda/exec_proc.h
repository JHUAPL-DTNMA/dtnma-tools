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
/** @file
 * @ingroup refda
 * Execution sequence and item processing internal APIs.
 * Use the functions in @ref exec.h as a public API.
 */
#ifndef REFDA_EXEC_PROC_H_
#define REFDA_EXEC_PROC_H_

#include "exec_seq.h"
#include "exec_item.h"
#include "runctx.h"
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Implement the expansion procedure from Section TBD of @cite ietf-dtn-amm-01.
 *
 * @param[in,out] seq A freshly initialized sequence to expand into.
 * @param[in] runctxp The agent state for ARI lookup.
 * @param[in] target The target to dereference, if necessary, and expand.
 * @return Zero if successful.
 */
int refda_exec_proc_expand(refda_exec_seq_t *seq, refda_runctx_ptr_t runctxp, const cace_ari_t *target);

/** Implement the running procedure from Section TBD of @cite ietf-dtn-amm-01.
 * This executes items in a sequence until the first deferred completion.
 *
 * @param[in,out] seq The sequence which will be popped as items are executed.
 * @return Zero if this sequence executed without error (so far).
 */
int refda_exec_proc_run(refda_exec_seq_t *seq);

/** Execute a single CTRL, possibly deferring its finish.
 */
int refda_exec_proc_ctrl_start(refda_exec_seq_t *seq);

/** Finish the execution of an item referencing a single CTRL.
 * Also report on a result if requested.
 */
int refda_exec_proc_ctrl_finish(refda_exec_item_t *item);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_PROC_H_ */
