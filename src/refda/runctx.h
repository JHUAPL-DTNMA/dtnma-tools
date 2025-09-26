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
#ifndef REFDA_RUNCTX_H_
#define REFDA_RUNCTX_H_

#include "acl.h"
#include "cace/ari/base.h"
#include "cace/amm/lookup.h"
#include <m-shared.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct refda_agent_s   refda_agent_t;
typedef struct refda_msgdata_s refda_msgdata_t;

/** Modifiable value state counter.
 * This is used by EDD and VAR objects.
 */
typedef struct refda_amm_modval_state_s
{
    /// The internal counter
    uint64_t _ctr;

    /** A callback used when the state counter is incremented.
     *
     * @param obj The specific counter which changed.
     * @param user_data A copy of the #notify_user_data pointer.
     */
    void (*notify)(struct refda_amm_modval_state_s *obj, void *user_data);
    /// Optional context data for the #notify callback
    void *notify_user_data;

} refda_amm_modval_state_t;

void refda_amm_modval_state_init(refda_amm_modval_state_t *obj);

void refda_amm_modval_state_deinit(refda_amm_modval_state_t *obj);

/** Increment the counter to the next value and signal to a registered callback.
 *
 */
void refda_amm_modval_state_inc(refda_amm_modval_state_t *obj);

/** Context for all agent runtime activities.
 */
typedef struct
{
    /** Reference to the agent being operated within.
     * This will never be null outside of unit tests.
     */
    refda_agent_t *agent;

    /** During execution, the manager identity which caused the execution.
     * When triggered by the agent itself this will be the undefined value.
     */
    cace_ari_t mgr_ident;

    /** During execution, a reference to a context nonce value.
     * Otherwise this will be the undefined value.
     */
    cace_ari_t nonce;

    /** Cached ACL-derived group ID.
     */
    refda_acl_id_tree_t acl_groups;

} refda_runctx_t;

void refda_runctx_init(refda_runctx_t *ctx);

void refda_runctx_deinit(refda_runctx_t *ctx);

/** Take values for a runtime context.
 *
 * @param[out] ctx The context to initialize.
 * @param[in] agent The agent being run within.
 * @param[in] msg The optional EXECSET message being run within.
 * @return Zero if successful.
 */
int refda_runctx_from(refda_runctx_t *ctx, refda_agent_t *agent, const refda_msgdata_t *msg);

/// M*LIB OPLIST for refda_runctx_t
#define M_OPL_refda_runctx_t() (INIT(API_2(refda_runctx_init)), INIT_SET(0), CLEAR(API_2(refda_runctx_deinit)), SET(0))

/// @cond Doxygen_Suppress
M_SHARED_PTR_DEF(refda_runctx_ptr, refda_runctx_t)
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_RUNCTX_H_ */
