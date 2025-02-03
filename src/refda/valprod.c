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
#include "valprod.h"
#include "edd_prod_ctx.h"
#include "amm/const.h"
#include "amm/var.h"
#include "amm/edd.h"
#include "cace/ari/type.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"

static int refda_valprod_const_run(const refda_amm_const_desc_t *obj, refda_valprod_ctx_t *ctx)
{
    CHKERR1(obj);
    CHKERR1(ctx);

    cace_ari_set_copy(&(ctx->value), &(obj->value));
    // FIXME use ctx parameters to substitute

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(ctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", string_get_cstr(buf));
        string_clear(buf);
    }

    return 0;
}

static int refda_valprod_var_run(const refda_amm_var_desc_t *obj, refda_valprod_ctx_t *ctx)
{
    CHKERR1(obj);
    CHKERR1(ctx);

    cace_ari_set_copy(&(ctx->value), &(obj->value));
    // FIXME use ctx parameters to substitute

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(ctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", string_get_cstr(buf));
        string_clear(buf);
    }

    return 0;
}

static int refda_valprod_edd_run(const refda_amm_edd_desc_t *obj, refda_valprod_ctx_t *prodctx)
{
    CHKERR1(obj)
    CHKERR1(prodctx)
    CHKERR1(cace_amm_type_is_valid(&(obj->prod_type)))
    CHKERR1(obj->produce)

    refda_edd_prod_ctx_t eddctx;
    refda_edd_prod_ctx_init(&eddctx, obj, prodctx);

    (obj->produce)(&eddctx);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(prodctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", string_get_cstr(buf));
        string_clear(buf);
    }

    return 0;
}

void refda_valprod_ctx_init(refda_valprod_ctx_t *obj, refda_runctx_t *parent, const cace_ari_t *ref,
                            const cace_amm_lookup_t *deref)
{
    CHKVOID(obj);
    CHKVOID(deref);

    obj->parent = parent;
    obj->ref    = ref;
    obj->deref  = deref;
    cace_ari_init(&(obj->value));
}

void refda_valprod_ctx_deinit(refda_valprod_ctx_t *obj)
{
    CHKVOID(obj);
    cace_ari_deinit(&(obj->value));
    memset(obj, 0, sizeof(refda_valprod_ctx_t));
}

int refda_valprod_run(refda_valprod_ctx_t *ctx)
{
    CHKERR1(ctx);
    CHKERR1(ctx->deref);
    CHKERR1(ctx->deref->obj);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, ctx->ref, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production for object %s", string_get_cstr(buf));
        string_clear(buf);
    }

    int retval = 0;
    switch (ctx->deref->obj_type)
    {
        case CACE_ARI_TYPE_CONST:
        {
            refda_amm_const_desc_t *cnst = ctx->deref->obj->app_data.ptr;

            retval = refda_valprod_const_run(cnst, ctx);
            break;
        }
        case CACE_ARI_TYPE_VAR:
        {
            refda_amm_var_desc_t *var = ctx->deref->obj->app_data.ptr;

            retval = refda_valprod_var_run(var, ctx);
            break;
        }
        case CACE_ARI_TYPE_EDD:
        {
            refda_amm_edd_desc_t *edd = ctx->deref->obj->app_data.ptr;

            retval = refda_valprod_edd_run(edd, ctx);
            break;
        }
        default:
            retval = 2;
            break;
    }
    CACE_LOG_DEBUG("Producing status %d", retval);
    return retval;
}
