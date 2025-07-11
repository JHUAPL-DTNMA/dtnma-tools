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
 * ADM module "ietf-inet-base" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_inet_base.h"
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

int refda_adm_ietf_inet_base_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-inet-base");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("inet-base", REFDA_ADM_IETF_INET_BASE_ENUM_ADM), "2025-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-bstr-ipaddress
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm-base/IDENT/display-hint-bstr
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 3);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-ipaddress",
                                            REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_IPADDRESS),
                objdata);
            // no parameters
        }
        { // For ./IDENT/ip-endpoint
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/network-base/IDENT/abstract-endpoint
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("ip-endpoint", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_ENDPOINT),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "address");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/inet-base/TYPEDEF/ip-address
                    cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "zone");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/inet-base/TYPEDEF/zone
                            cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 4);
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
        { // For ./IDENT/ip-transport
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/network-base/IDENT/abstract-endpoint
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("ip-transport", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_TRANSPORT),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "address");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/inet-base/TYPEDEF/ip-address
                    cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "port");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/inet-base/TYPEDEF/port-number
                    cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "zone");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/inet-base/TYPEDEF/zone
                            cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 4);
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
        { // For ./IDENT/ip-vlsm-pattern
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/network-base/IDENT/abstract-endpoint-pattern
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 1);
            }

            obj =
                refda_register_ident(adm,
                                     cace_amm_idseg_ref_withenum(
                                         "ip-vlsm-pattern", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_VLSM_PATTERN),
                                     objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "base");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/inet-base/TYPEDEF/ip-address
                    cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "prefix");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/ipv4-address
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BYTESTR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("ipv4-address", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_IPV4_ADDRESS),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/ipv6-address
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BYTESTR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("ipv6-address", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_IPV6_ADDRESS),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/ip-address
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
                        // ari://ietf/inet-base/TYPEDEF/ipv4-address
                        cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/inet-base/TYPEDEF/ipv6-address
                        cace_ari_set_objref_path_intid(&name, 1, 4, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("ip-address", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_IP_ADDRESS),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/zone
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
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("zone", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_ZONE), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/port-number
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("port-number", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_PORT_NUMBER),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/domain-name
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("domain-name", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_DOMAIN_NAME),
                objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
