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
#include "tbr.h"

void refda_amm_tbr_desc_init(refda_amm_tbr_desc_t *obj)
{
    obj->action                      = CACE_ARI_INIT_UNDEFINED;
    obj->start_time                  = CACE_ARI_INIT_UNDEFINED;
    obj->period                      = CACE_ARI_INIT_UNDEFINED;
    obj->max_exec_count              = 0;
    obj->init_enabled                = true;
    obj->enabled                     = true;
    obj->exec_count                  = 0;
    obj->absolute_start_time.tv_sec  = 0;
    obj->absolute_start_time.tv_nsec = 0;
}

void refda_amm_tbr_desc_deinit(refda_amm_tbr_desc_t *obj)
{
    cace_ari_deinit(&(obj->action));
    cace_ari_deinit(&(obj->start_time));
    cace_ari_deinit(&(obj->period));
    memset(obj, 0, sizeof(*obj));
}

bool refda_amm_tbr_desc_reached_max_exec_count(refda_amm_tbr_desc_t *obj)
{
    // Max execution count is only applicable if greater than 0,
    // so ensure if that is the case, that execution count has
    // not exceeded the limit.
    return (obj->exec_count > 0 && obj->max_exec_count > 0 && obj->exec_count >= obj->max_exec_count);
}