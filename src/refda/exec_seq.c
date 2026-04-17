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
#include <cace/util/logging.h>
#include <cace/util/defs.h>

void refda_exec_seq_init(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    obj->runctx = refda_runctx_ptr_new();
    obj->pid    = 0;
    refda_exec_item_list_init(obj->items);
    pthread_mutex_init(&obj->items_mutex, NULL);
    obj->status = NULL;
}

void refda_exec_seq_deinit(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    obj->status = NULL;
    pthread_mutex_destroy(&obj->items_mutex);
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

int refda_exec_seq_front_status(refda_exec_item_status_t *status, refda_exec_seq_t *obj)
{
    CHKERR1(status);
    CHKERR1(obj);

    int retval = 0;
    if (pthread_mutex_lock(&obj->items_mutex))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return 2;
    }
    if (!refda_exec_item_list_empty_p(obj->items))
    {
        refda_exec_item_ptr_t **front_ptr = refda_exec_item_list_front(obj->items);
        // safe during mutex lock
        refda_exec_item_t *item = refda_exec_item_ptr_ref(*front_ptr);

        *status                 = atomic_load(&(item->execution_stage));
    }
    else
    {
        retval = 2;
    }
    if (pthread_mutex_unlock(&obj->items_mutex))
    {
        CACE_LOG_CRIT("failed to unlock mutex");
        return 2;
    }
    return retval;
}
