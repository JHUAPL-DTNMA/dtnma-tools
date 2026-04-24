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
    cace_ari_init(&obj->objects_filter);
    refda_amm_ident_base_list_init(obj->permissions);
    obj->added_at   = CACE_ARI_INIT_UNDEFINED;
    obj->updated_at = CACE_ARI_INIT_UNDEFINED;
}

void refda_acl_access_deinit(refda_acl_access_t *obj)
{
    CHKVOID(obj);
    refda_amm_ident_base_list_clear(obj->permissions);
    cace_ari_deinit(&obj->objects_filter);
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
    refda_acl_id_tree_reset(groups);

    refda_runctx_t runctx;
    refda_runctx_init(&runctx);
    refda_runctx_from(&runctx, agent, NULL);

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

        cace_ari_t eval_result = CACE_ARI_INIT_UNDEFINED;
        // Substitute endpoint value for </label/0> items within filter EXPR
        int res = refda_eval_filter(&runctx, &eval_result, &grp->member_filter, &translator, (void *)endpoint);
        if (res)
        {
            cace_ari_deinit(&eval_result); // No longer needed at this point
            continue;
        }

        // True result indicates entry is purged
        bool is_match = cace_amm_ari_is_truthy(&eval_result);
        cace_ari_deinit(&eval_result);
        if (is_match)
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

/**
 * Translation helper function to substitute LABEL value 0 in a filter with
 * the endpoint identity.
 */
static cace_ari_translate_result_t acl_target_filter_sub_label(cace_ari_t *out, const cace_ari_t *in,
                                                               const cace_ari_translate_ctx_t *ctx)
{
    if (cace_ari_is_lit_typed(in, CACE_ARI_TYPE_LABEL))
    {
        const cace_ari_t *target = ctx->user_data;

        cace_ari_int as_int;
        if (!cace_ari_get_int(in, &as_int))
        {
            if (as_int == 0)
            {
                cace_ari_set_copy(out, target);
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

bool refda_acl_search_permission(refda_agent_t *agent, const refda_acl_id_tree_t groups, const cace_ari_t *tgt_ref,
                                 const cace_amm_lookup_t *tgt_deref, const cace_amm_obj_desc_ptr_set_t perm_objs,
                                 refda_amm_ident_base_ptr_set_t *match)
{
    CHKFALSE(agent);
    CHKFALSE(tgt_ref);

    // group zero is special
    bool is_grp_zero = false;
    // otherwise count found
    size_t found = 0;

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        refda_acl_id_tree_get_str(buf, groups, false);
        CACE_LOG_DEBUG("matched from %zu groups: %s", refda_acl_id_tree_size(groups), m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    const cace_ari_translator_t translator = { .map_ari = acl_target_filter_sub_label };

    const cace_ari_t *target;
    // Prefer integer forms from actual object
    cace_ari_t from_deref = CACE_ARI_INIT_UNDEFINED;
    if (tgt_deref)
    {
        cace_amm_lookup_ref_int(&from_deref, tgt_deref);
        target = &from_deref;
    }
    else
    {
        target = tgt_ref;
    }
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, target, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("matching for target: %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    // evaluate as the agent
    refda_runctx_t runctx;
    refda_runctx_init(&runctx);
    refda_runctx_from(&runctx, agent, NULL);

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
            is_grp_zero = true;
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

            cace_ari_t eval_result = CACE_ARI_INIT_UNDEFINED;
            // Substitute target reference value for </label/0> items within filter EXPR
            int res = refda_eval_filter(&runctx, &eval_result, &acc->objects_filter, &translator, (void *)target);
            if (res)
            {
                cace_ari_deinit(&eval_result); // No longer needed at this point
                continue;
            }

            bool is_match = cace_amm_ari_is_truthy(&eval_result);
            cace_ari_deinit(&eval_result);
            if (!is_match)
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
                    found += 1;
                    if (match)
                    {
                        refda_amm_ident_base_ptr_set_push(*match, perm);
                    }
                }
            }
        }
    }

    if (pthread_mutex_unlock(&(agent->acl_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock agent ACL");
        return false;
    }

    refda_runctx_deinit(&runctx);
    cace_ari_deinit(&from_deref);

    if (!is_grp_zero && cace_log_is_enabled_for(LOG_DEBUG))
    {
        if (match)
        {
            m_string_t buf;
            m_string_init(buf);
            refda_amm_ident_base_ptr_set_get_str(buf, *match, false);
            CACE_LOG_DEBUG("matched to %zu permissions: %s", found, m_string_get_cstr(buf));
            m_string_clear(buf);
        }
        else
        {
            CACE_LOG_DEBUG("matched to %zu permissions", found);
        }
    }

    return is_grp_zero || (found > 0);
}

bool refda_acl_search_one_permission(refda_agent_t *agent, const refda_acl_id_tree_t groups, const cace_ari_t *tgt_ref,
                                     const cace_amm_lookup_t *tgt_deref, const cace_amm_obj_desc_t *perm_obj,
                                     refda_amm_ident_base_ptr_set_t *match)
{
    CHKFALSE(perm_obj);

    cace_amm_obj_desc_ptr_set_t perm_objs;
    cace_amm_obj_desc_ptr_set_init(perm_objs);
    cace_amm_obj_desc_ptr_set_push(perm_objs, (cace_amm_obj_desc_t *)perm_obj);
    bool found = refda_acl_search_permission(agent, groups, tgt_ref, tgt_deref, perm_objs, match);
    cace_amm_obj_desc_ptr_set_clear(perm_objs);
    if (!found)
    {
        CACE_LOG_ERR("Lack of permission for: %s", m_string_get_cstr(perm_obj->obj_id.name));
    }
    return found;
}
