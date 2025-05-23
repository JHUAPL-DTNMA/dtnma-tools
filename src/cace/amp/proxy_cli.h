/*
 * Copyright (c) 2025 The Johns Hopkins University Applied Physics
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
 * This header is associated with proxied messaging from the user/client side,
 * where the peer identity prefix and the entire message contents are
 * visible.
 */
#ifndef CACE_AMP_PROXY_CLI_H_
#define CACE_AMP_PROXY_CLI_H_

#include "cace/amm/msg_if.h"
#include "cace/ari.h"
#include <m-string.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/** State for the BP-proxy socket transport.
 */
typedef struct
{
    /// Original requested socket path
    m_string_t path;
    /// Mutex for value of #sock_fd, but not its state
    pthread_mutex_t sock_mutex;
    /// Actual socket FD
    int sock_fd;
} cace_amp_proxy_cli_state_t;

/** Initialize an empty state.
 */
void cace_amp_proxy_cli_state_init(cace_amp_proxy_cli_state_t *state);

/** Deinitialize any resources.
 */
void cace_amp_proxy_cli_state_deinit(cace_amp_proxy_cli_state_t *state);

/** Connect a unix domain SEQDATA socket to a specific filesystem path.
 *
 * @param[in,out] state The state to bind.
 * @param[in] sock_path The file path to bind to.
 * @return Zero if successful.
 */
int cace_amp_proxy_cli_state_connect(cace_amp_proxy_cli_state_t *state, const m_string_t sock_path);

/** Disconnect any current socket and remove it if necessary.
 *
 * @param[in,out] state The state to bind.
 */
void cace_amp_proxy_cli_state_disconnect(cace_amp_proxy_cli_state_t *state);

/** Provider of ::cace_amm_msg_if_send_f interface for BP-proxy socket transport.
 * The user data for this function is a ::cace_amp_proxy_cli_state_t instance.
 */
int cace_amp_proxy_cli_send(const cace_ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx);

/** Provider of ::cace_amm_msg_if_recv_f interface for BP-proxy socket transport.
 * The user data for this function is a ::cace_amp_proxy_cli_state_t instance.
 */
int cace_amp_proxy_cli_recv(cace_ari_list_t data, cace_amm_msg_if_metadata_t *meta, cace_daemon_run_t *running,
                            void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMP_PROXY_CLI_H_ */
