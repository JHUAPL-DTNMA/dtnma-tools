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

#ifndef REFDA_TIMELINE_H_
#define REFDA_TIMELINE_H_

#include "exec_item.h"
#include "ctrl_exec_ctx.h"
#include <m-rbtree.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration for callback reference
struct refda_ctrl_exec_ctx_s;
typedef struct refda_ctrl_exec_ctx_s refda_ctrl_exec_ctx_t;

typedef struct refda_timeline_event_s
{
    /** Specific time at which the event should occur.
     */
    struct timespec ts;
    /// Execution item which created the wait
    refda_exec_item_t *item;
    /** Execution-defined callback, which should not be null.
     *
     * @param[in,out] ctx The associated execution context.
     * A result value is set when the execution has finished.
     */
    void (*callback)(refda_ctrl_exec_ctx_t *ctx);

} refda_timeline_event_t;

/** Compare timestamps of two events.
 */
int refda_timeline_event_cmp(const refda_timeline_event_t *lt, const refda_timeline_event_t *rt);

/// M*LIB OPLIST for refda_timeline_event_t
#define M_OPL_refda_timeline_event_t() M_OPEXTEND(M_POD_OPLIST, CMP(API_6(refda_timeline_event_cmp)))

/// @cond Doxygen_Suppress
RBTREE_DEF(refda_timeline, refda_timeline_event_t)
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_TIMELINE_H_ */
