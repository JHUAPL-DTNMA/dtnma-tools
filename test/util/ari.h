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
#ifndef TEST_UTIL_ARI_H_
#define TEST_UTIL_ARI_H_

#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Decode an ARI from a hex-encoded text string.
 *
 */
int test_util_ari_decode(cace_ari_t *ari, const char *inhex);

/** Encode an ARI to a hex-encoded text string.
 *
 * @param[out] outhex An uninitialized string buffer.
 */
int test_util_ari_encode(m_string_t outhex, const cace_ari_t *ari);

#ifdef __cplusplus
} // extern C
#endif

#endif /* TEST_UTIL_ARI_H_ */
