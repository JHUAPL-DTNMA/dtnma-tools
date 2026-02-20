/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "msg_if.h"

void cace_amm_msg_if_metadata_init(cace_amm_msg_if_metadata_t *meta)
{
    cace_ari_init(&meta->src);
    cace_ari_init(&meta->dest);
    cace_ari_init(&meta->timestamp);
}

void cace_amm_msg_if_metadata_deinit(cace_amm_msg_if_metadata_t *meta)
{
    cace_ari_deinit(&meta->dest);
    cace_ari_deinit(&meta->src);
    cace_ari_deinit(&meta->timestamp);
}
