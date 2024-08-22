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
#ifndef CACE_AMM_RUNCTX_H_
#define CACE_AMM_RUNCTX_H_

#include "cace/ari/base.h"
#include "parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    ari_actual_param_set_t aparams;

    /** Storage for the produced value.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful production.
     */
    ari_t value;
} cace_amm_valprod_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] fparams Formal parameters from the dereferenced object.
 * @param[in] ref The object reference ARI.
 * The ARI must outlive this context.
 */
int cace_amm_valprod_ctx_init(cace_amm_valprod_ctx_t *obj, const ari_formal_param_list_t fparams, const ari_t *ref);

void cace_amm_valprod_ctx_deinit(cace_amm_valprod_ctx_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_RUNCTX_H_ */
