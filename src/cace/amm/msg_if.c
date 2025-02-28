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
#include "msg_if.h"

void cace_amm_msg_if_metadata_init(cace_amm_msg_if_metadata_t *meta)
{
    m_string_init(meta->src);
    m_string_init(meta->dest);
}

void cace_amm_msg_if_metadata_deinit(cace_amm_msg_if_metadata_t *meta)
{
    m_string_clear(meta->dest);
    m_string_clear(meta->src);
}
