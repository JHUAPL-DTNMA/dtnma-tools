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
#include "ident.h"

void refda_amm_ident_base_init(refda_amm_ident_base_t *obj)
{
    cace_ari_init(&(obj->name));
    obj->ident = NULL;
}

void refda_amm_ident_base_deinit(refda_amm_ident_base_t *obj)
{
    obj->ident = NULL;
    cace_ari_deinit(&(obj->name));
}

void refda_amm_ident_desc_init(refda_amm_ident_desc_t *obj)
{
    refda_amm_ident_base_list_init(obj->bases);
    cace_amm_user_data_init(&(obj->user_data));
}

void refda_amm_ident_desc_deinit(refda_amm_ident_desc_t *obj)
{
    cace_amm_user_data_deinit(&(obj->user_data));
    refda_amm_ident_base_list_clear(obj->bases);
}
