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

bool refda_edd_prod_ctx_has_aparam_undefined(const refda_edd_prod_ctx_t *ctx)
{
    CHKFALSE(ctx);
    return ctx->prodctx->deref->aparams.any_undefined;
}

const cace_ari_t *refda_edd_prod_ctx_get_aparam_index(const refda_edd_prod_ctx_t *ctx, size_t index)
{
    CHKNULL(ctx);
    return cace_ari_array_cget(ctx->prodctx->deref->aparams.ordered, index);
}

const cace_ari_t *refda_edd_prod_ctx_get_aparam_name(const refda_edd_prod_ctx_t *ctx, const char *name)
{
    CHKNULL(ctx);
    return *cace_named_ari_ptr_dict_cget(ctx->prodctx->deref->aparams.named, name);
}

static int refda_edd_prod_check_result(refda_edd_prod_ctx_t *ctx)
{
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        cace_ari_t ariname = CACE_ARI_INIT_UNDEFINED;
        const bool valid   = cace_amm_type_get_name(&(ctx->edd->prod_type), &ariname);
        if (valid)
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &ariname, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("EDD type %s", m_string_get_cstr(buf));
            m_string_clear(buf);
        }
        cace_ari_deinit(&ariname);
    }
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(ctx->prodctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("EDD result value %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    bool valid = (CACE_AMM_TYPE_MATCH_POSITIVE == cace_amm_type_match(&(ctx->edd->prod_type), &(ctx->prodctx->value)));
    if (!valid)
    {
        CACE_LOG_ERR("EDD result type failed to match a produced value");
        cace_ari_set_undefined(&(ctx->prodctx->value));
    }

    return valid ? 0 : REFDA_EDD_PROD_RESULT_TYPE_NOMATCH;
}

int refda_edd_prod_ctx_set_result_move(refda_edd_prod_ctx_t *ctx, cace_ari_t *value)
{
    CHKERR1(ctx);
    CHKERR1(value);
    cace_ari_set_move(&(ctx->prodctx->value), value);
    return refda_edd_prod_check_result(ctx);
}

int refda_edd_prod_ctx_set_result_copy(refda_edd_prod_ctx_t *ctx, const cace_ari_t *value)
{
    CHKERR1(ctx);
    CHKERR1(value);
    cace_ari_set_copy(&(ctx->prodctx->value), value);
    return refda_edd_prod_check_result(ctx);
}
