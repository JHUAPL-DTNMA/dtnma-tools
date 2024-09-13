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
#ifndef REFDA_AMM_RUNCTX_H_
#define REFDA_AMM_RUNCTX_H_

#include "../agent.h"
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

/** Context for execution activities.
 */
typedef struct
{
    /** Reference to the agent being executed upon.
     * This will never be null.
     */
    refda_agent_t *agent;

    /** Dereference result which led to this execution.
     * This will never be null.
     */
    const cace_amm_lookup_t *deref;

    /** Storage for an optional result value.
     * This is initialized as undefined and may be set to any other value
     * to indicate that a result is produced.
     */
    ari_t result;
} refda_amm_exec_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] deref The dereference result.
 * The result must outlive this context.
 */
void refda_amm_exec_ctx_init(refda_amm_exec_ctx_t *obj, refda_agent_t *agent, const cace_amm_lookup_t *deref);

void refda_amm_exec_ctx_deinit(refda_amm_exec_ctx_t *obj);

/** Context for value production activities.
 */
typedef struct
{
    /** Original path dereferenced to the object being produced from.
     * All path segments are in their original form.
     */
    const ari_objpath_t *objpath;
    /** Actual parameters normalized for this object from the given parameters.
     */
    cace_amm_actual_param_set_t aparams;

    /** Storage for the produced value.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful production.
     */
    ari_t value;
} refda_amm_valprod_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] fparams Formal parameters from the dereferenced object.
 * @param[in] ref The object reference ARI.
 * The ARI must outlive this context.
 */
int refda_amm_valprod_ctx_init(refda_amm_valprod_ctx_t *obj, const cace_amm_formal_param_list_t fparams,
                               const ari_t *ref);

void refda_amm_valprod_ctx_deinit(refda_amm_valprod_ctx_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_RUNCTX_H_ */
