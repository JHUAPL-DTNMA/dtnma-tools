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
#include "runctx.h"
#include "cace/util/defs.h"

void refda_amm_modval_state_init(refda_amm_modval_state_t *obj)
{
    obj->_ctr             = 0;
    obj->notify           = NULL;
    obj->notify_user_data = NULL;
}

void refda_amm_modval_state_deinit(refda_amm_modval_state_t *obj)
{
    // no real cleanup
    memset(obj, 0, sizeof(*obj));
}

void refda_amm_modval_state_inc(refda_amm_modval_state_t *obj)
{
    obj->_ctr += 1;

    if (obj->notify)
    {
        (obj->notify)(obj, obj->notify_user_data);
    }
}

int refda_runctx_init(refda_runctx_t *ctx, refda_agent_t *agent, const refda_msgdata_t *msg)
{
    CHKERR1(ctx);

    ctx->agent = agent;

    if (msg)
    {
        ctx->mgr_ident = msg->ident.ptr ? &(msg->ident) : NULL;

        const ari_execset_t *eset = ari_get_execset(&(msg->value));
        // should not be null, but guard anyway
        ctx->nonce = eset ? &(eset->nonce) : NULL;
    }
    else
    {
        ctx->mgr_ident = NULL;
        ctx->nonce     = NULL;
    }

    return 0;
}
