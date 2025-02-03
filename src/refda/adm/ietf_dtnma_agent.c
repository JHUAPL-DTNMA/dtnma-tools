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

/** @file
 * This is the compilation unit for the implementation of the
 * ADM "ietf-dtnma-agent" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_dtnma_agent.h"
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
#include <timespec.h>
/*   STOP CUSTOM INCLUDES HERE  */

/*   START CUSTOM FUNCTIONS HERE */

static bool refda_adm_ietf_dtnma_agent_ctrl_wait_finished(refda_exec_item_t *item)
{
    atomic_store(&(item->waiting), false);
    return true;
}

static void refda_adm_ietf_dtnma_agent_set_objpath(ari_objpath_t *path, const cace_amm_obj_ns_t *ns,
                                                   ari_type_t obj_type, const cace_amm_obj_desc_t *obj)
{
    if (ns->has_enum)
    {
        path->ns_id.form   = ARI_IDSEG_INT;
        path->ns_id.as_int = ns->intenum;
    }
    else
    {
        path->ns_id.form = ARI_IDSEG_TEXT;
        string_init_set_str(path->ns_id.as_text, string_get_cstr(ns->name));
    }

    path->type_id.form   = ARI_IDSEG_INT;
    path->type_id.as_int = obj_type;
    path->has_ari_type   = true;
    path->ari_type       = obj_type;

    if (obj->has_enum)
    {
        path->obj_id.form   = ARI_IDSEG_INT;
        path->obj_id.as_int = obj->intenum;
    }
    else
    {
        path->obj_id.form = ARI_IDSEG_TEXT;
        string_init_set_str(path->obj_id.as_text, string_get_cstr(obj->name));
    }
}

/*   STOP CUSTOM FUNCTIONS HERE  */

/* Name: sw-vendor
 * Description:
 *   The vendor for this Agent implementation.
 *
 * Parameters: none
 *
 * Produced type: use of ari:/ARITYPE/TEXTSTR
 */
static void refda_adm_ietf_dtnma_agent_edd_sw_vendor(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_vendor BODY
     * +-------------------------------------------------------------------------+
     */
    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tstr(&result, "JHU/APL", false);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_vendor BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: sw-version
 * Description:
 *   The version for this Agent implementation.
 *
 * Parameters: none
 *
 * Produced type: use of ari:/ARITYPE/TEXTSTR
 */
static void refda_adm_ietf_dtnma_agent_edd_sw_version(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_version BODY
     * +-------------------------------------------------------------------------+
     */
    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tstr(&result, "0.0.0", false);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_version BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: capability
 * Description:
 *   A table to indicate the ADM capability of the sending agent.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 4 columns (use of ari:/ARITYPE/TEXTSTR, use of ari:/ARITYPE/VAST, use of
 * ari:/ARITYPE/TEXTSTR, ulist of use of ari:/ARITYPE/TEXTSTR)
 */
static void refda_adm_ietf_dtnma_agent_edd_capability(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_capability BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    ari_tbl_t table;
    ari_tbl_init(&table, 4, 0);

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_list_cref(ns_it);
        if (ns->intenum < 0)
        {
            // ignore ODMs
            continue;
        }

        ari_array_t row;
        ari_array_init(row);
        ari_array_resize(row, 4);

        ari_set_tstr(ari_array_get(row, 0), m_string_get_cstr(ns->name), true);
        if (ns->has_enum)
        {
            ari_set_vast(ari_array_get(row, 1), ns->intenum);
        }
        ari_set_tstr(ari_array_get(row, 2), m_string_get_cstr(ns->revision), true);
        {
            ari_t   *col = ari_array_get(row, 3);
            ari_ac_t list;
            ari_ac_init(&list);

            string_tree_set_it_t feat_it;
            for (string_tree_set_it(feat_it, ns->feature_supp); !string_tree_set_end_p(feat_it);
                 string_tree_set_next(feat_it))
            {
                const m_string_t *feat = string_tree_set_cref(feat_it);
                ari_t            *item = ari_list_push_back_new(list.items);
                ari_set_tstr(item, m_string_get_cstr(*feat), true);
            }

            ari_set_ac(col, &list);
        }

        // append the row
        ari_tbl_move_row_array(&table, row);
        ari_array_clear(row);
    }

    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_capability BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-rx
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf-amm/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_rx(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-rx-failed
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf-amm/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-tx
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf-amm/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_tx(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-exec-started
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf-amm/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_started(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_started BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_started BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-exec-succeeded
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf-amm/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-exec-failed
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf-amm/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_failed(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_failed BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_failed BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: exec-running
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: TBLT with 3 columns (use of ari:/ARITYPE/UVAST, use of ari://ietf-amm/TYPEDEF/ANY, use of
 * ari:/ARITYPE/BYTE)
 */
static void refda_adm_ietf_dtnma_agent_edd_exec_running(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_exec_running BODY
     * +-------------------------------------------------------------------------+
     */
    enum exec_state_e
    {
        EXEC_WAITING = 0,
        EXEC_RUNNING = 1,
    };

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    if (pthread_mutex_lock(&(agent->exec_state_mutex)))
    {
        CACE_LOG_ERR("failed to lock exec_state_mutex");
        return;
    }

    ari_tbl_t table;
    ari_tbl_init(&table, 3, 0);

    refda_exec_seq_list_it_t seq_it;
    for (refda_exec_seq_list_it(seq_it, agent->exec_state); !refda_exec_seq_list_end_p(seq_it);
         refda_exec_seq_list_next(seq_it))
    {
        const refda_exec_seq_t *seq = refda_exec_seq_list_ref(seq_it);
        if (refda_exec_item_list_empty_p(seq->items))
        {
            // intermediate state
            continue;
        }
        const refda_exec_item_t *front = refda_exec_item_list_front(seq->items);

        ari_array_t row;
        ari_array_init(row);
        ari_array_resize(row, 3);

        ari_set_uvast(ari_array_get(row, 0), seq->pid);

        ari_set_copy(ari_array_get(row, 1), &(front->ref));
        {
            int state;
            if (atomic_load(&(front->waiting)))
            {
                state = EXEC_WAITING;
            }
            else
            {
                state = EXEC_RUNNING;
            }
            ari_set_int(ari_array_get(row, 2), state);
        }

        // append the row
        ari_tbl_move_row_array(&table, row);
        ari_array_clear(row);
    }

    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    if (pthread_mutex_unlock(&(agent->exec_state_mutex)))
    {
        CACE_LOG_ERR("failed to unlock exec_state_mutex");
        return;
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_exec_running BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: typedef-list
 * Description:
 *   A table of TYPEDEF within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 1 columns (use of ari:/ARITYPE/TYPEDEF)
 */
static void refda_adm_ietf_dtnma_agent_edd_typedef_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_typedef_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm;
    if (ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    ari_tbl_t table;
    ari_tbl_init(&table, 1, 0);

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_list_cref(ns_it);
        if ((ns->intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        const ari_type_t       obj_type = ARI_TYPE_TYPEDEF;
        cace_amm_obj_ns_ctr_t *ctr      = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr)
        {
            continue;
        }

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            ari_array_t row;
            ari_array_init(row);
            ari_array_resize(row, 1);

            {
                ari_ref_t *ref = ari_set_objref(ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }

            // append the row
            ari_tbl_move_row_array(&table, row);
            ari_array_clear(row);
        }
    }

    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_typedef_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: const-list
 * Description:
 *   A table of CONST within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 2 columns (use of ari:/ARITYPE/CONST, use of ari://ietf-amm/TYPEDEF/TYPE-REF)
 */
static void refda_adm_ietf_dtnma_agent_edd_const_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_const_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm;
    if (ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    ari_tbl_t table;
    ari_tbl_init(&table, 2, 0);

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_list_cref(ns_it);
        if ((ns->intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        const ari_type_t       obj_type = ARI_TYPE_CONST;
        cace_amm_obj_ns_ctr_t *ctr      = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr)
        {
            continue;
        }

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            ari_array_t row;
            ari_array_init(row);
            ari_array_resize(row, 2);

            {
                ari_ref_t *ref = ari_set_objref(ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }
            ari_array_get(row, 1); // FIXME populate this column

            // append the row
            ari_tbl_move_row_array(&table, row);
            ari_array_clear(row);
        }
    }

    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_const_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: var-list
 * Description:
 *   A table of VAR within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 2 columns (use of ari:/ARITYPE/VAR, use of ari://ietf-amm/TYPEDEF/TYPE-REF)
 */
static void refda_adm_ietf_dtnma_agent_edd_var_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_var_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm;
    if (ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    ari_tbl_t table;
    ari_tbl_init(&table, 2, 0);

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_list_cref(ns_it);
        if ((ns->intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        const ari_type_t       obj_type = ARI_TYPE_VAR;
        cace_amm_obj_ns_ctr_t *ctr      = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr)
        {
            continue;
        }

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            ari_array_t row;
            ari_array_init(row);
            ari_array_resize(row, 2);

            {
                ari_ref_t *ref = ari_set_objref(ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }
            ari_array_get(row, 1); // FIXME populate this column

            // append the row
            ari_tbl_move_row_array(&table, row);
            ari_array_clear(row);
        }
    }

    ari_t result = ARI_INIT_UNDEFINED;
    ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_var_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: sbr-list
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: TBLT with 6 columns (use of ari:/ARITYPE/SBR, use of ari://ietf-amm/TYPEDEF/MAC, use of
 * ari://ietf-amm/TYPEDEF/TIME, use of ari://ietf-amm/TYPEDEF/EXPR, use of ari:/ARITYPE/TD, use of ari:/ARITYPE/UVAST)
 */
static void refda_adm_ietf_dtnma_agent_edd_sbr_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sbr_list BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sbr_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: tbr-list
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: TBLT with 5 columns (use of ari:/ARITYPE/TBR, use of ari://ietf-amm/TYPEDEF/MAC, use of
 * ari://ietf-amm/TYPEDEF/TIME, use of ari:/ARITYPE/TD, use of ari:/ARITYPE/UVAST)
 */
static void refda_adm_ietf_dtnma_agent_edd_tbr_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_tbr_list BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_tbr_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: if-then-else
 * Description:
 *   Evaluate an expression and follow one of two branches of further
 *   evaluation.
 *
 * Parameters list:
 *  * Index 0, name "condition", type: use of ari://ietf-amm/TYPEDEF/eval-tgt
 *  * Index 1, name "on-truthy", type: union of 2 types (use of ari://ietf-amm/TYPEDEF/exec-tgt, use of
 * ari:/ARITYPE/NULL)
 *  * Index 2, name "on-falsy", type: union of 2 types (use of ari://ietf-amm/TYPEDEF/exec-tgt, use of
 * ari:/ARITYPE/NULL)
 *
 * Result name "branch", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_ctrl_if_then_else(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_if_then_else BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_if_then_else BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: catch
 * Description:
 *   Attempt to execute a target, and if there is some failure catch it and
 *   execute an alternative target.
 *
 * Parameters list:
 *  * Index 0, name "try", type: use of ari://ietf-amm/TYPEDEF/exec-tgt
 *  * Index 1, name "on-failure", type: union of 2 types (use of ari://ietf-amm/TYPEDEF/exec-tgt, use of
 * ari:/ARITYPE/NULL)
 *
 * Result name "try-success", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_ctrl_catch(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_catch BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_catch BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: wait-for
 * Description:
 *   This control causes the execution to pause for a given amount of time.
 *   This is intended to be used within a macro to separate controls in
 *   time.
 *
 * Parameters list:
 *  * Index 0, name "duration", type: use of ari:/ARITYPE/TD
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_wait_for(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_for BODY
     * +-------------------------------------------------------------------------+
     */
    const ari_t *duration = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    struct timespec nowtime;

    int res = clock_gettime(CLOCK_REALTIME, &nowtime);
    if (res)
    {
        // handled as failure
        return;
    }

    refda_timeline_event_t event = {
        .ts       = timespec_add(nowtime, duration->as_lit.value.as_timespec),
        .item     = ctx->item,
        .callback = refda_adm_ietf_dtnma_agent_ctrl_wait_finished,
    };
    refda_timeline_push(ctx->runctx->agent->exec_timeline, event);

    refda_ctrl_exec_ctx_set_waiting(ctx);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_for BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: wait-until
 * Description:
 *   This control causes the execution to pause until a specific absolute
 *   time point. This is intended to be used within a macro to separate
 *   controls in time or as a first macro item to delay execution after the
 *   time of reception.
 *
 * Parameters list:
 *  * Index 0, name "time", type: use of ari:/ARITYPE/TP
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_wait_until(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_until BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_until BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: wait-cond
 * Description:
 *   This control causes the execution to pause until a condition
 *   expression evaluates to truthy. This is intended to be used within a
 *   macro to separate controls in time or as a first macro item to delay
 *   execution until the condition is met.
 *
 * Parameters list:
 *  * Index 0, name "condition", type: use of ari://ietf-amm/TYPEDEF/eval-tgt
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_wait_cond(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_cond BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_cond BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: inspect
 * Description:
 *   Produce a result value to inspect the agent state. This does not
 *   perform any EXPR evaluation or RPTT handling.
 *
 * Parameters list:
 *  * Index 0, name "ref", type: use of ari://ietf-amm/TYPEDEF/VALUE-OBJ
 *
 * Result name "val", type: use of ari://ietf-amm/TYPEDEF/ANY
 */
static void refda_adm_ietf_dtnma_agent_ctrl_inspect(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_inspect BODY
     * +-------------------------------------------------------------------------+
     */
    CACE_LOG_WARNING("executed!");

    const ari_t *ref = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    // FIXME mutex-serialize object store access
    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(ctx->runctx->agent->objs), ref);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode_objpath(buf, &(ref->as_ref.objpath), ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_DEBUG("Lookup reference to %s", string_get_cstr(buf));
        string_clear(buf);
    }
    if (res)
    {
        CACE_LOG_WARNING("inspect lookup failed with status %d", res);
    }
    else
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, ctx->runctx, ref, &deref);

        res = refda_valprod_run(&prodctx);
        if (res)
        {
            // not setting a result will be treated as failure
            CACE_LOG_WARNING("inspect production failed with status %d", res);
        }
        else
        {
            // result of the CTRL is the produced value
            refda_ctrl_exec_ctx_set_result_move(ctx, &prodctx.value);
        }

        refda_valprod_ctx_deinit(&prodctx);
    }

    cace_amm_lookup_deinit(&deref);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_inspect BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: report-on
 * Description:
 *   Generate a report on an object without needing to define an object.
 *   The parameter is a single RPTT list that would be produced by an
 *   object. If used for more than one-shot diagnostics, defining a RPTT
 *   (e.g. in a VAR) is more efficient because the RPTT item would not be
 *   present in the report.
 *
 * Parameters list:
 *  * Index 0, name "rptt", type: use of ari://ietf-amm/TYPEDEF/rpt-tgt
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_report_on(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_report_on BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_report_on BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: var-reset
 * Description:
 *   Modify a VAR state to its default value.
 *
 * Parameters list:
 *  * Index 0, name "target", type: use of ari:/ARITYPE/VAR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_var_reset(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_reset BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_reset BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: var-store
 * Description:
 *   Modify a VAR state to a specific value.
 *
 * Parameters list:
 *  * Index 0, name "target", type: use of ari:/ARITYPE/VAR
 *  * Index 1, name "value", type: use of ari://ietf-amm/TYPEDEF/ANY
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_var_store(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_store BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_store BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-const
 * Description:
 *   Ensure a specific CONST is present.
 *
 * Parameters list:
 *  * Index 0, name "obj", type: use of ari:/ARITYPE/CONST
 *  * Index 1, name "type", type: use of ari://ietf-amm/TYPEDEF/TYPE-REF
 *  * Index 2, name "init", type: use of ari://ietf-amm/TYPEDEF/EXPR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_const(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_const BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_const BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: discard-const
 * Description:
 *   Discard a specific CONST if it is present.
 *
 * Parameters list:
 *  * Index 0, name "obj", type: use of ari:/ARITYPE/CONST
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_discard_const(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_discard_const BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_discard_const BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-var
 * Description:
 *   Ensure a specific VAR is present.
 *
 * Parameters list:
 *  * Index 0, name "obj", type: use of ari:/ARITYPE/VAR
 *  * Index 1, name "type", type: use of ari://ietf-amm/TYPEDEF/TYPE-REF
 *  * Index 2, name "init", type: union of 2 types (use of ari:/ARITYPE/NULL, use of ari://ietf-amm/TYPEDEF/EXPR)
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_var(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_var BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_var BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: discard-var
 * Description:
 *   Discard a specific VAR if it is present.
 *
 * Parameters list:
 *  * Index 0, name "obj", type: use of ari:/ARITYPE/VAR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_discard_var(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_discard_var BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_discard_var BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: negate
 * Description:
 *   Negate a value. This is equivalent to multiplying by -1 but a shorter
 *   expression.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_negate(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_negate BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_negate BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: add
 * Description:
 *   Add two numeric values. The operands are cast to the least compatible
 *   numeric type before the arithmetic.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_add(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_add BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_add BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: sub
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_sub(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_sub BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_sub BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: multiply
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_multiply(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_multiply BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_multiply BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: divide
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_divide(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_divide BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_divide BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: remainder
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_remainder(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_remainder BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_remainder BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-not
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_not(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_not BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_not BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-and
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_and(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_and BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_and BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-or
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_or(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_or BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_or BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-xor
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf-amm/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_xor(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_xor BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_xor BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-not
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_not(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_not BODY
     * +-------------------------------------------------------------------------+
     */
    const ari_t *val  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    ari_bool     raw_val;

    if (!ari_get_bool(val, &raw_val))
    {
        ari_t result = ARI_INIT_UNDEFINED;
        ari_set_bool(&result, !raw_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_not BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-and
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_and(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_and BODY
     * +-------------------------------------------------------------------------+
     */
    const ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    ari_bool     l_val, r_val;

    if (!ari_get_bool(left, &l_val) && !ari_get_bool(right, &r_val))
    {
        ari_t result = ARI_INIT_UNDEFINED;
        ari_set_bool(&result, l_val && r_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_and BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-or
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_or(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_or BODY
     * +-------------------------------------------------------------------------+
     */
    const ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    ari_bool     l_val, r_val;

    if (!ari_get_bool(left, &l_val) && !ari_get_bool(right, &r_val))
    {
        ari_t result = ARI_INIT_UNDEFINED;
        ari_set_bool(&result, l_val || r_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_or BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-xor
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_xor(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_xor BODY
     * +-------------------------------------------------------------------------+
     */
    const ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    ari_bool     l_val, r_val;

    if (!ari_get_bool(left, &l_val) && !ari_get_bool(right, &r_val))
    {
        ari_t result = ARI_INIT_UNDEFINED;
        ari_set_bool(&result, l_val != r_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_xor BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-eq
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_eq(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_eq BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_eq BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-ne
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_ne(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ne BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ne BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-gt
 * Description:
 *   Compare two numbers by value. The result is true if the left value is
 *   greater than the right. The operands are cast to the least compatible
 *   numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_gt(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_gt BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_gt BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-ge
 * Description:
 *   Compare two numbers by value. The result is true if the left value is
 *   greater than or equal to the right. The operands are cast to the least
 *   compatible numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_ge(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ge BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ge BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-lt
 * Description:
 *   Compare two operands by value. The result is true if the left value is
 *   less than the right. The operands are cast to the least compatible
 *   numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_lt(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_lt BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_lt BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-le
 * Description:
 *   Compare two operands by value. The result is true if the left value is
 *   less than or equal to the right. The operands are cast to the least
 *   compatible numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_le(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_le BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_le BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: tbl-filter
 * Description:
 *   Filter a table first by rows and then by columns.
 *
 * Parameters list:
 *  * Index 0, name "row-match", type: ulist of use of ari://ietf-amm/TYPEDEF/EXPR
 *  * Index 1, name "columns", type: ulist of use of ari://ietf-dtnma-agent/TYPEDEF/column-id
 *
 * Result name "out", type: use of ari:/ARITYPE/TBL
 */
static void refda_adm_ietf_dtnma_agent_oper_tbl_filter(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_tbl_filter BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_tbl_filter BODY
     * +-------------------------------------------------------------------------+
     */
}

int refda_adm_ietf_dtnma_agent_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-dtnma-agent");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(&(agent->objs), "ietf-dtnma-agent", "2024-07-03", true,
                                                       REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM);
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/hellotyp
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_BYTE);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UINT);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("hellotyp", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_TYPEDEF_HELLOTYP),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/column-id
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("column-id", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_TYPEDEF_COLUMN_ID),
                objdata);
            // no parameters possible
        }

        /**
         * Register CONST objects
         */
        { // For ./CONST/hello
            refda_amm_const_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_const_desc_t));
            refda_amm_const_desc_init(objdata);
            // constant value:
            {
                ari_ac_t acinit;
                ari_ac_init(&acinit);
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    // ari://ietf-dtnma-agent/EDD/sw-vendor
                    ari_set_objref_path_intid(item, 1, ARI_TYPE_EDD, 0);
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    // ari://ietf-dtnma-agent/EDD/sw-version
                    ari_set_objref_path_intid(item, 1, ARI_TYPE_EDD, 1);
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    // ari://ietf-dtnma-agent/EDD/capability
                    ari_set_objref_path_intid(item, 1, ARI_TYPE_EDD, 2);
                }
                ari_set_ac(&(objdata->value), &acinit);
            }

            obj = refda_register_const(
                adm, cace_amm_obj_id_withenum("hello", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CONST_HELLO), objdata);
            // no parameters
        }

        /**
         * Register EDD objects
         */
        { // For ./EDD/sw-vendor
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_vendor;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("sw-vendor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VENDOR),
                objdata);
            // no parameters
        }
        { // For ./EDD/sw-version
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_version;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("sw-version", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VERSION),
                objdata);
            // no parameters
        }
        { // For ./EDD/capability
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 4);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "adm-name");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "enum");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_VAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "revision");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "features");
                    {
                        amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(col->typeobj));
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                            amm_type_set_use_ref_move(&(semtype->item_type), &name);
                        }
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_capability;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("capability", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_CAPABILITY),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-rx
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_rx;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("num-msg-rx", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-rx-failed
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed;

            obj = refda_register_edd(
                adm,
                cace_amm_obj_id_withenum("num-msg-rx-failed",
                                         REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX_FAILED),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-tx
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_tx;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("num-msg-tx", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_TX),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-exec-started
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_started;

            obj =
                refda_register_edd(adm,
                                   cace_amm_obj_id_withenum("num-exec-started",
                                                            REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_STARTED),
                                   objdata);
            // no parameters
        }
        { // For ./EDD/num-exec-succeeded
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded;

            obj = refda_register_edd(
                adm,
                cace_amm_obj_id_withenum("num-exec-succeeded",
                                         REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_SUCCEEDED),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-exec-failed
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_failed;

            obj = refda_register_edd(
                adm,
                cace_amm_obj_id_withenum("num-exec-failed", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_FAILED),
                objdata);
            // no parameters
        }
        { // For ./EDD/exec-running
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 3);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "pid");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "target");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/ANY
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "state");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_BYTE);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_exec_running;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("exec-running", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_EXEC_RUNNING),
                objdata);
            // no parameters
        }
        { // For ./EDD/typedef-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 1);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TYPEDEF);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_typedef_list;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("typedef-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TYPEDEF_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/const-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 2);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_CONST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "type");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TYPE-REF
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_const_list;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("const-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_CONST_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/var-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 2);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_VAR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "type");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TYPE-REF
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_var_list;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("var-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_VAR_LIST), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/sbr-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 6);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_SBR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "action");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/MAC
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 21);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "start-time");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TIME
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 5);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "condition");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/EXPR
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "min-interval");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TD);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 5);
                    m_string_set_cstr(col->name, "max-count");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sbr_list;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("sbr-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SBR_LIST), objdata);
            // no parameters
        }
        { // For ./EDD/tbr-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 5);
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TBR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "action");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/MAC
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 21);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "start-time");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TIME
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 5);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "period");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TD);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_named_type_t *col = amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "max-count");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_tbr_list;

            obj = refda_register_edd(
                adm, cace_amm_obj_id_withenum("tbr-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TBR_LIST), objdata);
            // no parameters
        }

        /**
         * Register CTRL objects
         */
        { // For ./CTRL/if-then-else
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_if_then_else;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("if-then-else", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_IF_THEN_ELSE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/eval-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 16);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-truthy");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/exec-tgt
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-falsy");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/exec-tgt
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                ari_set_null(&(fparam->defval));
            }
        }
        { // For ./CTRL/catch
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_catch;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("catch", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_CATCH), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "try");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/exec-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-failure");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/exec-tgt
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                ari_set_null(&(fparam->defval));
            }
        }
        { // For ./CTRL/wait-for
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_for;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("wait-for", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_FOR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "duration");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_TD);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/wait-until
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_until;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("wait-until", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_UNTIL),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "time");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_TP);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/wait-cond
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_cond;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("wait-cond", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_COND),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/eval-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 16);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/inspect
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/ANY
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_inspect;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("inspect", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_INSPECT), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "ref");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/report-on
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_report_on;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("report-on", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_REPORT_ON),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "rptt");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/rpt-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 22);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/var-reset
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_var_reset;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("var-reset", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_RESET),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "target");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/var-store
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_var_store;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("var-store", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_STORE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "target");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "value");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/ANY
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-const
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_const;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("ensure-const", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_CONST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_CONST);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "type");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/TYPE-REF
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/EXPR
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/discard-const
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_discard_const;

            obj = refda_register_ctrl(
                adm,
                cace_amm_obj_id_withenum("discard-const", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_DISCARD_CONST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_CONST);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-var
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_var;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("ensure-var", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_VAR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "type");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/TYPE-REF
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/EXPR
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                ari_set_null(&(fparam->defval));
            }
        }
        { // For ./CTRL/discard-var
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_discard_var;

            obj = refda_register_ctrl(
                adm, cace_amm_obj_id_withenum("discard-var", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_DISCARD_VAR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }

        /**
         * Register OPER objects
         */
        { // For ./OPER/negate
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 1);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "val");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_negate;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("negate", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_NEGATE), objdata);
            // no parameters
        }
        { // For ./OPER/add
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_add;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("add", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_ADD), objdata);
            // no parameters
        }
        { // For ./OPER/sub
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_sub;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("sub", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_SUB), objdata);
            // no parameters
        }
        { // For ./OPER/multiply
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_multiply;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("multiply", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_MULTIPLY),
                objdata);
            // no parameters
        }
        { // For ./OPER/divide
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_divide;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("divide", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_DIVIDE), objdata);
            // no parameters
        }
        { // For ./OPER/remainder
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_remainder;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("remainder", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_REMAINDER),
                objdata);
            // no parameters
        }
        { // For ./OPER/bit-not
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 1);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "val");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_not;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bit-not", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_NOT), objdata);
            // no parameters
        }
        { // For ./OPER/bit-and
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_and;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bit-and", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_AND), objdata);
            // no parameters
        }
        { // For ./OPER/bit-or
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_or;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bit-or", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_OR), objdata);
            // no parameters
        }
        { // For ./OPER/bit-xor
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/INTEGER
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_xor;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bit-xor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_XOR), objdata);
            // no parameters
        }
        { // For ./OPER/bool-not
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 1);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "operand");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_not;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bool-not", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_NOT),
                objdata);
            // no parameters
        }
        { // For ./OPER/bool-and
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_and;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bool-and", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_AND),
                objdata);
            // no parameters
        }
        { // For ./OPER/bool-or
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_or;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bool-or", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_OR), objdata);
            // no parameters
        }
        { // For ./OPER/bool-xor
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_xor;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("bool-xor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_XOR),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-eq
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/ANY
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/ANY
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_eq;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("compare-eq", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_EQ),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-ne
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/ANY
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/ANY
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_ne;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("compare-ne", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_NE),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-gt
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_gt;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("compare-gt", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GT),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-ge
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_ge;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("compare-ge", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GE),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-lt
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_lt;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("compare-lt", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LT),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-le
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/NUMERIC
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_le;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("compare-le", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LE),
                objdata);
            // no parameters
        }
        { // For ./OPER/tbl-filter
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            amm_named_type_array_resize(objdata->operand_types, 1);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "in");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_TBL);
                    amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TBL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_tbl_filter;

            obj = refda_register_oper(
                adm, cace_amm_obj_id_withenum("tbl-filter", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_TBL_FILTER),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "row-match");
                {
                    amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(fparam->typeobj));
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/EXPR
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                        amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "columns");
                {
                    amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(fparam->typeobj));
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-dtnma-agent/TYPEDEF/column-id
                        ari_set_objref_path_intid(&name, 1, ARI_TYPE_TYPEDEF, 1);
                        amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
