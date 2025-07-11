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
#ifndef CACE_AMM_OBJ_ORG_H_
#define CACE_AMM_OBJ_ORG_H_

#include "obj_ns.h"
#include "idseg_val.h"
#include "cace/util/nocase.h"
#include <m-shared-ptr.h>
#include <m-rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct cace_amm_obj_org_list_t
 * Storage of all registered models
 */
/** @struct cace_amm_obj_org_by_enum_t
 * Index of models by their enumeration.
 */
/** @struct cace_amm_obj_org_by_name_t
 * Index of models by their name.
 */
/// @cond Doxygen_Suppress
M_SHARED_PTR_DEF(cace_amm_obj_ns_ptr, cace_amm_obj_ns_t)
M_DEQUE_DEF(cace_amm_obj_ns_list, cace_amm_obj_ns_ptr_t *,
            M_SHARED_PTR_OPLIST(cace_amm_obj_ns_ptr, M_OPL_cace_amm_obj_ns_t()))
// allow multiple revisions present but expect only one
M_BPTREE_MULTI_DEF2(cace_amm_obj_ns_by_enum, 4, int64_t, M_BASIC_OPLIST, cace_amm_obj_ns_t *, M_PTR_OPLIST)
M_BPTREE_MULTI_DEF2(cace_amm_obj_ns_by_name, 4, const char *, M_CSTR_NOCASE_OPLIST, cace_amm_obj_ns_t *, M_PTR_OPLIST)
/// @endcond

/** An organization as an index for model namespaces.
 */
typedef struct cace_amm_obj_org_s
{
    /// Organization ID
    cace_amm_idseg_val_t org_id;
    /// Index by enumeration
    cace_amm_obj_ns_by_enum_t ns_by_enum;
    /// Index by name
    cace_amm_obj_ns_by_name_t ns_by_name;

} cace_amm_obj_org_t;

void cace_amm_obj_org_init(cace_amm_obj_org_t *org);
void cace_amm_obj_org_deinit(cace_amm_obj_org_t *org);

/// M*LIB oplist for cace_amm_obj_org_t
#define M_OPL_cace_amm_obj_org_t() \
    (INIT(API_2(cace_amm_obj_org_init)), INIT_SET(0), CLEAR(API_2(cace_amm_obj_org_deinit)), SET(0))

cace_amm_obj_ns_t *cace_amm_obj_org_find_ns_name(const cace_amm_obj_org_t *org, const char *name);

cace_amm_obj_ns_t *cace_amm_obj_org_find_ns_enum(const cace_amm_obj_org_t *org, cace_ari_int_id_t intenum);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_ORG_H_ */
