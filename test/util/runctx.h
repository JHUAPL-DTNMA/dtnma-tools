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
#ifndef TEST_UTIL_RUNCTX_H_
#define TEST_UTIL_RUNCTX_H_

#include <refda/runctx.h>

#ifdef __cplusplus
extern "C" {
#endif

int test_util_runctx_init(refda_runctx_t *ctx, refda_agent_t *agent);

#ifdef __cplusplus
} // extern C
#endif

#endif /* TEST_UTIL_RUNCTX_H_ */
