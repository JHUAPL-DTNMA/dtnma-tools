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
#include "ctrl_exec_ctx.h"
#include "cace/util/logging.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"

void refda_ctrl_exec_ctx_init(refda_ctrl_exec_ctx_t *obj, const refda_amm_ctrl_desc_t *ctrl, refda_exec_item_t *item)
{
    CHKVOID(obj);
    CHKVOID(item);

    obj->runctx = refda_runctx_ptr_ref(item->seq->runctx);
    obj->ctrl   = ctrl;
    obj->item   = item;
}

void refda_ctrl_exec_ctx_deinit(refda_ctrl_exec_ctx_t *obj)
{
    CHKVOID(obj);
}

const ari_t *refda_ctrl_exec_ctx_get_aparam_index(refda_ctrl_exec_ctx_t *ctx, size_t index)
{
    return ari_array_cget(ctx->item->deref.aparams.ordered, index);
}

const ari_t *refda_ctrl_exec_ctx_get_aparam_name(refda_ctrl_exec_ctx_t *ctx, const char *name)
{
    return *named_ari_ptr_dict_cget(ctx->item->deref.aparams.named, name);
}

void refda_ctrl_exec_ctx_set_waiting(refda_ctrl_exec_ctx_t *ctx, const refda_timeline_event_t *event)
{
    CHKVOID(ctx);
    atomic_store(&(ctx->item->waiting), true);

    if (event)
    {
        refda_timeline_push(ctx->runctx->agent->exec_timeline, *event);
    }
}

void refda_ctrl_exec_ctx_set_result_copy(refda_ctrl_exec_ctx_t *ctx, const ari_t *value)
{
    ari_set_copy(&(ctx->item->result), value);
}

void refda_ctrl_exec_ctx_set_result_move(refda_ctrl_exec_ctx_t *ctx, ari_t *value)
{
    ari_set_move(&(ctx->item->result), value);
}

void refda_ctrl_exec_ctx_set_result_null(refda_ctrl_exec_ctx_t *ctx)
{
    ari_set_null(&(ctx->item->result));
}
