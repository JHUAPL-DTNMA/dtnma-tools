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
#ifndef CACE_AMM_NUMERIC_H_
#define CACE_AMM_NUMERIC_H_

#include "cace/ari/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Descriptor for numeric binary operators.
 * Each function is used for a different promoted primitive type.
 */
typedef struct
{
    cace_ari_uvast (*binop_uvast)(cace_ari_uvast, cace_ari_uvast);
    cace_ari_vast (*binop_vast)(cace_ari_vast, cace_ari_vast);
    cace_ari_real64 (*binop_real64)(cace_ari_real64, cace_ari_real64);
    int (*binop_timespec)(cace_ari_t *, const cace_ari_t *, const cace_ari_t *);
} cace_numeric_binary_desc_t;

/** Execute a numeric operation accepting two integers
 *
 * @param[out] result Output value.
 * @param[in] lt_val The left input value.
 * @param[in] rt_val The right input value.
 * @param[in] desc The operator descriptor, with only uvast and vast functions needed.
 * @return Zero if successful.
 */
int cace_numeric_integer_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                         const cace_numeric_binary_desc_t *desc);

/** Execute a numeric operation accepting two parameters.
 *
 * @param[out] result Output value.
 * @param[in] lt_val The left input value.
 * @param[in] rt_val The right input value.
 * @param[in] desc The operator descriptor, with all functions needed.
 * @return Zero if successful.
 */
int cace_numeric_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                 const cace_numeric_binary_desc_t *desc);

/** Descriptor for numeric binary operators.
 * Each function is used for a different promoted primitive type.
 */
typedef struct
{
    bool (*binop_uvast)(cace_ari_uvast, cace_ari_uvast);
    bool (*binop_vast)(cace_ari_vast, cace_ari_vast);
    bool (*binop_real64)(cace_ari_real64, cace_ari_real64);
    bool (*binop_timespec)(struct timespec, struct timespec);
} cace_numeric_compare_desc_t;

/** Compare two numeric values.
 *
 * @param[out] result Output result as a boolean.
 * @param[in] lt_val The left input value.
 * @param[in] rt_val The right input value.
 * @param[in] desc The operator descriptor, with all functions needed.
 * @return Zero if successful.
 */
int cace_numeric_compare_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                  const cace_numeric_compare_desc_t *desc);

/** Determine if given value is zero.
 *
 * @param[in] val Input value.
 * @return true if equal to zero.
 */
bool cace_numeric_is_zero(const cace_ari_t *val);

/** Determine if given object is using a numeric primitive type.
 *
 * @param[in] val Input object.
 * @return true if numeric prim type.
 */
bool cace_has_numeric_prim_type(const cace_ari_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_NUMERIC_H_ */
