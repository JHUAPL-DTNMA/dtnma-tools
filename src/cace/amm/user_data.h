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
#ifndef CACE_AMM_USER_DATA_H_
#define CACE_AMM_USER_DATA_H_

#include "parameters.h"
#include "cace/ari.h"
#include "cace/util/nocase.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Signature for cace_amm_user_data_t::deinit function.
 *
 * @param ptr The user data being de-initialized.
 */
typedef void (*cace_amm_user_data_deinit_f)(void *ptr);

typedef struct cace_amm_user_data_s
{
    /// Pointer to the opaque user data being managed
    void *ptr;
    /** Set to true if the pointed-to data is owned by this struct and
     * must be freed (using ARI_FREE) by this deinit.
     */
    bool owned;
    /// An optional cleanup function for the #ptr.
    cace_amm_user_data_deinit_f deinit;

#ifdef __cplusplus
    cace_amm_user_data_s(const cace_amm_user_data_s &)            = delete;
    cace_amm_user_data_s &operator=(const cace_amm_user_data_s &) = delete;
#endif

} cace_amm_user_data_t;

void cace_amm_user_data_init(cace_amm_user_data_t *obj);

void cace_amm_user_data_deinit(cace_amm_user_data_t *obj);

void cace_amm_user_data_set_from(cace_amm_user_data_t *obj, void *ptr, bool owned, cace_amm_user_data_deinit_f deinit);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_USER_DATA_H_ */
