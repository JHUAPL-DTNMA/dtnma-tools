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
#ifndef REFDA_RUNCTX_H_
#define REFDA_RUNCTX_H_

#include "agent.h"
#include "cace/ari/base.h"
#include "cace/amm/lookup.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/** Increment the counter to the next value and signal to a registerd callback.
 *
 */
void refda_amm_modval_state_inc(refda_amm_modval_state_t *obj);

/** Context for all agent runtime activities.
 * Because all contents are external pointers, no deinit function is needed.
 */
typedef struct
{
    /** Reference to the agent being operated within.
     * This will never be null outside of unit tests.
     */
    refda_agent_t *agent;

    /** During execution, a reference to a context nonce value.
     * This can be null if there is no context nonce.
     */
    const ari_t *nonce;

} refda_runctx_t;

/** Initialize a runtime context.
 *
 * @param[out] ctx The contect to initialize.
 * @param[in] agent The agent being run within.
 * @param[in] exec The optional EXECSET message being run within.
 * @return Zero if successful.
 */
int refda_runctx_init(refda_runctx_t *ctx, refda_agent_t *agent, const ari_t *exec);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_RUNCTX_H_ */