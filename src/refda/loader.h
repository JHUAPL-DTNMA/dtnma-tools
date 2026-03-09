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

#ifndef REFDA_LOADER_H_
#define REFDA_LOADER_H_

#include "agent.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize base models on an agent at startup.
 * This should be called before any other ADMs and uses of refda_agent_bindrefs().
 * @param[in] agent The agent to load models into.
 * @return Zero if successful.
 */
int refda_loader_basemods(refda_agent_t *agent);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_LOADER_H_ */
