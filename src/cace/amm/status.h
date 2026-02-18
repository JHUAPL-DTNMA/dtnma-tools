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
#ifndef CACE_AMM_STATUS_H_
#define CACE_AMM_STATUS_H_

#include "idseg_val.h"
#include "user_data.h"
#include "parameters.h"
#include "cace/ari.h"
#include "cace/util/nocase.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Possible status values for a namespace and for an object.
 */
typedef enum
{
    /**
     * This means that the definition is current and valid.
     */
    CACE_AMM_STATUS_CURRENT,
    /**
     * This indicates an obsolete definition, but it permits
     * new/continued implementation in order to foster interoperability
     * with older/existing implementations.
     */
    CACE_AMM_STATUS_DEPRECATED,
    /**
     * This means that the definition is obsolete and SHOULD NOT be
     * implemented and/or can be removed from implementations.
     */
    CACE_AMM_STATUS_OBSOLETE,

} cace_amm_status_t;

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_STATUS_H_ */
