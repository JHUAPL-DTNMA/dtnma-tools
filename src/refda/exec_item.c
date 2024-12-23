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
    ari_init(&(obj->ref));
    cace_amm_lookup_init(&(obj->deref));
    atomic_init(&(obj->waiting), false);
    ari_init(&(obj->result));
}

void refda_exec_item_deinit(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->result));
    cace_amm_lookup_deinit(&(obj->deref));
    ari_deinit(&(obj->ref));
    obj->seq = NULL;
}

void refda_exec_item_set_result_copy(refda_exec_item_t *obj, const ari_t *value)
{
    CHKVOID(obj);
    ari_set_copy(&(obj->result), value);
    atomic_store(&(obj->waiting), false);
}

void refda_exec_item_set_result_move(refda_exec_item_t *obj, ari_t *value)
{
    CHKVOID(obj);
    ari_set_copy(&(obj->result), value);
    atomic_store(&(obj->waiting), false);
}

void refda_exec_item_set_result_null(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    ari_set_null(&(obj->result));
    atomic_store(&(obj->waiting), false);
}
