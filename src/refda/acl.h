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
#ifndef REFDA_ACL_H_
#define REFDA_ACL_H_

#include "amm/ident.h"
#include "cace/ari/base.h"
#include <m-deque.h>
#include <m-rbtree.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Identifier for ACL groups.
 * The group ID zero is reserved for the agent itself.
 */
typedef cace_ari_uint refda_acl_group_id_t;

/** A single entry of the ACL group table.
 *
 */
typedef struct
{
    /// Unique group ID
    refda_acl_group_id_t id;
    /// Unique group name
    m_string_t name;
    /// Endpoint patterns for members of this group
    refda_amm_ident_base_list_t member_pats;

} refda_acl_group_t;

void refda_acl_group_init(refda_acl_group_t *obj);

void refda_acl_group_deinit(refda_acl_group_t *obj);

#define M_OPL_refda_acl_group_t() \
    (INIT(API_2(refda_acl_group_init)), INIT_SET(0), CLEAR(API_2(refda_acl_group_deinit)), SET(0))

/** @struct refda_acl_group_list_t
 * An ordered list of ::refda_acl_group_t instances.
 */
/// @cond Doxygen_Suppress
M_ARRAY_DEF(refda_acl_group_list, refda_acl_group_t)
/// @endcond

/** @struct refda_acl_id_tree_t
 * An ordered list of unique IDs.
 */
/// @cond Doxygen_Suppress
M_RBTREE_DEF(refda_acl_id_tree, refda_acl_group_id_t, M_BASIC_OPLIST)
/// @endcond

/** A single entry of the ACL access table.
 *
 */
typedef struct
{
    /// Unique entry ID
    uint32_t id;
    /// Groups to which this entry applies
    refda_acl_id_tree_t groups;
    /// Permissions granted to these groups and objects
    refda_amm_ident_base_list_t permissions;

} refda_acl_access_t;

void refda_acl_access_init(refda_acl_access_t *obj);

void refda_acl_access_deinit(refda_acl_access_t *obj);

#define M_OPL_refda_acl_access_t() \
    (INIT(API_2(refda_acl_access_init)), INIT_SET(0), CLEAR(API_2(refda_acl_access_deinit)), SET(0))

/** @struct refda_acl_access_list_t
 * An ordered list of ::refda_acl_access_t instances.
 */
/// @cond Doxygen_Suppress
M_ARRAY_DEF(refda_acl_access_list, refda_acl_access_t)
/// @endcond

/** Storage of the agent ACL and its derived caches.
 */
typedef struct
{
    /** Base IDENT for permissions.
     */
    refda_amm_ident_desc_t *perm_base;

    /** All groups configured in the Agent.
     */
    refda_acl_group_list_t groups;

    /** All access configured in the Agent.
     */
    refda_acl_access_list_t access;

} refda_acl_t;

void refda_acl_init(refda_acl_t *obj);

void refda_acl_deinit(refda_acl_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_ACL_H_ */
