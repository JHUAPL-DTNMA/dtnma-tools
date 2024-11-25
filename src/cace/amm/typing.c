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
#include "typing.h"
#include "semtype.h"
#include "cace/ari/algo.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"
#include "cace/config.h"
#include <m-dict.h>
#include <pthread.h>
#include <math.h>
#include <float.h>
#include <fenv.h>

/** Implement common match logic.
 * This checks that the object-reference or literal type, if present, is
 * as required by the type object.
 *
 * @return False if the common logic failed the match.
 */
static bool builtin_common_lit_match(const amm_type_t *self, const ari_t *ari)
{
    if (ari->is_ref)
    {
        return false;
    }
    else
    {
        const ari_lit_t *obj = &(ari->as_lit);
        // explicit type matching
        if (obj->has_ari_type && (obj->ari_type != self->as_builtin.ari_type))
        {
            return false;
        }
    }
    return true;
}

/** Implement common conversion logic.
 * This passes-through undefined values.
 *
 * @return True if the common logic did the conversion.
 */
static bool builtin_common_convert(const amm_type_t *self _U_, ari_t *out, const ari_t *in)
{
    if (ari_is_undefined(in))
    {
        ari_set_undefined(out);
        return true;
    }
    return false;
}

static void ari_force_lit_type(ari_t *out, ari_type_t ari_type)
{
    out->as_lit.has_ari_type = true;
    out->as_lit.ari_type     = ari_type;
}

static bool builtin_literal_match(const amm_type_t *self _U_, const ari_t *ari)
{
    return !(ari->is_ref);
}

/** A default convert function that simply copies a value that matches using
 * the type object, and copies the undefined value unconditionally.
 */
static int builtin_default_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }
    if (!(self->match))
    {
        return CACE_AMM_ERR_CONVERT_NULLFUNC;
    }
    if (!(self->match)(self, in))
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    ari_set_copy(out, in);
    return 0;
}

static bool builtin_null_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_NULL;
}

static int builtin_null_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }
    ari_set_null(out);
    ari_force_lit_type(out, self->as_builtin.ari_type);
    return 0;
}

static bool builtin_bool_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_BOOL;
}

static int builtin_bool_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
            case ARI_PRIM_UNDEFINED:
                // not really needed but satisfies the compiler
            case ARI_PRIM_NULL:
                result = false;
                break;
            case ARI_PRIM_BOOL:
                result = in->as_lit.value.as_bool;
                break;
            case ARI_PRIM_UINT64:
                result = (in->as_lit.value.as_uint64 != 0);
                break;
            case ARI_PRIM_INT64:
                result = (in->as_lit.value.as_int64 != 0);
                break;
            case ARI_PRIM_FLOAT64:
                result = (!isnan(in->as_lit.value.as_float64) && (in->as_lit.value.as_float64 != 0));
                break;
            case ARI_PRIM_TSTR:
            case ARI_PRIM_BSTR:
                result = (in->as_lit.value.as_data.len != 0);
                break;
            case ARI_PRIM_TIMESPEC:
                result = ((in->as_lit.value.as_timespec.tv_sec != 0) || (in->as_lit.value.as_timespec.tv_nsec != 0));
                break;
            case ARI_PRIM_OTHER:
                switch (in->as_lit.ari_type)
                {
                    case ARI_TYPE_AC:
                    case ARI_TYPE_AM:
                    case ARI_TYPE_TBL:
                    case ARI_TYPE_EXECSET:
                    case ARI_TYPE_RPTSET:
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

    ari_set_bool(out, result);
    ari_force_lit_type(out, self->as_builtin.ari_type);
    return 0;
}

/// Limitations for integer ARI types
typedef struct
{
    uint64_t uint_max;
    int64_t  int_min;
    int64_t  int_max;
} ari_type_anyint_constraints_t;

static const ari_type_anyint_constraints_t ari_builtin_byte_cnst = {
    .uint_max = UINT8_MAX,
    .int_min  = 0,
    .int_max  = UINT8_MAX,
};

static const ari_type_anyint_constraints_t ari_builtin_int_cnst = {
    .uint_max = INT32_MAX,
    .int_min  = INT32_MIN,
    .int_max  = INT32_MAX,
};

static const ari_type_anyint_constraints_t ari_builtin_uint_cnst = {
    .uint_max = UINT32_MAX,
    .int_min  = 0,
    .int_max  = UINT32_MAX,
};

static const ari_type_anyint_constraints_t ari_builtin_vast_cnst = {
    .uint_max = INT64_MAX,
    .int_min  = 0,
    .int_max  = INT64_MAX,
};

static const ari_type_anyint_constraints_t ari_builtin_uvast_cnst = {
    .uint_max = UINT64_MAX,
    .int_min  = 0,
    .int_max  = INT64_MAX, // fudge the truth to not overflow
};

static bool builtin_anyint_constraints(const ari_type_anyint_constraints_t *cnst, const ari_t *ari)
{
    const ari_lit_t *obj = &(ari->as_lit);
    switch (obj->prim_type)
    {
        case ARI_PRIM_UINT64:
            return (obj->value.as_uint64 <= cnst->uint_max);
        case ARI_PRIM_INT64:
            return ((obj->value.as_int64 >= cnst->int_min) && (obj->value.as_int64 <= cnst->int_max));
        case ARI_PRIM_FLOAT64:
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
            case ARI_PRIM_BOOL:                                                           \
                result = (valtype)(in->as_lit.value.as_bool);                             \
                break;                                                                    \
            case ARI_PRIM_UINT64:                                                         \
                result = (valtype)(in->as_lit.value.as_uint64);                           \
                break;                                                                    \
            case ARI_PRIM_INT64:                                                          \
                result = (valtype)(in->as_lit.value.as_int64);                            \
                break;                                                                    \
            case ARI_PRIM_FLOAT64:                                                        \
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
        ari_force_lit_type(out, self->as_builtin.ari_type);                               \
    }                                                                                     \
    while (false)

/// Limitations for REAL* ARI types
typedef struct
{
    double float_min;
    double float_max;
} ari_type_anyfloat_constraints_t;

static const ari_type_anyfloat_constraints_t ari_builtin_real32_cnst = {
    .float_min = -FLT_MAX,
    .float_max = FLT_MAX,
};

static const ari_type_anyfloat_constraints_t ari_builtin_real64_cnst = {
    .float_min = -DBL_MAX,
    .float_max = DBL_MAX,
};

static bool builtin_anyfloat_constraints(const ari_type_anyfloat_constraints_t *cnst, double *out, const ari_t *ari)
{
    const ari_lit_t *obj = &(ari->as_lit);
    double           value;
    switch (obj->prim_type)
    {
        case ARI_PRIM_UINT64:
            value = obj->value.as_uint64;
            break;
        case ARI_PRIM_INT64:
            value = obj->value.as_int64;
            break;
        case ARI_PRIM_FLOAT64:
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

static bool builtin_byte_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyint_constraints(&ari_builtin_byte_cnst, ari);
}

static int builtin_byte_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
        if (!builtin_anyint_constraints(&ari_builtin_byte_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(ari_byte, ari_set_int);
        return 0;
    }
}

static bool builtin_int_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyint_constraints(&ari_builtin_int_cnst, ari);
}

static int builtin_int_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
        if (!builtin_anyint_constraints(&ari_builtin_int_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(ari_int, ari_set_int);
        return 0;
    }
}

static bool builtin_uint_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyint_constraints(&ari_builtin_uint_cnst, ari);
}

static int builtin_uint_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
        if (!builtin_anyint_constraints(&ari_builtin_uint_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(ari_uint, ari_set_uint);
        return 0;
    }
}

static bool builtin_vast_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyint_constraints(&ari_builtin_vast_cnst, ari);
}

static int builtin_vast_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
        if (!builtin_anyint_constraints(&ari_builtin_vast_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(ari_vast, ari_set_vast);
        return 0;
    }
}

static bool builtin_uvast_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyint_constraints(&ari_builtin_uvast_cnst, ari);
}

static int builtin_uvast_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
        if (!builtin_anyint_constraints(&ari_builtin_uvast_cnst, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        BUILTIN_ANYINT_EXTRACT(ari_uvast, ari_set_uvast);
        return 0;
    }
}

static bool builtin_real32_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyfloat_constraints(&ari_builtin_real32_cnst, NULL, ari);
}

static bool builtin_real64_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return builtin_anyfloat_constraints(&ari_builtin_real64_cnst, NULL, ari);
}

static int builtin_real64_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
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
        if (!builtin_anyfloat_constraints(&ari_builtin_real64_cnst, &result, in))
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
        ari_set_real64(out, result);
        ari_force_lit_type(out, self->as_builtin.ari_type);
        return 0;
    }
}

static bool builtin_time_constraints(struct timespec *out, const ari_t *ari)
{
    const ari_lit_t *obj = &(ari->as_lit);
    struct timespec  value;
    switch (obj->prim_type)
    {
        case ARI_PRIM_UINT64:
            value.tv_sec  = obj->value.as_uint64;
            value.tv_nsec = 0;
            break;
        case ARI_PRIM_INT64:
            value.tv_sec  = obj->value.as_int64;
            value.tv_nsec = 0;
            break;
        case ARI_PRIM_FLOAT64:
        {
            double v = obj->value.as_float64;
            if (isnan(v) || isinf(v) || v > INT64_MAX || v < INT64_MIN)
            {
                return false;
            }

            double integral;
            double frac = modf(v, &integral);

            value.tv_sec  = (time_t)integral;
            value.tv_nsec = (time_t)(frac * 1000000000);
            break;
        }
        case ARI_PRIM_TIMESPEC:
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

static int builtin_time_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    if (builtin_common_convert(self, out, in))
    {
        return 0;
    }

    if (in->is_ref)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }
    else if ((self->as_builtin.ari_type == ARI_TYPE_TP && in->as_lit.ari_type == ARI_TYPE_TD)
             || (self->as_builtin.ari_type == ARI_TYPE_TD && in->as_lit.ari_type == ARI_TYPE_TP))
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
        ari_set_tp(out, result);
        ari_force_lit_type(out, self->as_builtin.ari_type);
        return 0;
    }
}

static bool builtin_textstr_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_TSTR;
}

static bool builtin_bytestr_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_BSTR;
}

static bool builtin_idseg_match(const amm_type_t *self, const ari_t *ari)
{
    if (!builtin_common_lit_match(self, ari))
    {
        return false;
    }
    return ((ari->as_lit.prim_type == ARI_PRIM_TSTR) || (ari->as_lit.prim_type == ARI_PRIM_INT64));
}

static bool builtin_object_match(const amm_type_t *self _U_, const ari_t *ari)
{
    return ari->is_ref;
}

static bool builtin_anyobj_match(const amm_type_t *self, const ari_t *ari)
{
    if (!(ari->is_ref))
    {
        return false;
    }
    const ari_ref_t *obj = &(ari->as_ref);
    if (!(obj->objpath.has_ari_type))
    {
        return false;
    }
    if (obj->objpath.ari_type != self->as_builtin.ari_type)
    {
        return false;
    }
    return true;
}

static amm_type_t amm_builtins[] = {
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_LITERAL,
        .match               = builtin_literal_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_NULL,
        .match               = builtin_null_match,
        .convert             = builtin_null_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_BOOL,
        .match               = builtin_bool_match,
        .convert             = builtin_bool_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_BYTE,
        .match               = builtin_byte_match,
        .convert             = builtin_byte_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_INT,
        .match               = builtin_int_match,
        .convert             = builtin_int_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_UINT,
        .match               = builtin_uint_match,
        .convert             = builtin_uint_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_VAST,
        .match               = builtin_vast_match,
        .convert             = builtin_vast_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_UVAST,
        .match               = builtin_uvast_match,
        .convert             = builtin_uvast_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_REAL32,
        .match               = builtin_real32_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_REAL64,
        .match               = builtin_real64_match,
        .convert             = builtin_real64_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_TEXTSTR,
        .match               = builtin_textstr_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_BYTESTR,
        .match               = builtin_bytestr_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_TP,
        .match               = builtin_common_lit_match,
        .convert             = builtin_time_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_TD,
        .match               = builtin_common_lit_match,
        .convert             = builtin_time_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_LABEL,
        .match               = builtin_idseg_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_CBOR,
        .match               = builtin_common_lit_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_ARITYPE,
        .match               = builtin_idseg_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_AC,
        .match               = builtin_common_lit_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_AM,
        .match               = builtin_common_lit_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_TBL,
        .match               = builtin_common_lit_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_EXECSET,
        .match               = builtin_common_lit_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_RPTSET,
        .match               = builtin_common_lit_match,
        .convert             = builtin_default_convert,
    },
    // FIXME add containers

    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_OBJECT,
        .match               = builtin_object_match,
        .convert             = builtin_default_convert,
    },
    {
        .type_class          = AMM_TYPE_BUILTIN,
        .as_builtin.ari_type = ARI_TYPE_IDENT,
        .match               = builtin_anyobj_match,
        .convert             = builtin_default_convert,
    },
    // FIXME add objects
};

#ifdef ENABLE_LUT_CACHE

DICT_DEF2(amm_type_lookup, ari_type_t, M_BASIC_OPLIST, const amm_type_t *, M_PTR_OPLIST)

/// Cached type dictionary
static amm_type_lookup_t amm_builtin_dict;

/// Initializer for #amm_builtin_dict
void amm_builtin_dict_init(void)
{
    const amm_type_t *curs = amm_builtins;
    const size_t      len  = sizeof(amm_builtins) / sizeof(amm_type_t);
    const amm_type_t *end  = curs + len;

    amm_type_lookup_init(amm_builtin_dict);
    for (; curs < end; ++curs)
    {
        amm_type_lookup_set_at(amm_builtin_dict, curs->as_builtin.ari_type, curs);
    }
}

/// Guard for amm_builtin_dict_init()
static pthread_once_t amm_builtin_dict_ctrl = PTHREAD_ONCE_INIT;

const amm_type_t *amm_type_get_builtin(ari_type_t ari_type)
{
    pthread_once(&amm_builtin_dict_ctrl, amm_builtin_dict_init);
    const amm_type_t **found = amm_type_lookup_get(amm_builtin_dict, ari_type);
    return found ? *found : NULL;
}

#else

const amm_type_t *amm_type_get_builtin(ari_type_t ari_type)
{
    amm_type_t *end = amm_builtins + sizeof(amm_builtins) / sizeof(amm_type_t);
    for (amm_type_t *curs = amm_builtins; curs < end; ++curs)
    {
        if (curs->as_builtin.ari_type == ari_type)
        {
            return curs;
        }
    }
    return NULL;
}

#endif /* ENABLE_LUT_CACHE */

void amm_typeptr_init(amm_typeptr_t *ptr)
{
    CHKVOID(ptr);

    ptr->obj = ARI_MALLOC(sizeof(amm_type_t));
    if (ptr->obj)
    {
        amm_type_init(ptr->obj);
    }
}

void amm_typeptr_deinit(amm_typeptr_t *ptr)
{
    CHKVOID(ptr);
    if (ptr->obj)
    {
        ARI_FREE(ptr->obj);
        ptr->obj = NULL;
    }
}

void amm_typeptr_take(amm_typeptr_t *ptr, amm_type_t *obj)
{
    CHKVOID(ptr);
    amm_typeptr_deinit(ptr);

    ptr->obj = obj;
}

void amm_type_tblt_col_init(amm_semtype_tblt_col_t *obj)
{
    CHKVOID(obj);
    string_init(obj->name);
    amm_type_init(&(obj->typeobj));
}

void amm_type_tblt_col_deinit(amm_semtype_tblt_col_t *obj)
{
    CHKVOID(obj);
    amm_type_deinit(&(obj->typeobj));
    string_clear(obj->name);
}

void amm_type_init(amm_type_t *type)
{
    CHKVOID(type)
    *type = AMM_TYPE_INIT_INVALID;
}

void amm_type_deinit(amm_type_t *type)
{
    CHKVOID(type);
    // clean up state
    switch (type->type_class)
    {
        case AMM_TYPE_INVALID:
        case AMM_TYPE_BUILTIN:
            break;
        case AMM_TYPE_USE:
            amm_semtype_use_deinit(type->as_semtype);
            break;
        case AMM_TYPE_ULIST:
            amm_semtype_ulist_deinit(type->as_semtype);
            break;
        case AMM_TYPE_DLIST:
            amm_semtype_dlist_deinit(type->as_semtype);
            break;
        case AMM_TYPE_UMAP:
            amm_semtype_umap_deinit(type->as_semtype);
            break;
        case AMM_TYPE_TBLT:
            amm_semtype_tblt_deinit(type->as_semtype);
            break;
        case AMM_TYPE_UNION:
            amm_semtype_union_deinit(type->as_semtype);
            break;
    }

    // release memory
    switch (type->type_class)
    {
        case AMM_TYPE_INVALID:
        case AMM_TYPE_BUILTIN:
            break;
        default:
            ARI_FREE(type->as_semtype);
            type->as_semtype = NULL;
    }

    *type = AMM_TYPE_INIT_INVALID;
}

void amm_type_reset(amm_type_t *type)
{
    amm_type_deinit(type);
    // left in a good state above
}

bool amm_type_is_valid(const amm_type_t *type)
{
    CHKFALSE(type)
    return type->type_class != AMM_TYPE_INVALID;
}

bool amm_type_match(const amm_type_t *type, const ari_t *ari)
{
    CHKFALSE(type);
    CHKFALSE(ari);
    if (!(type->match))
    {
        return false;
    }

    return type->match(type, ari);
}

int amm_type_convert(const amm_type_t *type, ari_t *out, const ari_t *in)
{
    CHKERR1(type);
    CHKERR1(out);
    CHKERR1(in);

    CHKRET(type->convert, CACE_AMM_ERR_CONVERT_NULLFUNC);

    return type->convert(type, out, in);
}

/** Match a single ARI with its literal type.
 *
 * @return True if it is a typed literal with matching value, or if it
 * is not a typed literal.
 */
static bool amm_builtin_validate_lit(const ari_t *ari, bool is_map_key)
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

    const amm_type_t *typ = amm_type_get_builtin(ari->as_lit.ari_type);
    if (!typ || !(typ->match))
    {
        // failed lookup
        return false;
    }
    return typ->match(typ, ari);
}

/// Adapt to the visitor interface
static int amm_builtin_validate_visit_ari(const ari_t *ari, const ari_visit_ctx_t *ctx)
{
    return amm_builtin_validate_lit(ari, ctx->is_map_key) ? 0 : 1;
}

bool amm_builtin_validate(const ari_t *ari)
{
    CHKFALSE(ari);
    static const ari_visitor_t visitor = {
        .visit_ari = amm_builtin_validate_visit_ari,
    };

    return (ari_visit(ari, &visitor, NULL) == 0);
}
