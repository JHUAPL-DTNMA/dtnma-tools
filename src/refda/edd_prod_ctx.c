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
#include "edd_prod_ctx.h"
#include "cace/util/logging.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"

void refda_edd_prod_ctx_init(refda_edd_prod_ctx_t *obj, const refda_amm_edd_desc_t *edd, refda_valprod_ctx_t *prodctx)
{
    CHKVOID(obj);

    obj->prodctx = prodctx;
    obj->edd     = edd;
}

void refda_edd_prod_ctx_deinit(refda_edd_prod_ctx_t *obj)
{
    CHKVOID(obj);
}

const ari_t *refda_edd_prod_ctx_get_aparam_index(const refda_edd_prod_ctx_t *ctx, size_t index)
{
    return ari_array_cget(ctx->prodctx->deref->aparams.ordered, index);
}

const ari_t *refda_edd_prod_ctx_get_aparam_name(const refda_edd_prod_ctx_t *ctx, const char *name)
{
    return *named_ari_ptr_dict_cget(ctx->prodctx->deref->aparams.named, name);
}

void refda_edd_prod_ctx_set_result_move(refda_edd_prod_ctx_t *ctx, ari_t *value)
{
    ari_set_move(&(ctx->prodctx->value), value);
}

void refda_edd_prod_ctx_set_result_copy(refda_edd_prod_ctx_t *ctx, const ari_t *value)
{
    ari_set_copy(&(ctx->prodctx->value), value);
}
