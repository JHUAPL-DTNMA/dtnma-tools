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
#ifndef CACE_AMM_OBJ_REF_H_
#define CACE_AMM_OBJ_REF_H_

#include "obj_desc.h"
#include "cace/ari.h"
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A generic object reference common to AMM object types in an Agent.
 * This contains the original reference ARI as well as a possible
 * dereferenced object pointer.
 */
typedef struct cace_amm_obj_ref_s
{
    /** The decoded reference value.
     * The reference may or may not be valid, and depends on a lookup.
     */
    cace_ari_t ref;

    /** The specific dereferenced object after a lookup.
     * This may be null even after the lookup if the reference is not valid.
     */
    const struct cace_amm_obj_desc_s *obj;

#ifdef __cplusplus
    cace_amm_obj_ref_s(const cace_amm_obj_ref_s &)            = delete;
    cace_amm_obj_ref_s &operator=(const cace_amm_obj_ref_s &) = delete;
#endif

} cace_amm_obj_ref_t;

void cace_amm_obj_ref_init(cace_amm_obj_ref_t *obj);

void cace_amm_obj_ref_init_set(cace_amm_obj_ref_t *obj, const cace_amm_obj_ref_t *src);

void cace_amm_obj_ref_deinit(cace_amm_obj_ref_t *obj);

void cace_amm_obj_ref_set(cace_amm_obj_ref_t *obj, const cace_amm_obj_ref_t *src);

#define M_OPL_cace_amm_obj_ref_t()                                                                                     \
    (INIT(API_2(cace_amm_obj_ref_init)), INIT_SET(API_6(cace_amm_obj_ref_init_set)), SET(API_6(cace_amm_obj_ref_set)), \
     CLEAR(API_2(cace_amm_obj_ref_deinit)))

/** @struct refda_amm_obj_ref_list
 * A list of object references and lookup results.
 */
/// @cond Doxygen_Suppress
M_DEQUE_DEF(cace_amm_obj_ref_list, cace_amm_obj_ref_t)
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_REF_H_ */
