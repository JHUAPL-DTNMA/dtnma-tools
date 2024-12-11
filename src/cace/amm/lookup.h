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
    /// The found namespace, or a null pointer
    cace_amm_obj_ns_t *ns;
    /// The found object, or a null pointer
    cace_amm_obj_desc_t *obj;
    /// The found object type, if #obj is non-null
    ari_type_t obj_type;

    /** Actual parameters normalized for this object from the given parameters.
     * This is set if the #obj is non-null and parameter processing succeeded
     */
    cace_ari_itemized_t aparams;

} cace_amm_lookup_t;

void cace_amm_lookup_init(cace_amm_lookup_t *res);

void cace_amm_lookup_deinit(cace_amm_lookup_t *res);

/** Initializer with move semantics.
 */
void cace_amm_lookup_init_move(cace_amm_lookup_t *res, cace_amm_lookup_t *src);

/** Setter with move semantics.
 */
void cace_amm_lookup_set_move(cace_amm_lookup_t *res, cace_amm_lookup_t *src);

/** Perform a lookup into an object store.
 *
 * @param[in,out] res The lookup result to reset and populate.
 * @param[in] store The object store to lookup within.
 * @param[in] ref The reference to lookup from.
 * @return Zero if successful.
 * 1 if parameters are invalid (including an ARI that is not an object reference).
 * 2 if the ARI type is invalid.
 * 3 if the namespace ID does not resolve to a namespace.
 * 4 if the object ID does not resolve to an object.
 */
int cace_amm_lookup_deref(cace_amm_lookup_t *res, const cace_amm_obj_store_t *store, const ari_t *ref);

#define M_OPL_cace_amm_lookup_t()                                                    \
    (INIT(API_2(cace_amm_lookup_init)), INIT_MOVE(API_6(cace_amm_lookup_init_move)), \
     CLEAR(API_2(cace_amm_lookup_deinit)), MOVE(API_6(cace_amm_lookup_move)))

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_LOOKUP_H_ */
