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
#include "obj_base.h"

void cace_amm_obj_desc_init(cace_amm_obj_desc_t *obj)
{
    obj->has_enum = false;
    string_init(obj->name);
    ari_formal_param_list_init(obj->fparams);
}

void cace_amm_obj_desc_deinit(cace_amm_obj_desc_t *obj)
{
    ari_formal_param_list_clear(obj->fparams);
    string_clear(obj->name);
}
