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
#include "oper.h"
#include "cace/util/logging.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"

void refda_amm_oper_desc_init(refda_amm_oper_desc_t *obj)
{
    amm_type_init(&(obj->res_type));
    obj->evaluate = NULL;
}

void refda_amm_oper_desc_deinit(refda_amm_oper_desc_t *obj)
{
    amm_type_deinit(&(obj->res_type));
    // not necessary but helpful
    memset(obj, 0, sizeof(*obj));
}

int refda_amm_oper_desc_evaluate(const refda_amm_oper_desc_t *obj, refda_eval_ctx_t *ctx)
{
    CHKERR1(obj)
    CHKERR1(ctx)
    CHKERR1(obj->evaluate)

    int    res = (obj->evaluate)(obj, ctx);
    ari_t *top = ari_list_back(ctx->stack);
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, top, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("evaluation finished with status %d and result %s", res, string_get_cstr(buf));
        string_clear(buf);
    }
    if (res)
    {
        return 2;
    }

    // FIXME skip type checking
    return 0;
    if (amm_type_is_valid(&(obj->res_type)))
    {
        // force result type
        ari_t tmp;
        ari_init(&tmp);
        res = amm_type_convert(&(obj->res_type), &tmp, top);
        ari_set_move(top, &tmp);
        if (res)
        {
            return 3;
        }
    }

    return 0;
}
