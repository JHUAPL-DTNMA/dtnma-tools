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
#ifndef CACE_AMM_IDSEG_REF_H_
#define CACE_AMM_IDSEG_REF_H_

#include "cace/ari/ref.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An identifer segment with name by-reference.
 */
typedef struct
{
    /// Mandatory pointer to externally stored name string
    const char *name;
    /// True if the #intenum is valid
    bool has_intenum;
    /// Optional integer enumeration for this object, valid if #has_intenum is true
    cace_ari_int_id_t intenum;
} cace_amm_idseg_ref_t;

/** Construct an object ID with an integer enum.
 *
 * @param[in] name The object name, which must be non-null.
 * @param The object enumeration.
 * @return The full object ID.
 */
static cace_amm_idseg_ref_t cace_amm_idseg_ref_withenum(const char *name, cace_ari_int_id_t intenum);
/// @overload
static cace_amm_idseg_ref_t cace_amm_idseg_ref_noenum(const char *name);

static inline cace_amm_idseg_ref_t cace_amm_idseg_ref_withenum(const char *name, cace_ari_int_id_t intenum)
{
    cace_amm_idseg_ref_t ref;
    ref.name        = name;
    ref.has_intenum = true;
    ref.intenum     = intenum;
    return ref;
}

static inline cace_amm_idseg_ref_t cace_amm_idseg_ref_noenum(const char *name)
{
    cace_amm_idseg_ref_t ref;
    ref.name        = name;
    ref.has_intenum = false;
    ref.intenum     = 0;
    return ref;
}

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_IDSEG_REF_H_ */
