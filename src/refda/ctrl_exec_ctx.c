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
#include "ctrl_exec_ctx.h"
#include "exec_seq.h"
#include "timeline.h"
#include "agent.h"
#include "cace/util/logging.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"

void refda_ctrl_exec_ctx_init(refda_ctrl_exec_ctx_t *obj, refda_exec_item_t *item)
{
    CHKVOID(obj);
    CHKVOID(item);

    obj->runctx = refda_runctx_ptr_ref(item->seq->runctx);
    // check ACL cache at last moment
    refda_runctx_check_acl(obj->runctx);

    obj->ctrl = item->deref.obj ? item->deref.obj->app_data.ptr : NULL;
    obj->item = item;
}

void refda_ctrl_exec_ctx_deinit(refda_ctrl_exec_ctx_t *obj)
{
    CHKVOID(obj);
}

bool refda_ctrl_exec_ctx_has_aparam_undefined(const refda_ctrl_exec_ctx_t *ctx)
{
    CHKFALSE(ctx);
    return ctx->item->deref.aparams.any_undefined;
}

const cace_ari_t *refda_ctrl_exec_ctx_get_aparam_index(const refda_ctrl_exec_ctx_t *ctx, size_t index)
{
    CHKNULL(ctx);
    return cace_ari_array_cget(ctx->item->deref.aparams.ordered, index);
}

const cace_ari_t *refda_ctrl_exec_ctx_get_aparam_name(const refda_ctrl_exec_ctx_t *ctx, const char *name)
{
    CHKNULL(ctx);
    return *cace_named_ari_ptr_dict_cget(ctx->item->deref.aparams.named, name);
}

void refda_ctrl_exec_ctx_set_waiting(refda_ctrl_exec_ctx_t *ctx, const refda_timeline_event_t *event)
{
    CHKVOID(ctx);
    atomic_store(&(ctx->item->execution_stage), REFDA_EXEC_WAITING);

    if (event)
    {
        refda_timeline_push(ctx->runctx->agent->exec_timeline, *event);
    }
}

static int refda_ctrl_exec_ctx_check_result(refda_ctrl_exec_ctx_t *ctx)
{
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(ctx->item->result), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("CTRL result value %s", string_get_cstr(buf));
        string_clear(buf);
    }

    bool valid = false;
    if (cace_amm_type_is_valid(&(ctx->ctrl->res_type)))
    {
        // undefined is the failure indicator
        if (!cace_ari_is_undefined(&(ctx->item->result)))
        {
            valid = (CACE_AMM_TYPE_MATCH_POSITIVE == cace_amm_type_match(&(ctx->ctrl->res_type), &(ctx->item->result)));
            if (!valid)
            {
                CACE_LOG_ERR("CTRL result type failed to match a result value");
                cace_ari_set_undefined(&(ctx->item->result));
            }
        }
    }
    else
    {
        // no explicit result type means only the null value is acceptable
        if (cace_ari_is_null(&(ctx->item->result)))
        {
            valid = true;
        }
        else
        {
            CACE_LOG_ERR("CTRL result value without result type");
            // should not have a result
            cace_ari_set_undefined(&(ctx->item->result));
        }
    }

    atomic_store(&(ctx->item->execution_stage), REFDA_EXEC_COMPLETE);

    if (valid)
    {
        return 0;
    }
    else
    {
        cace_ari_deinit(&(ctx->item->result));
        return REFDA_CTRL_EXEC_RESULT_TYPE_NOMATCH;
    }
}

int refda_ctrl_exec_ctx_set_result_copy(refda_ctrl_exec_ctx_t *ctx, const cace_ari_t *value)
{
    CHKERR1(ctx);
    CHKERR1(value);
    cace_ari_set_copy(&(ctx->item->result), value);
    return refda_ctrl_exec_ctx_check_result(ctx);
}

int refda_ctrl_exec_ctx_set_result_move(refda_ctrl_exec_ctx_t *ctx, cace_ari_t *value)
{
    CHKERR1(ctx);
    CHKERR1(value);
    cace_ari_set_move(&(ctx->item->result), value);
    return refda_ctrl_exec_ctx_check_result(ctx);
}

int refda_ctrl_exec_ctx_set_result_null(refda_ctrl_exec_ctx_t *ctx)
{
    CHKERR1(ctx);
    cace_ari_set_null(&(ctx->item->result));
    return refda_ctrl_exec_ctx_check_result(ctx);
}
