/*
 * Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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
 * @ingroup ari
 * This file contains definitions for comman ARI time related functionality.
 */
#ifndef CACE_ARI_TIME_UTIL_H_
#define CACE_ARI_TIME_UTIL_H_

#include "cace/ari.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Store the current system time as an ARI timestamp.
 *
 * @param [out] timestamp The ARI struct to store the system timestamp.
 * @return Zero upon success.
 */
int cace_get_system_time(cace_ari_t *timestamp);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_TIME_UTIL_H_ */
