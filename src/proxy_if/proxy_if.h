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
#ifndef PROXY_IF_PROXY_IF_H_
#define PROXY_IF_PROXY_IF_H_

#include "shared/utils/nm_types.h"
#include "shared/primitives/blob.h"
#include <m-bstring.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Send a message via datagram socket proxy to an ultimate destination.
 *
 * @param sock_fd The socket file descriptor.
 * The socket must be ready for writing (e.g. determined via poll() and
 * POLLOUT event) before this call.
 * @return AMP_OK if successful.
 */
int proxy_if_send(int sock_fd, const eid_t *dst, const uint8_t *data_ptr, size_t data_len);

/** Receive a message via datagram socket proxy from an ultimate source.
 *
 * @param sock_fd The socket file descriptor.
 * The socket must be ready for reading (e.g. determined via poll() and
 * POLLIN event) before this call.
 * @param[out] src The source EID to write into.
 * @param[out] data The data to populate, which must already be initialized.
 * @return AMP_OK if successful.
 */
int proxy_if_recv(int sock_fd, eid_t *src, m_bstring_t data);

#ifdef __cplusplus
}
#endif

#endif /* PROXY_IF_PROXY_IF_H_ */
