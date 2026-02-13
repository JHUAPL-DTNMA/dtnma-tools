/*
 * Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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
#include "cace/ari/algo.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"

static cace_ari_translate_result_t refda_valprod_label_subst_map(cace_ari_t *out, const cace_ari_t *in,
                                                                 const cace_ari_translate_ctx_t *ctx)
{
    if (cace_ari_is_lit_typed(in, CACE_ARI_TYPE_LABEL))
    {
        const cace_ari_itemized_t *aparams = ctx->user_data;

        const cace_ari_t *aparam  = NULL;
        const char       *as_text = cace_ari_cget_tstr_cstr(in);
        cace_ari_int      as_int;
        if (as_text)
        {
            cace_ari_t *const *aparam_ptr = cace_named_ari_ptr_dict_cget(aparams->named, as_text);
            if (aparam_ptr)
            {
                aparam = *aparam_ptr;
            }
            else
            {
                CACE_LOG_DEBUG("LABEL value %s is not an actual parameter", as_text);
            }
        }
        else if (!cace_ari_get_int(in, &as_int))
        {
            if (as_int >= 0)
            {
                aparam = cace_ari_array_cget(aparams->ordered, as_int);
            }

            if (!aparam)
            {
                CACE_LOG_DEBUG("LABEL value %" PRId32 " is not an actual parameter", as_int);
            }
        }
        else
        {
            CACE_LOG_ERR("invalid LABEL primitive type");
        }

        if (aparam)
        {
            cace_ari_set_copy(out, aparam);
            return CACE_ARI_TRANSLATE_FINAL;
        }
        else
        {
            // does not represent a formal parameter, keep it
            cace_ari_set_copy(out, in);
            return CACE_ARI_TRANSLATE_FINAL;
        }
    }
    return CACE_ARI_TRANSLATE_DEFAULT;
}

/** Perform LABEL substitution in the produced value.
 */
static int refda_valprod_label_subst(refda_valprod_ctx_t *ctx)
{
    cace_ari_translator_t translator = { .map_ari = refda_valprod_label_subst_map };

    // operate on temporary value
    cace_ari_t src;
    cace_ari_init_move(&src, &ctx->value);
    cace_ari_init(&ctx->value);

    int res = cace_ari_translate(&ctx->value, &src, &translator, (void *)&(ctx->deref->aparams));
    cace_ari_deinit(&src);
    if (res)
    {
        CACE_LOG_ERR("Unable to translate produced value, error %d", res);
    }
    return res;
}

static int refda_valprod_const_run(const refda_amm_const_desc_t *cnst, refda_valprod_ctx_t *ctx)
{
    CHKERR1(cnst);
    CHKERR1(ctx);

    cace_ari_set_copy(&(ctx->value), &(cnst->value));
    refda_valprod_label_subst(ctx);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(ctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    return 0;
}

static int refda_valprod_var_run(const refda_amm_var_desc_t *var, refda_valprod_ctx_t *ctx)
{
    CHKERR1(var);
    CHKERR1(ctx);

    cace_ari_set_copy(&(ctx->value), &(var->value));
    refda_valprod_label_subst(ctx);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(ctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", m_string_get_cstr(buf));
        m_string_clear(buf);
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
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(prodctx->value), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    return 0;
}

void refda_valprod_ctx_init(refda_valprod_ctx_t *obj, refda_runctx_t *runctx, const cace_ari_t *ref,
                            const cace_amm_lookup_t *deref)
{
    CHKVOID(obj);

    obj->runctx = runctx;
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
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, ctx->ref, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production for object %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    // access check, this permission has no parameters
    refda_amm_ident_base_ptr_set_t acl_match;
    refda_amm_ident_base_ptr_set_init(acl_match);
    bool acl_found = refda_acl_search_one_permission(ctx->runctx->agent, ctx->runctx->acl_groups, ctx->deref->obj,
                                                     ctx->runctx->agent->acl.perm_produce, acl_match);
    refda_amm_ident_base_ptr_set_clear(acl_match);
    if (!acl_found)
    {
        CACE_LOG_ERR("Lack of permission for: produce");
        return 3;
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
