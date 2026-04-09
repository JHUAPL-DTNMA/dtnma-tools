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

/* Name: match-ip-vlsm
 * Description:
 *   This is a unary predicate operator which will compare an IP address
 *   operand based on concepts of variable-length subnet masking (VLSM)
 *   from Classless Inter-domain Routing (CIDR) definitions. This
 *   representation does not use the text form of CIDR notation to avoid
 *   processors needing to parse text content.  This form of pattern can
 *   only match 'ip-endpoint' or 'ip-transport' references.  Because of the
 *   limitations of VLSM, each reference to this object can only represent
 *   a single contiguous, bit-aligned IP block. Not all possible blocks can
 *   be represented by a single VLSM pattern.
 *
 * Parameters list:
 *   - Index 0, name "base", type use of ari://ietf/inet-base/TYPEDEF/ip-address
 *   - Index 1, name "prefix", type use of ari:/ARITYPE/UINT
 *
 * Operand list:
 *   - Index 0, name "value", type union of 3 types (use of ari://ietf/inet-base/TYPEDEF/ip-address, use of
 * ari:/ARITYPE/IDENT, use of ari:/ARITYPE/IDENT)
 *
 * Result name "is-match", type use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_inet_base_oper_match_ip_vlsm(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_inet_base_oper_match_ip_vlsm BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_inet_base_oper_match_ip_vlsm BODY
     * +-------------------------------------------------------------------------+
     */
}

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
        (void)obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-bstr-ipaddress
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
                cace_amm_idseg_ref_withenum("display-bstr-ipaddress",
                                            REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_IPADDRESS),
                objdata);
            // no parameters
        }
        { // For ./IDENT/ip-endpoint
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = false;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/network-base/IDENT/abstract-endpoint
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("ip-endpoint", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_ENDPOINT),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "address");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/inet-base/TYPEDEF/ip-address
                    cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
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
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // reference to ari://ietf/inet-base/TYPEDEF/zone
                            cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 4);
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
                cace_ari_set_null(&(fparam->defval));
            }
        }
        { // For ./IDENT/ip-transport
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            objdata->abstract = false;
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // reference to ari://ietf/network-base/IDENT/abstract-endpoint
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
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/inet-base/TYPEDEF/ip-address
                    cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "port");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/inet-base/TYPEDEF/port-number
                    cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
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
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // reference to ari://ietf/inet-base/TYPEDEF/zone
                            cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 4);
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
                cace_ari_set_null(&(fparam->defval));
            }
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/ipv4-address
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/BYTESTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTESTR);
                cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);

                cace_amm_semtype_cnst_t *cnst;
                {
                    // Constraint: StringLength(ranges=[4])
                    cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                    cace_util_range_size_t *range = cace_amm_semtype_cnst_set_strlen(cnst);
                    {
                        cace_util_range_intvl_size_t intvl;
                        cace_util_range_intvl_size_set_min(&intvl, 4);
                        cace_util_range_intvl_size_set_max(&intvl, 4);
                        cace_util_range_size_push(*range, intvl);
                    }
                }
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
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/BYTESTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BYTESTR);
                cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);

                cace_amm_semtype_cnst_t *cnst;
                {
                    // Constraint: StringLength(ranges=[16])
                    cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                    cace_util_range_size_t *range = cace_amm_semtype_cnst_set_strlen(cnst);
                    {
                        cace_util_range_intvl_size_t intvl;
                        cace_util_range_intvl_size_set_min(&intvl, 16);
                        cace_util_range_intvl_size_set_max(&intvl, 16);
                        cace_util_range_size_push(*range, intvl);
                    }
                }
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
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/inet-base/TYPEDEF/ipv4-address
                        cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 0);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // reference to ari://ietf/inet-base/TYPEDEF/ipv6-address
                        cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
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
            // named semantic type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/UINT
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UINT);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                        // use of ari:/ARITYPE/TEXTSTR
                        cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(choice, &typeref);
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
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/UINT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UINT);
                cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);

                cace_amm_semtype_cnst_t *cnst;
                {
                    // Constraint: StringLength(ranges=[0,65535])
                    cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                    cace_util_range_size_t *range = cace_amm_semtype_cnst_set_strlen(cnst);
                    {
                        cace_util_range_intvl_size_t intvl;
                        cace_util_range_intvl_size_set_min(&intvl, 0);
                        cace_util_range_intvl_size_set_max(&intvl, 65535);
                        cace_util_range_size_push(*range, intvl);
                    }
                }
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
            // named semantic type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/TEXTSTR
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_TEXTSTR);
                cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(&(objdata->typeobj), &typeref);

                cace_amm_semtype_cnst_t *cnst;
                {
                    // Constraint: StringLength(ranges=[1,253])
                    cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                    cace_util_range_size_t *range = cace_amm_semtype_cnst_set_strlen(cnst);
                    {
                        cace_util_range_intvl_size_t intvl;
                        cace_util_range_intvl_size_set_min(&intvl, 1);
                        cace_util_range_intvl_size_set_max(&intvl, 253);
                        cace_util_range_size_push(*range, intvl);
                    }
                }
                {
                    // Constraint:
                    // TextPattern(pattern='((([a-zA-Z0-9_]([a-zA-Z0-9\\-_]){0,61})?[a-zA-Z0-9]\\.)*([a-zA-Z0-9_]([a-zA-Z0-9\\-_]){0,61})?[a-zA-Z0-9]\\.?)|\\.')
                    cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                    cace_amm_semtype_cnst_set_textpat(cnst, "((([a-zA-Z0-9_]([a-zA-Z0-9\\-_]){0,61})?[a-zA-Z0-9]\\.)*(["
                                                            "a-zA-Z0-9_]([a-zA-Z0-9\\-_]){0,61})?[a-zA-Z0-9]\\.?)|\\.");
                }
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("domain-name", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_DOMAIN_NAME),
                objdata);
            // no parameters possible
        }

        /**
         * Register OPER objects
         */
        { // For ./OPER/match-ip-vlsm
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 1);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                m_string_set_cstr(operand->name, "value");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(operand->typeobj), 3);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // reference to ari://ietf/inet-base/TYPEDEF/ip-address
                            cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                            cace_amm_type_set_use_ref_move(choice, &typeref);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // use of ari:/ARITYPE/IDENT
                            cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                            cace_amm_semtype_use_t *semtype_d1 = cace_amm_type_set_use_ref_move(choice, &typeref);

                            cace_amm_semtype_cnst_t *cnst;
                            {
                                // Constraint: IdentRefBase(base_text='./ident/ip-endpoint',
                                // base_ari=ReferenceARI(ident=Identity(org_id='ietf', model_id='inet-base',
                                // model_rev=None, type_id=<StructType.IDENT: -1>, obj_id='ip-endpoint'), params=None),
                                // base_ident=None)
                                cnst = cace_amm_semtype_cnst_array_push_new(semtype_d1->constraints);

                                // FIXME unhandled constraint IdentRefBase(base_text='./ident/ip-endpoint',
                                // base_ari=ReferenceARI(ident=Identity(org_id='ietf', model_id='inet-base',
                                // model_rev=None, type_id=<StructType.IDENT: -1>, obj_id='ip-endpoint'), params=None),
                                // base_ident=None)
                            }
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 2);
                        {
                            cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                            // use of ari:/ARITYPE/IDENT
                            cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_IDENT);
                            cace_amm_semtype_use_t *semtype_d1 = cace_amm_type_set_use_ref_move(choice, &typeref);

                            cace_amm_semtype_cnst_t *cnst;
                            {
                                // Constraint: IdentRefBase(base_text='./ident/ip-transport',
                                // base_ari=ReferenceARI(ident=Identity(org_id='ietf', model_id='inet-base',
                                // model_rev=None, type_id=<StructType.IDENT: -1>, obj_id='ip-transport'), params=None),
                                // base_ident=None)
                                cnst = cace_amm_semtype_cnst_array_push_new(semtype_d1->constraints);

                                // FIXME unhandled constraint IdentRefBase(base_text='./ident/ip-transport',
                                // base_ari=ReferenceARI(ident=Identity(org_id='ietf', model_id='inet-base',
                                // model_rev=None, type_id=<StructType.IDENT: -1>, obj_id='ip-transport'), params=None),
                                // base_ident=None)
                            }
                        }
                    }
                }
            }
            // result type:
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/BOOL
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &typeref);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_inet_base_oper_match_ip_vlsm;

            obj = refda_register_oper(
                adm,
                cace_amm_idseg_ref_withenum("match-ip-vlsm", REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_OPER_MATCH_IP_VLSM),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "base");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // reference to ari://ietf/inet-base/TYPEDEF/ip-address
                    cace_ari_set_objref_path_intid(&typeref, 1, 4, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "prefix");
                {
                    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                    // use of ari:/ARITYPE/UINT
                    cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_UINT);
                    cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);

                    cace_amm_semtype_cnst_t *cnst;
                    {
                        // Constraint: NumericRange(ranges=[0,128])
                        cnst = cace_amm_semtype_cnst_array_push_new(semtype->constraints);

                        cace_util_range_int64_t *range = cace_amm_semtype_cnst_set_range_int64(cnst);
                        {
                            cace_util_range_intvl_int64_t intvl;
                            cace_util_range_intvl_int64_set_min(&intvl, 0);
                            cace_util_range_intvl_int64_set_max(&intvl, 128);
                            cace_util_range_int64_push(*range, intvl);
                        }
                    }
                }
            }
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
