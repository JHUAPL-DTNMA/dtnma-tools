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

typedef cace_ari_uvast (*cace_binop_uvast)(cace_ari_uvast, cace_ari_uvast);
typedef cace_ari_vast (*cace_binop_vast)(cace_ari_vast, cace_ari_vast);
typedef cace_ari_real64 (*cace_binop_real64)(cace_ari_real64, cace_ari_real64);
typedef int (*cace_binop_timespec)(cace_ari_t *, const cace_ari_t *, const cace_ari_t *);

/** Determine the numeric type which is the least common promotion type.
 * This is done in accordance with Section 6.11.2.1 of @cite amm.
 *
 * @param[out] result The promotion type if the return is zero.
 * @param[in] left One input value.
 * @param[in] right The other input value.
 * @return Zero if successful.
 */
int cace_amm_numeric_promote_type(cace_ari_type_t *result, const cace_ari_t *left, const cace_ari_t *right);

/**
 * Determine the equivalent ari type of the provided literal-value ARI.
 *
 * @param[in] The input value.
 * @return The equivalent ari type. Returns CACE_ARI_TYPE_NULL if there is no equivalent.
 */
cace_ari_type_t cace_eqiv_ari_type(const cace_ari_lit_t *lit);

/** Execute a numeric operation accepting two integers
 *
 * @param[out] result Output value.
 * @param[in] left One input value.
 * @param[in] right The other input value.
 * @return Zero if successful.
 */
int cace_numeric_integer_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                         cace_binop_uvast op_uvast, cace_binop_vast op_vast);

/** Execute a numeric operation accepting two parameters.
 *
 * @param[out] result Output value.
 * @param[in] left One input value.
 * @param[in] right The other input value.
 * @param[in] op_uvast Operator function for uvast types.
 * @param[in] op_vast Operator function for vast types.
 * @param[in] op_real64 Operator function for real64 types.
 * @param[in] op_timespec Operator function for timespec related types.
 * @return Zero if successful.
 */
int cace_numeric_binary_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                 cace_binop_uvast op_uvast, cace_binop_vast op_vast, cace_binop_real64 op_real64,
                                 cace_binop_timespec op_timespec);

/** Compare two numeric values.
 *
 * @param[out] result Output result as a boolean.
 * @param[in] left One input value.
 * @param[in] right The other input value.
 * @param[in] op_uvast Operator function for uvast types.
 * @param[in] op_vast Operator function for vast types.
 * @param[in] op_real64 Operator function for real64 types.
 * @return Zero if successful.
 */
int cace_numeric_binary_comparison_operator(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val,
                                            cace_binop_uvast op_uvast, cace_binop_vast op_vast,
                                            cace_binop_real64 op_real64);

/** Determine if given value is zero.
 *
 * @param[in] val Input value.
 * @return true if equal to zero.
 */
bool cace_numeric_is_zero(const cace_ari_t *val);

/** Determine if given type is numeric.
 *
 * @param[in] val Input value.
 * @return true if numeric type.
 */
bool cace_is_numeric_type(cace_ari_type_t typ);

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
