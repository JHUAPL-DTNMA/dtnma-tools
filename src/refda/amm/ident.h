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
#ifndef REFDA_AMM_IDENT_H_
#define REFDA_AMM_IDENT_H_

#include "cace/amm/lookup.h"
#include "cace/amm/obj_store.h"
#include "cace/amm/user_data.h"
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Each base IDENT object of an intermediate or leaf object.
 */
typedef struct
{
    /// Reference to the object
    cace_ari_t name;
    /// Lookup result
    cace_amm_lookup_t deref;
    /** The bound object being used, which is bound based on #name.
     * This is always a reference to an externally-owned object.
     */
    const struct refda_amm_ident_desc_s *ident;
} refda_amm_ident_base_t;

void refda_amm_ident_base_init(refda_amm_ident_base_t *obj);

void refda_amm_ident_base_deinit(refda_amm_ident_base_t *obj);

/** Set the name and perform a reference lookup on this object.
 *
 * @param[in,out] obj The object to set.
 * @param[in] ref The new name.
 * @param[in] objs The object store to search.
 * @return Zero if the lookup was fully successful.
 */
int refda_amm_ident_base_populate(refda_amm_ident_base_t *obj, const cace_ari_t *ref, const cace_amm_obj_store_t *objs);

/// M*LIB OPLIST for refda_amm_ident_base_t
#define M_OPL_refda_amm_ident_base_t() \
    (INIT(API_2(refda_amm_ident_base_init)), INIT_SET(0), CLEAR(API_2(refda_amm_ident_base_deinit)), SET(0))

/** @struct refda_amm_ident_base_list_t
 * A list of dereferenced IDENT objects.
 */
/** @struct refda_amm_ident_base_ptr_set_t
 * A set of pointers to dereferenced IDENT objects.
 */
/// @cond Doxygen_Suppress
M_ARRAY_DEF(refda_amm_ident_base_list, refda_amm_ident_base_t)
M_RBTREE_DEF(refda_amm_ident_base_ptr_set, refda_amm_ident_base_t *, M_PTR_OPLIST)
/// @endcond

/** An IDENT descriptor.
 * This defines the properties of an IDENT in an Agent.
 */
typedef struct refda_amm_ident_desc_s
{
    /** All base IDENT objects for this object.
     * This list will not change during the lifetime of the IDENT.
     */
    refda_amm_ident_base_list_t bases;

    /** Optional ADM data associated with this object.
     */
    cace_amm_user_data_t user_data;

} refda_amm_ident_desc_t;

void refda_amm_ident_desc_init(refda_amm_ident_desc_t *obj);

void refda_amm_ident_desc_deinit(refda_amm_ident_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_IDENT_H_ */
