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
 * ADM module "ietf-bp-base" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_bp_base.h"
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

int refda_adm_ietf_bp_base_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-bp-base");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("bp-base", REFDA_ADM_IETF_BP_BASE_ENUM_ADM), "2025-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/display-bstr-eid
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // ari://ietf/amm-base/IDENT/display-hint-bstr
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 3);
            }

            obj =
                refda_register_ident(adm,
                                     cace_amm_idseg_ref_withenum(
                                         "display-bstr-eid", REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_EID),
                                     objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-eid-pattern
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // ari://ietf/amm-base/IDENT/display-hint-bstr
                cace_ari_set_objref_path_intid(&(base->name), 1, 25, CACE_ARI_TYPE_IDENT, 3);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-eid-pattern",
                                            REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_EID_PATTERN),
                objdata);
            // no parameters
        }
        { // For ./IDENT/bp-endpoint
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // ari://ietf/network-base/IDENT/abstract-endpoint
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("bp-endpoint", REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_BP_ENDPOINT),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "eid");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/bp-base/TYPEDEF/eid-cbor
                    cace_ari_set_objref_path_intid(&name, 1, 5, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./IDENT/bp-eid-pattern
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // ari://ietf/network-base/IDENT/abstract-endpoint-pattern
                cace_ari_set_objref_path_intid(&(base->name), 1, 26, CACE_ARI_TYPE_IDENT, 1);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("bp-eid-pattern", REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_BP_EID_PATTERN),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "pattern");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/bp-base/TYPEDEF/eid-pattern-cbor
                    cace_ari_set_objref_path_intid(&name, 1, 5, CACE_ARI_TYPE_TYPEDEF, 2);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/eid-text
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/network-base/TYPEDEF/uri
                cace_ari_set_objref_path_intid(&name, 1, 26, CACE_ARI_TYPE_TYPEDEF, 0);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("eid-text", REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_TYPEDEF_EID_TEXT),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/eid-cbor
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_CBOR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("eid-cbor", REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_TYPEDEF_EID_CBOR),
                objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/eid-pattern-cbor
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_CBOR);
                cace_amm_type_set_use_ref_move(&(objdata->typeobj), &name);
            }

            obj = refda_register_typedef(
                adm,
                cace_amm_idseg_ref_withenum("eid-pattern-cbor",
                                            REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_TYPEDEF_EID_PATTERN_CBOR),
                objdata);
            // no parameters possible
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
