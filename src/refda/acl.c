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
#if defined(PCRE_FOUND)
#include <pcre2.h>
#endif /* PCRE_FOUND */

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
    obj->generation = 0;
    obj->perm_base  = NULL;
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

/// Match pattern for specific IDENT leaves
static bool refda_acl_search_endpoint_ref(const refda_acl_t *acl _U_, const cace_ari_t *endpoint, const refda_amm_ident_base_t *pat)
{
    if (!pat || !(pat->deref.ns) || !(pat->deref.obj) || (pat->deref.obj_type != CACE_ARI_TYPE_IDENT))
    {
        return false;
    }

    if ((pat->deref.ns->org_id.intenum == 1) && (pat->deref.ns->model_id.intenum == 26) && (pat->deref.obj->obj_id.intenum == 2))
    {
        // Logic for //ietf/network-base/ident/uri-regexp-pattern
        const char *pattern = cace_ari_cget_tstr_cstr(cace_ari_array_cget(pat->deref.aparams.ordered, 0));
        const char *value = cace_ari_cget_tstr_cstr(endpoint);
        if (pattern && value)
        {
            bool is_match = false;
#if defined(PCRE_FOUND)
            const int   opts        = PCRE2_ANCHORED | PCRE2_ENDANCHORED;
            int         errorcode   = 0;
            PCRE2_SIZE  erroroffset = 0;
            pcre2_code *cfg         = pcre2_compile((PCRE2_SPTR8)pattern, strlen(pattern), opts, &errorcode, &erroroffset, NULL);
            if (!cfg)
            {
                CACE_LOG_ERR("Failed to compile regex pattern (error %d at %z): %s", errorcode, erroroffset, pattern);
            }
            else
            {
                pcre2_match_data *md   = pcre2_match_data_create_from_pattern(cfg, NULL);
                const int         opts = 0;
                // ignore terminating null
                int res = pcre2_match(cfg, (PCRE2_SPTR8)value, strlen(value), 0, opts, md, NULL);
                CACE_LOG_DEBUG("Matching pattern %s with value %s, result %d", pattern, value, res);
                if (res > 0)
                {
                    is_match = true;
                }
                pcre2_match_data_free(md);

                pcre2_code_free(cfg);
            }
#else  /* PCRE_FOUND */
            CACE_AMM_ERR("Cannot evaluate uri-regexp-pattern without PCRE")
#endif /* PCRE_FOUND */
            if (is_match)
            {
                return true;
            }
        }
    }
    return false;
}

int refda_acl_search_endpoint(const refda_acl_t *acl, const cace_ari_t *endpoint, refda_acl_id_tree_t groups)
{
    CHKERR1(acl);
    CHKERR1(endpoint);
    CACE_LOG_INFO("searching groups");

    refda_acl_id_tree_reset(groups);

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, acl->groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        const refda_acl_group_t *grp = refda_acl_group_list_cref(grp_it);

        refda_amm_ident_base_list_it_t pat_it;
        for (refda_amm_ident_base_list_it(pat_it, grp->member_pats); !refda_amm_ident_base_list_end_p(pat_it);
             refda_amm_ident_base_list_next(pat_it))
        {
            const refda_amm_ident_base_t *pat = refda_amm_ident_base_list_cref(pat_it);

            if (refda_acl_search_endpoint_ref(acl, endpoint, pat))
            {
                refda_acl_id_tree_push(groups, grp->id);
            }
        }
    }

    return 2;
}
