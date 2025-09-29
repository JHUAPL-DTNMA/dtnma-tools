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
#include "acl.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_acl_group_init(refda_acl_group_t *obj)
{
    CHKVOID(obj);
    obj->id = 0;
    m_string_init(obj->name);
    refda_amm_ident_base_list_init(obj->member_pats);
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
    obj->perm_base = NULL;
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
    obj->perm_base = NULL;
}

int refda_acl_search_endpoint(const refda_acl_t *obj, const cace_ari_t *endpoint, refda_acl_id_tree_t groups)
{
    CHKERR1(obj);
    CHKERR1(endpoint);
    CACE_LOG_INFO("searching groups");

    refda_acl_id_tree_reset(groups);

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, obj->groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        const refda_acl_group_t *grp = refda_acl_group_list_cref(grp_it);

        refda_amm_ident_base_list_it_t pat_it;
        for (refda_amm_ident_base_list_it(pat_it, grp->member_pats); !refda_amm_ident_base_list_end_p(pat_it);
             refda_amm_ident_base_list_next(pat_it))
        {
            const refda_amm_ident_base_t *pat = refda_amm_ident_base_list_cref(pat_it);

            // TODO match pattern in actual parameter
            const cace_ari_ref_t *ref = cace_ari_cget_ref(&pat->name);
            if (!ref)
            {
                continue;
            }
            // ref->params

            refda_acl_id_tree_push(groups, grp->id);
        }
    }

    return 2;
}
