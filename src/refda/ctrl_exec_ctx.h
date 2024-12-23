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

#ifndef REFDA_CTRL_EXEC_CTX_H_
#define REFDA_CTRL_EXEC_CTX_H_

#include "agent.h"
#include "runctx.h"
#include "refda/amm/ctrl.h"
#include <cace/amm/lookup.h>
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Context for CTRL execution activities.
 */
typedef struct refda_ctrl_exec_ctx_s
{
    /** Parent running context.
     * This will never be null.
     */
    refda_runctx_t *runctx;

    /** Descriptor for the CTRL being executed.
     * This will never be null.
     */
    const refda_amm_ctrl_desc_t *ctrl;

    /** Internal execution bookkeeping item.
     * This will never be null.
     */
    refda_exec_item_t *item;
} refda_ctrl_exec_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] item The internal execution item.
 * The result must outlive this context.
 */
void refda_ctrl_exec_ctx_init(refda_ctrl_exec_ctx_t *obj, const refda_amm_ctrl_desc_t *ctrl, refda_exec_item_t *item);

void refda_ctrl_exec_ctx_deinit(refda_ctrl_exec_ctx_t *obj);

const ari_t *refda_ctrl_exec_ctx_get_aparam_index(refda_ctrl_exec_ctx_t *ctx, size_t index);

const ari_t *refda_ctrl_exec_ctx_get_aparam_name(refda_ctrl_exec_ctx_t *ctx, const char *name);

/** Mark this control run as waiting for some finish condition.
 *
 * @param[in,out] ctx The context to update.
 * @param[in] event An optional callback event to check the finished condition later.
 */
void refda_ctrl_exec_ctx_set_waiting(refda_ctrl_exec_ctx_t *ctx, const refda_timeline_event_t *event);

/** Set the result for an execution, which marks it as finished.
 *
 * @param[in,out] ctx The context to update.
 * @param[in] value The value to use as the execution result.
 */
void refda_ctrl_exec_ctx_set_result_copy(refda_ctrl_exec_ctx_t *ctx, const ari_t *value);
/// @overload
void refda_ctrl_exec_ctx_set_result_move(refda_ctrl_exec_ctx_t *ctx, ari_t *value);
/** @overload
 * Convenience member to mark the result as successful but null-value.
 */
void refda_ctrl_exec_ctx_set_result_null(refda_ctrl_exec_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_CTRL_EXEC_CTX_H_ */
