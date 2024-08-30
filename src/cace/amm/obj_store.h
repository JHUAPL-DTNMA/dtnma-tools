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
#ifndef CACE_AMM_OBJ_STORE_H_
#define CACE_AMM_OBJ_STORE_H_

#include "obj_ns.h"
#include "cace/util/nocase.h"
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
DEQUE_DEF(cace_amm_obj_ns_list, cace_amm_obj_ns_t)
M_BPTREE_DEF2(cace_amm_obj_ns_by_enum, 4, int64_t, M_BASIC_OPLIST, cace_amm_obj_ns_t *, M_PTR_OPLIST)
M_BPTREE_DEF2(cace_amm_obj_ns_by_name, 4, const char *, M_CSTR_NOCASE_OPLIST, cace_amm_obj_ns_t *, M_PTR_OPLIST)
/// @endcond

/** A container for AMM object descriptors within separate namespaces.
 */
typedef struct
{
    cace_amm_obj_ns_list_t    ns_list;
    cace_amm_obj_ns_by_enum_t ns_by_enum;
    cace_amm_obj_ns_by_name_t ns_by_name;
} cace_amm_obj_store_t;

void cace_amm_obj_store_init(cace_amm_obj_store_t *store);

void cace_amm_obj_store_deinit(cace_amm_obj_store_t *store);

cace_amm_obj_ns_t *cace_amm_obj_store_add_ns(cace_amm_obj_store_t *store, const char *name, bool has_enum,
                                             int64_t intenum);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_STORE_H_ */
