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
 * @ingroup amp
 * Provide an ION Bundle Protocol (BP) adapter for AMP messaging.
 */
#ifndef CACE_AMP_ION_BP_H_
#define CACE_AMP_ION_BP_H_

#include "cace/amm/msg_if.h"
#include "cace/ari.h"

#if defined(ION_FOUND)
#include <bp.h>
#endif /* ION_FOUND */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ION_FOUND)

/** State for the ION BP transport.
 */
typedef struct
{
    /// EID URI to bind to
    m_string_t eid;
    BpSAP      sap;
} cace_amp_ion_bp_state_t;

/** Initialize an empty state.
 */
void cace_amp_ion_bp_state_init(cace_amp_ion_bp_state_t *state);

/** Deinitialize any resources.
 */
void cace_amp_ion_bp_state_deinit(cace_amp_ion_bp_state_t *state);

/** Bind a unix domain data gram socket to a specific filesystem path.
 *
 * @param[in,out] state The state to bind.
 * @param[in] eid The EID URI to bind to.
 */
int cace_amp_ion_bp_state_bind(cace_amp_ion_bp_state_t *state, const m_string_t eid);

/** Unbind any current socket and remove it if necessary.
 *
 * @param[in,out] state The state to bind.
 */
void cace_amp_ion_bp_state_unbind(cace_amp_ion_bp_state_t *state);

/** Provider of ::cace_amm_msg_if_send_f interface for ION BP transport.
 * The user data for this function is a ::BpSAP instance.
 */
int cace_amp_ion_bp_send(const cace_ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx);

/** Provider of ::cace_amm_msg_if_recv_f interface for ION BP transport.
 * The user data for this function is a ::BpSAP instance.
 */
int cace_amp_ion_bp_recv(cace_ari_list_t data, cace_amm_msg_if_metadata_t *meta, cace_daemon_run_t *running, void *ctx);

#endif /* ION_FOUND */

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMP_ION_BP_H_ */
