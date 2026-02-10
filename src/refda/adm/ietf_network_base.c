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
 * ADM module "ietf-network-base" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_network_base.h"
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

int refda_adm_ietf_network_base_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-network-base");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("network-base", REFDA_ADM_IETF_NETWORK_BASE_ENUM_ADM), "2025-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;
        (void)obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-bstr-uuid
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = false;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/amm-base/IDENT/display-hint-bstr
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 3);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-uuid",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_UUID),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-oid
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = false;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/amm-base/IDENT/display-hint-bstr
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 3);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-oid",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_OID),
                objdata);
            // no parameters
        }
        { // For ./IDENT/abstract-endpoint
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // no IDENT bases

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("abstract-endpoint",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_ABSTRACT_ENDPOINT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/abstract-endpoint-pattern
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = true;
            // no IDENT bases

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("abstract-endpoint-pattern",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_ABSTRACT_ENDPOINT_PATTERN),
                objdata);
            // no parameters
        }
        { // For ./IDENT/uri-regexp-pattern
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = false;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/network-base/IDENT/abstract-endpoint-pattern
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("uri-regexp-pattern",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_URI_REGEXP_PATTERN),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "regexp");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // use of ari:/ARITYPE/TEXTSTR
                    cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/uri
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/TEXTSTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("uri", REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_URI), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/uuid
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/BYTESTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTESTR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("uuid", REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_UUID), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/oid
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/BYTESTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTESTR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("oid", REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_OID), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/endpoint
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/IDENT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("endpoint", REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_ENDPOINT),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/endpoint-or-uri
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
                        // reference to ari://ietf/network-base/TYPEDEF/endpoint
                        cace_ari_set_objref_path_intid(&typeref, 1, 26, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/network-base/TYPEDEF/uri
                        cace_ari_set_objref_path_intid(&typeref, 1, 26, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("endpoint-or-uri",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_ENDPOINT_OR_URI),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/endpoint-pattern
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/IDENT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("endpoint-pattern",
                                            REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_ENDPOINT_PATTERN),
                objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
