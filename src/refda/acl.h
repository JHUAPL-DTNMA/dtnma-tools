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
#include <m-bptree.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Identifier for ACL groups and access items.
 * The group ID zero is reserved for the agent itself.
 */
typedef cace_ari_uint refda_acl_id_t;

/** A single entry of the ACL group table.
 *
 */
typedef struct
{
    /// Unique group ID
    refda_acl_id_t id;
    /// Unique group name
    m_string_t name;
    /// Endpoint patterns for members of this group
    refda_amm_ident_base_list_t member_pats;

    /// Added-at timestamp
    cace_ari_t added_at;
    /// Updated-at timestamp
    cace_ari_t updated_at;
} refda_acl_group_t;

void refda_acl_group_init(refda_acl_group_t *obj);

void refda_acl_group_deinit(refda_acl_group_t *obj);

#define M_OPL_refda_acl_group_t() (INIT(API_2(refda_acl_group_init)), CLEAR(API_2(refda_acl_group_deinit)))

/** @struct refda_acl_group_list_t
 * An ordered list of ::refda_acl_group_t instances.
 */
/// @cond Doxygen_Suppress
M_DEQUE_DEF(refda_acl_group_list, refda_acl_group_t)
/// @endcond

/** @struct refda_acl_id_tree_t
 * An ordered list of unique IDs.
 */
/// @cond Doxygen_Suppress
M_RBTREE_DEF(refda_acl_id_tree, refda_acl_id_t, M_BASIC_OPLIST)
/// @endcond

/** A single entry of the ACL access table.
 *
 */
typedef struct
{
    /// Unique entry ID
    refda_acl_id_t id;
    /// Groups to which this entry applies
    refda_acl_id_tree_t groups;
    /// Permissions granted to these groups and objects
    refda_amm_ident_base_list_t permissions;

    /// Added-at timestamp
    cace_ari_t added_at;
    /// Updated-at timestamp
    cace_ari_t updated_at;
} refda_acl_access_t;

void refda_acl_access_init(refda_acl_access_t *obj);

void refda_acl_access_deinit(refda_acl_access_t *obj);

#define M_OPL_refda_acl_access_t() (INIT(API_2(refda_acl_access_init)), CLEAR(API_2(refda_acl_access_deinit)))

/** @struct refda_acl_access_list_t
 * An ordered list of ::refda_acl_access_t instances.
 */
/** @struct refda_acl_access_by_group_t
 * A multi-map from group ID to ::refda_acl_access_t pointers for fast lookup.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_DEQUE_DEF(refda_acl_access_list, refda_acl_access_t)
M_RBTREE_DEF(refda_acl_access_ptr_set, refda_acl_access_t *, M_PTR_OPLIST)
M_BPTREE_DEF2(refda_acl_access_by_group, 4, refda_acl_id_t, M_BASIC_OPLIST, refda_acl_access_ptr_set_t,
              M_RBTREE_OPLIST(refda_acl_access_ptr_set, M_PTR_OPLIST))
// GCOV_EXCL_STOP
/// @endcond

/** Storage of the agent ACL and its derived caches.
 */
typedef struct
{
    /** The generation of this ACL data for internal caching.
     * Any update to other members of this ACL will bump the generation
     * and invalidate any derived-and-cached group, access, or permission data.
     * This value is internal and runtime dependent, so not persisted across
     * agent processes.
     */
    uint64_t generation;

    /** Base IDENT for permissions.
     */
    refda_amm_ident_desc_t *perm_base;

    /** All groups configured in the Agent.
     */
    refda_acl_group_list_t groups;

    /** All access configured in the Agent.
     */
    refda_acl_access_list_t access;

    /** Lookup from group ID to accesses.
     * This is kept in sync with #groups and #access
     */
    refda_acl_access_by_group_t access_by_group;

} refda_acl_t;

void refda_acl_init(refda_acl_t *obj);

void refda_acl_deinit(refda_acl_t *obj);

typedef struct refda_agent_s refda_agent_t;

/** Search in an ACL for a specific endpoint.
 *
 * @param[in] agent The agent state.
 * @param[in] endpoint The endpoint to search for.
 * @param[out] groups The set of groups to add to.
 * @return Zero if successful, which may result in empty groups.
 */
int refda_acl_search_endpoint(const refda_agent_t *agent, const cace_ari_t *endpoint, refda_acl_id_tree_t groups);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_ACL_H_ */
