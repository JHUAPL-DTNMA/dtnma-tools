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
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Each item in an execution sequence, which corresponds to a
 * dereferenced control.
 */
typedef struct
{
    /** A copy of the single-CTRL reference which created this item.
     */
    ari_t ref;

    /** Dereference result for CTRL of this item.
     */
    cace_amm_lookup_t deref;

    atomic_bool waiting;

} refda_exec_item_t;

void refda_exec_item_init(refda_exec_item_t *obj);

void refda_exec_item_deinit(refda_exec_item_t *obj);

/// M*LIB OPLIST for refda_exec_item_t
#define M_OPL_refda_exec_item_t() (INIT(API_2(refda_exec_item_init)), CLEAR(API_2(refda_exec_item_deinit)))

/// @cond Doxygen_Suppress
DEQUE_DEF(refda_exec_item_list, refda_exec_item_t)
/// @endcond

/** The state of a single execution within an Agent.
 * Each item has an original target and a sequence of
 */
typedef struct
{
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
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_ITEM_H_ */
