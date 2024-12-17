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

/// Dummy manager identity URI
static const char *mgr_ident = "data:test";

int test_util_runctx_init(refda_runctx_t *ctx, refda_agent_t *agent)
{
    refda_runctx_init(ctx);
    // no nonce for test
    refda_runctx_from(ctx, agent, NULL);

    // CACE data does not include terminating null
    cace_data_copy_from(&(ctx->mgr_ident), strlen(mgr_ident), (cace_data_ptr_t)mgr_ident);

    return 0;
}
