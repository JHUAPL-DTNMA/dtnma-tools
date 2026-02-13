/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "acl.h"
#include "endpoint.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_acl_group_init(refda_acl_group_t *obj)
{
    CHKVOID(obj);
    obj->id = 0;
    m_string_init(obj->name);
    refda_amm_ident_base_list_init(obj->member_pats);
    obj->added_at   = CACE_ARI_INIT_UNDEFINED;
    obj->updated_at = CACE_ARI_INIT_UNDEFINED;
}

void refda_acl_group_deinit(refda_acl_group_t *obj)
{
    CHKVOID(obj);
    refda_amm_ident_base_list_clear(obj->member_pats);
    m_string_clear(obj->name);
    obj->id = 0;
}

void refda_acl_access_init(refda_acl_access_t *obj)
{
    CHKVOID(obj);
    obj->id = 0;
    refda_acl_id_tree_init(obj->groups);
    refda_amm_ident_base_list_init(obj->permissions);
    obj->added_at   = CACE_ARI_INIT_UNDEFINED;
    obj->updated_at = CACE_ARI_INIT_UNDEFINED;
}

void refda_acl_access_deinit(refda_acl_access_t *obj)
{
    CHKVOID(obj);
    refda_amm_ident_base_list_clear(obj->permissions);
    refda_acl_id_tree_clear(obj->groups);
    obj->id = 0;
}

void refda_acl_init(refda_acl_t *obj)
{
    CHKVOID(obj);
    atomic_store(&obj->generation, 0);
    obj->perm_base    = NULL;
    obj->perm_produce = NULL;
    refda_acl_group_list_init(obj->groups);
    refda_acl_access_list_init(obj->access);
    refda_acl_access_by_group_init(obj->access_by_group);
}

void refda_acl_deinit(refda_acl_t *obj)
{
    CHKVOID(obj);
    refda_acl_access_by_group_clear(obj->access_by_group);
    refda_acl_access_list_clear(obj->access);
    refda_acl_group_list_clear(obj->groups);
    obj->perm_produce = NULL;
    obj->perm_base    = NULL;
}

int refda_acl_search_endpoint(refda_agent_t *agent, const cace_ari_t *endpoint, refda_acl_id_tree_t groups)
{
    CHKERR1(agent);
    CHKERR1(endpoint);
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, endpoint, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("searching groups for %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    refda_acl_id_tree_reset(groups);

    if (pthread_mutex_lock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to lock agent ACL");
        return 2;
    }

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, agent->acl.groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        const refda_acl_group_t *grp = refda_acl_group_list_cref(grp_it);

        refda_amm_ident_base_list_it_t pat_it;
        for (refda_amm_ident_base_list_it(pat_it, grp->member_pats); !refda_amm_ident_base_list_end_p(pat_it);
             refda_amm_ident_base_list_next(pat_it))
        {
            const refda_amm_ident_base_t *pat = refda_amm_ident_base_list_cref(pat_it);

            if (refda_endpoint_pat_match(agent, endpoint, pat))
            {
                refda_acl_id_tree_push(groups, grp->id);
            }
        }
    }

    if (pthread_mutex_unlock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock agent ACL");
        return 2;
    }

    if (cace_log_is_enabled_for(LOG_INFO))
    {
        m_string_t buf;
        m_string_init(buf);
        refda_acl_id_tree_get_str(buf, groups, false);
        CACE_LOG_INFO("matched to %zu groups: %s", refda_acl_id_tree_size(groups), m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    return 0;
}

bool refda_acl_search_permission(refda_agent_t *agent, const refda_acl_id_tree_t groups,
                                 const cace_amm_obj_desc_t *acc_obj, const cace_amm_obj_desc_ptr_set_t perm_objs,
                                 refda_amm_ident_base_ptr_set_t match)
{
    bool found = false;
    CACE_LOG_DEBUG("matched from %zu groups", refda_acl_id_tree_size(groups));
    if (pthread_mutex_lock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to lock agent ACL");
        return false;
    }

    refda_acl_id_tree_it_t grp_it;
    for (refda_acl_id_tree_it(grp_it, groups); !refda_acl_id_tree_end_p(grp_it); refda_acl_id_tree_next(grp_it))
    {
        const refda_acl_id_t *grp_id = refda_acl_id_tree_cref(grp_it);

        const refda_acl_access_ptr_set_t *accesses =
            refda_acl_access_by_group_cget(agent->acl.access_by_group, *grp_id);
        if (!accesses)
        {
            continue;
        }
        CACE_LOG_DEBUG("matched to %zu accesses", refda_acl_access_ptr_set_size(*accesses));

        refda_acl_access_ptr_set_it_t acc_it;
        for (refda_acl_access_ptr_set_it(acc_it, *accesses); !refda_acl_access_ptr_set_end_p(acc_it);
             refda_acl_access_ptr_set_next(acc_it))
        {
            refda_acl_access_t *const *acc_ptr = refda_acl_access_ptr_set_cref(acc_it);
            const refda_acl_access_t  *acc     = *acc_ptr;

            // TODO filter by acc_obj
            (void)acc_obj;

            refda_amm_ident_base_list_it_t perm_it;
            for (refda_amm_ident_base_list_it(perm_it, acc->permissions); !refda_amm_ident_base_list_end_p(perm_it);
                 refda_amm_ident_base_list_next(perm_it))
            {
                refda_amm_ident_base_t *perm = refda_amm_ident_base_list_ref(perm_it);

                // filter-in specific desired permission objects
                if (cace_amm_obj_desc_ptr_set_cget(perm_objs, perm->deref.obj))
                {
                    found = true;
                    refda_amm_ident_base_ptr_set_push(match, perm);
                }
            }
        }
    }

    if (pthread_mutex_unlock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock agent ACL");
        return false;
    }

    CACE_LOG_DEBUG("matched to %zu permissions", refda_amm_ident_base_ptr_set_size(match));
    return found;
}

bool refda_acl_search_one_permission(refda_agent_t *agent, const refda_acl_id_tree_t groups,
                                     const cace_amm_obj_desc_t *acc_obj, const cace_amm_obj_desc_t *perm_obj,
                                     refda_amm_ident_base_ptr_set_t match)
{
    cace_amm_obj_desc_ptr_set_t perm_objs;
    cace_amm_obj_desc_ptr_set_init(perm_objs);
    cace_amm_obj_desc_ptr_set_push(perm_objs, (cace_amm_obj_desc_t *)perm_obj);
    bool found = refda_acl_search_permission(agent, groups, acc_obj, perm_objs, match);
    cace_amm_obj_desc_ptr_set_clear(perm_objs);
    return found;
}
