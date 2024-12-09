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
#ifndef CACE_AMM_RANGE_H_
#define CACE_AMM_RANGE_H_

#include "cace/config.h"
#include "cace/util/defs.h"
#include <m-rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A closed interval of numeric values with optional minimum and maximum.
 *
 * A single interval struct is defined as `intvl_t`.
 * A range containing ordered sequence of intervals is defined as `range_t`
 *
 * Members of the interval struct are:
 *  * `has_min` True if this interval has a finite minimum.
 *  * `i_min` The minimum value of the interval, valid if @c has_min is true.
 *  * `has_max` True if this interval has a finite maximum.
 *  * `i_max` The maximum value of the interval, valid if @c has_max is true.
 *
 * Functions for the interval struct are the following:
 *
 * `intvl_init()` Initialize a fully infinite interval.
 * `intvl_deinit()` Clean up state.
 * `intvl_is_infinite()` Determine if this interval is open on both ends (is fully infinite).
 * `intvl_cmp()` Compare to intervals by their extents.
 * `intvl_contains()` Determine if a value is within the interval.
 *
 * Functions for the range struct are the following:
 *
 * `range_contains()` Determine if a value is within the range.
 */
#define CACE_RANGE_DEF(range, intvl, type)                                                               \
    typedef struct                                                                                       \
    {                                                                                                    \
        bool has_min;                                                                                    \
        type i_min;                                                                                      \
        bool has_max;                                                                                    \
        type i_max;                                                                                      \
    } M_C(intvl, _t);                                                                                    \
                                                                                                         \
    static inline void M_C(intvl, _set_infinite)(M_C(intvl, _t) * obj)                                   \
    {                                                                                                    \
        *obj = (M_C(intvl, _t)) {                                                                        \
            .has_min = false,                                                                            \
            .has_max = false,                                                                            \
        };                                                                                               \
    }                                                                                                    \
    static inline void M_C(intvl, _set_singleton)(M_C(intvl, _t) * obj, type val)                        \
    {                                                                                                    \
        *obj = (M_C(intvl, _t)) {                                                                        \
            .has_min = true,                                                                             \
            .i_min   = val,                                                                              \
            .has_max = true,                                                                             \
            .i_max   = val,                                                                              \
        };                                                                                               \
    }                                                                                                    \
    static inline void M_C(intvl, _set_min)(M_C(intvl, _t) * obj, type val)                        \
    {                                                                                                    \
        obj->has_min = true;                                                                             \
        obj->i_min   = val;                                                                              \
    }                                                                                                    \
    static inline void M_C(intvl, _set_max)(M_C(intvl, _t) * obj, type val)                        \
    {                                                                                                    \
        obj->has_max = true;                                                                             \
        obj->i_max   = val;                                                                              \
    }                                                                                                    \
    static inline void M_C(intvl, _set_finite)(M_C(intvl, _t) * obj, type min_val, type max_val)         \
    {                                                                                                    \
        *obj = (M_C(intvl, _t)) {                                                                        \
            .has_min = true,                                                                             \
            .i_min   = min_val,                                                                          \
            .has_max = true,                                                                             \
            .i_max   = max_val,                                                                          \
        };                                                                                               \
    }                                                                                                    \
                                                                                                         \
    static inline bool M_C(intvl, _is_infinite)(const M_C(intvl, _t) * obj)                              \
    {                                                                                                    \
        return !(obj->has_min) && !(obj->has_max);                                                       \
    }                                                                                                    \
                                                                                                         \
    static inline int M_C(intvl, _cmp)(const M_C(intvl, _t) * lt, const M_C(intvl, _t) * rt)             \
    {                                                                                                    \
        const bool lt_inf = M_C(intvl, _is_infinite)(lt);                                                \
        const bool rt_inf = M_C(intvl, _is_infinite)(rt);                                                \
        if (lt_inf && rt_inf)                                                                            \
        {                                                                                                \
            return 0;                                                                                    \
        }                                                                                                \
        if (lt_inf)                                                                                      \
        {                                                                                                \
            return -1;                                                                                   \
        }                                                                                                \
        if (rt_inf)                                                                                      \
        {                                                                                                \
            return 1;                                                                                    \
        }                                                                                                \
        if ((lt->has_min == rt->has_min) && (!(lt->has_min) || (lt->i_min == rt->i_min))                 \
            && (lt->has_max == rt->has_max) && (!(lt->has_max) || (lt->i_max == rt->i_max)))             \
        {                                                                                                \
            return 0;                                                                                    \
        }                                                                                                \
        const type *lt_val = lt->has_max ? &(lt->i_max) : &(lt->i_min);                                  \
        const type *rt_val = rt->has_max ? &(rt->i_max) : &(rt->i_min);                                  \
        return *lt_val < *rt_val ? -1 : 1;                                                               \
    }                                                                                                    \
    static inline bool M_C(intvl, _contains)(const M_C(intvl, _t) * obj, type val)                       \
    {                                                                                                    \
        if (obj->has_min && (obj->i_min > val))                                                          \
        {                                                                                                \
            return false;                                                                                \
        }                                                                                                \
        if (obj->has_max && (obj->i_max < val))                                                          \
        {                                                                                                \
            return false;                                                                                \
        }                                                                                                \
        return true;                                                                                     \
    }                                                                                                    \
                                                                                                         \
    RBTREE_DEF(M_C(intvl, _seq), M_C(intvl, _t), M_OPEXTEND(M_POD_OPLIST, CMP(API_6(M_C(intvl, _cmp))))) \
                                                                                                         \
    typedef struct                                                                                       \
    {                                                                                                    \
        M_C(intvl, _seq_t) intvls;                                                                       \
    } M_C(range, _t);                                                                                    \
                                                                                                         \
    static inline void M_C(range, _init)(M_C(range, _t) * obj)                                           \
    {                                                                                                    \
        M_C(intvl, _seq_init)(obj->intvls);                                                              \
    }                                                                                                    \
                                                                                                         \
    static inline void M_C(range, _deinit)(M_C(range, _t) * obj)                                         \
    {                                                                                                    \
        M_C(intvl, _seq_clear)(obj->intvls);                                                             \
    }                                                                                                    \
    static inline bool M_C(range, _contains)(const M_C(range, _t) * obj, type val)                       \
    {                                                                                                    \
        M_C(intvl, _t) chkval;                                                                           \
        M_C(intvl, _set_singleton)(&chkval, val);                                                        \
                                                                                                         \
        M_C(intvl, _seq_it_t) it;                                                                        \
        M_C(intvl, _seq_it_from)(it, obj->intvls, chkval);                                               \
        if (M_C(intvl, _seq_end_p)(it))                                                                  \
        {                                                                                                \
            const M_C(intvl, _t) *last = M_C(intvl, _seq_cmax)(obj->intvls);                             \
            if (last)                                                                                    \
            {                                                                                            \
                return M_C(intvl, _contains)(last, val);                                                 \
            }                                                                                            \
            return false;                                                                                \
        }                                                                                                \
        return M_C(intvl, _contains)(M_C(intvl, _seq_cref)(it), val);                                    \
    }

/** @struct cace_amm_range_intvl_size_t
 * An interval of size_t values.
 */
/** @struct cace_amm_range_size_t
 * A range of size_t values.
 */
/** @struct cace_amm_range_intvl_int64_t
 * An interval of int64_t values.
 */
/** @struct cace_amm_range_int64_t
 * A range of int64_t values.
 */
/// @cond Doxygen_Suppress
CACE_RANGE_DEF(cace_amm_range_size, cace_amm_range_intvl_size, size_t)
CACE_RANGE_DEF(cace_amm_range_int64, cace_amm_range_intvl_int64, int64_t)
/// @endcond

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_RANGE_H_ */
