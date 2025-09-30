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
#ifndef CACE_AMM_OBJ_STORE_H_
#define CACE_AMM_OBJ_STORE_H_

#include "obj_org.h"
#include "idseg_ref.h"
#include "cace/util/nocase.h"
#include <m-shared-ptr.h>
#include <m-rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct cace_amm_obj_org_list_t
 * Storage of all registered organizations
 */
/** @struct cace_amm_obj_org_by_enum_t
 * Index of orgs by their enumeration.
 */
/** @struct cace_amm_obj_org_by_name_t
 * Index of orgs by their name.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_SHARED_PTR_DEF(cace_amm_obj_org_ptr, cace_amm_obj_org_t, M_OPL_cace_amm_obj_org_t())
M_DEQUE_DEF(cace_amm_obj_org_list, cace_amm_obj_org_ptr_t *,
            M_SHARED_PTR_OPLIST(cace_amm_obj_org_ptr, M_OPL_cace_amm_obj_org_t()))
M_BPTREE_DEF2(cace_amm_obj_org_by_enum, 4, cace_ari_int_id_t, M_BASIC_OPLIST, cace_amm_obj_org_t *, M_PTR_OPLIST)
M_BPTREE_DEF2(cace_amm_obj_org_by_name, 4, const char *, M_CSTR_NOCASE_OPLIST, cace_amm_obj_org_t *, M_PTR_OPLIST)
// GCOV_EXCL_STOP
/// @endcond

/** A container for AMM object descriptors within separate namespaces.
 */
typedef struct
{
    /// Actual storage for all registered NS
    cace_amm_obj_ns_list_t ns_list;

    /// Storage for organization index instances
    cace_amm_obj_org_list_t org_list;
    /// Index by enumeration
    cace_amm_obj_org_by_enum_t org_by_enum;
    /// Index by name
    cace_amm_obj_org_by_name_t org_by_name;
} cace_amm_obj_store_t;

void cace_amm_obj_store_init(cace_amm_obj_store_t *store);

void cace_amm_obj_store_deinit(cace_amm_obj_store_t *store);

/** Attempt to add a new namespace to the object store.
 *
 * @param[in,out] store The store to add to.
 * @param[in] org_id The organization ID.
 * @param[in] model_id The model ID within the organization.
 * @param[in] revision Optional specific revision of an ADM.
 */
cace_amm_obj_ns_t *cace_amm_obj_store_add_ns(cace_amm_obj_store_t *store, const cace_amm_idseg_ref_t org_id,
                                             const cace_amm_idseg_ref_t model_id, const char *revision);

/** Find existing namespace in the object store.
 *
 * @param[in] store The store to search.
 * @param[in] ref Reference to the org/model to search for.
 */
cace_amm_obj_ns_t *cace_amm_obj_store_find_ns(cace_amm_obj_store_t *store, const cace_ari_t *ref);

cace_amm_obj_org_t *cace_amm_obj_store_find_org_name(const cace_amm_obj_store_t *store, const char *name);

cace_amm_obj_org_t *cace_amm_obj_store_find_org_enum(const cace_amm_obj_store_t *store, cace_ari_int_id_t intenum);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_STORE_H_ */
