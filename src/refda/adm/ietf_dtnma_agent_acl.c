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

/** @file
 * This is the compilation unit for the implementation of the
 * ADM module "ietf-dtnma-agent-acl" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_dtnma_agent_acl.h"
#include "refda/agent.h"
#include "refda/register.h"
#include "refda/edd_prod_ctx.h"
#include "refda/ctrl_exec_ctx.h"
#include "refda/oper_eval_ctx.h"
#include <cace/amm/semtype.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

/*   START CUSTOM FUNCTIONS HERE */
#define AGENT_ACL_LOCK(agent)                      \
    if (pthread_mutex_lock(&((agent)->acl_mutex))) \
    {                                              \
        CACE_LOG_CRIT("failed to lock agent ACL"); \
        return;                                    \
    }

#define AGENT_ACL_UNLOCK(agent)                      \
    if (pthread_mutex_unlock(&((agent)->acl_mutex))) \
    {                                                \
        CACE_LOG_CRIT("failed to unlock agent ACL"); \
        return;                                      \
    }

/*   STOP CUSTOM FUNCTIONS HERE  */

/* Name: access-list
 * Description:
 *   List the current group--object access.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 4 columns:
 *   - Index 0, name "access-id", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 1, name "group-ids", type ulist of use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 2, name "objects", type use of ari://ietf/amm-base/TYPEDEF/ari-pattern
 *   - Index 3, name "permissions", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/permission-list
 */
static void refda_adm_ietf_dtnma_agent_acl_edd_access_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_edd_access_list BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent = ctx->prodctx->parent->agent;
    AGENT_ACL_LOCK(agent)

    cace_ari_t      result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_tbl_t *table  = cace_ari_set_tbl(&result, NULL);
    cace_ari_tbl_reset(table, 4, 0);

    refda_acl_access_list_it_t acc_it;
    for (refda_acl_access_list_it(acc_it, agent->acl.access); !refda_acl_access_list_end_p(acc_it);
         refda_acl_access_list_next(acc_it))
    {
        const refda_acl_access_t *acc = refda_acl_access_list_cref(acc_it);

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, 4);

        cace_ari_set_uint(cace_ari_array_get(row, 0), acc->id);
        {
            cace_ari_ac_t *grps_ac = cace_ari_set_ac(cace_ari_array_get(row, 1), NULL);

            refda_acl_id_tree_it_t grpid_it;
            for (refda_acl_id_tree_it(grpid_it, acc->groups); !refda_acl_id_tree_end_p(grpid_it);
                 refda_acl_id_tree_next(grpid_it))
            {
                const refda_acl_id_t *grpid = refda_acl_id_tree_cref(grpid_it);
                // arbitrary order
                cace_ari_set_uint(cace_ari_list_push_back_new(grps_ac->items), *grpid);
            }
        }
        {
            cace_ari_ac_t grps;
            cace_ari_ac_init(&grps);
            cace_ari_set_ac(cace_ari_array_get(row, 3), &grps);
        }

        // append the row
        cace_ari_tbl_move_row_array(table, row);
    }

    AGENT_ACL_UNLOCK(agent)
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_edd_access_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: current-group-id
 * Description:
 *   Get the group ID for the current execution context, which may be the
 *   implicit Agent group ID zero.
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 */
static void refda_adm_ietf_dtnma_agent_acl_edd_current_group_id(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_edd_current_group_id BODY
     * +-------------------------------------------------------------------------+
     */
    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_uint(&result, ctx->prodctx->parent->acl_group_id);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_edd_current_group_id BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: group-list
 * Description:
 *   Get the defined access control groups.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 3 columns:
 *   - Index 0, name "group-id", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 1, name "name", type use of ari:/ARITYPE/TEXTSTR
 *   - Index 2, name "members", type ulist of use of ari://ietf/network-base/TYPEDEF/endpoint-pattern
 */
static void refda_adm_ietf_dtnma_agent_acl_edd_group_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_edd_group_list BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent = ctx->prodctx->parent->agent;
    AGENT_ACL_LOCK(agent)

    cace_ari_t      result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_tbl_t *table  = cace_ari_set_tbl(&result, NULL);
    cace_ari_tbl_reset(table, 3, 0);

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, agent->acl.groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        const refda_acl_group_t *grp = refda_acl_group_list_cref(grp_it);

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, 3);

        cace_ari_set_uint(cace_ari_array_get(row, 0), grp->id);
        cace_ari_set_tstr(cace_ari_array_get(row, 1), m_string_get_cstr(grp->name), true);
        {
            cace_ari_ac_t *memb_ac = cace_ari_set_ac(cace_ari_array_get(row, 2), NULL);

            refda_amm_ident_base_list_it_t memb_it;
            for (refda_amm_ident_base_list_it(memb_it, grp->member_pats); !refda_amm_ident_base_list_end_p(memb_it);
                 refda_amm_ident_base_list_next(memb_it))
            {
                const refda_amm_ident_base_t *memb = refda_amm_ident_base_list_cref(memb_it);

                cace_ari_list_push_back(memb_ac->items, memb->name);
            }
        }

        // append the row
        cace_ari_tbl_move_row_array(table, row);
    }

    AGENT_ACL_UNLOCK(agent)
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_edd_group_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-access
 * Description:
 *   Ensures that an access control permission is set for the given group
 *   and ARI pattern.
 *
 * Parameters list:
 *   - Index 0, name "access-id", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 1, name "group-ids", type ulist of use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 2, name "objects", type use of ari://ietf/amm-base/TYPEDEF/ari-pattern
 *   - Index 3, name "permissions", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/permission-list
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_access(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_access BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_aid   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *p_gid   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);
    const cace_ari_t *p_perms = refda_ctrl_exec_ctx_get_aparam_index(ctx, 3);

    cace_ari_uint aid;
    if (cace_ari_get_uint(p_aid, &aid))
    {
        CACE_LOG_ERR("Invalid access-id parameter");
        return;
    }
    const cace_ari_ac_t *gid_ac = cace_ari_cget_ac(p_gid);
    if (!gid_ac)
    {
        CACE_LOG_ERR("Invalid group-ids parameter");
        return;
    }
    const cace_ari_ac_t *perms_ac = cace_ari_cget_ac(p_perms);
    if (!perms_ac)
    {
        CACE_LOG_ERR("Invalid permissions parameter");
        return;
    }

    refda_agent_t *agent = ctx->runctx->agent;
    AGENT_ACL_LOCK(agent)

    refda_acl_access_t *found_id = NULL;

    refda_acl_access_list_it_t acc_it;
    for (refda_acl_access_list_it(acc_it, agent->acl.access); !refda_acl_access_list_end_p(acc_it);
         refda_acl_access_list_next(acc_it))
    {
        refda_acl_access_t *acc = refda_acl_access_list_ref(acc_it);

        if (acc->id == aid)
        {
            found_id = acc;
            break;
        }
    }

    if (found_id)
    {}
    else
    {
        // new item
        found_id     = refda_acl_access_list_push_back_new(agent->acl.access);
        found_id->id = aid;
    }

    refda_acl_id_tree_reset(found_id->groups);
    cace_ari_list_it_t gid_it;
    for (cace_ari_list_it(gid_it, gid_ac->items); !cace_ari_list_end_p(gid_it); cace_ari_list_next(gid_it))
    {
        const cace_ari_t *gid_val = cace_ari_list_cref(gid_it);

        refda_acl_id_t gid;
        if (!cace_ari_get_uint(gid_val, &gid))
        {
            continue;
        }

        refda_acl_id_tree_push(found_id->groups, gid);
    }

    refda_amm_ident_base_list_reset(found_id->permissions);
    cace_ari_list_it_t perm_it;
    for (cace_ari_list_it(perm_it, perms_ac->items); !cace_ari_list_end_p(perm_it); cace_ari_list_next(perm_it))
    {
        const cace_ari_t *perm_ref = cace_ari_list_cref(perm_it);

        refda_amm_ident_base_t *perm = refda_amm_ident_base_list_push_new(found_id->permissions);
        refda_amm_ident_base_populate(perm, perm_ref, &agent->objs);
    }

    refda_ctrl_exec_ctx_set_result_null(ctx);

    AGENT_ACL_UNLOCK(agent)
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_access BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: discard-access
 * Description:
 *   Discard any permissions for the given group and ARI pattern.
 *
 * Parameters list:
 *   - Index 0, name "group", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/optional-entry-id
 *   - Index 1, name "objects", type use of ari://ietf/amm-base/TYPEDEF/ari-pattern
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_acl_ctrl_discard_access(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_discard_access BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_aid   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    cace_ari_uint aid;
    if (cace_ari_get_uint(p_aid, &aid))
    {
        CACE_LOG_ERR("Invalid access-id parameter");
        return;
    }

    refda_agent_t *agent = ctx->runctx->agent;
    AGENT_ACL_LOCK(agent)

    refda_acl_access_list_it_t found_it;
    refda_acl_access_list_it_end(found_it, agent->acl.access);

    refda_acl_access_list_it_t acc_it;
    for (refda_acl_access_list_it(acc_it, agent->acl.access); !refda_acl_access_list_end_p(acc_it);
         refda_acl_access_list_next(acc_it))
    {
        refda_acl_access_t *acc = refda_acl_access_list_ref(acc_it);

        if (acc->id == aid)
        {
            refda_acl_access_list_it_set(found_it, acc_it);
            break;
        }
    }

    if (!refda_acl_access_list_end_p(found_it))
    {
        refda_acl_access_list_remove(agent->acl.access, acc_it);
    }
    refda_ctrl_exec_ctx_set_result_null(ctx);

    AGENT_ACL_UNLOCK(agent)
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_discard_access BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-group
 * Description:
 *   Ensure that after the control completes a group exists with the given
 *   information
 *
 * Parameters list:
 *   - Index 0, name "group-id", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 1, name "name", type use of ari:/ARITYPE/TEXTSTR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_gid  = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *p_name = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);

    cace_ari_uint gid;
    if (cace_ari_get_uint(p_gid, &gid) || (gid == 0))
    {
        CACE_LOG_ERR("Invalid group-id parameter");
        return;
    }

    const char *name = cace_ari_cget_tstr_cstr(p_name);
    if (!name)
    {
        CACE_LOG_ERR("Invalid group name");
        return;
    }

    bool success = false;

    refda_agent_t *agent = ctx->runctx->agent;
    AGENT_ACL_LOCK(agent)

    refda_acl_group_t *found_id = NULL, *found_name = NULL;

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, agent->acl.groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        refda_acl_group_t *grp = refda_acl_group_list_ref(grp_it);
        if (grp->id == gid)
        {
            found_id = grp;
        }
        if (m_string_equal_cstr_p(grp->name, name))
        {
            found_id = grp;
        }
    }

    if (found_id && found_name)
    {
        if (found_id != found_name)
        {
            CACE_LOG_ERR("Mismatch existing groups with ID %" PRIu32 " and name %s", gid, name);
        }
        // nothing else to do
    }
    else if (found_id)
    {
        // TODO handle this
    }
    else if (found_name)
    {
        // TODO handle this
    }
    else
    {
        // new item
        found_id = refda_acl_group_list_push_back_new(agent->acl.groups);

        found_id->id = gid;
        m_string_set_cstr(found_id->name, name);
        success = true;
    }

    AGENT_ACL_UNLOCK(agent)
    if (success)
    {
        refda_ctrl_exec_ctx_set_result_null(ctx);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-group-members
 * Description:
 *   Ensure that the membership of a group has a specific set of endpoint
 *   patterns.
 *
 * Parameters list:
 *   - Index 0, name "group-id", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *   - Index 1, name "members", type ulist of use of ari://ietf/network-base/TYPEDEF/endpoint-pattern
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group_members(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group_members BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_gid  = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *p_memb = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);

    cace_ari_uint gid;
    if (cace_ari_get_uint(p_gid, &gid) || (gid == 0))
    {
        CACE_LOG_ERR("Invalid group-id parameter");
        return;
    }
    const cace_ari_ac_t *memb_ac = cace_ari_cget_ac(p_memb);
    if (!memb_ac)
    {
        CACE_LOG_ERR("Invalid members parameter");
        return;
    }

    bool success = false;

    refda_agent_t *agent = ctx->runctx->agent;
    AGENT_ACL_LOCK(agent)

    refda_acl_group_t *found_id = NULL;

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, agent->acl.groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        refda_acl_group_t *grp = refda_acl_group_list_ref(grp_it);
        if (grp->id == gid)
        {
            found_id = grp;
            break;
        }
    }
    if (found_id)
    {
        // exact copy
        refda_amm_ident_base_list_reset(found_id->member_pats);

        cace_ari_list_it_t memb_it;
        for (cace_ari_list_it(memb_it, memb_ac->items); !cace_ari_list_end_p(memb_it); cace_ari_list_next(memb_it))
        {
            const cace_ari_t *memb_ref = cace_ari_list_cref(memb_it);

            refda_amm_ident_base_t *memb = refda_amm_ident_base_list_push_new(found_id->member_pats);
            refda_amm_ident_base_populate(memb, memb_ref, &agent->objs);
        }

        refda_ctrl_exec_ctx_set_result_null(ctx);
    }

    AGENT_ACL_UNLOCK(agent)
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group_members BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: discard-group
 * Description:
 *   Discard any group with the given ID.
 *
 * Parameters list:
 *   - Index 0, name "group-id", type use of ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_acl_ctrl_discard_group(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_discard_group BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_gid   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    cace_ari_uint gid;
    if (cace_ari_get_uint(p_gid, &gid))
    {
        CACE_LOG_ERR("Invalid group-id parameter");
        return;
    }

    refda_agent_t *agent = ctx->runctx->agent;
    AGENT_ACL_LOCK(agent)

    refda_acl_group_list_it_t found_it;
    refda_acl_group_list_it_end(found_it, agent->acl.groups);

    refda_acl_group_list_it_t grp_it;
    for (refda_acl_group_list_it(grp_it, agent->acl.groups); !refda_acl_group_list_end_p(grp_it);
         refda_acl_group_list_next(grp_it))
    {
        refda_acl_group_t *grp = refda_acl_group_list_ref(grp_it);

        if (grp->id == gid)
        {
            refda_acl_group_list_it_set(found_it, grp_it);
            break;
        }
    }

    if (!refda_acl_group_list_end_p(found_it))
    {
        refda_acl_group_list_remove(agent->acl.groups, grp_it);
    }
    refda_ctrl_exec_ctx_set_result_null(ctx);

    AGENT_ACL_UNLOCK(agent)
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_acl_ctrl_discard_group BODY
     * +-------------------------------------------------------------------------+
     */
}

int refda_adm_ietf_dtnma_agent_acl_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-dtnma-agent-acl");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("dtnma-agent-acl", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_ADM), "2025-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;
        (void)obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/permission
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("permission", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PERMISSION),
                objdata);
            // no parameters
        }
        { // For ./IDENT/execute
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("execute", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_EXECUTE),
                objdata);
            // no parameters
        }
        { // For ./IDENT/produce
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("produce", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PRODUCE),
                objdata);
            // no parameters
        }
        { // For ./IDENT/modify-var
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("modify-var", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_MODIFY_VAR),
                objdata);
            // no parameters
        }
        { // For ./IDENT/create-odm
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("create-odm", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_CREATE_ODM),
                objdata);
            // no parameters
        }
        { // For ./IDENT/delete-odm
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("delete-odm", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_DELETE_ODM),
                objdata);
            // no parameters
        }
        { // For ./IDENT/create-object
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("create-object",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_CREATE_OBJECT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/delete-object
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/dtnma-agent-acl/IDENT/permission
                cace_ari_set_objref_path_intid(&(base->name), 1, 2, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("delete-object",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_DELETE_OBJECT),
                objdata);
            // no parameters
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/entry-id
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/UINT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("entry-id", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_TYPEDEF_ENTRY_ID),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/optional-entry-id
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                        cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/NULL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_NULL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("optional-entry-id",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_TYPEDEF_OPTIONAL_ENTRY_ID),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/permission-list
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // uniform list
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // use of ari:/ARITYPE/IDENT
                    cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &typeref);
                }
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("permission-list",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_TYPEDEF_PERMISSION_LIST),
                objdata);
            // no parameters possible
        }

        /**
         * Register VAR objects
         */
        { // For ./VAR/default-access
            refda_amm_var_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_var_desc_t));
            refda_amm_var_desc_init(objdata);
            // stored value type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/permission-list
                cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 0);
                cace_amm_type_set_use_ref_move(&(objdata->val_type), &typeref);
            }
            // initial value:

            obj =
                refda_register_var(adm,
                                   cace_amm_idseg_ref_withenum(
                                       "default-access", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_VAR_DEFAULT_ACCESS),
                                   objdata);
            // no parameters
        }

        /**
         * Register EDD objects
         */
        { // For ./EDD/access-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 4);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "access-id");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                        cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "group-ids");
                    {
                        // uniform list
                        cace_amm_semtype_ulist_t *semtype_d1 = cace_amm_type_set_ulist(&(col->typeobj));
                        {
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                            cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                            cace_amm_type_set_use_ref_move(&(semtype_d1->item_type), &typeref);
                        }
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "objects");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/ari-pattern
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 27);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "permissions");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/permission-list
                        cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_acl_edd_access_list;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("access-list", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_EDD_ACCESS_LIST),
                objdata);
            // no parameters
        }
        { // For ./EDD/current-group-id
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &typeref);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_acl_edd_current_group_id;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("current-group-id",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_EDD_CURRENT_GROUP_ID),
                objdata);
            // no parameters
        }
        { // For ./EDD/group-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 3);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "group-id");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                        cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "name");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/TEXTSTR
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "members");
                    {
                        // uniform list
                        cace_amm_semtype_ulist_t *semtype_d1 = cace_amm_type_set_ulist(&(col->typeobj));
                        {
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // reference to ari://ietf/network-base/TYPEDEF/endpoint-pattern
                            cace_ari_set_objref_path_intid(&typeref, 1, 26, CACE_ARI_TYPE_TYPEDEF, 2);
                            cace_amm_type_set_use_ref_move(&(semtype_d1->item_type), &typeref);
                        }
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_acl_edd_group_list;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("group-list", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_EDD_GROUP_LIST),
                objdata);
            // no parameters
        }

        /**
         * Register CTRL objects
         */
        { // For ./CTRL/ensure-access
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_access;

            obj =
                refda_register_ctrl(adm,
                                    cace_amm_idseg_ref_withenum(
                                        "ensure-access", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_ENSURE_ACCESS),
                                    objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "access-id");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                    cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "group-ids");
                {
                    // uniform list
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                        cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &typeref);
                    }
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "objects");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/amm-base/TYPEDEF/ari-pattern
                    cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 27);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "permissions");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/permission-list
                    cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 0);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
        { // For ./CTRL/discard-access
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_acl_ctrl_discard_access;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("discard-access",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_DISCARD_ACCESS),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "group");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/optional-entry-id
                    cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "objects");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/amm-base/TYPEDEF/ari-pattern
                    cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 27);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
        { // For ./CTRL/ensure-group
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group;

            obj = refda_register_ctrl(adm,
                                      cace_amm_idseg_ref_withenum(
                                          "ensure-group", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_ENSURE_GROUP),
                                      objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "group-id");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                    cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "name");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // use of ari:/ARITYPE/TEXTSTR
                    cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
        { // For ./CTRL/ensure-group-members
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_acl_ctrl_ensure_group_members;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("ensure-group-members",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_ENSURE_GROUP_MEMBERS),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "group-id");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                    cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "members");
                {
                    // uniform list
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/network-base/TYPEDEF/endpoint-pattern
                        cace_ari_set_objref_path_intid(&typeref, 1, 26, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &typeref);
                    }
                }
            }
        }
        { // For ./CTRL/discard-group
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_acl_ctrl_discard_group;

            obj =
                refda_register_ctrl(adm,
                                    cace_amm_idseg_ref_withenum(
                                        "discard-group", REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_DISCARD_GROUP),
                                    objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "group-id");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent-acl/TYPEDEF/entry-id
                    cace_ari_set_objref_path_intid(&typeref, 1, 2, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
