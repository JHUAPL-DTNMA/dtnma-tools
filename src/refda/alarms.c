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
#include "alarms.h"
#include "agent.h"
#include "cace/ari/time_util.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_alarms_entry_init(refda_alarms_entry_t *obj)
{
    refda_amm_ident_base_init(&obj->resource);
    refda_amm_ident_base_init(&obj->category);
    obj->severity = 0; // TODO
    cace_ari_init(&obj->created_at);
    cace_ari_init(&obj->updated_at);
    obj->mgr_state = 0; // TODO
    obj->mgr_ident = CACE_ARI_INIT_NULL;
    cace_ari_init(&obj->mgr_state_at);
}

void refda_alarms_entry_deinit(refda_alarms_entry_t *obj)
{
    cace_ari_deinit(&obj->mgr_state_at);
    cace_ari_deinit(&obj->mgr_ident);
    cace_ari_deinit(&obj->updated_at);
    cace_ari_deinit(&obj->created_at);
    refda_amm_ident_base_deinit(&obj->category);
    refda_amm_ident_base_deinit(&obj->resource);
}

int refda_alarms_entry_key_cmp(const refda_alarms_entry_key_t *left, const refda_alarms_entry_key_t *right)
{
    int res = M_CMP_DEFAULT(left->resource, right->resource);
    if (res)
    {
        return res;
    }
    res = M_CMP_DEFAULT(left->category, right->category);
    return res;
}

void refda_alarms_init(refda_alarms_t *obj)
{
    refda_alarms_entry_list_init(obj->alarm_list);
    refda_alarms_entry_index_init(obj->alarm_index);
    pthread_mutex_init(&(obj->alarm_mutex), NULL);
}

void refda_alarms_deinit(refda_alarms_t *obj)
{
    pthread_mutex_destroy(&(obj->alarm_mutex));
    refda_alarms_entry_index_clear(obj->alarm_index);
    refda_alarms_entry_list_clear(obj->alarm_list);
}

void refda_alarms_set_refs(refda_agent_t *agent, const cace_ari_t *resource, const cace_ari_t *category,
                      refda_alarms_severity_t severity)
{
    refda_amm_ident_base_t res_ref, cat_ref;
    refda_amm_ident_base_init(&res_ref);
    refda_amm_ident_base_init(&cat_ref);

    // validate lookup fields only
    REFDA_AGENT_LOCK(agent, );
    refda_amm_ident_base_populate(&res_ref, resource, &agent->objs);
    if (!cace_ari_is_null(category))
    {
        refda_amm_ident_base_populate(&cat_ref, category, &agent->objs);
    }
    else
    {
        cace_ari_set_null(&cat_ref.name);
    }
    REFDA_AGENT_UNLOCK(agent, );

    if (!res_ref.ident)
    {
        CACE_LOG_ERR("invalid resource reference");
        refda_amm_ident_base_deinit(&res_ref);
        refda_amm_ident_base_deinit(&cat_ref);
        return;
    }
    if (!cace_ari_is_null(&cat_ref.name) && !cat_ref.ident)
    {
        CACE_LOG_ERR("invalid category reference");
        refda_amm_ident_base_deinit(&res_ref);
        refda_amm_ident_base_deinit(&cat_ref);
        return;
    }

    if (pthread_mutex_lock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return;
    }

    refda_alarms_entry_key_t entry_key = {
        .resource = res_ref.ident,
        .category = cat_ref.ident,
    };

    refda_alarms_entry_t  *entry = NULL;
    refda_alarms_entry_t **entry_found;
    entry_found = refda_alarms_entry_index_get(agent->alarms.alarm_index, entry_key);
    if (entry_found)
    {
        entry = *entry_found;
        // done with key
        refda_amm_ident_base_deinit(&res_ref);
        refda_amm_ident_base_deinit(&cat_ref);
    }
    else
    {
        refda_alarms_entry_ptr_t *entry_ptr = *refda_alarms_entry_list_push_new(agent->alarms.alarm_list);
        // new entry with default non-key fields
        entry = refda_alarms_entry_ptr_ref(entry_ptr);
        refda_amm_ident_base_set_move(&entry->resource, &res_ref);
        refda_amm_ident_base_set_move(&entry->category, &cat_ref);

        // key on new instance
        refda_alarms_entry_index_set_at(agent->alarms.alarm_index, entry_key, entry);
    }

    // update main state
    entry->severity = severity;
    cace_get_system_time(&entry->updated_at);
    if (cace_ari_is_undefined(&entry->created_at))
    {
        // ensure first timestamp matches exactly
        cace_ari_set_copy(&entry->created_at, &entry->updated_at);
    }

    // TODO record history

    if (pthread_mutex_unlock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
    }
}

void refda_alarms_purge(refda_agent_t *agent, const cace_ari_t *expr)
{

}
