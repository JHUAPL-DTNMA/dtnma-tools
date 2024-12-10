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
#include "eval_ctx.h"
#include "cace/util/defs.h"

void refda_eval_ctx_init(refda_eval_ctx_t *obj, refda_runctx_t *parent)
{
    CHKVOID(obj);
    obj->parent = parent;
    obj->deref  = NULL;
    refda_eval_list_init(obj->input);
    ari_list_init(obj->stack);
}

void refda_eval_ctx_deinit(refda_eval_ctx_t *obj)
{
    CHKVOID(obj);
    ari_list_clear(obj->stack);
    refda_eval_list_clear(obj->input);
    obj->deref  = NULL;
    obj->parent = NULL;
}

const ari_t *refda_eval_ctx_get_aparam_index(refda_eval_ctx_t *ctx, size_t index)
{
    return ari_array_cget(ctx->deref->aparams.ordered, index);
}

const ari_t *refda_eval_ctx_get_aparam_name(refda_eval_ctx_t *ctx, const char *name)
{
    return *named_ari_ptr_dict_cget(ctx->deref->aparams.named, name);
}
