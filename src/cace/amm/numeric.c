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
#include "numeric.h"
#include "promote.h"
#include "cace/amm/typing.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

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

int cace_numeric_integer_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                         const cace_numeric_binary_desc_t *desc)
{
    CHKERR1(result);
    CHKERR1(desc);
    CHKERR1(desc->binop_uvast);
    CHKERR1(desc->binop_vast);

    cace_amm_promote_state_t promote;
    cace_amm_promote_init(&promote);
    if (cace_amm_promote_process(&promote, lt_val, rt_val))
    {
        cace_amm_promote_deinit(&promote);
        return 2;
    }

    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    int retval = 0;
    switch (promote.lt_use->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            res_lit->value.as_uint64 =
                desc->binop_uvast(promote.lt_use->as_lit.value.as_uint64, promote.rt_use->as_lit.value.as_uint64);
            break;
        case CACE_ARI_PRIM_INT64:
            res_lit->value.as_int64 =
                desc->binop_vast(promote.lt_use->as_lit.value.as_int64, promote.rt_use->as_lit.value.as_int64);
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = promote.lt_use->as_lit.prim_type;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = promote.common;
    }

    cace_amm_promote_deinit(&promote);
    return retval;
}

int cace_numeric_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                 const cace_numeric_binary_desc_t *desc)
{
    CHKERR1(result);
    CHKERR1(lt_val);
    CHKERR1(rt_val);
    CHKERR1(desc);
    CHKERR1(desc->binop_uvast);
    CHKERR1(desc->binop_vast);
    CHKERR1(desc->binop_real64);
    CHKERR1(desc->binop_timespec);

    if (cace_ari_is_undefined(lt_val) || cace_ari_is_undefined(rt_val))
    {
        // short-circuit to undefined result
        return 1;
    }

    // TP and TD operands are handled specially
    cace_ari_type_t lt_typ = cace_amm_promote_eqiv_lit_type(&(lt_val->as_lit));
    cace_ari_type_t rt_typ = cace_amm_promote_eqiv_lit_type(&(rt_val->as_lit));
    // any combination
    bool is_oper_TS = ((lt_typ == CACE_ARI_TYPE_TD) || (lt_typ == CACE_ARI_TYPE_TP) || (rt_typ == CACE_ARI_TYPE_TD)
                       || (rt_typ == CACE_ARI_TYPE_TP));
    if (is_oper_TS)
    {
        return desc->binop_timespec(result, lt_val, rt_val);
    }

    // Logic for non timespec operations
    cace_amm_promote_state_t promote;
    cace_amm_promote_init(&promote);
    if (cace_amm_promote_process(&promote, lt_val, rt_val))
    {
        cace_amm_promote_deinit(&promote);
        return 2;
    }

    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    int retval = 0;
    switch (promote.lt_use->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            res_lit->value.as_uint64 =
                desc->binop_uvast(promote.lt_use->as_lit.value.as_uint64, promote.rt_use->as_lit.value.as_uint64);
            break;
        case CACE_ARI_PRIM_INT64:
            res_lit->value.as_int64 =
                desc->binop_vast(promote.lt_use->as_lit.value.as_int64, promote.rt_use->as_lit.value.as_int64);
            break;
        case CACE_ARI_PRIM_FLOAT64:
            res_lit->value.as_float64 =
                desc->binop_real64(promote.lt_use->as_lit.value.as_float64, promote.rt_use->as_lit.value.as_float64);
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = promote.lt_use->as_lit.prim_type;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = promote.common;
    }

    cace_amm_promote_deinit(&promote);
    return retval;
}

int cace_numeric_compare_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                  const cace_numeric_compare_desc_t *desc)
{
    CHKERR1(result);
    CHKERR1(lt_val);
    CHKERR1(rt_val);
    CHKERR1(desc);
    CHKERR1(desc->binop_uvast);
    CHKERR1(desc->binop_vast);
    CHKERR1(desc->binop_real64);
    CHKERR1(desc->binop_timespec);

    cace_amm_promote_state_t promote;
    cace_amm_promote_init(&promote);
    if (cace_amm_promote_process(&promote, lt_val, rt_val))
    {
        cace_amm_promote_deinit(&promote);
        return 2;
    }

    bool result_bool = false;
    // allow for failure
    int retval = 0;
    switch (promote.lt_use->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            result_bool =
                desc->binop_uvast(promote.lt_use->as_lit.value.as_uint64, promote.rt_use->as_lit.value.as_uint64);
            break;
        case CACE_ARI_PRIM_INT64:
            result_bool =
                desc->binop_vast(promote.lt_use->as_lit.value.as_int64, promote.rt_use->as_lit.value.as_int64);
            break;
        case CACE_ARI_PRIM_FLOAT64:
            result_bool =
                desc->binop_real64(promote.lt_use->as_lit.value.as_float64, promote.rt_use->as_lit.value.as_float64);
            break;
        case CACE_ARI_PRIM_TIMESPEC:
            result_bool = desc->binop_timespec(promote.lt_use->as_lit.value.as_timespec,
                                               promote.rt_use->as_lit.value.as_timespec);
            break;
        default:
            retval = 3;
            break;
    }
    if (!retval)
    {
        cace_ari_set_bool(result, result_bool);
    }

    cace_amm_promote_deinit(&promote);
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
