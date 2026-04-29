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
#ifndef CACE_AMM_PROMOTE_H_
#define CACE_AMM_PROMOTE_H_

#include "cace/ari/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Determine the equivalent built-in type of the provided literal-value.
 *
 * @param[in] lit The input value.
 * @return The equivalent built-in type. Returns CACE_ARI_TYPE_NULL if there is no equivalent.
 */
cace_ari_type_t cace_amm_promote_eqiv_lit_type(const cace_ari_lit_t *lit);

/** Determine if given type is numeric.
 *
 * @param[in] typ Input type.
 * @return true if numeric type.
 */
bool cace_amm_promote_is_numeric(cace_ari_type_t typ);

typedef struct
{
    const cace_ari_t *lt_val;
    const cace_ari_t *rt_val;

    /// Common promotion built-in type
    cace_ari_type_t common;

    /// Optionally used convert storage
    cace_ari_t lt_prom;
    /// Optionally used convert storage
    cace_ari_t rt_prom;

    /// The final left value to use
    const cace_ari_t *lt_use;
    /// The final right value to use
    const cace_ari_t *rt_use;
} cace_amm_promote_state_t;

void cace_amm_promote_init(cace_amm_promote_state_t *obj);

void cace_amm_promote_deinit(cace_amm_promote_state_t *obj);

/** Determine the numeric type which is the least common promotion type.
 * This is done in accordance with Section 6.11.2.1 of @cite amm.
 *
 * @param[out] result The promotion type if the return is zero.
 * @param[in] lt_val The left input value.
 * @param[in] rt_val The right input value.
 * @return Zero if successful.
 */
int cace_amm_promote_process(cace_amm_promote_state_t *obj, const cace_ari_t *lt_val, const cace_ari_t *rt_val);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_PROMOTE_H_ */
