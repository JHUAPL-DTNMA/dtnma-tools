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
#include "var.h"
#include "cace/util/logging.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"

void refda_amm_var_desc_init(refda_amm_var_desc_t *obj)
{
    amm_type_init(&(obj->val_type));
    obj->value = ARI_INIT_UNDEFINED;
}

void refda_amm_var_desc_deinit(refda_amm_var_desc_t *obj)
{
    ari_deinit(&(obj->value));
    amm_type_deinit(&(obj->val_type));
    memset(obj, 0, sizeof(*obj));
}

int refda_amm_var_desc_produce(const refda_amm_var_desc_t *obj, refda_valprod_ctx_t *ctx)
{
    CHKERR1(obj);
    CHKERR1(ctx);

    ari_set_copy(&(ctx->value), &(obj->value));
    // FIXME use ctx parameters to substitute

    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &(ctx->value), ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("production finished with value %s", string_get_cstr(buf));
        string_clear(buf);
    }

    return 0;
}
