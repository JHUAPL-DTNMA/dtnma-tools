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
#include "var.h"

void refda_amm_var_desc_init(refda_amm_var_desc_t *obj)
{
    cace_amm_type_init(&(obj->val_type));
    obj->value    = CACE_ARI_INIT_UNDEFINED;
    obj->init_val = CACE_ARI_INIT_UNDEFINED;
    obj->obsolete = false;
}

void refda_amm_var_desc_deinit(refda_amm_var_desc_t *obj)
{
    cace_ari_deinit(&(obj->init_val));
    cace_ari_deinit(&(obj->value));
    cace_amm_type_deinit(&(obj->val_type));
    memset(obj, 0, sizeof(*obj));
}
