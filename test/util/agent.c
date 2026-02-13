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
#include "agent.h"
#include <refda/adm/ietf.h>
#include <refda/adm/ietf_amm.h>
#include <refda/adm/ietf_amm_base.h>
#include <refda/adm/ietf_amm_semtype.h>
#include <refda/adm/ietf_network_base.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/adm/ietf_dtnma_agent_acl.h>
#include <refda/agent.h>
#include <cace/ari/time_util.h>
#include <assert.h>

void test_util_agent_crit_adms(refda_agent_t *agent)
{
    assert(0 == refda_adm_ietf_amm_init(agent));
    assert(0 == refda_adm_ietf_amm_base_init(agent));
    assert(0 == refda_adm_ietf_amm_semtype_init(agent));
    assert(0 == refda_adm_ietf_network_base_init(agent));
    assert(0 == refda_adm_ietf_dtnma_agent_init(agent));
    assert(0 == refda_adm_ietf_dtnma_agent_acl_init(agent));
}

void test_util_agent_permission(refda_agent_t *agent, cace_ari_int_id_t obj_id)
{
    atomic_fetch_add(&agent->acl.generation, 1);

    refda_acl_access_t *access = refda_acl_access_list_push_back_new(agent->acl.access);
    cace_get_system_time(&access->added_at);
    access->id = refda_acl_access_list_size(agent->acl.access);
    refda_acl_id_tree_push(access->groups, 0);

    {
        cace_ari_t ref = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_objref_path_intid(&ref, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_ADM,
                                       CACE_ARI_TYPE_IDENT, obj_id);

        refda_amm_ident_base_t *perm = refda_amm_ident_base_list_push_new(access->permissions);
        if (refda_amm_ident_base_populate(perm, &ref, &agent->objs))
        {
            static_assert(true, "no permission found");
        }
        cace_ari_deinit(&ref);
    }

    refda_acl_access_ptr_set_t *set = refda_acl_access_by_group_safe_get(agent->acl.access_by_group, 0);
    refda_acl_access_ptr_set_push(*set, access);
}

void test_util_group_add(refda_agent_t *agent, refda_acl_id_t group_id, const char *uri_pattern)
{
    atomic_fetch_add(&agent->acl.generation, 1);

    refda_acl_group_t *grp = refda_acl_group_list_push_back_new(agent->acl.groups);
    cace_get_system_time(&grp->added_at);
    grp->id = refda_acl_group_list_size(agent->acl.groups);

    m_string_printf(grp->name, "%" PRIu32, group_id);

    {
        cace_ari_t      pat_name = CACE_ARI_INIT_UNDEFINED;
        cace_ari_ref_t *ref      = cace_ari_set_objref_path_intid(
            &pat_name, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_NETWORK_BASE_ENUM_ADM, CACE_ARI_TYPE_IDENT,
            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_URI_REGEXP_PATTERN);

        cace_ari_list_t params;
        cace_ari_list_init(params);
        {
            cace_ari_t *param = cace_ari_list_push_back_new(params);
            cace_ari_set_tstr(param, uri_pattern, true);
        }
        cace_ari_params_set_ac(&(ref->params), params);

        refda_amm_ident_base_t *pat = refda_amm_ident_base_list_push_new(grp->member_pats);
        if (refda_amm_ident_base_populate(pat, &pat_name, &agent->objs))
        {
            // CACE_LOG_CRIT("no uri_regexp_pattern found");
            assert(false);
        }
        cace_ari_deinit(&pat_name);
    }
}

void test_util_group_permission(refda_agent_t *agent, refda_acl_id_t group_id, cace_ari_int_id_t obj_id)
{
    atomic_fetch_add(&agent->acl.generation, 1);

    refda_acl_access_t *access = refda_acl_access_list_push_back_new(agent->acl.access);
    cace_get_system_time(&access->added_at);
    access->id = refda_acl_access_list_size(agent->acl.access);

    refda_acl_id_tree_push(access->groups, group_id);

    {
        cace_ari_t perm_name = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_objref_path_intid(&perm_name, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_ADM,
                                       CACE_ARI_TYPE_IDENT, obj_id);

        refda_amm_ident_base_t *perm = refda_amm_ident_base_list_push_new(access->permissions);
        if (refda_amm_ident_base_populate(perm, &perm_name, &agent->objs))
        {
            // CACE_LOG_CRIT("no permission found");
            assert(false);
        }
        cace_ari_deinit(&perm_name);
    }

    refda_acl_access_ptr_set_t *set = refda_acl_access_by_group_safe_get(agent->acl.access_by_group, group_id);
    refda_acl_access_ptr_set_push(*set, access);
}
