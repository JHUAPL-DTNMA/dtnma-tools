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

/** Possible execution states for a single CTRL item ::refda_exec_item_t.
 * The final state of finished is when the item gets deleted.
 */
typedef enum
{
    /**
     * Indicator that this item is yet to be completed or paused
     */
    REFDA_EXEC_PENDING = 0,
    /**
     * Indicator that this item is running
     */
    REFDA_EXEC_RUNNING,
    /**
     * Indicator that this item is waiting on an external event to complete
     */
    REFDA_EXEC_WAITING,
    /**
     * Indicator that this item completed execution but is not finished
     */
    REFDA_EXEC_COMPLETE
} refda_exec_item_status_t;

/** Each item in an execution sequence, which corresponds to a
 * dereferenced control.
 */
typedef struct
{
    /// Weak reference to a parent execution sequence
    refda_exec_seq_t *seq;

    /** A copy of the single-CTRL reference which created this item.
     * This includes given parameters for the execution.
     */
    cace_ari_t ref;

    /** Dereference result for CTRL of this item.
     * This includes actual parameters for the execution.
     */
    cace_amm_lookup_t deref;

    /** Storage for an optional result value.
     * This is initialized as undefined and may be set to any other value
     * to indicate that a result is produced.
     */
    cace_ari_t result;

    /**
     * Values are one of the ::refda_exec_item_status_t enumerations
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
void refda_exec_item_deinit(refda_exec_item_t *obj);

/// M*LIB OPLIST for refda_exec_item_t
#define M_OPL_refda_exec_item_t() \
    (INIT(API_2(refda_exec_item_init)), CLEAR(API_2(refda_exec_item_deinit)), INIT_SET(0), SET(0))

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_ITEM_H_ */
