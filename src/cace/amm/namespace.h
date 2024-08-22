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
#ifndef CACE_AMM_NAMESPACE_H_
#define CACE_AMM_NAMESPACE_H_

#include "const.h"
#include "edd.h"
#include "var.h"
#include <m-rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct string_set_t
 * Unique set of feature names in a single namespace.
 */
/// @cond Doxygen_Suppress
RBTREE_DEF(string_tree_set, m_string_t)
/// @endcond

typedef struct
{
    /// Integer enumeration for this namespace
    int64_t intenum;
    /// Mandatory name for this namespace
    m_string_t name;

    /// Features supported within this namespace
    string_tree_set_t feature_supp;

    // AMM object descriptors follow
    CACE_AMM_OBJTYPE_USE_STORAGE(const)
    CACE_AMM_OBJTYPE_USE_STORAGE(var)
    CACE_AMM_OBJTYPE_USE_STORAGE(edd)
} cace_amm_namespace_desc_t;

void cace_amm_namespace_desc_init(cace_amm_namespace_desc_t *obj);

void cace_amm_namespace_desc_deinit(cace_amm_namespace_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_NAMESPACE_H_ */
