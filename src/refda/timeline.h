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

#include <m-rbtree.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    /** Specific time at which the event should occur.
     */
    struct timespec ts;
    /// Reference to user data for this event
    void *ref;
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
