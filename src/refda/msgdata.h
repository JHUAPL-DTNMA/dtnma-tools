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

#ifndef REFDA_MSGDATA_H_
#define REFDA_MSGDATA_H_

#include <cace/ari.h>
#include <m-buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A combination of ARI and authenticated peer identity.
 * This applies to incoming EXECSET and outgoing RPTSET values.
 */
typedef struct refda_msgdata_s
{
    /// The peer identity as opaque value
    cace_ari_t ident;
    /// The message value
    cace_ari_t value;
} refda_msgdata_t;

void refda_msgdata_init(refda_msgdata_t *obj);

void refda_msgdata_init_move(refda_msgdata_t *obj, refda_msgdata_t *src);

void refda_msgdata_deinit(refda_msgdata_t *obj);

void refda_msgdata_set(refda_msgdata_t *obj, const refda_msgdata_t *src);

static inline void refda_msgdata_init_set(refda_msgdata_t *obj, const refda_msgdata_t *src)
{
    refda_msgdata_init(obj);
    refda_msgdata_set(obj, src);
}

/// OPLIST for refda_msgdata_t
#define M_OPL_refda_msgdata_t()                                                \
    (INIT(API_2(refda_msgdata_init)), INIT_SET(API_6(refda_msgdata_init_set)), \
     INIT_MOVE(API_6(refda_msgdata_init_move)), CLEAR(API_2(refda_msgdata_deinit)), SET(API_6(refda_msgdata_set)))

/// @cond Doxygen_Suppress
QUEUE_SPSC_DEF(refda_msgdata_queue, refda_msgdata_t, BUFFER_QUEUE | BUFFER_PUSH_INIT_POP_MOVE)
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_MSGDATA_H_ */
