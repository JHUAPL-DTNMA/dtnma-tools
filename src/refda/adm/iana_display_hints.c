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
 * ADM module "iana-display-hints" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "iana_display_hints.h"
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

int refda_adm_iana_display_hints_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "iana-display-hints");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("iana", 2),
        cace_amm_idseg_ref_withenum("display-hints", REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_ADM), "2024-07-20");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register IDENT objects
         */
        { // For ./IDENT/base-integer
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("base-integer", REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_INTEGER),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-int-dec
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-integer
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-int-dec",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_INT_DEC),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-int-bin
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-integer
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-int-bin",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_INT_BIN),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-int-hex
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-integer
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-int-hex",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_INT_HEX),
                objdata);
            // no parameters
        }
        { // For ./IDENT/base-float
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("base-float", REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_FLOAT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-float-dec
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-float
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-float-dec",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_FLOAT_DEC),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-float-exp
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-float
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-float-exp",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_FLOAT_EXP),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-float-hex
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-float
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-float-hex",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_FLOAT_HEX),
                objdata);
            // no parameters
        }
        { // For ./IDENT/base-bstr
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("base-bstr", REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_BSTR),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-text
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-bstr
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-text",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_TEXT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-base16
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-bstr
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-base16",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_BASE16),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-base64
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-bstr
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-base64",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_BASE64),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-ipaddress
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-bstr
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-ipaddress",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_IPADDRESS),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-bstr-uuid
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-bstr
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-bstr-uuid",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_UUID),
                objdata);
            // no parameters
        }
        { // For ./IDENT/base-time
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/display-hint
                cace_ari_set_objref_path_intid(&(base->name), 1, 0, CACE_ARI_TYPE_IDENT, 0);
            }

            obj = refda_register_ident(
                adm, cace_amm_idseg_ref_withenum("base-time", REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_TIME),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-time-text
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-time
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-time-text",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_TIME_TEXT),
                objdata);
            // no parameters
        }
        { // For ./IDENT/display-time-dec
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_back_new(objdata->bases);
                // ari://ietf/amm/IDENT/base-time
                // FIXME reference to unknown object
            }

            obj = refda_register_ident(
                adm,
                cace_amm_idseg_ref_withenum("display-time-dec",
                                            REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_TIME_DEC),
                objdata);
            // no parameters
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
