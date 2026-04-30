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
#include "promote.h"
#include "typing.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

cace_ari_type_t cace_amm_promote_eqiv_lit_type(const cace_ari_lit_t *lit)
{
    if (lit->has_ari_type)
    {
        return lit->ari_type;
    }

    switch (lit->prim_type)
    {
        case CACE_ARI_PRIM_NULL:
            return CACE_ARI_TYPE_NULL;
        case CACE_ARI_PRIM_BOOL:
            return CACE_ARI_TYPE_BOOL;
        case CACE_ARI_PRIM_BSTR:
            return CACE_ARI_TYPE_BYTESTR;
        case CACE_ARI_PRIM_TSTR:
            return CACE_ARI_TYPE_TEXTSTR;
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

bool cace_amm_promote_is_numeric(cace_ari_type_t typ)
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

void cace_amm_promote_init(cace_amm_promote_state_t *obj)
{
    CHKVOID(obj);
    memset(obj, 0, sizeof(*obj));

    cace_ari_init(&obj->lt_prom);
    cace_ari_init(&obj->rt_prom);
}

void cace_amm_promote_deinit(cace_amm_promote_state_t *obj)
{
    CHKVOID(obj);
    cace_ari_deinit(&obj->lt_prom);
    cace_ari_deinit(&obj->rt_prom);
}

static int cace_amm_promote_valtype(cace_ari_type_t *result, const cace_ari_t *val)
{
    if (val->is_ref)
    {
        if (val->as_ref.objpath.has_ari_type)
        {
            *result = val->as_ref.objpath.ari_type;
            return 0;
        }
        else
        {
            CACE_LOG_WARNING("object reference has no internal type");
            return 2;
        }
    }
    else
    {
        if (val->as_lit.has_ari_type)
        {
            *result = val->as_lit.ari_type;
            return 0;
        }
        else
        {
            CACE_LOG_WARNING("literal has no internal type");
            return 2;
        }
    }
}

/** For the numeric types, provide a rank ordering.
 * @sa cace_amm_promote_is_numeric()
 */
static int cace_amm_promote_numeric_rank(cace_ari_type_t typ)
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

/** Implement the numeric type promotion procedure from
 * Section 6.11.2.1 of the AMM @cite draft-ietf-dtn-amm
 * by choosing the least compatible type between two values.
 */
static int cace_amm_promote_type(cace_ari_type_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val)
{
    CHKERR1(result);
    CHKERR1(lt_val);
    CHKERR1(rt_val);

    if (lt_val->is_ref != rt_val->is_ref)
    {
        // no possible common type
        return 2;
    }

    if (lt_val->is_ref)
    {
        return cace_amm_promote_valtype(result, lt_val);
    }

    // simple case of one typed and one untyped literal
    if (lt_val->as_lit.has_ari_type && !(rt_val->as_lit.has_ari_type))
    {
        *result = lt_val->as_lit.ari_type;
        return 0;
    }
    else if (rt_val->as_lit.has_ari_type && !(lt_val->as_lit.has_ari_type))
    {
        *result = rt_val->as_lit.ari_type;
        return 0;
    }

    cace_ari_type_t lt_typ = cace_amm_promote_eqiv_lit_type(&(lt_val->as_lit));
    cace_ari_type_t rt_typ = cace_amm_promote_eqiv_lit_type(&(rt_val->as_lit));

    if (lt_typ == rt_typ)
    {
        // easy case they are the same
        *result = lt_typ;
        return 0;
    }
    if (!(cace_amm_promote_is_numeric(lt_typ) && cace_amm_promote_is_numeric(rt_typ)))
    {
        CACE_LOG_DEBUG("input is not a numeric type");
        return 2;
    }

    // promotion is symmetric, so swap to make logic more simple
    if (cace_amm_promote_numeric_rank(lt_typ) > cace_amm_promote_numeric_rank(rt_typ))
    {
        M_SWAP(cace_ari_type_t, lt_typ, rt_typ);
    }

    if ((lt_typ == CACE_ARI_TYPE_INT) && (rt_typ == CACE_ARI_TYPE_UVAST))
    {
        // special case
        *result = CACE_ARI_TYPE_VAST;
    }
    else
    {
        // higher-rank wins
        *result = rt_typ;
    }

    return 0;
}

int cace_amm_promote_process(cace_amm_promote_state_t *obj, const cace_ari_t *lt_val, const cace_ari_t *rt_val)
{
    obj->lt_val = lt_val;
    obj->rt_val = rt_val;

    if (cace_amm_promote_type(&(obj->common), lt_val, rt_val))
    {
        CACE_LOG_DEBUG("could not find a common promotion type");
        return 2;
    }
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        CACE_LOG_DEBUG("promoting to built-in type %s", cace_ari_type_to_name(obj->common));
    }
    int res;

    cace_ari_type_t lt_current;
    res = cace_amm_promote_valtype(&lt_current, lt_val);
    // is conversion needed?
    bool lt_need_convert = !(!res && (lt_current == obj->common));

    cace_ari_type_t rt_current;
    res = cace_amm_promote_valtype(&rt_current, rt_val);
    // is conversion needed?
    bool rt_need_convert = !(!res && (rt_current == obj->common));

    const cace_amm_type_t *typeobj = cace_amm_type_get_builtin(obj->common);
    if (lt_need_convert)
    {
        if (cace_amm_type_convert(typeobj, &(obj->lt_prom), lt_val))
        {
            CACE_LOG_DEBUG("could not convert left operand to promotion type");
            return 3;
        }
        obj->lt_use = &(obj->lt_prom);
    }
    else
    {
        obj->lt_use = obj->lt_val;
    }

    if (rt_need_convert)
    {
        if (cace_amm_type_convert(typeobj, &(obj->rt_prom), rt_val))
        {
            CACE_LOG_DEBUG("could not convert right operand to promotion type");
            return 3;
        }
        obj->rt_use = &(obj->rt_prom);
    }
    else
    {
        obj->rt_use = obj->rt_val;
    }

    CACE_LOG_DEBUG("converted both values to promotion type %s", cace_ari_type_to_name(obj->common));
    return 0;
}
