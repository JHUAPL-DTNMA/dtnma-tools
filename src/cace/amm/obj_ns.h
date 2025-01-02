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
#ifndef CACE_AMM_OBJ_NS_H_
#define CACE_AMM_OBJ_NS_H_

#include "obj_desc.h"
#include "cace/util/nocase.h"
#include <m-rbtree.h>
#include <m-shared-ptr.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct string_tree_set_t
 * Unique set of feature names in a single namespace.
 */
/** @struct cace_amm_obj_desc_list_t
 * Linked list of object descriptors of a single type within a namespace.
 * The list owns the object lifetimes and is indexed by pointers to
 * list members.
 */
/// @cond Doxygen_Suppress
M_RBTREE_DEF(string_tree_set, m_string_t)
M_SHARED_WEAK_PTR_DEF(cace_amm_obj_desc_ptr, cace_amm_obj_desc_t)
M_DEQUE_DEF(cace_amm_obj_desc_list, cace_amm_obj_desc_ptr_t *,
            M_SHARED_PTR_OPLIST(cace_amm_obj_desc_ptr, M_OPL_cace_amm_obj_desc_t()))
M_BPTREE_DEF2(cace_amm_obj_desc_by_enum, 4, int64_t, M_BASIC_OPLIST, cace_amm_obj_desc_t *, M_PTR_OPLIST)
M_BPTREE_DEF2(cace_amm_obj_desc_by_name, 4, const char *, M_CSTR_NOCASE_OPLIST, cace_amm_obj_desc_t *, M_PTR_OPLIST)
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

#define M_OPL_cace_amm_obj_ns_ctr_t() (INIT(API_2(cace_amm_obj_ns_ctr_init)), CLEAR(API_2(cace_amm_obj_ns_ctr_deinit)))

/** @struct cace_amm_obj_ns_ctr_dict_t
 * A mapping from ari_type_t integer enumeration to cace_amm_obj_ns_ctr_t
 * object containers.
 */
/// @cond Doxygen_Suppress
M_DICT_DEF2(cace_amm_obj_ns_ctr_dict, ari_type_t, M_OPL_ari_type_t(), cace_amm_obj_ns_ctr_t,
            M_OPL_cace_amm_obj_ns_ctr_t())
/// @endcond

typedef struct
{
    /// Indication of whether this namespace has an enumeration assigned
    bool has_enum;
    /// Optional integer enumeration for this namespace if #has_enum is true
    int64_t intenum;
    /// Mandatory name for this namespace
    m_string_t name;
    /// Revision being supported
    m_string_t revision;

    /// Features supported within this namespace
    string_tree_set_t feature_supp;

    /// AMM object descriptors organized by object type
    cace_amm_obj_ns_ctr_dict_t object_types;
} cace_amm_obj_ns_t;

void cace_amm_obj_ns_init(cace_amm_obj_ns_t *ns);

void cace_amm_obj_ns_deinit(cace_amm_obj_ns_t *ns);

/// Oplist to store namespaces in containers
#define M_OPL_cace_amm_obj_ns_t() (INIT(API_2(cace_amm_obj_ns_init)), CLEAR(API_2(cace_amm_obj_ns_deinit)))

typedef struct
{
    const char *name;
    bool        has_enum;
    int64_t     intenum;
} cace_amm_obj_id_t;

/** Construct an object ID with an integer enum.
 *
 * @param[in] name The object name, which must be non-null.
 * @param The object enumeration.
 * @return The full object ID.
 */
cace_amm_obj_id_t cace_amm_obj_id_withenum(const char *name, int64_t intenum);
/// @overload
cace_amm_obj_id_t cace_amm_obj_id_noenum(const char *name);

cace_amm_obj_desc_t *cace_amm_obj_ns_add_obj(cace_amm_obj_ns_t *ns, ari_type_t obj_type,
                                             const cace_amm_obj_id_t obj_id);

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_name(const cace_amm_obj_ns_t *ns, ari_type_t obj_type, const char *name);

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_enum(const cace_amm_obj_ns_t *ns, ari_type_t obj_type, int64_t intenum);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_NS_H_ */
