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
#include "exec_item.h"
#include <cace/util/defs.h>

void refda_exec_item_init(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    obj->seq = NULL;
    cace_ari_init(&(obj->ref));
    cace_amm_lookup_init(&(obj->deref));
    atomic_init(&(obj->waiting), false);
    cace_ari_init(&(obj->result));
}

void refda_exec_item_init_set(refda_exec_item_t *obj, const refda_exec_item_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    obj->seq = NULL;
    cace_ari_init_copy(&(obj->ref), &(src->ref));
    cace_amm_lookup_init_set(&(obj->deref), &(src->deref));
    atomic_init(&(obj->waiting), atomic_load(&(src->waiting)));
    cace_ari_init_copy(&(obj->result), &(src->result));
}

void refda_exec_item_deinit(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    cace_ari_deinit(&(obj->result));
    cace_amm_lookup_deinit(&(obj->deref));
    cace_ari_deinit(&(obj->ref));
    obj->seq = NULL;
}

void refda_exec_item_set(refda_exec_item_t *obj, const refda_exec_item_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    obj->seq = src->seq;
    cace_ari_set_copy(&(obj->ref), &(src->ref));
    cace_amm_lookup_set(&(obj->deref), &(src->deref));
    atomic_store(&(obj->waiting), atomic_load(&(src->waiting)));
    cace_ari_set_copy(&(obj->result), &(src->result));
}
