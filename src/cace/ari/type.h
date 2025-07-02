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
/** @file
 * @ingroup ari
 * This file contains ARI type definitions and basic lookup functions.
 */
#ifndef CACE_ARI_TYPE_H_
#define CACE_ARI_TYPE_H_

#include "cace/config.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/** All value types representable by an ARI.
 * These are encodable enumerations, which are different than
 * ::ari_prim_type_e internal union-switching enumerations.
 */
typedef enum
{
    // Catch-all literal type ID
    CACE_ARI_TYPE_LITERAL = 255,

    // Primitive types
    CACE_ARI_TYPE_NULL    = 0,
    CACE_ARI_TYPE_BOOL    = 1,
    CACE_ARI_TYPE_BYTE    = 2,
    CACE_ARI_TYPE_INT     = 4,
    CACE_ARI_TYPE_UINT    = 5,
    CACE_ARI_TYPE_VAST    = 6,
    CACE_ARI_TYPE_UVAST   = 7,
    CACE_ARI_TYPE_REAL32  = 8,
    CACE_ARI_TYPE_REAL64  = 9,
    CACE_ARI_TYPE_TEXTSTR = 10,
    CACE_ARI_TYPE_BYTESTR = 11,

    // Non-primitive types
    CACE_ARI_TYPE_TP      = 12,
    CACE_ARI_TYPE_TD      = 13,
    CACE_ARI_TYPE_LABEL   = 14,
    CACE_ARI_TYPE_CBOR    = 15,
    CACE_ARI_TYPE_ARITYPE = 16,

    // Container types
    CACE_ARI_TYPE_AC      = 17,
    CACE_ARI_TYPE_AM      = 18,
    CACE_ARI_TYPE_TBL     = 19,
    CACE_ARI_TYPE_EXECSET = 20,
    CACE_ARI_TYPE_RPTSET  = 21,

    /// Catch-all object reference type ID
    CACE_ARI_TYPE_OBJECT = -256,
    /// Catch-all namespace reference type ID
    CACE_ARI_TYPE_NAMESPACE = -255,

    // AMM object types
    CACE_ARI_TYPE_IDENT   = -1,
    CACE_ARI_TYPE_CONST   = -2,
    CACE_ARI_TYPE_CTRL    = -3,
    CACE_ARI_TYPE_EDD     = -4,
    CACE_ARI_TYPE_OPER    = -6,
    CACE_ARI_TYPE_SBR     = -8,
    CACE_ARI_TYPE_TBR     = -10,
    CACE_ARI_TYPE_VAR     = -11,
    CACE_ARI_TYPE_TYPEDEF = -12,
} cace_ari_type_t;

/** Lookup a type ID from its integer enumeration.
 *
 * @param typenum The integer value to search for.
 * @return Non-null text pointer upon success.
 */
const char *cace_ari_type_to_name(int32_t typenum);

/** Lookup a type ID from its text name.
 *
 * @param[out] typenum The integer value to store the found result.
 * A null value means just search but don't store.
 * @param[in] name The name to search for.
 * @return Zero upon success.
 */
int cace_ari_type_from_name(int32_t *typenum, const char *name);

/** M*LIB OPLIST for the enum cace_ari_type_t.
 */
#define M_OPL_cace_ari_type_t() M_ENUM_OPLIST(cace_ari_type_t, CACE_ARI_TYPE_NULL)

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_TYPE_H_ */
