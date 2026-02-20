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

/** @file
 * This is the compilation unit for the implementation of the
 * ADM module "ietf-amm-base" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_amm_base.h"
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

int refda_adm_ietf_amm_base_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-amm-base");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("amm-base", REFDA_ADM_IETF_AMM_BASE_ENUM_ADM), "2025-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;
        (void)obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-hint
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // no IDENT bases

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("display-hint", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-hint-integer
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/amm-base/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-hint-integer",
                                            REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_INTEGER),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-hint-float
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/amm-base/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-hint-float",
                                            REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_FLOAT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-hint-bstr
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/amm-base/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-hint-bstr",
                                            REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_BSTR),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-hint-time
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/amm-base/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-hint-time",
                                            REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_TIME),
                objdata);
            // no parameters
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/type-ref
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
                        // use of ari:/ARITYPE/ARITYPE
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_ARITYPE);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/TYPEDEF
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TYPEDEF);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("type-ref", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_TYPE_REF),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/integer
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/BYTE
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTE);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/INT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_INT);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/UINT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UINT);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 3);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/VAST
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_VAST);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 4);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/UVAST
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("integer", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_INTEGER),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/float
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
                        // use of ari:/ARITYPE/REAL32
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_REAL32);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/REAL64
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_REAL64);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("float", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_FLOAT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/numeric
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
                        // reference to ari://ietf/amm-base/TYPEDEF/integer
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/float
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 2);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("numeric", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_NUMERIC),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/primitive
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/NULL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_NULL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/BOOL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BOOL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/numeric
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 3);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/TEXTSTR
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 4);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/BYTESTR
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTESTR);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("primitive", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_PRIMITIVE),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/time
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
                        // use of ari:/ARITYPE/TP
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TP);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/TD
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TD);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("time", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_TIME), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/simple
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
                        // reference to ari://ietf/amm-base/TYPEDEF/PRIMITIVE
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 4);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/TIME
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 5);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("simple", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_SIMPLE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/nested
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/AC
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_AC);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/AM
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_AM);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/TBL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TBL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("nested", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_NESTED), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/any
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
                        // use of ari:/ARITYPE/LITERAL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_LITERAL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/OBJECT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_OBJECT);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("any", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_ANY), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/value-obj
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/CONST
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_CONST);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/EDD
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_EDD);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/VAR
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_VAR);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("value-obj", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_VALUE_OBJ),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/nonce
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 3);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/BYTESTR
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTESTR);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/UVAST
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/NULL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_NULL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("nonce", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_NONCE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/id-text
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/TEXTSTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);

                cace_amm_semtype_cnst_t *cnst;
                {
                    // Constraint: TextPattern(pattern='[A-Za-z_][0-9A-Za-z_\\-\\.]*')
                    cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                    cace_amm_semtype_cnst_set_textpat(cnst, "[A-Za-z_][0-9A-Za-z_\\-\\.]*");
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("id-text", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_ID_TEXT),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/id-int
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/INT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_INT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("id-int", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_ID_INT), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/counter32
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
                adm, cace_amm_idseg_ref_withenum("counter32", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_COUNTER32),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/counter64
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/UVAST
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UVAST);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("counter64", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_COUNTER64),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/gauge32
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/INT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_INT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("gauge32", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_GAUGE32),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/gauge64
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/VAST
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_VAST);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("gauge64", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_GAUGE64),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/timestamp
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/TP
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TP);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("timestamp", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_TIMESTAMP),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/eval-tgt
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
                        // reference to ari://ietf/amm-base/TYPEDEF/value-obj
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/expr
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("eval-tgt", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EVAL_TGT),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/expr-item
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 5);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/simple
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 6);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/value-obj
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/type-ref
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 3);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/LABEL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_LABEL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 4);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/OPER
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_OPER);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("expr-item", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXPR_ITEM),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/expr
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // uniform list
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/amm-base/TYPEDEF/expr-item
                    cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 17);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &typeref);
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("expr", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXPR), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/exec-tgt
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
                        // reference to ari://ietf/amm-base/TYPEDEF/exec-item
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 20);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/mac
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 21);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("exec-tgt", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXEC_TGT),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/exec-item
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
                        // use of ari:/ARITYPE/CTRL
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_CTRL);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/value-obj
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("exec-item", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXEC_ITEM),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/mac
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // uniform list
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/amm-base/TYPEDEF/exec-item
                    cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 20);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &typeref);
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("mac", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_MAC), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rpt-tgt
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
                        // reference to ari://ietf/amm-base/TYPEDEF/value-obj
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/rptt
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 24);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("rpt-tgt", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPT_TGT),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rptt-item
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
                        // reference to ari://ietf/amm-base/TYPEDEF/value-obj
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 9);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/amm-base/TYPEDEF/expr
                        cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("rptt-item", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPTT_ITEM),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/rptt
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                // uniform list
                cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(objdata->typeobj));
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/amm-base/TYPEDEF/rptt-item
                    cace_ari_set_objref_path_intid(&typeref, 1, 25, CACE_ARI_TYPE_TYPEDEF, 23);
                    cace_amm_type_set_use_ref_move(&(semtype->item_type), &typeref);
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("rptt", REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPTT), objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
