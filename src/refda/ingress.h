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

#ifndef REFDA_INGRESS_H_
#define REFDA_INGRESS_H_

#include <cace/util/daemon_run.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Work thread function for the Agent ingress.
 *
 * @param[in] arg The context ::refda_agent_t pointer.
 * @return Always NULL pointer.
 */
void *refda_ingress_worker(void *arg);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_INGRESS_H_ */
