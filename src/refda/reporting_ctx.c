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
#include "reporting_ctx.h"
#include "acl.h"
#include "cace/util/defs.h"

void refda_reporting_ctx_init(refda_reporting_ctx_t *obj, const refda_runctx_t *runctx, const cace_ari_t *mgr_ident)
{
    CHKVOID(obj);

    obj->runctx = CACE_MALLOC(sizeof(refda_runctx_t));
    refda_runctx_init(obj->runctx);
    obj->runctx->agent = runctx->agent;
    cace_ari_init_copy(&obj->runctx->mgr_ident, mgr_ident);
    cace_ari_set_null(&obj->runctx->nonce);
    // Check ACL for this manager
    refda_runctx_check_acl(obj->runctx);

    cace_ari_list_init(obj->items);
}

void refda_reporting_ctx_deinit(refda_reporting_ctx_t *obj)
{
    CHKVOID(obj);
    cace_ari_list_clear(obj->items);

    refda_runctx_deinit(obj->runctx);
    CACE_FREE(obj->runctx);
    obj->runctx = NULL;
}
