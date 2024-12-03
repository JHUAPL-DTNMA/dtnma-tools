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
#include "cace/amm/semtype.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

int refda_adm_ietf_amm_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: ietf-amm");

    REFDA_AGENT_LOCK(agent);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(&(agent->objs), "ietf-amm", true, REFDA_ADM_IETF_AMM_ENUM);
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register IDENT objects
         */

        obj = refda_register_ident(adm, cace_amm_obj_id_withenum("display-hint", 0), NULL);

        /**
         * Register TYPEDEF objects
         */
        { // ari://ietf-amm/TYPEDEF/type-ref
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);

            amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_ARITYPE));
            }
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_TYPEDEF));
            }

            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("type-ref", 0), objdata);
            // no parameters
        }
        { // ari://ietf-amm/TYPEDEF/any
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);

            amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_LITERAL));
            }
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_OBJECT));
            }

            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("any", 8), objdata);
            // no parameters
        }
        { // ari://ietf-amm/TYPEDEF/value-obj
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);

            amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 3);
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_CONST));
            }
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_EDD));
            }
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 2);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_VAR));
            }

            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("value-obj", 9), objdata);
            // no parameters
        }
        { // ari://ietf-amm/TYPEDEF/exec-item
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);

            amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_CTRL));
            }
            {
                amm_type_t *choice = amm_type_array_get(semtype->choices, 1);

                ari_t ref = ARI_INIT_UNDEFINED;
                // Type ari://ietf-amm/TYPEDEF/value-obj
                ari_set_objref_path_intid(&ref, REFDA_ADM_IETF_AMM_ENUM, ARI_TYPE_TYPEDEF, 9);
                amm_type_set_use_ref_move(choice, &ref);
            }

            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("exec-item", 20), objdata);
            // no parameters
        }
        { // ari://ietf-amm/TYPEDEF/mac
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);

            amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(objdata->typeobj));
            {
                ari_t ref = ARI_INIT_UNDEFINED;
                // Type ari://ietf-amm/TYPEDEF/exec-item
                ari_set_objref_path_intid(&ref, REFDA_ADM_IETF_AMM_ENUM, ARI_TYPE_TYPEDEF, 20);
                amm_type_set_use_ref_move(&(semtype->item_type), &ref);
            }

            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("mac", 21), objdata);
            // no parameters
        }
    }

    REFDA_AGENT_UNLOCK(agent);
    return 0;
}
