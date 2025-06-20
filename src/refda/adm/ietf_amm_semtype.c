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
 * ADM module "ietf-amm-semtype" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_amm_semtype.h"
#include "refda/agent.h"
#include "refda/register.h"
#include "refda/edd_prod_ctx.h"
#include "refda/ctrl_exec_ctx.h"
#include "refda/oper_eval_ctx.h"
#include "refda/reporting.h"
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

int refda_adm_ietf_amm_semtype_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-amm-semtype");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("amm-semtype", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_ADM), "2025-05-28");
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
                adm,
                cace_amm_idseg_ref_withenum("display-hint", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_DISPLAY_HINT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/semtype
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("semtype", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_SEMTYPE),
                objdata);
            // no parameters
        }
        { // For ./IDENT/type-use
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("type-use", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TYPE_USE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/type-ref
                    // FIXME reference to unknown object
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/ulist
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("ulist", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_ULIST), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "item-type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
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
        { // For ./IDENT/dlist
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("dlist", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_DLIST), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "item-types");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-semtype/TYPEDEF/semtype
                        cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
        }
        { // For ./IDENT/umap
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("umap", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_UMAP), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "key-type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "value-type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/tblt
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("tblt", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TBLT), objdata);
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
                    cace_ari_ac_t acinit0;
                    cace_ari_ac_init(&acinit0);
                    cace_ari_set_ac(&(fparam->defval), &acinit0);
                }
            }
        }
        { // For ./IDENT/tblt-col
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("tblt-col", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TBLT_COL),
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
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/union
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("union", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_UNION), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "choices");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-semtype/TYPEDEF/semtype
                        cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
        }
        { // For ./IDENT/seq
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-semtype/IDENT/semtype
                cace_ari_set_objref_path_intid(&(base->name), 1, 24, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("seq", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_SEQ), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "item-type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
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
                adm, cace_amm_idseg_ref_withenum("semtype", REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_TYPEDEF_SEMTYPE),
                objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
