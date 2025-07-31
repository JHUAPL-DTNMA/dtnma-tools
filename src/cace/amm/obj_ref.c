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
#include "obj_ref.h"

void cace_amm_obj_ref_init(cace_amm_obj_ref_t *obj)
{
    cace_ari_init(&(obj->ref));
    obj->obj = NULL;
}

void cace_amm_obj_ref_deinit(cace_amm_obj_ref_t *obj)
{
    obj->obj = NULL;
    cace_ari_deinit(&(obj->ref));
}

void cace_amm_obj_ref_init_set(cace_amm_obj_ref_t *obj, const cace_amm_obj_ref_t *src)
{
    cace_ari_init(&(obj->ref));
    cace_ari_set_copy(&(obj->ref), &(src->ref));
    obj->obj = NULL;
}

void cace_amm_obj_ref_set(cace_amm_obj_ref_t *obj, const cace_amm_obj_ref_t *src)
{
    cace_ari_set_copy(&(obj->ref), &(src->ref));
    obj->obj = src->obj;
}
