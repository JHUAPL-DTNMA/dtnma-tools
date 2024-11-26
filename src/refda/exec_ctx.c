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
#include "exec_ctx.h"
#include "cace/util/defs.h"

void refda_exec_ctx_init(refda_exec_ctx_t *obj, refda_runctx_t *parent, const ari_t *ref, const cace_amm_lookup_t *deref)
{
    CHKVOID(obj);
    CHKVOID(deref);

    obj->parent = parent;
    obj->ref = ref;
    obj->deref  = deref;
    ari_init(&(obj->result));
}

void refda_exec_ctx_deinit(refda_exec_ctx_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->result));
}
