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

#ifndef REFDA_EXEC_ITEM_H_
#define REFDA_EXEC_ITEM_H_

#include <cace/amm/lookup.h>
#include <cace/ari.h>
#include <m-atomic.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct refda_exec_seq_s refda_exec_seq_t;

#define EXEC_PENDING  0
#define EXEC_RUNNING  1
#define EXEC_WAITING  2
#define EXEC_COMPLETE 3

/** Each item in an execution sequence, which corresponds to a
 * dereferenced control.
 */
typedef struct
{
    /// Weak reference to a parent execution sequence
    refda_exec_seq_t *seq;

    /** A copy of the single-CTRL reference which created this item.
     */
    cace_ari_t ref;

    /** Dereference result for CTRL of this item.
     */
    cace_amm_lookup_t deref;

    /** Storage for an optional result value.
     * This is initialized as undefined and may be set to any other value
     * to indicate that a result is produced.
     */
    cace_ari_t result;

    /**
     * Indicator if this item is
     * 1) yet to be completed or paused
     * 2) waiting on some external event to complete
     * 3) completed execution but not finished
     * While waiting this item cannot be executed and will not yet have a
     * valid #result.
     *
     * Values are:
     * EXEC_PENDING 0
     * EXEC_WAITING 1
     * EXEC_COMPLETE 2
     */
    atomic_int execution_stage;

} refda_exec_item_t;

/**
 * Wake up a "waiting" exec item. This can be called externally in the use case where
 * there is an exec item that has been paused and deferred without blocking or using the timeline.
 * Once this function is called, refda_exec_ctrl_finish will be called in the exec worker.
 */
void refda_exec_item_resume(refda_exec_item_t *obj);

/** Interface for M*LIB use.
 */
void refda_exec_item_init(refda_exec_item_t *obj);

/** Interface for M*LIB use.
 */
void refda_exec_item_init_set(refda_exec_item_t *obj, const refda_exec_item_t *src);

/** Interface for M*LIB use.
 */
void refda_exec_item_deinit(refda_exec_item_t *obj);

/** Interface for M*LIB use.
 */
void refda_exec_item_set(refda_exec_item_t *obj, const refda_exec_item_t *src);

/// M*LIB OPLIST for refda_exec_item_t
#define M_OPL_refda_exec_item_t()                                                                                   \
    (INIT(API_2(refda_exec_item_init)), INIT_SET(API_6(refda_exec_item_init_set)), SET(API_6(refda_exec_item_set)), \
     CLEAR(API_2(refda_exec_item_deinit)))

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_ITEM_H_ */
