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
#ifndef TEST_CACE_FUZZ_COMMON_H_
#define TEST_CACE_FUZZ_COMMON_H_

#include <cace/util/logging.h>
#include <inttypes.h>
#include <stddef.h>

#define EXPECT_EQ(expect, got)           \
    if ((expect) != (got))               \
    {                                    \
        CACE_LOG_CRIT("EXPECT failure"); \
    }

#ifdef __cplusplus
extern "C" {
#endif

int LLVMFuzzerInitialize(int *argc, char ***argv);
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

#ifdef __cplusplus
} // extern C
#endif

#endif /* TEST_CACE_FUZZ_COMMON_H_ */
