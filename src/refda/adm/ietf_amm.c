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
 * ADM "ietf-amm" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_amm.h"
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
/*   STOP CUSTOM INCLUDES HERE  */

/*   START CUSTOM FUNCTIONS HERE */
/*   STOP CUSTOM FUNCTIONS HERE  */

int refda_adm_ietf_amm_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-amm");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm =
        cace_amm_obj_store_add_ns(&(agent->objs), cace_amm_idseg_ref_withenum("ietf", REFDA_ADM_IETF_ENUM),
                                  cace_amm_idseg_ref_withenum("amm", REFDA_ADM_IETF_AMM_ENUM_ADM), "2024-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-hint
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("display-hint", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_DISPLAY_HINT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/semtype
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE), objdata);
            // no parameters
        }
        { // For ./IDENT/semtype-use
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-use", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_USE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/type-ref
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 0);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/semtype-ulist
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-ulist", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_ULIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "item-type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "min-elements");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "max-elements");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
        }
        { // For ./IDENT/semtype-dlist
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-dlist", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_DLIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "item-types");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/semtype
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
        }
        { // For ./IDENT/semtype-umap
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-umap", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_UMAP),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "keys");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "keys");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/semtype-tblt
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-tblt", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_TBLT),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "columns");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_IDENT);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "min-elements");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "max-elements");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "key");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "unique");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
                {
                    cace_ari_ac_t acinit;
                    cace_ari_ac_init(&acinit);
                    cace_ari_set_ac(&(fparam->defval), &acinit);
                }
            }
        }
        { // For ./IDENT/semtype-tblt-col
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("semtype-tblt-col", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_TBLT_COL),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_LABEL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "datatype");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/semtype-union
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-union", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_UNION),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "choices");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/semtype
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
        }
        { // For ./IDENT/semtype-seq
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype-seq", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_SEMTYPE_SEQ),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "item-type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "min-elements");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "max-elements");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/type-ref
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_ARITYPE);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TYPEDEF);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("type-ref", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_TYPE_REF), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/semtype
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_IDENT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("semtype", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_SEMTYPE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/integer
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BYTE);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_INT);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 3);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 4);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAST);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("integer", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_INTEGER), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/float
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_REAL32);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_REAL64);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("float", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_FLOAT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/numeric
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/INTEGER
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/FLOAT
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("numeric", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_NUMERIC), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/primitive
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/NUMERIC
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 3);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 3);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 4);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BYTESTR);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("primitive", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_PRIMITIVE),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/time
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TP);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TD);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("time", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_TIME), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/simple
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/PRIMITIVE
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 4);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/TIME
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 5);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("simple", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_SIMPLE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/nested
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_AC);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_AM);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("nested", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_NESTED), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/any
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_LITERAL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_OBJECT);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("any", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_ANY), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/value-obj
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_CONST);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_EDD);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAR);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("value-obj", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_VALUE_OBJ),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/nonce
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BYTESTR);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("nonce", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_NONCE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/counter32
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("counter32", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_COUNTER32),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/counter64
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("counter64", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_COUNTER64),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/gauge32
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_INT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("gauge32", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_GAUGE32), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/gauge64
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAST);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("gauge64", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_GAUGE64), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/timestamp
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_TP);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("timestamp", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_TIMESTAMP),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/eval-tgt
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/SIMPLE
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 6);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/VALUE-OBJ
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/EXPR
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 18);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("eval-tgt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EVAL_TGT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/expr-item
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/SIMPLE
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 6);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/VALUE-OBJ
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/TYPE-REF
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 3);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_LABEL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 4);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_OPER);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("expr-item", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXPR_ITEM),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/expr
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/EXPR-item
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 17);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("expr", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXPR), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/exec-tgt
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/exec-item
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 20);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/MAC
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 21);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("exec-tgt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXEC_TGT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/exec-item
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_CTRL);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/VALUE-OBJ
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("exec-item", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXEC_ITEM),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/mac
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/EXEC-item
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 20);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("mac", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_MAC), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rpt-tgt
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/VALUE-OBJ
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/RPTT
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 24);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("rpt-tgt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_RPT_TGT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rptt-item
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/VALUE-OBJ
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm/TYPEDEF/EXPR
                        cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 18);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("rptt-item", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_RPTT_ITEM),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rptt
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm/TYPEDEF/RPTT-item
                    cace_ari_set_objref_path_intid(&name, 1, 0, CACE_ARI_TYPE_TYPEDEF, 23);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("rptt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_RPTT), objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
