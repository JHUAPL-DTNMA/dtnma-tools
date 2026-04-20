/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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

#ifndef REFDA_EXEC_STATUS_H_
#define REFDA_EXEC_STATUS_H_

#include <m-atomic.h>
#include <semaphore.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct refda_exec_seq_s refda_exec_seq_t;

/** Mechanism to provide a "future"-type interface for execution.
 * This allows a caller to refda_exec_add_target() to synchronize on the
 * finish of the execution, either as success or failure.
 * The user can either set #on_finished callback or wait on the #finished
 * semaphore.
 */
typedef struct refda_exec_status_s
{
    /** Sequence associated with this status.
     * This is non-null while the sequence is valid.
     */
    refda_exec_seq_t *seq;

    /** An optional callback executed when target has finished
     * but before the #finished semaphore is posted.
     * This callback can be executed from any execution thread.
     *
     * @param failed The finished failed state.
     * @param[in] user_data A copy of the #on_finished_arg for this status.
     */
    void (*on_finished)(bool failed, void *user_data);
    /// User data for #on_finished
    void *on_finished_arg;

    /// Once #finished has a value, this is an indicator that a CTRL failed
    atomic_bool failed;
    /** A semaphore which has a value when the target is finished executing
     * and #failed is marked.
     */
    sem_t finished;
} refda_exec_status_t;

void refda_exec_status_init(refda_exec_status_t *obj);

void refda_exec_status_deinit(refda_exec_status_t *obj);

/** Wait for the finished semaphore and take it.
 *
 * @param[in] obj The status to wait on.
 * @return True if there was a failure.
 */
bool refda_exec_status_wait(refda_exec_status_t *obj);

/** Set the failed state and post the semaphore.
 *
 * @param[in,out] obj The status to update.
 * @param failed True to mark as a failure.
 * @post Any waiting call will be awoken.
 */
void refda_exec_status_post(refda_exec_status_t *obj, bool failed);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_STATUS_H_ */
