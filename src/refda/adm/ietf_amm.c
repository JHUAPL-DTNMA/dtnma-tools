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
    REFDA_AGENT_LOCK(agent);

    cace_amm_obj_ns_t *adm =
        cace_amm_obj_store_add_ns(&(agent->objs), "ietf-amm", "2024-07-03", true, REFDA_ADM_IETF_AMM_ENUM_ADM);
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-hint
            refda_amm_ident_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_obj_id_withenum("display-hint", REFDA_ADM_IETF_AMM_ENUM_OBJID_IDENT_DISPLAY_HINT),
                objdata);
            // no parameters
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/type-ref
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
                        ari_set_aritype(&name, ARI_TYPE_ARITYPE);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TYPEDEF);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("type-ref", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_TYPE_REF), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/integer
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 5);
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
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_INT);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 3);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 4);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_VAST);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("integer", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_INTEGER), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/float
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
                        ari_set_aritype(&name, ARI_TYPE_REAL32);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_REAL64);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("float", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_FLOAT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/numeric
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
                        // ari://ietf-amm/TYPEDEF/INTEGER
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/FLOAT
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 2);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("numeric", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_NUMERIC), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/primitive
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 5);
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
                        ari_set_aritype(&name, ARI_TYPE_BOOL);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/NUMERIC
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 3);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 4);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_BYTESTR);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("primitive", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_PRIMITIVE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/time
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
                        ari_set_aritype(&name, ARI_TYPE_TP);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TD);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("time", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_TIME), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/simple
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
                        // ari://ietf-amm/TYPEDEF/PRIMITIVE
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 4);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TIME
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 5);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("simple", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_SIMPLE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/nested
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_AC);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_AM);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TBL);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("nested", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_NESTED), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/any
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
                        ari_set_aritype(&name, ARI_TYPE_LITERAL);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_OBJECT);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("any", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_ANY), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/value-obj
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_CONST);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_EDD);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_VAR);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("value-obj", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_VALUE_OBJ), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/nonce
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_BYTESTR);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_NULL);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("nonce", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_NONCE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/counter32
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_UINT);
                amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("counter32", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_COUNTER32), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/counter64
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_UVAST);
                amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("counter64", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_COUNTER64), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/gauge32
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_INT);
                amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("gauge32", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_GAUGE32), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/gauge64
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_VAST);
                amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("gauge64", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_GAUGE64), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/timestamp
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TP);
                amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("timestamp", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_TIMESTAMP), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/eval-tgt
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/SIMPLE
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 6);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/EXPR
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("eval-tgt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EVAL_TGT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/expr-item
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/SIMPLE
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 6);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TYPE-REF
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 3);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_LABEL);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 4);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_OPER);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("expr-item", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXPR_ITEM), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/expr
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(objdata->typeobj));
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/EXPR-item
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 17);
                    amm_type_set_use_ref_move(&(semtype->item_type), &name);
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("expr", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXPR), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/exec-tgt
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
                        // ari://ietf-amm/TYPEDEF/exec-item
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 20);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/MAC
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 21);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("exec-tgt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXEC_TGT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/exec-item
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
                        ari_set_aritype(&name, ARI_TYPE_CTRL);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("exec-item", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_EXEC_ITEM), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/mac
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(objdata->typeobj));
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/EXEC-item
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 20);
                    amm_type_set_use_ref_move(&(semtype->item_type), &name);
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("mac", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_MAC), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rpt-tgt
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
                        // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/RPTT
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 24);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("rpt-tgt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_RPT_TGT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rptt-item
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
                        // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
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
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("rptt-item", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_RPTT_ITEM), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rptt
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(objdata->typeobj));
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/RPTT-item
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 23);
                    amm_type_set_use_ref_move(&(semtype->item_type), &name);
                }
            }
            obj = refda_register_typedef(
                adm, cace_amm_obj_id_withenum("rptt", REFDA_ADM_IETF_AMM_ENUM_OBJID_TYPEDEF_RPTT), objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent);
    return 0;
}
