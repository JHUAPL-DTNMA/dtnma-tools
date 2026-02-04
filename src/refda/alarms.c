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

void refda_alarms_entry_init(refda_alarms_entry_t *obj)
{
    refda_amm_ident_base_init(&obj->resource);
    refda_amm_ident_base_init(&obj->category);
    obj->severity = 0; // TODO
    cace_ari_init(&obj->added_at);
    cace_ari_init(&obj->updated_at);
    obj->mgr_state = 0; // TODO
    cace_ari_init(&obj->mgr_ident);
    cace_ari_init(&obj->mgr_state_at);
}

void refda_alarms_entry_deinit(refda_alarms_entry_t *obj)
{
    cace_ari_deinit(&obj->mgr_state_at);
    cace_ari_deinit(&obj->mgr_ident);
    cace_ari_deinit(&obj->updated_at);
    cace_ari_deinit(&obj->added_at);
    refda_amm_ident_base_deinit(&obj->category);
    refda_amm_ident_base_deinit(&obj->resource);
}

int refda_alarms_entry_cmp(const refda_alarms_entry_t *left, const refda_alarms_entry_t *right)
{
    int res = cace_ari_cmp(&left->resource.name, &right->resource.name);
    if (res)
    {
        return res;
    }
    res = cace_ari_cmp(&left->category.name, &right->category.name);
    return res;
}

void refda_alarms_init(refda_alarms_t *obj)
{
    refda_alarms_entry_list_init(obj->alarms);
}

void refda_alarms_deinit(refda_alarms_t *obj)
{
    refda_alarms_entry_list_clear(obj->alarms);
}
