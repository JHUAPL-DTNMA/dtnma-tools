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
#include "exec_seq.h"
#include <cace/util/defs.h>
#include "ctrl_exec_ctx.h"
#include "agent.h"
#include "runctx.h"

void refda_exec_item_resume(refda_exec_item_t *obj)
{
    atomic_store(&obj->execution_stage, EXEC_COMPLETE);
    sem_post(&refda_runctx_ptr_ref(obj->seq->runctx)->agent->execs_sem);
}

void refda_exec_item_init(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    obj->seq = NULL;
    cace_ari_init(&(obj->ref));
    cace_amm_lookup_init(&(obj->deref));
    atomic_init(&(obj->execution_stage), EXEC_PENDING);
    cace_ari_init(&(obj->result));
}

void refda_exec_item_init_set(refda_exec_item_t *obj, const refda_exec_item_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    obj->seq = NULL;
    cace_ari_init_copy(&(obj->ref), &(src->ref));
    cace_amm_lookup_init_set(&(obj->deref), &(src->deref));
    atomic_init(&(obj->execution_stage), atomic_load(&(src->execution_stage)));
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
    atomic_store(&(obj->execution_stage), atomic_load(&(src->execution_stage)));
    cace_ari_set_copy(&(obj->result), &(src->result));
}
