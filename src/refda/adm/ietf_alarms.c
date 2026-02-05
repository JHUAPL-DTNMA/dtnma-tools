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
 * ADM module "ietf-alarms" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_alarms.h"
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
/*             TODO              */
/*   STOP CUSTOM FUNCTIONS HERE  */

/* Name: alarm-list
 * Description:
 *   A table to indicate the current and historical alarm states. Rows of
 *   the table SHALL be ordered by their 'time-created' values.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 9 columns:
 *   - Index 0, name "resource", type use of ari:/ARITYPE/IDENT
 *   - Index 1, name "category", type union of 2 types (use of ari:/ARITYPE/NULL, use of ari:/ARITYPE/IDENT)
 *   - Index 2, name "severity", type use of ari://ietf/alarms/TYPEDEF/severity
 *   - Index 3, name "time-created", type use of ari://ietf/amm-base/TYPEDEF/timestamp
 *   - Index 4, name "time-updated", type use of ari://ietf/amm-base/TYPEDEF/timestamp
 *   - Index 5, name "history", type TBLT with 2 columns:
 *       - Index 0, name "time", type use of ari://ietf/amm-base/TYPEDEF/timestamp
 *       - Index 1, name "severity", type use of ari://ietf/alarms/TYPEDEF/severity
 *   - Index 6, name "manager-state", type use of ari://ietf/alarms/TYPEDEF/manager-state
 *   - Index 7, name "manager-identity", type use of ari://ietf/network-base/TYPEDEF/endpoint
 *   - Index 8, name "manager-time", type union of 2 types (use of ari://ietf/amm-base/TYPEDEF/timestamp, use of
 * ari:/ARITYPE/NULL)
 */
static void refda_adm_ietf_alarms_edd_alarm_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_edd_alarm_list BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent = ctx->prodctx->runctx->agent;
    if (pthread_mutex_lock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return;
    }

    cace_ari_t      result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_tbl_t *table  = cace_ari_set_tbl(&result, NULL);
    cace_ari_tbl_reset(table, 9, 0);

    // table is naturally sorted
    refda_alarms_entry_list_it_t entry_it;
    for (refda_alarms_entry_list_it(entry_it, agent->alarms.alarm_list); !refda_alarms_entry_list_end_p(entry_it);
         refda_alarms_entry_list_next(entry_it))
    {
        const refda_alarms_entry_t *entry = refda_alarms_entry_ptr_cref(*refda_alarms_entry_list_cref(entry_it));

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, 9);

        cace_ari_set_copy(cace_ari_array_get(row, 0), &entry->resource.name);
        cace_ari_set_copy(cace_ari_array_get(row, 1), &entry->category.name);
        cace_ari_set_uint(cace_ari_array_get(row, 2), entry->severity);
        cace_ari_set_copy(cace_ari_array_get(row, 3), &entry->created_at);
        cace_ari_set_copy(cace_ari_array_get(row, 4), &entry->updated_at);
        // TODO other columns

        // append the row
        cace_ari_tbl_move_row_array(table, row);
    }

    refda_edd_prod_ctx_set_result_move(ctx, &result);

    if (pthread_mutex_unlock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
        return;
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_edd_alarm_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: resource-inventory
 * Description:
 *   A table to list all alarm resources defined on the Agent.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 1 columns:
 *   - Index 0, name "resource", type use of ari:/ARITYPE/IDENT
 */
static void refda_adm_ietf_alarms_edd_resource_inventory(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_edd_resource_inventory BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_edd_resource_inventory BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: category-inventory
 * Description:
 *   A table to list all alarm categories defined on the Agent.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 1 columns:
 *   - Index 0, name "category", type use of ari:/ARITYPE/IDENT
 */
static void refda_adm_ietf_alarms_edd_category_inventory(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_edd_category_inventory BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_edd_category_inventory BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: shelf-list
 * Description:
 *   A table to indicate which alarms are being shelved This can be edited
 *   by <./ctrl/ensure-shelf> and <./ctrl/discard-shelf> controls.
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/alarms/TYPEDEF/shelf-tblt
 */
static void refda_adm_ietf_alarms_edd_shelf_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_edd_shelf_list BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_edd_shelf_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: purge-alarms
 * Description:
 *   Remove specific entries of the <./edd/alarm-list> table
 *
 * Parameters list:
 *   - Index 0, name "filter", type use of ari://ietf/dtnma-agent/TYPEDEF/tbl-row-filter
 *
 * Result name "affected", type use of ari:/ARITYPE/UVAST
 */
static void refda_adm_ietf_alarms_ctrl_purge_alarms(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_purge_alarms BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_purge_alarms BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compress-alarms
 * Description:
 *   Compress the timeline expressed in the 'history' column of the
 *   <./edd/alarm-list> table. The result on affected entries will be a
 *   history which is compressed to only a single, current row.
 *
 * Parameters list:
 *   - Index 0, name "filter", type use of ari://ietf/dtnma-agent/TYPEDEF/tbl-row-filter
 *
 * Result name "affected", type use of ari:/ARITYPE/UVAST
 */
static void refda_adm_ietf_alarms_ctrl_compress_alarms(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_compress_alarms BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_compress_alarms BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-shelf
 * Description:
 *   Ensure a specific set of rows is present in the <./edd/shelf-list>,
 *   adding rows where necessary. If rows are added then existing alarm
 *   entries will be shelved as necessary.
 *
 * Parameters list:
 *   - Index 0, name "rows", type use of ari://ietf/alarms/TYPEDEF/shelf-tblt
 *
 * Result: none
 */
static void refda_adm_ietf_alarms_ctrl_ensure_shelf(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_ensure_shelf BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_ensure_shelf BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: discard-shelf
 * Description:
 *   Discard a specific set of rows from the <./edd/shelf-list> where
 *   necessary. If rows are removed then existing alarm entries will be un-
 *   shelved as necessary.
 *
 * Parameters list:
 *   - Index 0, name "rows", type use of ari://ietf/alarms/TYPEDEF/shelf-tblt
 *
 * Result: none
 */
static void refda_adm_ietf_alarms_ctrl_discard_shelf(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_discard_shelf BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_alarms_ctrl_discard_shelf BODY
     * +-------------------------------------------------------------------------+
     */
}

int refda_adm_ietf_alarms_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-alarms");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm =
        cace_amm_obj_store_add_ns(&(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
                                  cace_amm_idseg_ref_withenum("alarms", REFDA_ADM_IETF_ALARMS_ENUM_ADM), "2025-04-11");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;
        (void)obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/resource
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("resource", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_IDENT_RESOURCE), objdata);
            // no parameters
        }
        { // For ./IDENT/category
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("category", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_IDENT_CATEGORY), objdata);
            // no parameters
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/severity
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
                adm, cace_amm_idseg_ref_withenum("severity", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_SEVERITY),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/manager-state
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
                cace_amm_idseg_ref_withenum("manager-state", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_MANAGER_STATE),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/shelf-tblt
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->typeobj), 2);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "resources");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/ari-pattern
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 27);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "categories");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/ari-pattern
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 27);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("shelf-tblt", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_SHELF_TBLT),
                objdata);
            // no parameters possible
        }

        /**
         * Register EDD objects
         */
        { // For ./EDD/alarm-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 9);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "resource");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/IDENT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "category");
                    {
                        // union
                        cace_amm_semtype_union_t *semtype_d1 = cace_amm_type_set_union_size(&(col->typeobj), 2);
                        {
                            cace_amm_type_t *choice_d1 = cace_amm_type_array_get(semtype_d1->choices, 0);
                            {
                                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                                // use of ari:/ARITYPE/NULL
                                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_NULL);
                                cace_amm_type_set_use_ref_move(choice_d1, &typeref);
                            }
                        }
                        {
                            cace_amm_type_t *choice_d1 = cace_amm_type_array_get(semtype_d1->choices, 1);
                            {
                                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                                // use of ari:/ARITYPE/IDENT
                                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                                cace_amm_type_set_use_ref_move(choice_d1, &typeref);
                            }
                        }
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "severity");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/alarms/TYPEDEF/severity
                        cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "time-created");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/timestamp
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 15);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "time-updated");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/timestamp
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 15);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 5);
                    m_string_set_cstr(col->name, "history");
                    {
                        // table template
                        cace_amm_semtype_tblt_t *semtype_d1 = cace_amm_type_set_tblt_size(&(col->typeobj), 2);
                        {
                            cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype_d1->columns, 0);
                            m_string_set_cstr(col->name, "time");
                            {
                                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                                // reference to ari://ietf/amm-base/TYPEDEF/timestamp
                                cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 15);
                                cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                            }
                        }
                        {
                            cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype_d1->columns, 1);
                            m_string_set_cstr(col->name, "severity");
                            {
                                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                                // reference to ari://ietf/alarms/TYPEDEF/severity
                                cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 0);
                                cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                            }
                        }
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 6);
                    m_string_set_cstr(col->name, "manager-state");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/alarms/TYPEDEF/manager-state
                        cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 7);
                    m_string_set_cstr(col->name, "manager-identity");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/network-base/TYPEDEF/endpoint
                        cace_ari_set_objref_path_intid(&typeref, 1, 26, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 8);
                    m_string_set_cstr(col->name, "manager-time");
                    {
                        // union
                        cace_amm_semtype_union_t *semtype_d1 = cace_amm_type_set_union_size(&(col->typeobj), 2);
                        {
                            cace_amm_type_t *choice_d1 = cace_amm_type_array_get(semtype_d1->choices, 0);
                            {
                                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                                // reference to ari://ietf/amm-base/TYPEDEF/timestamp
                                cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 15);
                                cace_amm_type_set_use_ref_move(choice_d1, &typeref);
                            }
                        }
                        {
                            cace_amm_type_t *choice_d1 = cace_amm_type_array_get(semtype_d1->choices, 1);
                            {
                                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                                // use of ari:/ARITYPE/NULL
                                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_NULL);
                                cace_amm_type_set_use_ref_move(choice_d1, &typeref);
                            }
                        }
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_alarms_edd_alarm_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("alarm-list", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_ALARM_LIST),
                objdata);
            // no parameters
        }
        { // For ./EDD/resource-inventory
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 1);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "resource");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/IDENT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_alarms_edd_resource_inventory;

            obj = refda_register_edd(adm,
                                     cace_amm_idseg_ref_withenum(
                                         "resource-inventory", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_RESOURCE_INVENTORY),
                                     objdata);
            // no parameters
        }
        { // For ./EDD/category-inventory
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 1);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "category");
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/IDENT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &typeref);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_alarms_edd_category_inventory;

            obj = refda_register_edd(adm,
                                     cace_amm_idseg_ref_withenum(
                                         "category-inventory", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_CATEGORY_INVENTORY),
                                     objdata);
            // no parameters
        }
        { // For ./EDD/shelf-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // reference to ari://ietf/alarms/TYPEDEF/shelf-tblt
                cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 3);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &typeref);
            }
            // callback:
            objdata->produce = refda_adm_ietf_alarms_edd_shelf_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("shelf-list", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_SHELF_LIST),
                objdata);
            // no parameters
        }

        /**
         * Register CTRL objects
         */
        { // For ./CTRL/purge-alarms
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/UVAST
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UVAST);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &typeref);
            }
            // callback:
            objdata->execute = refda_adm_ietf_alarms_ctrl_purge_alarms;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("purge-alarms", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_PURGE_ALARMS),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "filter");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent/TYPEDEF/tbl-row-filter
                    cace_ari_set_objref_path_intid(&typeref, 1, 1, CACE_ARI_TYPE_TYPEDEF, 0);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
        { // For ./CTRL/compress-alarms
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/UVAST
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UVAST);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &typeref);
            }
            // callback:
            objdata->execute = refda_adm_ietf_alarms_ctrl_compress_alarms;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("compress-alarms", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_COMPRESS_ALARMS),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "filter");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/dtnma-agent/TYPEDEF/tbl-row-filter
                    cace_ari_set_objref_path_intid(&typeref, 1, 1, CACE_ARI_TYPE_TYPEDEF, 0);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
        { // For ./CTRL/ensure-shelf
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_alarms_ctrl_ensure_shelf;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("ensure-shelf", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_ENSURE_SHELF),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "rows");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/alarms/TYPEDEF/shelf-tblt
                    cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
        { // For ./CTRL/discard-shelf
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_alarms_ctrl_discard_shelf;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("discard-shelf", REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_DISCARD_SHELF),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "rows");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/alarms/TYPEDEF/shelf-tblt
                    cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
