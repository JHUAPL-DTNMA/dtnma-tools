/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "exec_seq.h"
#include <cace/util/defs.h>

void refda_exec_seq_init(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    obj->runctx = refda_runctx_ptr_new();
    obj->pid    = 0;
    refda_exec_item_list_init(obj->items);
    obj->status = NULL;
}

void refda_exec_seq_deinit(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    obj->status = NULL;
    refda_exec_item_list_clear(obj->items);
    obj->pid = 0;
    refda_runctx_ptr_clear(obj->runctx);
}

int refda_exec_seq_cmp(const refda_exec_seq_t *lt, const refda_exec_seq_t *rt)
{
    CHKRET(lt, 1);
    CHKRET(rt, -1);
    if (lt->pid < rt->pid)
    {
        return -1;
    }
    else if (lt->pid > rt->pid)
    {
        return 1;
    }
    return 0;
}
