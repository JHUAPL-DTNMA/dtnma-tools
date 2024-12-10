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
#include "exec_ctx.h"
#include "cace/util/defs.h"

void refda_exec_ctx_init(refda_exec_ctx_t *obj, refda_exec_item_t *item)
{
    CHKVOID(obj);
    CHKVOID(item);

    obj->runctx = refda_runctx_ptr_ref(item->seq->runctx);
    obj->item   = item;
}

void refda_exec_ctx_deinit(refda_exec_ctx_t *obj)
{
    CHKVOID(obj);
}

const ari_t *refda_exec_ctx_get_aparam_index(refda_exec_ctx_t *ctx, size_t index)
{
    return ari_array_cget(ctx->item->deref.aparams.ordered, index);
}

const ari_t *refda_exec_ctx_get_aparam_name(refda_exec_ctx_t *ctx, const char *name)
{
    return *named_ari_ptr_dict_cget(ctx->item->deref.aparams.named, name);
}

void refda_exec_ctx_set_waiting(refda_exec_ctx_t *ctx)
{
    CHKVOID(ctx);
    atomic_store(&(ctx->item->waiting), true);
}

void refda_exec_ctx_set_result_move(refda_exec_ctx_t *ctx, ari_t *value)
{
    ari_set_move(&(ctx->item->result), value);
}

void refda_exec_ctx_set_result_copy(refda_exec_ctx_t *ctx, const ari_t *value)
{
    ari_set_copy(&(ctx->item->result), value);
}
