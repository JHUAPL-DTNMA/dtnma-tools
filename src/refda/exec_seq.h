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

#ifndef REFDA_EXEC_SEQ_H_
#define REFDA_EXEC_SEQ_H_

#include "exec_item.h"
#include "exec_status.h"
#include "runctx.h"
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond Doxygen_Suppress
DEQUE_DEF(refda_exec_item_list, refda_exec_item_t)
/// @endcond

/** The state of a single execution within an Agent.
 * Each item has an original target and a sequence of
 */
typedef struct refda_exec_seq_s
{
    /// Context for the source of this sequence
    refda_runctx_ptr_t runctx;

    /** Internal unique processing identifier for the execution.
     * Zero is an invalid value and will not be assigned.
     */
    uint64_t pid;

    /** Remaining list of items to be executed in this sequence.
     * As executions complete, this list is manipulated to pop off each
     * front item.
     */
    refda_exec_item_list_t items;

    /** Pointer to optional externally-owned finish state tracker.
     */
    refda_exec_status_t *status;

} refda_exec_seq_t;

void refda_exec_seq_init(refda_exec_seq_t *obj);

void refda_exec_seq_deinit(refda_exec_seq_t *obj);

/// Comparison function to allow sorting by PID
int refda_exec_seq_cmp(const refda_exec_seq_t *lt, const refda_exec_seq_t *rt);

/// M*LIB OPLIST for refda_exec_seq_t
#define M_OPL_refda_exec_seq_t() \
    (INIT(API_2(refda_exec_seq_init)), CLEAR(API_2(refda_exec_seq_deinit)), CMP(API_6(refda_exec_seq_cmp)))

/// @cond Doxygen_Suppress
M_DEQUE_DEF(refda_exec_seq_list, refda_exec_seq_t)
M_DEQUE_DEF(refda_exec_seq_ptr_list, refda_exec_seq_t *, M_PTR_OPLIST)
M_RBTREE_DEF(refda_exec_seq_ptr_tree, refda_exec_seq_t *, M_OPEXTEND(M_PTR_OPLIST, CMP(refda_exec_seq_cmp)))
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_SEQ_H_ */
