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
#include "ietf.h"
#include "refda/agent.h"
#include "refda/register.h"
#include "refda/valprod.h"
#include "refda/reporting.h"
#include <cace/amm/semtype.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <timespec.h>

void refda_adm_ietf_dtnma_agent_edd_sw_version(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx)
{
    ari_set_tstr(&(ctx->value), "0.0.0", false);
}

int refda_adm_ietf_dtnma_agent_ctrl_inspect(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx)
{
    CACE_LOG_WARNING("executed!");

    const ari_t *ref = refda_exec_ctx_get_aparam_index(ctx, 0);

    // FIXME mutex-serialize object store access
    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(ctx->parent->agent->objs), ref);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode_objpath(buf, &(ref->as_ref.objpath), ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_DEBUG("Lookup reference to %s", string_get_cstr(buf));
        string_clear(buf);
    }
    if (res)
    {
        CACE_LOG_WARNING("inspect lookup failed with status %d", res);
    }
    else
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, ctx->parent, &deref);

        res = refda_valprod_run(&prodctx);
        if (res)
        {
            CACE_LOG_WARNING("inspect production failed with status %d", res);
        }
        else
        {
            // result of the CTRL is the produced value
            ari_set_move(&(ctx->result), &prodctx.value);
        }

        refda_valprod_ctx_deinit(&prodctx);
    }

    cace_amm_lookup_deinit(&deref);

    return res;
}

/** CTRL execution callback for ari://ietf-dtnma-agent/CTRL/wait-for
 * Description:
 *   This control causes the execution to pause for a given amount of time.
 *   This is intended to be used within a macro to separate controls
 *   in time.";
 */
int refda_adm_ietf_dtnma_agent_ctrl_wait_for(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx)
{
    const ari_t *duration = refda_exec_ctx_get_aparam_index(ctx, 0);

    struct timespec nowtime;

    int res = clock_gettime(CLOCK_REALTIME, &nowtime);
    if (res)
    {
        return 2;
    }

    refda_timeline_event_t event = {
        .ts  = timespec_add(nowtime, duration->as_lit.value.as_timespec),
        .ref = ctx,
    };
    refda_timeline_push(ctx->parent->agent->exec_timeline, event);

    refda_exec_ctx_set_waiting(ctx);
    return 0;
}

int refda_adm_ietf_dtnma_agent_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: ietf-dtnma-agent");

    if (pthread_mutex_lock(&(agent->objs_mutex)))
    {
        return 2;
    }

    cace_amm_obj_ns_t *adm =
        cace_amm_obj_store_add_ns(&(agent->objs), "ietf-dtnma-agent", true, REFDA_ADM_IETF_DTNMA_AGENT_ENUM);
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register CONST objects
         */
        {
            refda_amm_const_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_const_desc_t));
            refda_amm_const_desc_init(objdata);
            {
                ari_ac_t acinit;
                ari_ac_init(&acinit);
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    ari_set_objref_path_intid(item, REFDA_ADM_IETF_DTNMA_AGENT_ENUM, ARI_TYPE_EDD, 1);
                }
                // FIXME: should be total
                // amm:init-value "/AC/(./EDD/sw-vendor,./EDD/sw-version,./EDD/capability)";

                ari_set_ac(&(objdata->value), &acinit);
            }

            obj = refda_register_const(adm, cace_amm_obj_id_withenum("hello", 0), objdata);
            // no parameters
        }

        /**
         * Register EDD objects
         */
        {
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            amm_type_set_use_direct(&(objdata->prod_type), amm_type_get_builtin(ARI_TYPE_TEXTSTR));
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_version;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("sw-version", 1), objdata);
            // no parameters
        }

        /**
         * Register CTRL objects
         */
        { // ari://ietf-dtnma-agent/CTRL/inspect
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_inspect;
            {
                // Result type ari://ietf-amm/TYPEDEF/any
                ari_t ref = ARI_INIT_UNDEFINED;
                ari_set_objref_path_intid(&ref, REFDA_ADM_IETF_AMM_ENUM, ARI_TYPE_TYPEDEF, 8);
                amm_type_set_use_ref_move(&(objdata->res_type), &ref);
            }

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("inspect", 5), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "ref");

                amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_OBJECT));
                // FIXME: above should really be a type use of //ietf-amm/TYPEDEF/VALUE-OBJ
            }
        }
        {
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_for;
            // No result type

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("wait-for", 2), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "duration");

                amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_TD));
                // FIXME: above should really be a type use of /ARITYPE/TD
            }
        }
    }

    if (pthread_mutex_unlock(&(agent->objs_mutex)))
    {
        return 2;
    }
    return 0;
}
