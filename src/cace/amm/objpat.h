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
/** @file
 * @ingroup amm
 * Containers and algorithms for working with sets of object patterns.
 */
#ifndef CACE_AMM_OBJPAT_H_
#define CACE_AMM_OBJPAT_H_

#include "lookup.h"
#include "cace/ari/objpat.h"
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct cace_amm_objpat_set_t
 * Set of ::cace_ari_objpat_t instances to use for matching objects.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_DEQUE_DEF(cace_amm_objpat_set, cace_ari_objpat_t)
// GCOV_EXCL_STOP
/// @endcond

/** Extract a copy of a pattern set from a literal value.
 *
 * @return Zero if successful.
 */
int cace_amm_objpat_set_from_value(cace_amm_objpat_set_t obj, const cace_ari_t *val);

/** Determine if a specific dereferenced object matches a pattern set.
 *
 * @return True if it is a match.
 */
bool cace_amm_objpat_set_match(const cace_amm_objpat_set_t obj, const cace_amm_lookup_t *deref);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJPAT_H_ */
