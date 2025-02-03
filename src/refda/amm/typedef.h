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
#ifndef REFDA_AMM_TYPEDEF_H_
#define REFDA_AMM_TYPEDEF_H_

#include "cace/amm/typing.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An named type definition (TYPEDEF) descriptor.
 */
typedef struct
{
    /** The type being named by this TYPEDEF.
     * This type will not change during the lifetime of the TYPEDEF.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    amm_type_t typeobj;

} refda_amm_typedef_desc_t;

void refda_amm_typedef_desc_init(refda_amm_typedef_desc_t *obj);

void refda_amm_typedef_desc_deinit(refda_amm_typedef_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_TYPEDEF_H_ */
