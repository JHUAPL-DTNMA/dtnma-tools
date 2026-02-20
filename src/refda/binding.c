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
#include "binding.h"
#include "amm/ident.h"
#include "amm/typedef.h"
#include "amm/const.h"
#include "amm/var.h"
#include "amm/edd.h"
#include "amm/ctrl.h"
#include "amm/oper.h"
#include "amm/sbr.h"
#include "amm/tbr.h"
#include <cace/amm/parameters.h>
#include <cace/amm/lookup.h>
#include <cace/amm/semtype.h>
#include <cace/ari.h>
#include <cace/ari/text.h>
#include <cace/util/defs.h>
#include <cace/util/logging.h>

static int refda_binding_semtype_use(const refda_binding_ctx_t *ctx, cace_amm_semtype_use_t *semtype)
{
    // do not rebind
    if (semtype->base)
    {
        return 0;
    }

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &(semtype->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Binding use of %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    int failcnt = 0;
    if (semtype->name.is_ref)
    {
        cace_amm_lookup_t deref;
        cace_amm_lookup_init(&deref);

        if (!cace_amm_lookup_deref(&deref, ctx->store, &(semtype->name)))
        {
            if (deref.obj_type == CACE_ARI_TYPE_TYPEDEF)
            {
                refda_amm_typedef_desc_t *desc = deref.obj->app_data.ptr;
                if (desc)
                {
                    semtype->base = &(desc->typeobj);
                }
                else
                {
                    CACE_LOG_CRIT("Binding failed because object has no TYPEDEF descriptor");
                    failcnt += 1;
                }
            }
            else
            {
                m_string_t buf;
                m_string_init(buf);
                cace_ari_text_encode(buf, &(semtype->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_WARNING("Binding failed because object is not a TYPEDEF, referenced as %s",
                                 m_string_get_cstr(buf));
                m_string_clear(buf);

                failcnt += 1;
            }
        }
        else
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &(semtype->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_WARNING("Binding failed because type use lookup failed, need %s", m_string_get_cstr(buf));
            m_string_clear(buf);

            failcnt += 1;
        }

        cace_amm_lookup_deinit(&deref);
    }
    else
    {
        const int64_t *aritype = cace_ari_get_aritype_int(&(semtype->name));
        if (aritype)
        {
            semtype->base = cace_amm_type_get_builtin((cace_ari_type_t)*aritype);
        }
        else
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &(semtype->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_WARNING("Binding failed because literal is not an ARITYPE, is %s", m_string_get_cstr(buf));
            m_string_clear(buf);

            failcnt += 1;
        }
    }

    CACE_LOG_DEBUG("bound to %p class %d", semtype->base, (semtype->base ? (int)(semtype->base->type_class) : -1));
    return failcnt;
}

static int refda_binding_semtype_ulist(const refda_binding_ctx_t *ctx, cace_amm_semtype_ulist_t *semtype)
{
    return refda_binding_typeobj(ctx, &(semtype->item_type));
}

static int refda_binding_semtype_dlist(const refda_binding_ctx_t *ctx, cace_amm_semtype_dlist_t *semtype)
{
    int failcnt = 0;

    cace_amm_type_array_it_t it;
    for (cace_amm_type_array_it(it, semtype->types); !cace_amm_type_array_end_p(it); cace_amm_type_array_next(it))
    {
        cace_amm_type_t *typeobj = cace_amm_type_array_ref(it);

        failcnt += refda_binding_typeobj(ctx, typeobj);
    }
    return failcnt;
}

static int refda_binding_semtype_umap(const refda_binding_ctx_t *ctx, cace_amm_semtype_umap_t *semtype)
{
    int failcnt = 0;
    failcnt += refda_binding_typeobj(ctx, &(semtype->key_type));
    failcnt += refda_binding_typeobj(ctx, &(semtype->val_type));
    return failcnt;
}

static int refda_binding_semtype_tblt(const refda_binding_ctx_t *ctx, cace_amm_semtype_tblt_t *semtype)
{
    int failcnt = 0;

    cace_amm_named_type_array_it_t it;
    for (cace_amm_named_type_array_it(it, semtype->columns); !cace_amm_named_type_array_end_p(it);
         cace_amm_named_type_array_next(it))
    {
        cace_amm_named_type_t *col = cace_amm_named_type_array_ref(it);

        failcnt += refda_binding_typeobj(ctx, &(col->typeobj));
    }
    return failcnt;
}

static int refda_binding_semtype_union(const refda_binding_ctx_t *ctx, cace_amm_semtype_union_t *semtype)
{
    int failcnt = 0;

    cace_amm_type_array_it_t it;
    for (cace_amm_type_array_it(it, semtype->choices); !cace_amm_type_array_end_p(it); cace_amm_type_array_next(it))
    {
        cace_amm_type_t *choice = cace_amm_type_array_ref(it);

        failcnt += refda_binding_typeobj(ctx, choice);
    }
    return failcnt;
}

static int refda_binding_semtype_seq(const refda_binding_ctx_t *ctx, cace_amm_semtype_seq_t *semtype)
{
    return refda_binding_typeobj(ctx, &(semtype->item_type));
}

int refda_binding_typeobj(const refda_binding_ctx_t *ctx, cace_amm_type_t *typeobj)
{
    switch (typeobj->type_class)
    {
        case CACE_AMM_TYPE_INVALID:
            CACE_LOG_WARNING("Binding failed due to default-initialized typeobj");
            return 1;
        case CACE_AMM_TYPE_USE:
            return refda_binding_semtype_use(ctx, typeobj->as_semtype);
        case CACE_AMM_TYPE_ULIST:
            return refda_binding_semtype_ulist(ctx, typeobj->as_semtype);
        case CACE_AMM_TYPE_DLIST:
            return refda_binding_semtype_dlist(ctx, typeobj->as_semtype);
        case CACE_AMM_TYPE_UMAP:
            return refda_binding_semtype_umap(ctx, typeobj->as_semtype);
        case CACE_AMM_TYPE_TBLT:
            return refda_binding_semtype_tblt(ctx, typeobj->as_semtype);
        case CACE_AMM_TYPE_UNION:
            return refda_binding_semtype_union(ctx, typeobj->as_semtype);
        case CACE_AMM_TYPE_SEQ:
            return refda_binding_semtype_seq(ctx, typeobj->as_semtype);
        default:
            CACE_LOG_WARNING("Binding failed due to invalid typeobj %d", typeobj->type_class);
            return 1;
    }
}

static int refda_binding_fparams(const refda_binding_ctx_t *ctx, cace_amm_formal_param_list_t fparams)
{
    int failcnt = 0;

    cace_amm_formal_param_list_it_t fit;
    for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit);
         cace_amm_formal_param_list_next(fit))
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_ref(fit);
        CACE_LOG_DEBUG("Binding formal parameter \"%s\" (index %zd)", m_string_get_cstr(fparam->name), fparam->index);
        failcnt += refda_binding_typeobj(ctx, &(fparam->typeobj));
    }

    return failcnt;
}

static int refda_binding_ident_bases(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj,
                                     refda_amm_ident_desc_t *desc)
{
    int failcnt = 0;

    refda_amm_ident_base_list_it_t it;
    for (refda_amm_ident_base_list_it(it, desc->bases); !refda_amm_ident_base_list_end_p(it);
         refda_amm_ident_base_list_next(it))
    {
        refda_amm_ident_base_t *base = refda_amm_ident_base_list_ref(it);

        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &(base->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("Binding IDENT base of %s", m_string_get_cstr(buf));
            m_string_clear(buf);
        }

        if (!refda_amm_ident_base_populate(base, NULL, ctx->store))
        {
            if (base->deref.obj_type == CACE_ARI_TYPE_IDENT)
            {
                refda_amm_ident_desc_t *base_desc = base->deref.obj->app_data.ptr;
                if (base_desc)
                {
                    // forward reference
                    base->ident = base_desc;

                    // reverse reference
                    cace_amm_lookup_t *obj_deref = cace_amm_lookup_list_push_new(base_desc->derived);
                    obj_deref->ns                = ctx->ns;
                    obj_deref->obj_type          = CACE_ARI_TYPE_IDENT;
                    obj_deref->obj               = obj;
                }
                else
                {
                    CACE_LOG_WARNING("Binding failed because object has no IDENT descriptor");
                    failcnt += 1;
                }
            }
            else
            {
                m_string_t buf;
                m_string_init(buf);
                cace_ari_text_encode(buf, &(base->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_WARNING("Binding failed because object is not an IDENT, referenced as %s",
                                 m_string_get_cstr(buf));
                m_string_clear(buf);

                failcnt += 1;
            }
        }
        else
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &(base->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_WARNING("Binding failed because IDENT base lookup failed, need %s", m_string_get_cstr(buf));
            m_string_clear(buf);

            failcnt += 1;
        }
    }

    return failcnt;
}

int refda_binding_ident(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_ident_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_fparams(ctx, obj->fparams);
    failcnt += refda_binding_ident_bases(ctx, obj, desc);
    return failcnt;
}

int refda_binding_typedef(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_typedef_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_typeobj(ctx, &(desc->typeobj));
    return failcnt;
}

int refda_binding_const(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_const_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_fparams(ctx, obj->fparams);
    return failcnt;
}

int refda_binding_var(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_var_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_typeobj(ctx, &(desc->val_type));
    failcnt += refda_binding_fparams(ctx, obj->fparams);
    return failcnt;
}

int refda_binding_edd(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_edd_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_typeobj(ctx, &(desc->prod_type));
    failcnt += refda_binding_fparams(ctx, obj->fparams);
    return failcnt;
}

int refda_binding_ctrl(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_ctrl_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    if (cace_amm_type_is_valid(&(desc->res_type)))
    {
        // optional
        failcnt += refda_binding_typeobj(ctx, &(desc->res_type));
    }
    failcnt += refda_binding_fparams(ctx, obj->fparams);
    return failcnt;
}

int refda_binding_oper(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_oper_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;

    cace_amm_named_type_array_it_t ait;
    for (cace_amm_named_type_array_it(ait, desc->operand_types); !cace_amm_named_type_array_end_p(ait);
         cace_amm_named_type_array_next(ait))
    {
        cace_amm_named_type_t *operand = cace_amm_named_type_array_ref(ait);
        CACE_LOG_DEBUG("Binding operand \"%s\"", m_string_get_cstr(operand->name));
        failcnt += refda_binding_typeobj(ctx, &(operand->typeobj));
    }
    failcnt += refda_binding_typeobj(ctx, &(desc->res_type));
    failcnt += refda_binding_fparams(ctx, obj->fparams);
    return failcnt;
}

int refda_binding_sbr(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_sbr_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    return failcnt;
}

int refda_binding_tbr(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    refda_amm_tbr_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    return failcnt;
}

int refda_binding_obj(const refda_binding_ctx_t *ctx, cace_ari_type_t obj_type, cace_amm_obj_desc_t *obj)
{
    CACE_LOG_DEBUG("Binding object ./%s/%s", cace_ari_type_to_name(obj_type), m_string_get_cstr(obj->obj_id.name));
    switch (obj_type)
    {
        case CACE_ARI_TYPE_IDENT:
            return refda_binding_ident(ctx, obj);
        case CACE_ARI_TYPE_TYPEDEF:
            return refda_binding_typedef(ctx, obj);
        case CACE_ARI_TYPE_CONST:
            return refda_binding_const(ctx, obj);
        case CACE_ARI_TYPE_VAR:
            return refda_binding_var(ctx, obj);
        case CACE_ARI_TYPE_EDD:
            return refda_binding_edd(ctx, obj);
        case CACE_ARI_TYPE_CTRL:
            return refda_binding_ctrl(ctx, obj);
        case CACE_ARI_TYPE_OPER:
            return refda_binding_oper(ctx, obj);
        case CACE_ARI_TYPE_SBR:
            return refda_binding_sbr(ctx, obj);
        case CACE_ARI_TYPE_TBR:
            return refda_binding_tbr(ctx, obj);
        default:
            CACE_LOG_WARNING("Binding failed due to invalid obj-type %d", obj_type);
            return 1;
    }
}
