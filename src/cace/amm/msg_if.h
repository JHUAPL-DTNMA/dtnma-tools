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
#ifndef CACE_AMM_MSG_IF_H_
#define CACE_AMM_MSG_IF_H_

#include "cace/ari.h"
#include "cace/util/daemon_run.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    /// Source endpoint ID (opaque bytestring)
    cace_data_t src;
    /// Destination endpoint ID (opaque bytestring)
    cace_data_t dest;
} cace_amm_msg_if_metadata_t;

void cace_amm_msg_if_metadata_init(cace_amm_msg_if_metadata_t *meta);

void cace_amm_msg_if_metadata_deinit(cace_amm_msg_if_metadata_t *meta);

/** Message sending function.
 * @param[in] data The list of ARIs to send.
 * @param[in] meta The destination endpoint.
 * @param[in] ctx The user context, which may be NULL.
 * @return Zero if successful, 2 if the output has closed.
 */
typedef int (*cace_amm_msg_if_send_f)(const ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx);

/// Status to indicate end-of-input
#define CACE_AMM_MSG_IF_RECV_END 2

/** Message receiving function.
 * @param[out] data An empty list of ARIs to receive into.
 * @param[out] meta Pointer to reception metadata, which is never NULL.
 * @param[in] running Pointer to the daemon run state, which can be checked to know
 * when to shut down the reading thread.
 * @param[in] ctx The user context, which may be NULL.
 * @return Zero if successful, CACE_AMM_MSG_IF_RECV_END if the input has closed (but there still may be items in @c
 * data).
 */
typedef int (*cace_amm_msg_if_recv_f)(ari_list_t data, cace_amm_msg_if_metadata_t *meta, daemon_run_t *running,
                                      void *ctx);

/**
 * The MSG Interface structure captures state necessary to communicate with
 * the local Bundle Protocol Agent (BPA).
 */
typedef struct
{
    /// Sending callback
    cace_amm_msg_if_send_f send;
    /// Receiving callback
    cace_amm_msg_if_recv_f recv;
    /// Context to provide to #send and #receive functions
    void *ctx;
} cace_amm_msg_if_t;

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_MSG_IF_H_ */
