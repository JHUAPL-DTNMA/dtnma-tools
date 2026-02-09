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
#include "algo.h"
#include "containers.h"
#include "text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include "cace/amm/numeric.h"
#include "cace/amm/typing.h"
#include <math.h>

static int cace_ari_visit_ari(cace_ari_t *ari, const cace_ari_visitor_t *visitor, const cace_ari_visit_ctx_t *ctx);

static int cace_ari_visit_ac(cace_ari_ac_t *obj, const cace_ari_visitor_t *visitor, const cace_ari_visit_ctx_t *ctx)
{
    int retval;

    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, obj->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        cace_ari_t *item = cace_ari_list_ref(it);

        retval = cace_ari_visit_ari(item, visitor, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_visit_am(cace_ari_am_t *obj, const cace_ari_visitor_t *visitor, cace_ari_visit_ctx_t *ctx)
{
    int retval;

    cace_ari_tree_it_t it;
    for (cace_ari_tree_it(it, obj->items); !cace_ari_tree_end_p(it); cace_ari_tree_next(it))
    {
        cace_ari_tree_subtype_ct *pair = cace_ari_tree_ref(it);

        ctx->is_map_key = true;
        retval          = cace_ari_visit_ari(pair->key_ptr, visitor, ctx);
        CHKERRVAL(retval);

        ctx->is_map_key = false;
        retval          = cace_ari_visit_ari(pair->value_ptr, visitor, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_visit_tbl(cace_ari_tbl_t *obj, const cace_ari_visitor_t *visitor, const cace_ari_visit_ctx_t *ctx)
{
    int retval;

    cace_ari_array_it_t it;
    for (cace_ari_array_it(it, obj->items); !cace_ari_array_end_p(it); cace_ari_array_next(it))
    {
        cace_ari_t *item = cace_ari_array_ref(it);

        retval = cace_ari_visit_ari(item, visitor, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_visit_execset(cace_ari_execset_t *obj, const cace_ari_visitor_t *visitor,
                                  const cace_ari_visit_ctx_t *ctx)
{
    int retval;

    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, obj->targets); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        cace_ari_t *item = cace_ari_list_ref(it);

        retval = cace_ari_visit_ari(item, visitor, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_visit_report(cace_ari_report_t *obj, const cace_ari_visitor_t *visitor,
                                 const cace_ari_visit_ctx_t *ctx)
{
    int retval;

    retval = cace_ari_visit_ari(&(obj->reltime), visitor, ctx);
    CHKERRVAL(retval);

    retval = cace_ari_visit_ari(&(obj->source), visitor, ctx);
    CHKERRVAL(retval);

    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, obj->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        cace_ari_t *item = cace_ari_list_ref(it);

        retval = cace_ari_visit_ari(item, visitor, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_visit_rptset(cace_ari_rptset_t *obj, const cace_ari_visitor_t *visitor,
                                 const cace_ari_visit_ctx_t *ctx)
{
    int retval;

    retval = cace_ari_visit_ari(&(obj->nonce), visitor, ctx);
    CHKERRVAL(retval);

    retval = cace_ari_visit_ari(&(obj->reftime), visitor, ctx);
    CHKERRVAL(retval);

    cace_ari_report_list_it_t it;
    for (cace_ari_report_list_it(it, obj->reports); !cace_ari_report_list_end_p(it); cace_ari_report_list_next(it))
    {
        cace_ari_report_t *item = cace_ari_report_list_ref(it);

        retval = cace_ari_visit_report(item, visitor, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_visit_ari(cace_ari_t *ari, const cace_ari_visitor_t *visitor, const cace_ari_visit_ctx_t *ctx)
{
    int retval;

    // visit main ARI first
    if (visitor->visit_ari)
    {
        retval = visitor->visit_ari(ari, ctx);
        CHKERRVAL(retval);
    }

    cace_ari_visit_ctx_t sub_ctx = {
        .parent    = ari,
        .user_data = ctx->user_data,
    };

    // dive into contents
    if (ari->is_ref)
    {
        if (visitor->visit_ref)
        {
            retval = visitor->visit_ref(&(ari->as_ref), ctx);
            CHKERRVAL(retval);
        }

        if (visitor->visit_objpath)
        {
            retval = visitor->visit_objpath(&(ari->as_ref.objpath), ctx);
            CHKERRVAL(retval);
        }

        switch (ari->as_ref.params.state)
        {
            case CACE_ARI_PARAMS_NONE:
                break;
            case CACE_ARI_PARAMS_AC:
            {
                retval = cace_ari_visit_ac(ari->as_ref.params.as_ac, visitor, &sub_ctx);
                CHKERRVAL(retval);
                break;
            }
            case CACE_ARI_PARAMS_AM:
            {
                {
                    retval = cace_ari_visit_am(ari->as_ref.params.as_am, visitor, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                }
            }
            default:
                return 0;
        }
    }
    else
    {
        if (visitor->visit_lit)
        {
            retval = visitor->visit_lit(&(ari->as_lit), ctx);
            CHKERRVAL(retval);
        }

        if (ari->as_lit.has_ari_type)
        {
            switch (ari->as_lit.ari_type)
            {
                case CACE_ARI_TYPE_AC:
                    retval = cace_ari_visit_ac(ari->as_lit.value.as_ac, visitor, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                case CACE_ARI_TYPE_AM:
                    retval = cace_ari_visit_am(ari->as_lit.value.as_am, visitor, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                case CACE_ARI_TYPE_TBL:
                    retval = cace_ari_visit_tbl(ari->as_lit.value.as_tbl, visitor, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                case CACE_ARI_TYPE_EXECSET:
                    retval = cace_ari_visit_execset(ari->as_lit.value.as_execset, visitor, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                case CACE_ARI_TYPE_RPTSET:
                    retval = cace_ari_visit_rptset(ari->as_lit.value.as_rptset, visitor, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                default:
                    break;
            }
        }
    }

    return retval;
}

int cace_ari_visit(cace_ari_t *ari, const cace_ari_visitor_t *visitor, void *user_data)
{
    CHKERR1(ari);
    CHKERR1(visitor);

    cace_ari_visit_ctx_t sub_ctx = {
        .parent    = NULL,
        .user_data = user_data,
    };
    return cace_ari_visit_ari(ari, visitor, &sub_ctx);
}

static int cace_ari_translate_ari(cace_ari_t *out, const cace_ari_t *in, const cace_ari_translator_t *translator,
                                  const cace_ari_translate_ctx_t *ctx);

static int cace_ari_map_ac(cace_ari_ac_t *out, const cace_ari_ac_t *in, const cace_ari_translator_t *translator,
                           const cace_ari_translate_ctx_t *ctx)
{
    int                retval;
    cace_ari_list_it_t it;
    for (cace_ari_list_it(it, in->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
    {
        const cace_ari_t *in_item  = cace_ari_list_cref(it);
        cace_ari_t       *out_item = cace_ari_list_push_back_new(out->items);
        retval                     = cace_ari_translate_ari(out_item, in_item, translator, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_map_am(cace_ari_am_t *out, const cace_ari_am_t *in, const cace_ari_translator_t *translator,
                           cace_ari_translate_ctx_t *ctx)
{
    int                retval;
    cace_ari_tree_it_t it;
    for (cace_ari_tree_it(it, in->items); !cace_ari_tree_end_p(it); cace_ari_tree_next(it))
    {
        const cace_ari_tree_subtype_ct *pair = cace_ari_tree_cref(it);

        cace_ari_t out_key  = CACE_ARI_INIT_UNDEFINED;
        ctx->is_map_key     = true;
        retval              = cace_ari_translate_ari(&out_key, pair->key_ptr, translator, ctx);
        cace_ari_t *out_val = cace_ari_tree_safe_get(out->items, out_key);
        cace_ari_deinit(&out_key);
        CHKERRVAL(retval);

        ctx->is_map_key = false;
        retval          = cace_ari_translate_ari(out_val, pair->value_ptr, translator, ctx);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_map_tbl(cace_ari_tbl_t *out, const cace_ari_tbl_t *in, const cace_ari_translator_t *translator,
                            const cace_ari_translate_ctx_t *ctx)
{
    int retval;

    out->ncols = in->ncols;

    cace_ari_array_it_t it;
    for (cace_ari_array_it(it, in->items); !cace_ari_array_end_p(it); cace_ari_array_next(it))
    {
        const cace_ari_t *in_item  = cace_ari_array_cref(it);
        cace_ari_t        out_item = CACE_ARI_INIT_UNDEFINED;
        retval                     = cace_ari_translate_ari(&out_item, in_item, translator, ctx);
        cace_ari_array_push_move(out->items, &out_item);
        CHKERRVAL(retval);
    }
    return 0;
}

static int cace_ari_translate_ari(cace_ari_t *out, const cace_ari_t *in, const cace_ari_translator_t *translator,
                                  const cace_ari_translate_ctx_t *ctx)
{
    int retval = 0;

    // handle main ARI first
    if (translator->map_ari)
    {
        retval = translator->map_ari(out, in, ctx);
        CHKERRVAL(retval);
    }
    else
    {
        out->is_ref = in->is_ref;
    }

    cace_ari_translate_ctx_t sub_ctx = {
        .parent     = in,
        .is_map_key = ctx->is_map_key,
        .user_data  = ctx->user_data,
    };

    if (in->is_ref)
    {
        if (translator->map_objpath)
        {
            retval = translator->map_objpath(&(out->as_ref.objpath), &(in->as_ref.objpath), ctx);
            CHKERRVAL(retval);
        }
        else
        {
            cace_ari_objpath_copy(&(out->as_ref.objpath), &(in->as_ref.objpath));
        }

        switch (in->as_ref.params.state)
        {
            case CACE_ARI_PARAMS_NONE:
                out->as_ref.params.state = CACE_ARI_PARAMS_NONE;
                break;
            case CACE_ARI_PARAMS_AC:
            {
                cace_ari_lit_t tmp;
                cace_ari_lit_init_container(&tmp, CACE_ARI_TYPE_AC);
                retval = cace_ari_map_ac(tmp.value.as_ac, in->as_ref.params.as_ac, translator, &sub_ctx);
                if (retval)
                {
                    cace_ari_lit_deinit(&tmp);
                    return retval;
                }
                out->as_ref.params.state = CACE_ARI_PARAMS_AC;
                out->as_ref.params.as_ac = tmp.value.as_ac;
                break;
            }
            case CACE_ARI_PARAMS_AM:
            {
                {
                    cace_ari_lit_t tmp;
                    cace_ari_lit_init_container(&tmp, CACE_ARI_TYPE_AM);
                    retval = cace_ari_map_am(tmp.value.as_am, in->as_ref.params.as_am, translator, &sub_ctx);
                    if (retval)
                    {
                        cace_ari_lit_deinit(&tmp);
                        return retval;
                    }
                    out->as_ref.params.state = CACE_ARI_PARAMS_AM;
                    out->as_ref.params.as_am = tmp.value.as_am;
                    break;
                }
            }
            default:
                return 0;
        }
    }
    else
    {
        if (in->as_lit.has_ari_type)
        {
            switch (in->as_lit.ari_type)
            {
                case CACE_ARI_TYPE_AC:
                    cace_ari_lit_init_container(&(out->as_lit), CACE_ARI_TYPE_AC);
                    retval = cace_ari_map_ac(out->as_lit.value.as_ac, in->as_lit.value.as_ac, translator, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                case CACE_ARI_TYPE_AM:
                {
                    cace_ari_lit_init_container(&(out->as_lit), CACE_ARI_TYPE_AM);
                    retval = cace_ari_map_am(out->as_lit.value.as_am, in->as_lit.value.as_am, translator, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                }
                case CACE_ARI_TYPE_TBL:
                {
                    cace_ari_lit_init_container(&(out->as_lit), CACE_ARI_TYPE_TBL);
                    retval = cace_ari_map_tbl(out->as_lit.value.as_tbl, in->as_lit.value.as_tbl, translator, &sub_ctx);
                    CHKERRVAL(retval);
                    break;
                }
                default:
                    if (translator->map_lit)
                    {
                        retval = translator->map_lit(&(out->as_lit), &(in->as_lit), ctx);
                        CHKERRVAL(retval);
                    }
                    else
                    {
                        cace_ari_lit_copy(&(out->as_lit), &(in->as_lit));
                    }
                    break;
            }
        }
        else
        {
            if (translator->map_lit)
            {
                retval = translator->map_lit(&(out->as_lit), &(in->as_lit), ctx);
                CHKERRVAL(retval);
            }
            else
            {
                cace_ari_lit_copy(&(out->as_lit), &(in->as_lit));
            }
        }
    }

    return retval;
}

int cace_ari_translate(cace_ari_t *out, const cace_ari_t *in, const cace_ari_translator_t *translator, void *user_data)
{
    CHKERR1(out);
    CHKERR1(in);
    CHKERR1(translator);

    cace_ari_translate_ctx_t sub_ctx = {
        .parent    = NULL,
        .user_data = user_data,
    };
    return cace_ari_translate_ari(out, in, translator, &sub_ctx);
}

static int cace_ari_hash_visit_objpath(cace_ari_objpath_t *path, const cace_ari_visit_ctx_t *ctx)
{
    size_t *accum = ctx->user_data;
    M_HASH_UP(*accum, cace_ari_idseg_hash(&(path->org_id)));
    M_HASH_UP(*accum, cace_ari_idseg_hash(&(path->model_id)));
    M_HASH_UP(*accum, M_HASH_DEFAULT(path->model_rev.valid));
    if (path->model_rev.valid)
    {
        M_HASH_UP(*accum, M_HASH_DEFAULT(path->model_rev.parts.tm_year));
        M_HASH_UP(*accum, M_HASH_DEFAULT(path->model_rev.parts.tm_mon));
        M_HASH_UP(*accum, M_HASH_DEFAULT(path->model_rev.parts.tm_mday));
    }
    M_HASH_UP(*accum, M_HASH_DEFAULT(path->has_ari_type));
    if (path->has_ari_type)
    {
        M_HASH_UP(*accum, M_HASH_DEFAULT(path->ari_type));
    }
    else
    {
        M_HASH_UP(*accum, cace_ari_idseg_hash(&(path->type_id)));
    }
    M_HASH_UP(*accum, cace_ari_idseg_hash(&(path->obj_id)));
    return 0;
}

static int cace_ari_hash_visit_lit(cace_ari_lit_t *obj, const cace_ari_visit_ctx_t *ctx)
{
    size_t *accum = ctx->user_data;

    M_HASH_UP(*accum, M_HASH_DEFAULT(obj->has_ari_type));
    if (obj->has_ari_type)
    {
        M_HASH_UP(*accum, M_HASH_DEFAULT(obj->ari_type));
        switch (obj->ari_type)
        {
            case CACE_ARI_TYPE_TBL:
                // include metadata
                M_HASH_UP(*accum, M_HASH_DEFAULT(obj->value.as_tbl->ncols));
                break;
            default:
                // container contents are visited separately
                break;
        }
    }
    M_HASH_UP(*accum, M_HASH_DEFAULT(obj->prim_type));
    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_UNDEFINED:
        case CACE_ARI_PRIM_NULL:
            break;
        case CACE_ARI_PRIM_BOOL:
            M_HASH_UP(*accum, M_HASH_POD_DEFAULT(obj->value.as_bool));
            break;
        case CACE_ARI_PRIM_UINT64:
            M_HASH_UP(*accum, M_HASH_DEFAULT(obj->value.as_uint64));
            break;
        case CACE_ARI_PRIM_INT64:
            M_HASH_UP(*accum, M_HASH_DEFAULT(obj->value.as_int64));
            break;
        case CACE_ARI_PRIM_FLOAT64:
            M_HASH_UP(*accum, M_HASH_DEFAULT(obj->value.as_float64));
            break;
        case CACE_ARI_PRIM_TSTR:
        case CACE_ARI_PRIM_BSTR:
            M_HASH_UP(*accum, cace_data_hash(&(obj->value.as_data)));
            break;
        case CACE_ARI_PRIM_TIMESPEC:
            M_HASH_UP(*accum, M_HASH_POD_DEFAULT(obj->value.as_timespec));
            break;
        default:
            break;
    }

    return 0;
}

size_t cace_ari_hash(const cace_ari_t *ari)
{
    CHKRET(ari, 0);
    static const cace_ari_visitor_t visitor = {
        .visit_objpath = cace_ari_hash_visit_objpath,
        .visit_lit     = cace_ari_hash_visit_lit,
    };

    M_HASH_DECL(accum);
    // the visit functions keep the value const
    cace_ari_visit((cace_ari_t *)ari, &visitor, &accum);
    accum = M_HASH_FINAL(accum);
    return accum;
}

static bool cace_ari_objpath_cmp(const cace_ari_objpath_t *left, const cace_ari_objpath_t *right)
{
    int part_cmp = cace_ari_idseg_cmp(&(left->org_id), &(right->org_id));
    if (part_cmp)
    {
        return part_cmp;
    }
    part_cmp = cace_ari_idseg_cmp(&(left->model_id), &(right->model_id));
    if (part_cmp)
    {
        return part_cmp;
    }
    part_cmp = cace_ari_date_cmp(&(left->model_rev), &(right->model_rev));
    if (part_cmp)
    {
        return part_cmp;
    }

    // prefer derived values
    if (left->has_ari_type && right->has_ari_type)
    {
        part_cmp = left->ari_type < right->ari_type;
    }
    else
    {
        part_cmp = cace_ari_idseg_cmp(&(left->type_id), &(right->type_id));
    }
    if (part_cmp)
    {
        return part_cmp;
    }

    return cace_ari_idseg_cmp(&(left->obj_id), &(right->obj_id));
}

static bool cace_ari_objpath_equal(const cace_ari_objpath_t *left, const cace_ari_objpath_t *right)
{
    // prefer derived values
    bool type_equal;
    if (left->has_ari_type && right->has_ari_type)
    {
        type_equal = left->ari_type == right->ari_type;
    }
    else
    {
        type_equal = cace_ari_idseg_equal(&(left->type_id), &(right->type_id));
    }

    return (cace_ari_idseg_equal(&(left->org_id), &(right->org_id))
            && cace_ari_idseg_equal(&(left->model_id), &(right->model_id))
            && (cace_ari_date_cmp(&(left->model_rev), &(right->model_rev)) == 0) && type_equal
            && cace_ari_idseg_equal(&(left->obj_id), &(right->obj_id)));
}

static int cace_ari_params_cmp(const cace_ari_params_t *left, const cace_ari_params_t *right)
{
    int part_cmp = M_CMP_DEFAULT(left->state, right->state);
    if (part_cmp)
    {
        return part_cmp;
    }

    switch (left->state)
    {
        case CACE_ARI_PARAMS_NONE:
            return 0;
        case CACE_ARI_PARAMS_AC:
            return cace_ari_ac_cmp(left->as_ac, right->as_ac);
        case CACE_ARI_PARAMS_AM:
            return cace_ari_am_cmp(left->as_am, right->as_am);
        default:
            return -2;
    }
}

static bool cace_ari_params_equal(const cace_ari_params_t *left, const cace_ari_params_t *right)
{
    if (left->state != right->state)
    {
        return false;
    }
    switch (left->state)
    {
        case CACE_ARI_PARAMS_NONE:
            return true;
        case CACE_ARI_PARAMS_AC:
            return cace_ari_ac_equal(left->as_ac, right->as_ac);
        case CACE_ARI_PARAMS_AM:
            return cace_ari_am_equal(left->as_am, right->as_am);
        default:
            return false;
    }
}

int cace_ari_cmp(const cace_ari_t *left, const cace_ari_t *right)
{
    CHKRET(left, -1);
    CHKRET(right, -1);

    if (left->is_ref != right->is_ref)
    {
        // literals order first
        return left->is_ref ? 1 : -1;
    }
    if (left->is_ref)
    {
        int part_cmp = cace_ari_objpath_cmp(&(left->as_ref.objpath), &(right->as_ref.objpath));
        if (part_cmp)
        {
            return part_cmp;
        }
        return cace_ari_params_cmp(&(left->as_ref.params), &(right->as_ref.params));
    }
    else
    {
        if (left->as_lit.has_ari_type != right->as_lit.has_ari_type)
        {
            // untyped order first
            return left->as_lit.has_ari_type ? 1 : -1;
        }
        if (left->as_lit.has_ari_type)
        {
            int part_cmp = M_CMP_DEFAULT(left->as_lit.ari_type, right->as_lit.ari_type);
            if (part_cmp)
            {
                return part_cmp;
            }
            switch (left->as_lit.ari_type)
            {
                case CACE_ARI_TYPE_AC:
                    part_cmp = cace_ari_ac_cmp(left->as_lit.value.as_ac, right->as_lit.value.as_ac);
                    if (part_cmp)
                    {
                        return part_cmp;
                    }
                    break;
                case CACE_ARI_TYPE_AM:
                    part_cmp = cace_ari_am_cmp(left->as_lit.value.as_am, right->as_lit.value.as_am);
                    if (part_cmp)
                    {
                        return part_cmp;
                    }
                    break;
                case CACE_ARI_TYPE_TBL:
                    part_cmp = cace_ari_tbl_cmp(left->as_lit.value.as_tbl, right->as_lit.value.as_tbl);
                    if (part_cmp)
                    {
                        return part_cmp;
                    }
                    break;
                case CACE_ARI_TYPE_EXECSET:
                    part_cmp = cace_ari_execset_cmp(left->as_lit.value.as_execset, right->as_lit.value.as_execset);
                    if (part_cmp)
                    {
                        return part_cmp;
                    }
                    break;
                case CACE_ARI_TYPE_RPTSET:
                    part_cmp = cace_ari_rptset_cmp(left->as_lit.value.as_rptset, right->as_lit.value.as_rptset);
                    if (part_cmp)
                    {
                        return part_cmp;
                    }
                    break;
                default:
                    break;
            }
        }

        int part_cmp = M_CMP_DEFAULT(left->as_lit.prim_type, right->as_lit.prim_type);
        if (part_cmp)
        {
            // ordered by primitive enum, not CBOR related
            return part_cmp;
        }
        switch (left->as_lit.prim_type)
        {
            case CACE_ARI_PRIM_UNDEFINED:
            case CACE_ARI_PRIM_NULL:
                part_cmp = 0;
                break;
            case CACE_ARI_PRIM_BOOL:
                part_cmp = M_CMP_BASIC(left->as_lit.value.as_bool, right->as_lit.value.as_bool);
                break;
            case CACE_ARI_PRIM_UINT64:
                part_cmp = M_CMP_BASIC(left->as_lit.value.as_uint64, right->as_lit.value.as_uint64);
                break;
            case CACE_ARI_PRIM_INT64:
                part_cmp = M_CMP_BASIC(left->as_lit.value.as_int64, right->as_lit.value.as_int64);
                break;
            case CACE_ARI_PRIM_FLOAT64:
                part_cmp = M_CMP_BASIC(isnan(left->as_lit.value.as_float64), isnan(right->as_lit.value.as_float64));
                if (part_cmp)
                {
                    return part_cmp;
                }
                if (!isnan(left->as_lit.value.as_float64))
                {
                    part_cmp = M_CMP_BASIC(left->as_lit.value.as_float64, right->as_lit.value.as_float64);
                }
                break;
            case CACE_ARI_PRIM_TSTR:
            case CACE_ARI_PRIM_BSTR:
                part_cmp = cace_data_cmp(&(left->as_lit.value.as_data), &(right->as_lit.value.as_data));
                break;
            case CACE_ARI_PRIM_TIMESPEC:
                part_cmp = M_CMP_BASIC(left->as_lit.value.as_timespec.tv_sec, right->as_lit.value.as_timespec.tv_sec);
                if (part_cmp)
                {
                    return part_cmp;
                }
                part_cmp = M_CMP_BASIC(left->as_lit.value.as_timespec.tv_nsec, right->as_lit.value.as_timespec.tv_nsec);
                break;
            case CACE_ARI_PRIM_OTHER:
                // already handled above
                break;
            default:
                CACE_LOG_ERR("no logic to compare primitive type %d", left->as_lit.prim_type);
                break;
        }

        return part_cmp;
    }
}

bool cace_ari_equal(const cace_ari_t *left, const cace_ari_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    if (left->is_ref != right->is_ref)
    {
        return false;
    }
    if (left->is_ref)
    {
        return (cace_ari_objpath_equal(&(left->as_ref.objpath), &(right->as_ref.objpath))
                && cace_ari_params_equal(&(left->as_ref.params), &(right->as_ref.params)));
    }
    else
    {
        if (left->as_lit.has_ari_type != right->as_lit.has_ari_type)
        {
            return false;
        }
        if (left->as_lit.has_ari_type)
        {
            if (left->as_lit.ari_type != right->as_lit.ari_type)
            {
                return false;
            }
            switch (left->as_lit.ari_type)
            {
                case CACE_ARI_TYPE_AC:
                    if (!cace_ari_ac_equal(left->as_lit.value.as_ac, right->as_lit.value.as_ac))
                    {
                        return false;
                    }
                    break;
                case CACE_ARI_TYPE_AM:
                    if (!cace_ari_am_equal(left->as_lit.value.as_am, right->as_lit.value.as_am))
                    {
                        return false;
                    }
                    break;
                case CACE_ARI_TYPE_TBL:
                    if (!cace_ari_tbl_equal(left->as_lit.value.as_tbl, right->as_lit.value.as_tbl))
                    {
                        return false;
                    }
                    break;
                case CACE_ARI_TYPE_EXECSET:
                    if (!cace_ari_execset_equal(left->as_lit.value.as_execset, right->as_lit.value.as_execset))
                    {
                        return false;
                    }
                    break;
                case CACE_ARI_TYPE_RPTSET:
                    if (!cace_ari_rptset_equal(left->as_lit.value.as_rptset, right->as_lit.value.as_rptset))
                    {
                        return false;
                    }
                    break;
                default:
                    break;
            }
        }

        bool result = true;

        cace_ari_type_t promote;
        cace_ari_t      lt_prom = CACE_ARI_INIT_UNDEFINED;
        cace_ari_t      rt_prom = CACE_ARI_INIT_UNDEFINED;

        if (cace_has_numeric_prim_type(left) && cace_has_numeric_prim_type(right)
            && !cace_amm_numeric_promote_type(&promote, left, right))
        {
            const cace_amm_type_t *amm_promote = cace_amm_type_get_builtin(promote);
            cace_amm_type_convert(amm_promote, &lt_prom, left);
            cace_amm_type_convert(amm_promote, &rt_prom, right);
            left  = &lt_prom;
            right = &rt_prom;
        }

        if (left->as_lit.prim_type != right->as_lit.prim_type)
        {
            result = false;
        }
        else
        {
            switch (left->as_lit.prim_type)
            {
                case CACE_ARI_PRIM_UNDEFINED:
                case CACE_ARI_PRIM_NULL:
                    break;
                case CACE_ARI_PRIM_BOOL:
                    if (left->as_lit.value.as_bool != right->as_lit.value.as_bool)
                    {
                        result = false;
                    }
                    break;
                case CACE_ARI_PRIM_UINT64:
                    if (left->as_lit.value.as_uint64 != right->as_lit.value.as_uint64)
                    {
                        result = false;
                    }
                    break;
                case CACE_ARI_PRIM_INT64:
                    if (left->as_lit.value.as_int64 != right->as_lit.value.as_int64)
                    {
                        result = false;
                    }
                    break;
                case CACE_ARI_PRIM_FLOAT64:
                    if (isnan(left->as_lit.value.as_float64) != isnan(right->as_lit.value.as_float64))
                    {
                        result = false;
                    }
                    if (!isnan(left->as_lit.value.as_float64)
                        && (left->as_lit.value.as_float64 != right->as_lit.value.as_float64))
                    {
                        result = false;
                    }
                    break;
                case CACE_ARI_PRIM_TSTR:
                case CACE_ARI_PRIM_BSTR:
                    if (!cace_data_equal(&(left->as_lit.value.as_data), &(right->as_lit.value.as_data)))
                    {
                        result = false;
                    }
                    break;
                case CACE_ARI_PRIM_TIMESPEC:
                    if (!M_MEMCMP1_DEFAULT(left->as_lit.value.as_timespec, right->as_lit.value.as_timespec))
                    {
                        result = false;
                    }
                    break;
                case CACE_ARI_PRIM_OTHER:
                    // already handled above
                    break;
                default:
                    CACE_LOG_ERR("no logic to compare primitive type %d", left->as_lit.prim_type);
                    break;
            }
        }

        cace_ari_deinit(&lt_prom);
        cace_ari_deinit(&rt_prom);
        return result;
    }
}

void cace_ari_get_str(m_string_t out, const cace_ari_t obj, bool append)
{
    m_string_t buf;
    m_string_init(buf);
    cace_ari_text_encode(buf, &obj, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
    if (append)
    {
        m_string_cat(out, buf);
        m_string_clear(buf);
    }
    else
    {
        m_string_move(out, buf);
    }
}
