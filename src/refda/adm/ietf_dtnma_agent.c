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
#include "refda/agent.h"
#include "refda/register.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_adm_ietf_dtnma_agent_edd_sw_version(const refda_amm_edd_desc_t *obj _U_, refda_amm_valprod_ctx_t *ctx)
{
    ari_set_tstr(&(ctx->value), "0.0.0", false);
}

int refda_adm_ietf_dtnma_agent_ctrl_inspect(const refda_amm_ctrl_desc_t *obj _U_, refda_amm_exec_ctx_t *ctx)
{
    CACE_LOG_WARNING("executed!");

    const ari_t *ref = ari_array_cget(ctx->deref->aparams.ordered, 0);

    // FIXME mutex-serialize object store access
    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(ctx->agent->objs), ref);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode_objpath(buf, &(ref->as_ref.objpath), ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_DEBUG("Lookup reference to %s", string_get_cstr(buf));
        string_clear(buf);
    }
    CACE_LOG_WARNING("inspect lookup result %d", res);

    cace_amm_lookup_deinit(&deref);

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

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(&(agent->objs), "ietf-dtnma-agent", true, 1);
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register EDD objects
         */
        {
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_version;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("sw-version", 1), objdata);
            {
                cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(obj->fparams);

                fparam->index = 0;
                string_set_str(fparam->name, "ref");
                fparam->typeobj = amm_type_get_builtin(ARI_TYPE_OBJECT);
                // FIXME: above should really be a type use of //ietf-amm/TYPEDEF/VALUE-OBJ
            }
        }

        /**
         * Register CTRL objects
         */
        {
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_inspect;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("inspect", 5), objdata);
            {
                cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(obj->fparams);

                fparam->index = 0;
                string_set_str(fparam->name, "ref");
                fparam->typeobj = amm_type_get_builtin(ARI_TYPE_OBJECT);
                // FIXME: above should really be a type use of //ietf-amm/TYPEDEF/VALUE-OBJ
            }
        }
    }

    if (pthread_mutex_unlock(&(agent->objs_mutex)))
    {
        return 2;
    }
    return 0;
}
