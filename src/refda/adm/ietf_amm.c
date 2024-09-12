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
#include "cace/util/logging.h"
#include "cace/util/defs.h"

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

    /**
     * Register IDENT objects
     */

    refda_register_ident(adm, cace_amm_obj_id_withenum("display-hint", 0), NULL);

    /**
     * Register TYPEDEF objects
     */

    {
        refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
        refda_amm_typedef_desc_init(objdata);

        amm_type_set_union_size(&(objdata->semtype), 2);
        {
            amm_type_t *choice = amm_type_set_union_get(&(objdata->semtype), 0);
            amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_ARITYPE));
        }
        {
            amm_type_t *choice = amm_type_set_union_get(&(objdata->semtype), 1);
            amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_TYPEDEF));
        }

        obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("type-ref", 0), objdata);
        // no parameters
    }

    if (pthread_mutex_unlock(&(agent->objs_mutex)))
    {
        return 2;
    }
    return 0;
}
