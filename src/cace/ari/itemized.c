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
#include "itemized.h"
#include "cace/util/defs.h"

void cace_ari_itemized_init(cace_ari_itemized_t *obj)
{
    CHKVOID(obj);
    ari_array_init(obj->ordered);
    named_ari_ptr_dict_init(obj->named);
}

void cace_ari_itemized_init_set(cace_ari_itemized_t *obj, const cace_ari_itemized_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    ari_array_init_set(obj->ordered, src->ordered);
    named_ari_ptr_dict_init_set(obj->named, src->named);
}

void cace_ari_itemized_init_move(cace_ari_itemized_t *obj, cace_ari_itemized_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    ari_array_init_move(obj->ordered, src->ordered);
    named_ari_ptr_dict_init_move(obj->named, src->named);
}

void cace_ari_itemized_deinit(cace_ari_itemized_t *obj)
{
    CHKVOID(obj);
    named_ari_ptr_dict_clear(obj->named);
    ari_array_clear(obj->ordered);
}

void cace_ari_itemized_reset(cace_ari_itemized_t *obj)
{
    CHKVOID(obj);
    ari_array_reset(obj->ordered);
    named_ari_ptr_dict_reset(obj->named);
}
