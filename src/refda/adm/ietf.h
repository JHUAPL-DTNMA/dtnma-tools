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
#ifndef REFDA_ADM_IETF_H_
#define REFDA_ADM_IETF_H_

#include "refda/agent.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REFDA_ADM_IETF_AMM_ENUM 0

int refda_adm_ietf_amm_init(refda_agent_t *agent);

#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM 1

int refda_adm_ietf_dtnma_agent_init(refda_agent_t *agent);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_ADM_IETF_H_ */
