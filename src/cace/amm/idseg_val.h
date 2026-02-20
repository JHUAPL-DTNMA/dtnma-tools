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
#ifndef CACE_AMM_IDSEG_VAL_H_
#define CACE_AMM_IDSEG_VAL_H_

#include "idseg_ref.h"
#include "cace/util/defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Each component of a namespace identifier
typedef struct
{
    /// Mandatory name for the component, stored with this struct
    m_string_t name;
    /// Indication of whether the #intenum is valid
    bool has_intenum;
    /// Optional integer enumeration for the component, valid if #has_intenum is true
    cace_ari_int_id_t intenum;
} cace_amm_idseg_val_t;

static inline void cace_amm_idseg_val_init(cace_amm_idseg_val_t *obj)
{
    CHKVOID(obj);
    m_string_init(obj->name);
    obj->has_intenum = false;
    obj->intenum     = 0;
}

static inline void cace_amm_idseg_val_deinit(cace_amm_idseg_val_t *obj)
{
    CHKVOID(obj);
    m_string_clear(obj->name);
    obj->has_intenum = false;
    obj->intenum     = 0;
}

/** Copy by-value from another instance.
 *
 * @param[out] obj The object to set state on.
 * @param[in] src The object to copy from.
 */
static inline void cace_amm_idseg_val_set(cace_amm_idseg_val_t *obj, const cace_amm_idseg_val_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    m_string_set(obj->name, src->name);
    obj->has_intenum = src->has_intenum;
    obj->intenum     = src->intenum;
}

/** Copy by-value from an ID segment reference.
 *
 * @param[out] obj The object to set state on.
 * @param[in] src The object to copy from.
 */
static inline void cace_amm_idseg_val_set_fromref(cace_amm_idseg_val_t *obj, const cace_amm_idseg_ref_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    m_string_set_cstr(obj->name, src->name);
    obj->has_intenum = src->has_intenum;
    obj->intenum     = src->intenum;
}

/** Check if this ID segment matches a specific integer enumeration.
 *
 * @param[in] obj The object to check.
 * @param id The integer value to check.
 * @return True if the object has the given value.
 */
static inline bool cace_amm_idseg_val_match_int(const cace_amm_idseg_val_t *obj, cace_ari_int_id_t id)
{
    CHKRET(obj, false);
    return (obj->has_intenum && (obj->intenum == id));
}

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_IDSEG_VAL_H_ */
