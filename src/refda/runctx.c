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
#include "runctx.h"
#include "agent.h"
#include "msgdata.h"
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

void refda_runctx_init(refda_runctx_t *ctx)
{
    CHKVOID(ctx);
    ctx->agent = NULL;
    cace_ari_init(&(ctx->mgr_ident));
    cace_ari_init(&(ctx->nonce));
    ctx->acl_gen = 0;
    refda_acl_id_tree_init(ctx->acl_groups);
}

void refda_runctx_deinit(refda_runctx_t *ctx)
{
    CHKVOID(ctx);
    refda_acl_id_tree_clear(ctx->acl_groups);
    cace_ari_deinit(&(ctx->nonce));
    cace_ari_deinit(&(ctx->mgr_ident));
    ctx->agent = NULL;
}

int refda_runctx_from(refda_runctx_t *ctx, refda_agent_t *agent, const refda_msgdata_t *msg)
{
    CHKERR1(ctx);

    ctx->agent = agent;

    if (msg)
    {
        cace_ari_set_copy(&ctx->mgr_ident, &msg->ident);

        const cace_ari_execset_t *eset = cace_ari_cget_execset(&(msg->value));
        // should not be null, but guard anyway
        if (eset)
        {
            cace_ari_set_copy(&(ctx->nonce), &(eset->nonce));
        }
        else
        {
            cace_ari_reset(&(ctx->nonce));
        }

        // Lookup ACL groups once now
        ctx->acl_gen = agent->acl.generation;
        refda_acl_search_endpoint(agent, &ctx->mgr_ident, ctx->acl_groups);
    }
    else
    {
        // Agent-directed, no manager
        cace_ari_reset(&(ctx->mgr_ident));
        cace_ari_reset(&(ctx->nonce));
        // agent group 0
        ctx->acl_gen = 0;
        refda_acl_id_tree_push(ctx->acl_groups, 0);
    }

    return 0;
}

void refda_runctx_check_acl(refda_runctx_t *ctx)
{
    if (ctx->acl_gen != ctx->agent->acl.generation)
    {
        refda_acl_search_endpoint(ctx->agent, &ctx->mgr_ident, ctx->acl_groups);
    }
}
