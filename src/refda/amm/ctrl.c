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
#include "ctrl.h"
#include "cace/util/logging.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"

void refda_amm_ctrl_desc_init(refda_amm_ctrl_desc_t *obj)
{
    amm_type_init(&(obj->res_type));
    obj->execute = NULL;
}

void refda_amm_ctrl_desc_deinit(refda_amm_ctrl_desc_t *obj)
{
    amm_type_deinit(&(obj->res_type));
    // not necessary but helpful
    memset(obj, 0, sizeof(*obj));
}

int refda_amm_ctrl_desc_execute(const refda_amm_ctrl_desc_t *obj, refda_exec_ctx_t *ctx)
{
    CHKERR1(obj)
    CHKERR1(ctx)
    CHKERR1(obj->execute)

    int res = (obj->execute)(obj, ctx);
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &(ctx->item->result), ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution finished with status %d and result %s", res, string_get_cstr(buf));
        string_clear(buf);
    }
    if (res)
    {
        ari_set_undefined(&(ctx->item->result));
        return 2;
    }

    return 0;
}
