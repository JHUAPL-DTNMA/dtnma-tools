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
    ari_init(&(obj->ref));
    cace_amm_lookup_init(&(obj->deref));
    atomic_init(&(obj->waiting), false);
}

void refda_exec_item_deinit(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    cace_amm_lookup_deinit(&(obj->deref));
    ari_deinit(&(obj->ref));
}

void refda_exec_seq_init(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    refda_exec_item_list_init(obj->items);
}

void refda_exec_seq_deinit(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    refda_exec_item_list_clear(obj->items);
}
