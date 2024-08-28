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
#ifndef CACE_AMM_NAMESPACE_H_
#define CACE_AMM_NAMESPACE_H_

#include "obj_desc.h"
#include <m-rbtree.h>

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
RBTREE_DEF(string_tree_set, m_string_t)
DEQUE_DEF(cace_amm_obj_desc_list, cace_amm_obj_desc_t)
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
} cace_amm_obj_container_t;

void cace_amm_obj_container_init(cace_amm_obj_container_t *obj);

void cace_amm_obj_container_deinit(cace_amm_obj_container_t *obj);

#define M_OPL_cace_amm_obj_container_t() \
    (INIT(API_2(cace_amm_obj_container_init)), CLEAR(API_2(cace_amm_obj_container_deinit)))

/// @cond Doxygen_Suppress
M_DICT_DEF2(cace_amm_obj_container_dict, ari_type_t, M_BASIC_OPLIST, cace_amm_obj_container_t,
            M_OPL_cace_amm_obj_container_t())
/// @endcond

typedef struct
{
    /// Integer enumeration for this namespace
    int64_t intenum;
    /// Mandatory name for this namespace
    m_string_t name;

    /// Features supported within this namespace
    string_tree_set_t feature_supp;

    // AMM object descriptors follow
    cace_amm_obj_container_dict_t object_types;
} cace_amm_namespace_t;

void cace_amm_namespace_init(cace_amm_namespace_t *ns);

void cace_amm_namespace_deinit(cace_amm_namespace_t *ns);

cace_amm_obj_desc_t *cace_amm_namespace_add_obj(cace_amm_namespace_t *ns, ari_type_t obj_type, const char *name, int64_t intenum);

cace_amm_obj_desc_t *cace_amm_namespace_find_obj_name(const cace_amm_namespace_t *ns, ari_type_t obj_type,
                                                      const char *name);

cace_amm_obj_desc_t *cace_amm_namespace_find_obj_enum(const cace_amm_namespace_t *ns, ari_type_t obj_type,
                                                      int64_t intenum);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_NAMESPACE_H_ */
