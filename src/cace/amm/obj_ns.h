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
#ifndef CACE_AMM_OBJ_NS_H_
#define CACE_AMM_OBJ_NS_H_

#include "obj_desc.h"
#include "idseg_val.h"
#include "idseg_ref.h"
#include "cace/ari/ref.h"
#include "cace/util/defs.h"
#include "cace/util/nocase.h"
#include <m-rbtree.h>
#include <m-shared-ptr.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct string_tree_set_t
 * Unique set of feature names in a single namespace.
 */
/** @struct cace_amm_obj_desc_ptr_t
 * A non-thread-safe shared pointer to a ::cace_amm_obj_desc_t instance.
 */
/** @struct cace_amm_obj_desc_list_t
 * Linked list of object descriptors of a single type within a namespace.
 * The list owns the object lifetimes and is indexed by pointers to
 * list members of ::cace_amm_obj_desc_t.
 */
/** @struct cace_amm_obj_desc_ptr_set_t
 * Unique set of object descriptor pointers to ::cace_amm_obj_desc_t.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_RBTREE_DEF(string_tree_set, m_string_t)
M_SHARED_WEAK_PTR_DEF(cace_amm_obj_desc_ptr, cace_amm_obj_desc_t)
M_DEQUE_DEF(cace_amm_obj_desc_list, cace_amm_obj_desc_ptr_t *,
            M_SHARED_PTR_OPLIST(cace_amm_obj_desc_ptr, M_OPL_cace_amm_obj_desc_t()))
M_RBTREE_DEF(cace_amm_obj_desc_ptr_set, cace_amm_obj_desc_t *, M_PTR_OPLIST)
M_BPTREE_DEF2(cace_amm_obj_desc_by_enum, 4, int64_t, M_BASIC_OPLIST, cace_amm_obj_desc_t *, M_PTR_OPLIST)
M_BPTREE_DEF2(cace_amm_obj_desc_by_name, 4, const char *, M_CSTR_NOCASE_OPLIST, cace_amm_obj_desc_t *, M_PTR_OPLIST)
// GCOV_EXCL_STOP
/// @endcond

/** A container of object descriptors all of a single type within a namespace.
 */
typedef struct
{
    cace_amm_obj_desc_list_t    obj_list;
    cace_amm_obj_desc_by_enum_t obj_by_enum;
    cace_amm_obj_desc_by_name_t obj_by_name;
} cace_amm_obj_ns_ctr_t;

void cace_amm_obj_ns_ctr_init(cace_amm_obj_ns_ctr_t *obj);

void cace_amm_obj_ns_ctr_deinit(cace_amm_obj_ns_ctr_t *obj);

#define M_OPL_cace_amm_obj_ns_ctr_t() \
    (INIT(API_2(cace_amm_obj_ns_ctr_init)), INIT_SET(0), CLEAR(API_2(cace_amm_obj_ns_ctr_deinit)), SET(0))

/** @struct cace_amm_obj_ns_ctr_dict_t
 * A mapping from ari_type_t integer enumeration to cace_amm_obj_ns_ctr_t
 * object containers.
 */
/// @cond Doxygen_Suppress
M_SHARED_WEAK_PTR_DEF(cace_amm_obj_ns_ctr_ptr, cace_amm_obj_ns_ctr_t, M_OPL_cace_amm_obj_ns_ctr_t())
M_DICT_DEF2(cace_amm_obj_ns_ctr_dict, cace_ari_type_t, M_OPL_cace_ari_type_t(), cace_amm_obj_ns_ctr_ptr_t *,
            M_SHARED_PTR_OPLIST(cace_amm_obj_ns_ctr_ptr, M_OPL_cace_amm_obj_ns_ctr_t()))
/// @endcond

typedef struct cace_amm_obj_org_s cace_amm_obj_org_t;

/** Storage of a namespace, its identifiers, and its contained objects.
 */
typedef struct
{
    /// Parent organization ID (non-authoritative but necessary for bookkeeping)
    cace_amm_idseg_val_t org_id;
    /// Model ID, which must have a text name
    cace_amm_idseg_val_t model_id;
    /// Revision being supported, which must be valid
    cace_ari_date_t model_rev;

    /// Features supported within this namespace
    string_tree_set_t feature_supp;

    /// AMM object descriptors organized by object type
    cace_amm_obj_ns_ctr_dict_t object_types;

    /// The status of this NS, which can change over time
    cace_amm_status_t status;

} cace_amm_obj_ns_t;

void cace_amm_obj_ns_init(cace_amm_obj_ns_t *ns);

void cace_amm_obj_ns_deinit(cace_amm_obj_ns_t *ns);

/// M*LIB oplist for cace_amm_obj_ns_t
#define M_OPL_cace_amm_obj_ns_t() \
    (INIT(API_2(cace_amm_obj_ns_init)), INIT_SET(0), CLEAR(API_2(cace_amm_obj_ns_deinit)), SET(0))

cace_amm_obj_desc_t *cace_amm_obj_ns_add_obj(cace_amm_obj_ns_t *ns, cace_ari_type_t obj_type,
                                             const cace_amm_idseg_ref_t obj_id);

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_name(const cace_amm_obj_ns_t *ns, cace_ari_type_t obj_type,
                                                   const char *name);

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_enum(const cace_amm_obj_ns_t *ns, cace_ari_type_t obj_type,
                                                   cace_ari_int_id_t intenum);

/**
 * Determine if the object namespace has specific enumerations.
 *
 * @param[in] The namespace to inspect.
 * @param org_id The organization enumeration.
 * @param model_id The model enumeration.
 * @return True if the namespace has the given identifiers.
 */
bool cace_amm_obj_ns_is_match(const cace_amm_obj_ns_t *ns, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id);

/**
 * Determine if the object namespace is an ODM.
 *
 * @param[in] The namespace to inspect.
 * @return True if the namespace is an ODM, false otherwise.
 */
bool cace_amm_obj_ns_is_odm(const cace_amm_obj_ns_t *ns);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_NS_H_ */
