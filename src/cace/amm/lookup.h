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
#ifndef CACE_AMM_LOOKUP_H_
#define CACE_AMM_LOOKUP_H_

#include "obj_ns.h"
#include "obj_desc.h"
#include "obj_store.h"
#include "parameters.h"
#include "cace/ari.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Result of dereferencing an object-reference ARI to an object
 * descriptor within a specific namespace.
 */
typedef struct
{
    /** Original ARI with any given parameters.
     * All path segments are in their original form.
     */
    const ari_t *ref;

    /// The found namespace, or a null pointer
    cace_amm_obj_ns_t *ns;
    /// The found object, or a null pointer
    cace_amm_obj_desc_t *obj;

    /** Actual parameters normalized for this object from the given parameters.
     * This is set if the #obj is non-null and parameter processing succeeded
     */
    cace_amm_actual_param_set_t aparams;

} cace_amm_lookup_t;

void cace_amm_lookup_init(cace_amm_lookup_t *res);

void cace_amm_lookup_deinit(cace_amm_lookup_t *res);

/** Perform a lookup into an object store.
 *
 * @return Zero if successful,
 * 1 if parameters are invalid (including an ARI that is not an object reference).
 */
int cace_amm_lookup_deref(cace_amm_lookup_t *res, const cace_amm_obj_store_t *store, const ari_t *ref);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_LOOKUP_H_ */
