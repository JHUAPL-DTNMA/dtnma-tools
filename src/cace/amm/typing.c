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
#include "typing.h"
#include "lookup.h"
#include "semtype.h"
#include "cace/ari/type.h"
#include "cace/ari/algo.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"
#include "cace/config.h"
#include <m-dict.h>
#include <pthread.h>
#include <math.h>
#include <float.h>
#include <fenv.h>

/// Name any builtin type
static void builtin_ari_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    cace_ari_set_aritype_text(name, self->as_builtin.ari_type);
}

/** Implement common match logic.
 * This checks that the object-reference or literal type, if present, is
 * as required by the type object.
 *
 * @return Whether the common logic positively or negatively matched.
 */
static cace_amm_type_match_res_t builtin_common_lit_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }
    else if (ari->is_ref)
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }
    else
    {
        const cace_ari_lit_t *obj = &(ari->as_lit);
        // explicit type matching
        if (obj->has_ari_type && (obj->ari_type != self->as_builtin.ari_type))
        {
            return CACE_AMM_TYPE_MATCH_NEGATIVE;
        }
    }
    return CACE_AMM_TYPE_MATCH_NOINFO;
}

/** Implement common conversion logic.
 * This passes-through undefined values.
 *
 * @return True if the common logic did the conversion.
 */
static bool builtin_common_convert(const cace_amm_type_t *self _U_, cace_ari_t *out, const cace_ari_t *in)
{
    if (cace_ari_is_undefined(in))
    {
        cace_ari_set_undefined(out);
        return true;
    }
    return false;
}

static void cace_ari_force_lit_type(cace_ari_t *out, cace_ari_type_t ari_type)
{
    out->as_lit.has_ari_type = true;
    out->as_lit.ari_type     = ari_type;
}

static cace_amm_type_match_res_t builtin_literal_match(const cace_amm_type_t *self _U_, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }
    return cace_amm_type_match_pos_neg(!(ari->is_ref));
}

static cace_amm_type_match_res_t builtin_default_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got == CACE_AMM_TYPE_MATCH_NOINFO)
    {
        // no other conditions here
        got = CACE_AMM_TYPE_MATCH_POSITIVE;
    }
    return got;
}

/** A default convert function that simply copies a value that matches using
 * the type object, and copies the undefined value unconditionally.
 */
static int builtin_default_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }
    if (!(self->match))
    {
        return CACE_AMM_ERR_CONVERT_NULLFUNC;
    }
    cace_amm_type_match_res_t got = self->match(self, in);
    switch (got)
    {
        case CACE_AMM_TYPE_MATCH_NOINFO:
        case CACE_AMM_TYPE_MATCH_NEGATIVE:
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        default:
            break;
    }
    cace_ari_set_copy(out, in);
    return 0;
}

static cace_amm_type_match_res_t builtin_null_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(ari->as_lit.prim_type == CACE_ARI_PRIM_NULL);
}

static int builtin_null_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }
    cace_ari_set_null(out);
    cace_ari_force_lit_type(out, self->as_builtin.ari_type);
    return 0;
}

static cace_amm_type_match_res_t builtin_bool_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(ari->as_lit.prim_type == CACE_ARI_PRIM_BOOL);
}

static int builtin_bool_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }
    // truthy check
    bool result = true;
    // any object reference is truthy
    if (!(in->is_ref))
    {
        switch (in->as_lit.prim_type)
        {
            case CACE_ARI_PRIM_UNDEFINED:
                // not really needed but satisfies the compiler
            case CACE_ARI_PRIM_NULL:
                result = false;
                break;
            case CACE_ARI_PRIM_BOOL:
                result = in->as_lit.value.as_bool;
                break;
            case CACE_ARI_PRIM_UINT64:
                result = (in->as_lit.value.as_uint64 != 0);
                break;
            case CACE_ARI_PRIM_INT64:
                result = (in->as_lit.value.as_int64 != 0);
                break;
            case CACE_ARI_PRIM_FLOAT64:
                result = (!isnan(in->as_lit.value.as_float64) && (in->as_lit.value.as_float64 != 0));
                break;
            case CACE_ARI_PRIM_TSTR:
            case CACE_ARI_PRIM_BSTR:
                result = (in->as_lit.value.as_data.len != 0);
                break;
            case CACE_ARI_PRIM_TIMESPEC:
                result = ((in->as_lit.value.as_timespec.tv_sec != 0) || (in->as_lit.value.as_timespec.tv_nsec != 0));
                break;
            case CACE_ARI_PRIM_OTHER:
                switch (in->as_lit.ari_type)
                {
                    case CACE_ARI_TYPE_ARITYPE:
                    case CACE_ARI_TYPE_AC:
                    case CACE_ARI_TYPE_AM:
                    case CACE_ARI_TYPE_TBL:
                    case CACE_ARI_TYPE_EXECSET:
                    case CACE_ARI_TYPE_RPTSET:
                        // unconditional on content
                        result = true;
                        break;
                    default:
                        // all object reference types
                        break;
                }
                break;
        }
    }

    cace_ari_set_prim_bool(out, result);
    cace_ari_force_lit_type(out, self->as_builtin.ari_type);
    return 0;
}

/// Limitations for integer ARI types
typedef struct
{
    uint64_t uint_max;
    int64_t  int_min;
    int64_t  int_max;
} cace_ari_type_anyint_constraints_t;

static const cace_ari_type_anyint_constraints_t _builtin_byte_cnst = {
    .uint_max = UINT8_MAX,
    .int_min  = 0,
    .int_max  = UINT8_MAX,
};

static const cace_ari_type_anyint_constraints_t _builtin_int_cnst = {
    .uint_max = INT32_MAX,
    .int_min  = INT32_MIN,
    .int_max  = INT32_MAX,
};

static const cace_ari_type_anyint_constraints_t _builtin_uint_cnst = {
    .uint_max = UINT32_MAX,
    .int_min  = 0,
    .int_max  = UINT32_MAX,
};

static const cace_ari_type_anyint_constraints_t _builtin_vast_cnst = {
    .uint_max = INT64_MAX,
    .int_min  = 0,
    .int_max  = INT64_MAX,
};

static const cace_ari_type_anyint_constraints_t _builtin_uvast_cnst = {
    .uint_max = UINT64_MAX,
    .int_min  = 0,
    .int_max  = INT64_MAX, // fudge the truth to not overflow
};

static bool builtin_anyint_constraints(const cace_ari_type_anyint_constraints_t *cnst, const cace_ari_t *ari)
{
    const cace_ari_lit_t *obj = &(ari->as_lit);
    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            return (obj->value.as_uint64 <= cnst->uint_max);
        case CACE_ARI_PRIM_INT64:
            return ((obj->value.as_int64 >= cnst->int_min) && (obj->value.as_int64 <= cnst->int_max));
        case CACE_ARI_PRIM_FLOAT64:
        {
            feclearexcept(FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_DIVBYZERO);
            long long tmp = llroundf(obj->value.as_float64);
            if (fetestexcept(FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_DIVBYZERO))
            {
                return false;
            }
            return ((tmp >= cnst->int_min) && (tmp <= cnst->int_max));
        }
        default:
            return false;
    }
}

#define BUILTIN_ANYINT_EXTRACT(valtype, setter)                                           \
    do                                                                                    \
    {                                                                                     \
        valtype result;                                                                   \
        switch (in->as_lit.prim_type)                                                     \
        {                                                                                 \
            case CACE_ARI_PRIM_BOOL:                                                      \
                result = (valtype)(in->as_lit.value.as_bool);                             \
                break;                                                                    \
            case CACE_ARI_PRIM_UINT64:                                                    \
                result = (valtype)(in->as_lit.value.as_uint64);                           \
                break;                                                                    \
            case CACE_ARI_PRIM_INT64:                                                     \
                result = (valtype)(in->as_lit.value.as_int64);                            \
                break;                                                                    \
            case CACE_ARI_PRIM_FLOAT64:                                                   \
            {                                                                             \
                feclearexcept(FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_DIVBYZERO);    \
                long long tmp = llroundf(in->as_lit.value.as_float64);                    \
                if (fetestexcept(FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_DIVBYZERO)) \
                {                                                                         \
                    return CACE_AMM_ERR_CONVERT_BADVALUE;                                 \
                }                                                                         \
                result = (valtype)tmp;                                                    \
                break;                                                                    \
            }                                                                             \
            default:                                                                      \
                return CACE_AMM_ERR_CONVERT_BADVALUE;                                     \
        }                                                                                 \
        setter(out, result);                                                              \
        cace_ari_force_lit_type(out, self->as_builtin.ari_type);                          \
    }                                                                                     \
    while (false)

/// Limitations for REAL* ARI types
typedef struct
{
    double float_min;
    double float_max;
} cace_ari_type_anyfloat_constraints_t;

static const cace_ari_type_anyfloat_constraints_t _builtin_real32_cnst = {
    .float_min = -FLT_MAX,
    .float_max = FLT_MAX,
};

static const cace_ari_type_anyfloat_constraints_t _builtin_real64_cnst = {
    .float_min = -DBL_MAX,
    .float_max = DBL_MAX,
};

static bool builtin_anyfloat_constraints(const cace_ari_type_anyfloat_constraints_t *cnst, double *out,
                                         const cace_ari_t *ari)
{
    const cace_ari_lit_t *obj = &(ari->as_lit);
    double                value;
    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            value = obj->value.as_uint64;
            break;
        case CACE_ARI_PRIM_INT64:
            value = obj->value.as_int64;
            break;
        case CACE_ARI_PRIM_FLOAT64:
            value = obj->value.as_float64;
            break;
        default:
            return false;
    }

    if (out)
    {
        *out = value;
    }
    return (isnan(value) || isinf(value) || ((value >= cnst->float_min) && (value <= cnst->float_max)));
}

static cace_amm_type_match_res_t builtin_byte_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyint_constraints(&_builtin_byte_cnst, ari));
}

static int builtin_byte_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        if (!builtin_anyint_constraints(&_builtin_byte_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(cace_ari_byte, cace_ari_set_byte);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_int_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyint_constraints(&_builtin_int_cnst, ari));
}

static int builtin_int_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        if (!builtin_anyint_constraints(&_builtin_int_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(cace_ari_int, cace_ari_set_int);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_uint_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyint_constraints(&_builtin_uint_cnst, ari));
}

static int builtin_uint_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        if (!builtin_anyint_constraints(&_builtin_uint_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(cace_ari_uint, cace_ari_set_uint);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_vast_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyint_constraints(&_builtin_vast_cnst, ari));
}

static int builtin_vast_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        if (!builtin_anyint_constraints(&_builtin_vast_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(cace_ari_vast, cace_ari_set_vast);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_uvast_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyint_constraints(&_builtin_uvast_cnst, ari));
}

static int builtin_uvast_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        if (!builtin_anyint_constraints(&_builtin_uvast_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(cace_ari_uvast, cace_ari_set_uvast);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_real32_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyfloat_constraints(&_builtin_real32_cnst, NULL, ari));
}

static int builtin_real32_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        double result;
        if (!builtin_anyfloat_constraints(&_builtin_real32_cnst, &result, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        cace_ari_set_real32(out, result);
        cace_ari_force_lit_type(out, self->as_builtin.ari_type);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_real64_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(builtin_anyfloat_constraints(&_builtin_real64_cnst, NULL, ari));
}

static int builtin_real64_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        double result;
        if (!builtin_anyfloat_constraints(&_builtin_real64_cnst, &result, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        cace_ari_set_real64(out, result);
        cace_ari_force_lit_type(out, self->as_builtin.ari_type);
        return 0;
    }
}

static bool builtin_time_constraints(struct timespec *out, const cace_ari_t *ari)
{
    const cace_ari_lit_t *obj = &(ari->as_lit);
    struct timespec       value;
    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            value.tv_sec  = obj->value.as_uint64;
            value.tv_nsec = 0;
            break;
        case CACE_ARI_PRIM_INT64:
            value.tv_sec  = obj->value.as_int64;
            value.tv_nsec = 0;
            break;
        case CACE_ARI_PRIM_FLOAT64:
        {
            double v = obj->value.as_float64;
            if (isnan(v) || isinf(v) || v > (double)INT64_MAX || v < (double)INT64_MIN)
            {
                return false;
            }

            double integral;
            double frac = modf(v, &integral);

            value.tv_sec  = (time_t)integral;
            value.tv_nsec = (time_t)(frac * 1000000000);
            break;
        }
        case CACE_ARI_PRIM_TIMESPEC:
            value = obj->value.as_timespec;
            break;
        default:
            return false;
    }

    if (out)
    {
        *out = value;
    }

    return true;
}

static int builtin_time_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else if ((self->as_builtin.ari_type == CACE_ARI_TYPE_TP && in->as_lit.ari_type == CACE_ARI_TYPE_TD)
             || (self->as_builtin.ari_type == CACE_ARI_TYPE_TD && in->as_lit.ari_type == CACE_ARI_TYPE_TP))
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else
    {
        struct timespec result;
        if (!builtin_time_constraints(&result, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        cace_ari_set_tp(out, result);
        cace_ari_force_lit_type(out, self->as_builtin.ari_type);
        return 0;
    }
}

static cace_amm_type_match_res_t builtin_textstr_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(ari->as_lit.prim_type == CACE_ARI_PRIM_TSTR);
}

static cace_amm_type_match_res_t builtin_bytestr_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg(ari->as_lit.prim_type == CACE_ARI_PRIM_BSTR);
}

static cace_amm_type_match_res_t builtin_idseg_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    cace_amm_type_match_res_t got = builtin_common_lit_match(self, ari);
    if (got != CACE_AMM_TYPE_MATCH_NOINFO)
    {
        return got;
    }
    return cace_amm_type_match_pos_neg((ari->as_lit.prim_type == CACE_ARI_PRIM_TSTR)
                                       || (ari->as_lit.prim_type == CACE_ARI_PRIM_INT64));
}

static cace_amm_type_match_res_t builtin_object_match(const cace_amm_type_t *self _U_, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }
    const cace_ari_objpath_t *path = cace_ari_cget_ref_objpath(ari);
    // must have object parts
    if (!path || (path->type_id.form == CACE_ARI_IDSEG_NULL) || (path->obj_id.form == CACE_ARI_IDSEG_NULL))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }
    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

static cace_amm_type_match_res_t builtin_namespace_match(const cace_amm_type_t *self _U_, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }
    const cace_ari_objpath_t *path = cace_ari_cget_ref_objpath(ari);
    // must not have object parts
    if (!path || (path->type_id.form != CACE_ARI_IDSEG_NULL) || (path->obj_id.form != CACE_ARI_IDSEG_NULL))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }
    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

static cace_amm_type_match_res_t builtin_common_objref_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }
    if (!(ari->is_ref))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }
    const cace_ari_ref_t *obj = &(ari->as_ref);
    if (!(obj->objpath.has_ari_type))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }
    return cace_amm_type_match_pos_neg(obj->objpath.ari_type == self->as_builtin.ari_type);
}

static cace_amm_type_t cace_amm_builtins[] = {
    // literal types below
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_LITERAL,
        .ari_name            = builtin_ari_name,
        .match               = builtin_literal_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_NULL,
        .ari_name            = builtin_ari_name,
        .match               = builtin_null_match,
        .convert             = builtin_null_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_BOOL,
        .ari_name            = builtin_ari_name,
        .match               = builtin_bool_match,
        .convert             = builtin_bool_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_BYTE,
        .ari_name            = builtin_ari_name,
        .match               = builtin_byte_match,
        .convert             = builtin_byte_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_INT,
        .ari_name            = builtin_ari_name,
        .match               = builtin_int_match,
        .convert             = builtin_int_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_UINT,
        .ari_name            = builtin_ari_name,
        .match               = builtin_uint_match,
        .convert             = builtin_uint_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_VAST,
        .ari_name            = builtin_ari_name,
        .match               = builtin_vast_match,
        .convert             = builtin_vast_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_UVAST,
        .ari_name            = builtin_ari_name,
        .match               = builtin_uvast_match,
        .convert             = builtin_uvast_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_REAL32,
        .ari_name            = builtin_ari_name,
        .match               = builtin_real32_match,
        .convert             = builtin_real32_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_REAL64,
        .ari_name            = builtin_ari_name,
        .match               = builtin_real64_match,
        .convert             = builtin_real64_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_TEXTSTR,
        .ari_name            = builtin_ari_name,
        .match               = builtin_textstr_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_BYTESTR,
        .ari_name            = builtin_ari_name,
        .match               = builtin_bytestr_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_TP,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_time_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_TD,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_time_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_LABEL,
        .ari_name            = builtin_ari_name,
        .match               = builtin_idseg_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_CBOR,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_ARITYPE,
        .ari_name            = builtin_ari_name,
        .match               = builtin_idseg_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_AC,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_AM,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_TBL,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_EXECSET,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_RPTSET,
        .ari_name            = builtin_ari_name,
        .match               = builtin_default_match,
        .convert             = builtin_default_convert,
    },
    // reference types below
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_OBJECT,
        .ari_name            = builtin_ari_name,
        .match               = builtin_object_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_NAMESPACE,
        .ari_name            = builtin_ari_name,
        .match               = builtin_namespace_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_IDENT,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_TYPEDEF,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_CONST,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_VAR,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_EDD,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_CTRL,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_OPER,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_SBR,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = CACE_AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = CACE_ARI_TYPE_TBR,
        .ari_name            = builtin_ari_name,
        .match               = builtin_common_objref_match,
        .convert             = builtin_default_convert,
    },
};

#ifdef ENABLE_LUT_CACHE

M_DICT_DEF2(cace_amm_type_lookup, cace_ari_type_t, M_BASIC_OPLIST, const cace_amm_type_t *, M_PTR_OPLIST)

/// Cached type dictionary
static cace_amm_type_lookup_t cace_amm_builtin_dict;

/// Initializer for #cace_amm_builtin_dict
void cace_amm_builtin_dict_init(void)
{
    const cace_amm_type_t *curs = cace_amm_builtins;
    const size_t           len  = sizeof(cace_amm_builtins) / sizeof(cace_amm_type_t);
    const cace_amm_type_t *end  = curs + len;

    cace_amm_type_lookup_init(cace_amm_builtin_dict);
    for (; curs < end; ++curs)
    {
        cace_amm_type_lookup_set_at(cace_amm_builtin_dict, curs->as_builtin.ari_type, curs);
    }
}

/// Guard for cace_amm_builtin_dict_init()
static pthread_once_t cace_amm_builtin_dict_ctrl = PTHREAD_ONCE_INIT;

const cace_amm_type_t *cace_amm_type_get_builtin(cace_ari_type_t ari_type)
{
    pthread_once(&cace_amm_builtin_dict_ctrl, cace_amm_builtin_dict_init);
    const cace_amm_type_t **found = cace_amm_type_lookup_get(cace_amm_builtin_dict, ari_type);
    if (!found)
    {
        CACE_LOG_WARNING("failed to get builtin type for %d", ari_type);
    }
    return found ? *found : NULL;
}

#else

const cace_amm_type_t *cace_amm_type_get_builtin(cace_ari_type_t ari_type)
{
    cace_amm_type_t *end = cace_amm_builtins + sizeof(cace_amm_builtins) / sizeof(cace_amm_type_t);
    for (cace_amm_type_t *curs = cace_amm_builtins; curs < end; ++curs)
    {
        if (curs->as_builtin.ari_type == ari_type)
        {
            return curs;
        }
    }
    return NULL;
}

#endif /* ENABLE_LUT_CACHE */

void cace_amm_type_init(cace_amm_type_t *type)
{
    CHKVOID(type)
    *type = CACE_AMM_TYPE_INIT_INVALID;
}

void cace_amm_type_deinit(cace_amm_type_t *type)
{
    CHKVOID(type);

    // clean up semtype state
    switch (type->type_class)
    {
        case CACE_AMM_TYPE_INVALID:
        case CACE_AMM_TYPE_BUILTIN:
            break;
        default:
            if (type->as_semtype_deinit)
            {
                type->as_semtype_deinit(type->as_semtype);
            }

            CACE_FREE(type->as_semtype);
            type->as_semtype = NULL;
    }

    *type = CACE_AMM_TYPE_INIT_INVALID;
}

void cace_amm_type_reset(cace_amm_type_t *type)
{
    cace_amm_type_deinit(type);
    // left in a good state above
}

bool cace_amm_type_is_valid(const cace_amm_type_t *type)
{
    CHKFALSE(type)
    return type->type_class != CACE_AMM_TYPE_INVALID;
}

bool cace_amm_type_get_name(const cace_amm_type_t *type, cace_ari_t *name)
{
    CHKFALSE(type);
    CHKFALSE(name);
    if (!(type->ari_name))
    {
        cace_ari_set_undefined(name);
        return false;
    }

    type->ari_name(type, name);
    return true;
}

/// non-recursive builtin
static int cace_amm_type_set_name_aritype(cace_amm_type_t *type, const cace_ari_t *name)
{
    int aritype_int;
    if (cace_ari_get_int(name, &aritype_int))
    {
        // not an int, can only be tstr
        const char *tstr = cace_ari_cget_tstr_cstr(name);
        if (cace_ari_type_from_name(&aritype_int, tstr))
        {
            CACE_LOG_ERR("Invalid ARITYPE text value %s", tstr);
            return 3;
        }
    }

    const cace_amm_type_t *src = cace_amm_type_get_builtin(aritype_int);
    if (!src)
    {
        CACE_LOG_ERR("Cannot convert from ARITYPE value %" PRId64, aritype_int);
        return 3;
    }

    // copy as plain-old-data
    *type = *src;
    return 0;
}

static int cace_amm_type_set_name_semtype(cace_amm_type_t *type, const cace_ari_t *name,
                                          const cace_amm_obj_store_t *store)
{
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, store, name);
    if (res)
    {
        CACE_LOG_ERR("Failed to dereference semantic type with error %d", res);
        retval = 3;
    }

    if (!retval)
    {
        if (!cace_amm_obj_ns_is_match(deref.ns, 1, 24))
        {
            CACE_LOG_ERR("Semantic type reference not in the namespace ari://ietf/amm-semtype/");
            retval = 3;
        }
    }
    if (!retval)
    {
        // some object in that NS
        if (!deref.obj->obj_id.has_intenum)
        {
            CACE_LOG_CRIT("No registration for amm-semtype");
            retval = 3;
        }
    }
    if (!retval)
    {
        switch (deref.obj->obj_id.intenum)
        {
            case 2: // type-use
                retval = cace_amm_type_set_use_from_name(type, &deref, store);
                break;
            case 3: // ulist
                retval = cace_amm_type_set_ulist_from_name(type, &deref, store);
                break;
            case 4: // dlist
                retval = cace_amm_type_set_dlist_from_name(type, &deref, store);
                break;
            case 5: // umap
                retval = cace_amm_type_set_umap_from_name(type, &deref, store);
                break;
            case 6: // tblt
                retval = cace_amm_type_set_tblt_from_name(type, &deref, store);
                break;
            case 8: // union
                retval = cace_amm_type_set_union_from_name(type, &deref, store);
                break;
            case 9: // seq
                retval = cace_amm_type_set_seq_from_name(type, &deref, store);
                break;
            default:
                CACE_LOG_ERR("Semantic type reference not a known object enumeration %" PRId64,
                             deref.obj->obj_id.intenum);
                retval = 3;
                break;
        }
    }
    cace_amm_lookup_deinit(&deref);
    return retval;
}

int cace_amm_type_set_name(cace_amm_type_t *type, const cace_ari_t *name, const cace_amm_obj_store_t *store)
{
    CHKERR1(type);
    CHKERR1(name);
    CHKERR1(store);

    cace_amm_type_reset(type);

    if (cace_ari_is_lit_typed(name, CACE_ARI_TYPE_ARITYPE))
    {
        return cace_amm_type_set_name_aritype(type, name);
    }

    // recursive semantic type
    const cace_ari_ref_t *ref = cace_ari_cget_ref(name);
    if (ref)
    {
        return cace_amm_type_set_name_semtype(type, name, store);
    }

    m_string_t buf;
    m_string_init(buf);
    cace_ari_text_encode(buf, name, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
    CACE_LOG_ERR("Cannot convert from type name %s", m_string_get_cstr(buf));
    m_string_clear(buf);
    return 2;
}

cace_amm_type_match_res_t cace_amm_type_match(const cace_amm_type_t *type, const cace_ari_t *ari)
{
    CHKRET(type, CACE_AMM_TYPE_MATCH_NEGATIVE);
    CHKRET(ari, CACE_AMM_TYPE_MATCH_NEGATIVE);
    if (!(type->match))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    CACE_LOG_DEBUG("matching with type class %d", type->type_class);
    return type->match(type, ari);
}

int cace_amm_type_convert(const cace_amm_type_t *type, cace_ari_t *out, const cace_ari_t *in)
{
    CHKERR1(type);
    CHKERR1(out);
    CHKERR1(in);

    CHKRET(type->convert, CACE_AMM_ERR_CONVERT_NULLFUNC);

    CACE_LOG_DEBUG("converting with type class %d", type->type_class);
    return type->convert(type, out, in);
}

/** Match a single ARI with its literal type.
 *
 * @return True if it is a typed literal with matching value, or if it
 * is not a typed literal.
 */
static bool cace_amm_builtin_validate_lit(const cace_ari_t *ari, bool is_map_key)
{
    if (is_map_key && ari->as_lit.has_ari_type)
    {
        // specifically disallowed
        return false;
    }

    if (ari->is_ref || !ari->as_lit.has_ari_type)
    {
        // nothing to check
        return true;
    }

    const cace_amm_type_t *typ = cace_amm_type_get_builtin(ari->as_lit.ari_type);
    if (!typ || !(typ->match))
    {
        // failed lookup
        return false;
    }
    return cace_amm_type_match(typ, ari) == CACE_AMM_TYPE_MATCH_POSITIVE;
}

/// Adapt to the visitor interface
static int cace_amm_builtin_validate_visit_ari(cace_ari_t *ari, const cace_ari_visit_ctx_t *ctx)
{
    return cace_amm_builtin_validate_lit(ari, ctx->is_map_key) ? 0 : 1;
}

bool cace_amm_builtin_validate(const cace_ari_t *ari)
{
    CHKFALSE(ari);
    static const cace_ari_visitor_t visitor = {
        .visit_ari = cace_amm_builtin_validate_visit_ari,
    };

    // the visit functions keep the value const
    return (cace_ari_visit((cace_ari_t *)ari, &visitor, NULL) == 0);
}

bool cace_amm_ari_is_truthy(cace_ari_t *obj)
{
    const cace_amm_type_t *type   = cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL);
    cace_ari_t             outval = CACE_ARI_INIT_UNDEFINED;
    int                    res    = cace_amm_type_convert(type, &outval, obj);
    if (res)
    {
        CACE_LOG_ERR("failed to convert object to bool, error %d", res);
        return false;
    }

    cace_ari_bool is_truthy = false;
    cace_ari_get_bool(&outval, &is_truthy);
    return is_truthy;
}
