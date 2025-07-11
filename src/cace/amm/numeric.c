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
#include "numeric.h"
#include "cace/util/defs.h"
#include "cace/amm/typing.h"

static cace_ari_type_t eqiv_ari_type(const cace_ari_lit_t *lit)
{
    if (lit->has_ari_type)
    {
        return lit->ari_type;
    }

    switch (lit->prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            const uint64_t *val = &(lit->value.as_uint64);
            if (*val <= UINT8_MAX)
            {
                return CACE_ARI_TYPE_BYTE;
            }
            if ((*val <= UINT32_MAX))
            {
                return CACE_ARI_TYPE_UINT;
            }
            return CACE_ARI_TYPE_UVAST;
        }
        case CACE_ARI_PRIM_INT64:
        {
            const int64_t *val = &(lit->value.as_int64);
            if ((*val >= INT32_MIN) && (*val <= INT32_MAX))
            {
                return CACE_ARI_TYPE_INT;
            }
            return CACE_ARI_TYPE_VAST;
        }
        case CACE_ARI_PRIM_FLOAT64:
            return CACE_ARI_TYPE_REAL64;
        default:
            break;
    }

    return CACE_ARI_TYPE_NULL;
}

static int numeric_rank(cace_ari_type_t typ)
{
    switch (typ)
    {
        case CACE_ARI_TYPE_BYTE:
            return 0;
        case CACE_ARI_TYPE_UINT:
            return 1;
        case CACE_ARI_TYPE_INT:
            return 2;
        case CACE_ARI_TYPE_UVAST:
            return 3;
        case CACE_ARI_TYPE_VAST:
            return 4;
        case CACE_ARI_TYPE_REAL32:
            return 5;
        case CACE_ARI_TYPE_REAL64:
            return 6;
        default:
            return -1;
    }
}

bool cace_is_numeric_type(cace_ari_type_t typ)
{
    switch (typ)
    {
        case CACE_ARI_TYPE_BYTE:
        case CACE_ARI_TYPE_UINT:
        case CACE_ARI_TYPE_INT:
        case CACE_ARI_TYPE_UVAST:
        case CACE_ARI_TYPE_VAST:
        case CACE_ARI_TYPE_REAL32:
        case CACE_ARI_TYPE_REAL64:
            return true;
        default:
            return false;
    }
}

bool cace_has_numeric_prim_type(const cace_ari_t *obj)
{
    CHKERR1(obj);
    switch (obj->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        case CACE_ARI_PRIM_INT64:
        case CACE_ARI_PRIM_FLOAT64:
            return true;
        default:
            return false;
    }
}

int cace_amm_numeric_promote_type(cace_ari_type_t *result, const cace_ari_t *left, const cace_ari_t *right)
{
    CHKERR1(result);
    CHKERR1(left);
    CHKERR1(right);
    CHKERR1(!(left->is_ref));
    CHKERR1(!(right->is_ref));

    cace_ari_type_t lt_typ = eqiv_ari_type(&(left->as_lit));
    cace_ari_type_t rt_typ = eqiv_ari_type(&(right->as_lit));

    // promotion is symmetric, so swap to make logic more simple
    if (numeric_rank(lt_typ) > numeric_rank(rt_typ))
    {
        M_SWAP(cace_ari_type_t, lt_typ, rt_typ);
    }

    if ((lt_typ == CACE_ARI_TYPE_INT) && (rt_typ == CACE_ARI_TYPE_UVAST))
    {
        *result = CACE_ARI_TYPE_VAST;
    }
    else
    {
        // higher-rank wins
        *result = rt_typ;
    }
    return 0;
}

int cace_numeric_integer_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                         cace_binop_uvast op_uvast, cace_binop_vast op_vast)
{
    cace_ari_type_t promote;
    if (cace_amm_numeric_promote_type(&promote, lt_val, rt_val))
    {
        return 2;
    }

    const cace_amm_type_t *amm_promote = cace_amm_type_get_builtin(promote);
    cace_ari_t             lt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_ari_t             rt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_amm_type_convert(amm_promote, &lt_prom, lt_val);
    cace_amm_type_convert(amm_promote, &rt_prom, rt_val);

    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    int retval = 0;
    switch (lt_prom.as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            res_lit->value.as_uint64 = op_uvast(lt_prom.as_lit.value.as_uint64, rt_prom.as_lit.value.as_uint64);
            break;
        case CACE_ARI_PRIM_INT64:
            res_lit->value.as_int64 = op_vast(lt_prom.as_lit.value.as_int64, rt_prom.as_lit.value.as_int64);
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = lt_prom.as_lit.prim_type;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = promote;
    }

    cace_ari_deinit(&lt_prom);
    cace_ari_deinit(&rt_prom);
    return retval;
}

int cace_numeric_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                 cace_binop_uvast op_uvast, cace_binop_vast op_vast, cace_binop_real64 op_real64)
{
    CHKERR1(result);
    CHKERR1(lt_val);
    CHKERR1(rt_val);
    CHKERR1(op_uvast);
    CHKERR1(op_vast);
    CHKERR1(op_real64);
    cace_ari_type_t promote;
    if (cace_amm_numeric_promote_type(&promote, lt_val, rt_val))
    {
        return 2;
    }

    const cace_amm_type_t *amm_promote = cace_amm_type_get_builtin(promote);
    cace_ari_t             lt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_ari_t             rt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_amm_type_convert(amm_promote, &lt_prom, lt_val);
    cace_amm_type_convert(amm_promote, &rt_prom, rt_val);

    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    int retval = 0;
    switch (lt_prom.as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            res_lit->value.as_uint64 = op_uvast(lt_prom.as_lit.value.as_uint64, rt_prom.as_lit.value.as_uint64);
            break;
        case CACE_ARI_PRIM_INT64:
            res_lit->value.as_int64 = op_vast(lt_prom.as_lit.value.as_int64, rt_prom.as_lit.value.as_int64);
            break;
        case CACE_ARI_PRIM_FLOAT64:
            res_lit->value.as_float64 = op_real64(lt_prom.as_lit.value.as_float64, rt_prom.as_lit.value.as_float64);
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = lt_prom.as_lit.prim_type;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = promote;
    }

    cace_ari_deinit(&lt_prom);
    cace_ari_deinit(&rt_prom);
    return retval;
}

int cace_numeric_binary_comparison_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                            cace_binop_uvast op_uvast, cace_binop_vast op_vast,
                                            cace_binop_real64 op_real64)
{
    CHKERR1(result);
    CHKERR1(lt_val);
    CHKERR1(rt_val);
    CHKERR1(op_uvast);
    CHKERR1(op_vast);
    CHKERR1(op_real64);

    cace_ari_type_t promote;
    if (cace_amm_numeric_promote_type(&promote, lt_val, rt_val))
    {
        return 2;
    }

    const cace_amm_type_t *amm_promote = cace_amm_type_get_builtin(promote);
    cace_ari_t             lt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_ari_t             rt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_amm_type_convert(amm_promote, &lt_prom, lt_val);
    cace_amm_type_convert(amm_promote, &rt_prom, rt_val);

    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    int retval = 0;
    switch (lt_prom.as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            res_lit->value.as_bool = op_uvast(lt_prom.as_lit.value.as_uint64, rt_prom.as_lit.value.as_uint64);
            break;
        case CACE_ARI_PRIM_INT64:
            res_lit->value.as_bool = op_vast(lt_prom.as_lit.value.as_int64, rt_prom.as_lit.value.as_int64);
            break;
        case CACE_ARI_PRIM_FLOAT64:
            res_lit->value.as_bool = op_real64(lt_prom.as_lit.value.as_float64, rt_prom.as_lit.value.as_float64);
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = CACE_ARI_PRIM_BOOL;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = CACE_ARI_TYPE_BOOL;
    }

    cace_ari_deinit(&lt_prom);
    cace_ari_deinit(&rt_prom);
    return retval;
}

bool cace_numeric_is_zero(const cace_ari_t *val)
{
    CHKERR1(val);
    switch (val->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            return val->as_lit.value.as_uint64 == 0;
        case CACE_ARI_PRIM_INT64:
            return val->as_lit.value.as_int64 == 0;
        case CACE_ARI_PRIM_FLOAT64:
            return val->as_lit.value.as_float64 == 0.0;
        default:
            return false;
    }
}