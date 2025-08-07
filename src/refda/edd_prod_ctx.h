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

#ifndef REFDA_EDD_PROD_CTX_H_
#define REFDA_EDD_PROD_CTX_H_

#include "agent.h"
#include "valprod.h"
#include "refda/amm/edd.h"
#include <cace/amm/lookup.h>
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REFDA_EDD_PROD_RESULT_TYPE_NOMATCH 2

/** Context for CTRL execution activities.
 */
typedef struct refda_edd_prod_ctx_s
{
    /** Parent production context.
     * This will never be null.
     */
    refda_valprod_ctx_t *prodctx;

    /** Descriptor for the EDD being produced from.
     * This will never be null.
     */
    const refda_amm_edd_desc_t *edd;

} refda_edd_prod_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] edd The EDD descriptor to produce from.
 * @param[in,out] prodctx The production state.
 * The result must outlive this context.
 */
void refda_edd_prod_ctx_init(refda_edd_prod_ctx_t *obj, const refda_amm_edd_desc_t *edd, refda_valprod_ctx_t *prodctx);

void refda_edd_prod_ctx_deinit(refda_edd_prod_ctx_t *obj);

/** Determine if any actual parameter is undefined.
 *
 * @param[in] ctx The production context.
 * @return True if there are any undefined values.
 */
bool refda_edd_prod_ctx_has_aparam_undefined(const refda_edd_prod_ctx_t *ctx);

/** Get an actual parameter for this production.
 *
 * @param[in] ctx The production context.
 * @param[in] index The index into the parameter list.
 * @return A pointer to the parameter value or NULL if no such parameter is present.
 */
const cace_ari_t *refda_edd_prod_ctx_get_aparam_index(const refda_edd_prod_ctx_t *ctx, size_t index);
/// @overload
const cace_ari_t *refda_edd_prod_ctx_get_aparam_name(const refda_edd_prod_ctx_t *ctx, const char *name);

/** Set the result for a value production.
 *
 * @param[in,out] ctx The context to update.
 * @param[in] value The value to use as the production result.
 * @return Zero if successful and the value has a matching type.
 * Otherwise REFDA_EDD_PROD_RESULT_TYPE_NOMATCH.
 */
int refda_edd_prod_ctx_set_result_copy(refda_edd_prod_ctx_t *ctx, const cace_ari_t *value);
/// @overload
int refda_edd_prod_ctx_set_result_move(refda_edd_prod_ctx_t *ctx, cace_ari_t *value);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EDD_PROD_CTX_H_ */
