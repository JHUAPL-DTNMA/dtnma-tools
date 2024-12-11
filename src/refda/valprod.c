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


static int refda_valprod_edd_run(const refda_amm_edd_desc_t *obj, refda_valprod_ctx_t *prodctx)
{
    CHKERR1(obj)
    CHKERR1(prodctx)
    CHKERR1(amm_type_is_valid(&(obj->prod_type)))
    CHKERR1(obj->produce)

    refda_edd_prod_ctx_t eddctx;
    refda_edd_prod_ctx_init(&eddctx, obj, prodctx);

    (obj->produce)(&eddctx);
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &(prodctx->value), ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", string_get_cstr(buf));
        string_clear(buf);
    }

    if (ari_is_undefined(&(prodctx->value)))
    {
        return 2;
    }

    // force output type
    ari_t tmp;
    ari_init(&tmp);
    int res = amm_type_convert(&(obj->prod_type), &tmp, &(prodctx->value));
    ari_set_move(&(prodctx->value), &tmp);
    if (res)
    {
        return 3;
    }

    return 0;
}

void refda_valprod_ctx_init(refda_valprod_ctx_t *obj, refda_runctx_t *parent, const cace_amm_lookup_t *deref)
{
    CHKVOID(obj);
    CHKVOID(deref);

    obj->parent = parent;
    obj->deref  = deref;
    ari_init(&(obj->value));
}

void refda_valprod_ctx_deinit(refda_valprod_ctx_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->value));
}

int refda_valprod_run(refda_valprod_ctx_t *ctx)
{
    CHKERR1(ctx);
    CHKERR1(ctx->deref);
    CHKERR1(ctx->deref->obj);
    CACE_LOG_DEBUG("Producing value for reference type %s", ari_type_to_name(ctx->deref->obj_type));

    int retval = 0;
    switch (ctx->deref->obj_type)
    {
        case ARI_TYPE_CONST:
        {
            refda_amm_const_desc_t *cnst = ctx->deref->obj->app_data.ptr;
            retval                       = refda_amm_const_desc_produce(cnst, ctx);
            break;
        }
        case ARI_TYPE_VAR:
        {
            refda_amm_var_desc_t *var = ctx->deref->obj->app_data.ptr;
            retval                    = refda_amm_var_desc_produce(var, ctx);
            break;
        }
        case ARI_TYPE_EDD:
        {
            refda_amm_edd_desc_t *edd = ctx->deref->obj->app_data.ptr;
            retval                    = refda_valprod_edd_run(edd, ctx);
            break;
        }
        default:
            retval = 2;
            break;
    }
    CACE_LOG_DEBUG("Producing status %d", retval);
    return retval;
}
