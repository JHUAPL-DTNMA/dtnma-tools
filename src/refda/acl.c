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
#include "eval.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_acl_group_init(refda_acl_group_t *obj)
{
    CHKVOID(obj);
    obj->id = 0;
    m_string_init(obj->name);
    obj->member_filter = CACE_ARI_INIT_UNDEFINED;
    obj->added_at      = CACE_ARI_INIT_UNDEFINED;
    obj->updated_at    = CACE_ARI_INIT_UNDEFINED;
}

void refda_acl_group_deinit(refda_acl_group_t *obj)
{
    CHKVOID(obj);
    cace_ari_deinit(&obj->updated_at);
    cace_ari_deinit(&obj->added_at);
    cace_ari_deinit(&obj->member_filter);
    m_string_clear(obj->name);
    obj->id = 0;
}

void refda_acl_access_init(refda_acl_access_t *obj)
{
    CHKVOID(obj);
    obj->id = 0;
    refda_acl_id_tree_init(obj->groups);
    cace_ari_init(&obj->objects);
    refda_amm_ident_base_list_init(obj->permissions);
    obj->added_at   = CACE_ARI_INIT_UNDEFINED;
    obj->updated_at = CACE_ARI_INIT_UNDEFINED;
}

void refda_acl_access_deinit(refda_acl_access_t *obj)
{
    CHKVOID(obj);
    refda_amm_ident_base_list_clear(obj->permissions);
    cace_ari_deinit(&obj->objects);
    refda_acl_id_tree_clear(obj->groups);
    obj->id = 0;
}

void refda_acl_access_get_str_id(m_string_t out, const refda_acl_access_t *obj, bool append)
{
    if (!obj)
    {
        return;
    }
    if (append)
    {
        m_string_cat_printf(out, "%" PRIu32, obj->id);
    }
    else
    {
        m_string_printf(out, "%" PRIu32, obj->id);
    }
}

void refda_acl_init(refda_acl_t *obj)
{
    CHKVOID(obj);
    atomic_store(&obj->generation, 0);
    obj->permissions = (refda_acl_permissions_t) { NULL };
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
    obj->permissions = (refda_acl_permissions_t) { NULL };
}

/**
 * Translation helper function to substitute LABEL value 0 in a filter with
 * the endpoint identity.
 */
static cace_ari_translate_result_t acl_endpoint_filter_sub_label(cace_ari_t *out, const cace_ari_t *in,
                                                                 const cace_ari_translate_ctx_t *ctx)
{
    if (cace_ari_is_lit_typed(in, CACE_ARI_TYPE_LABEL))
    {
        const cace_ari_t *endpoint = ctx->user_data;

        cace_ari_int as_int;
        if (!cace_ari_get_int(in, &as_int))
        {
            if (as_int == 0)
            {
                cace_ari_set_copy(out, endpoint);
                return CACE_ARI_TRANSLATE_FINAL;
            }
            else
            {
                CACE_LOG_ERR("invalid LABEL value %d", as_int);
                return CACE_ARI_TRANSLATE_FAILURE;
            }
        }
        else
        {
            CACE_LOG_ERR("invalid LABEL primitive type");
            return CACE_ARI_TRANSLATE_FAILURE;
        }
    }
    return CACE_ARI_TRANSLATE_DEFAULT;
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

    refda_runctx_t runctx;
    refda_runctx_init(&runctx);
    refda_runctx_from(&runctx, agent, NULL);

    refda_acl_id_tree_reset(groups);
    const cace_ari_translator_t translator = { .map_ari = acl_endpoint_filter_sub_label };

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

        // Substitute endpoint value for LABEL items within filter EXPR
        cace_ari_t expr = CACE_ARI_INIT_UNDEFINED;
        {
            int res = cace_ari_translate(&expr, &grp->member_filter, &translator, (void *)endpoint);
            if (res)
            {
                CACE_LOG_ERR("Unable to translate filter, error %d", res);
                cace_ari_deinit(&expr); // No longer needed at this point
                continue;
            }
        }

        // Evaluate the filter EXPR
        refda_eval_ctx_t evalctx;
        refda_eval_ctx_init(&evalctx, &runctx);
        cace_ari_t eval_result = CACE_ARI_INIT_UNDEFINED;

        REFDA_AGENT_LOCK(agent, 2);
        int res = refda_eval_expand_expr(&evalctx, &expr);
        cace_ari_deinit(&expr); // No longer needed at this point
        REFDA_AGENT_UNLOCK(agent, 2);
        if (res)
        {
            CACE_LOG_ERR("failed to evaluate condition, error %d", res);
            refda_eval_ctx_deinit(&evalctx);
            continue;
        }

        res = refda_eval_reduce(&evalctx, &eval_result);
        refda_eval_ctx_deinit(&evalctx);
        if (res)
        {
            CACE_LOG_ERR("failed to evaluate condition, error %d", res);
            cace_ari_deinit(&eval_result);
            continue;
        }

        // True result indicates entry is purged
        if (cace_amm_ari_is_truthy(&eval_result))
        {
            refda_acl_id_tree_push(groups, grp->id);
        }
    }

    if (pthread_mutex_unlock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock agent ACL");
        return 2;
    }

    refda_runctx_deinit(&runctx);

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
                                 const cace_amm_lookup_t *acc_obj, const cace_amm_obj_desc_ptr_set_t perm_objs,
                                 refda_amm_ident_base_ptr_set_t match)
{
    CHKFALSE(agent);
    CHKFALSE(acc_obj);

    bool found = false;
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        refda_acl_id_tree_get_str(buf, groups, false);
        CACE_LOG_DEBUG("matched from %zu groups: %s", refda_acl_id_tree_size(groups), m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    if (pthread_mutex_lock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to lock agent ACL");
        return false;
    }

    refda_acl_id_tree_it_t grp_it;
    for (refda_acl_id_tree_it(grp_it, groups); !refda_acl_id_tree_end_p(grp_it); refda_acl_id_tree_next(grp_it))
    {
        const refda_acl_id_t *grp_id = refda_acl_id_tree_cref(grp_it);
        if (*grp_id == 0)
        {
            // Agent group 0 has all-access
            CACE_LOG_DEBUG("matched as Agent group 0, short circuit");
            found = true;
            break;
        }

        const refda_acl_access_ptr_set_t *accesses =
            refda_acl_access_by_group_cget(agent->acl.access_by_group, *grp_id);
        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            m_string_t buf;
            m_string_init(buf);
            size_t count = 0;
            if (accesses)
            {
                count = refda_acl_access_ptr_set_size(*accesses);
                refda_acl_access_ptr_set_get_str(buf, *accesses, false);
            }
            CACE_LOG_DEBUG("matched to %zu accesses: %s", count, m_string_get_cstr(buf));
            m_string_clear(buf);
        }
        if (!accesses)
        {
            continue;
        }

        refda_acl_access_ptr_set_it_t acc_it;
        for (refda_acl_access_ptr_set_it(acc_it, *accesses); !refda_acl_access_ptr_set_end_p(acc_it);
             refda_acl_access_ptr_set_next(acc_it))
        {
            refda_acl_access_t *const *acc_ptr = refda_acl_access_ptr_set_cref(acc_it);
            const refda_acl_access_t  *acc     = *acc_ptr;

            if (!cace_amm_objpat_set_match(&acc->objects, acc_obj))
            {
                continue;
            }

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

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        refda_amm_ident_base_ptr_set_get_str(buf, match, false);
        CACE_LOG_DEBUG("matched to %zu permissions: %s", refda_amm_ident_base_ptr_set_size(match),
                       m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    return found;
}

bool refda_acl_search_one_permission(refda_agent_t *agent, const refda_acl_id_tree_t groups,
                                     const cace_amm_lookup_t *acc_obj, const cace_amm_obj_desc_t *perm_obj,
                                     refda_amm_ident_base_ptr_set_t match)
{
    CHKFALSE(perm_obj);

    cace_amm_obj_desc_ptr_set_t perm_objs;
    cace_amm_obj_desc_ptr_set_init(perm_objs);
    cace_amm_obj_desc_ptr_set_push(perm_objs, (cace_amm_obj_desc_t *)perm_obj);
    bool found = refda_acl_search_permission(agent, groups, acc_obj, perm_objs, match);
    cace_amm_obj_desc_ptr_set_clear(perm_objs);
    return found;
}
