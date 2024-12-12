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

#ifndef REFDA_EXEC_SEQ_H_
#define REFDA_EXEC_SEQ_H_

#include "exec_item.h"
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

} refda_exec_seq_t;

void refda_exec_seq_init(refda_exec_seq_t *obj);

void refda_exec_seq_deinit(refda_exec_seq_t *obj);

/// M*LIB OPLIST for refda_exec_seq_t
#define M_OPL_refda_exec_seq_t() (INIT(API_2(refda_exec_seq_init)), CLEAR(API_2(refda_exec_seq_deinit)))

/// @cond Doxygen_Suppress
DEQUE_DEF(refda_exec_seq_list, refda_exec_seq_t)
DEQUE_DEF(refda_exec_seq_ptr_list, refda_exec_seq_t *, M_PTR_OPLIST)
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_SEQ_H_ */
