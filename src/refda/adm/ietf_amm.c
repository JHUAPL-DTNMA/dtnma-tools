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
#include "refda/amm/ident.h"
#include "refda/amm/typedef.h"
#include "cace/amm/obj_ns.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

static refda_amm_typedef_desc_t typedefs[] = { {
    .semtype = AMM_TYPE_INIT_INVALID,
} };

int refda_adm_ietf_amm_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: ietf-amm");

    if (pthread_mutex_lock(&(agent->objs_mutex)))
    {
        return 2;
    }

    cace_amm_obj_ns_t   *adm = cace_amm_obj_store_add_ns(&(agent->objs), "ietf-amm", true, 1);
    cace_amm_obj_desc_t *obj;

    cace_amm_obj_ns_add_obj(adm, ARI_TYPE_IDENT, cace_amm_obj_id_withenum("display-hint", 0));

    obj = cace_amm_obj_ns_add_obj(adm, ARI_TYPE_TYPEDEF, cace_amm_obj_id_withenum("type-ref", 0));
    {
        refda_amm_typedef_desc_t *objdata = typedefs + 0;
        cace_amm_user_data_set_from(&(obj->app_data), objdata, false,
                                    (cace_amm_user_data_deinit_f)refda_amm_typedef_desc_deinit);
    }

    if (pthread_mutex_unlock(&(agent->objs_mutex)))
    {
        return 2;
    }
    return 0;
}
