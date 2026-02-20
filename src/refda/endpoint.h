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
/** @file
 * @ingroup refda
 * Manage logic for endpoints and endpoint pattern matching.
 */
#ifndef REFDA_ENDPOINT_H_
#define REFDA_ENDPOINT_H_

#include "agent.h"
#include "cace/ari/base.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Match a specific endpoint value against a pattern reference.
 * @param[in] agent The agent state for reference lookup.
 * @param[in] endpoint The endpoint to match.
 * @param[in pat The pattern reference to match with.
 * @return True if the endpoint matches the pattern.
 */
bool refda_endpoint_pat_match(const refda_agent_t *agent, const cace_ari_t *endpoint,
                              const refda_amm_ident_base_t *pat);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_ENDPOINT_H_ */
