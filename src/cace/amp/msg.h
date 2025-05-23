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
 * @ingroup amp
 * Handle AMP binary message encoding and decoding.
 */
#ifndef CACE_AMP_MSG_H_
#define CACE_AMP_MSG_H_

#include "cace/ari/containers.h"
#include <m-bstring.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Encode a single AMP message from a set of ARI items.
 *
 * @param[out] msgbuf The message buffer to append to.
 * This must be already initialized.
 * @param[in] items The items to encode.
 * @return Zero if successful.
 */
int cace_amp_msg_encode(m_bstring_t msgbuf, const cace_ari_list_t items);

/** Decode a single AMP message into a set of ARI items.
 *
 * @param[out] items The items list to decode into.
 * This must be already initialized.
 * @param[in] msgbuf_ptr The message buffer to decode from.
 * @param msgbuf_len The length of valid data at @c msgbuf_ptr.
 * @return Zero if successful.
 */
int cace_amp_msg_decode(cace_ari_list_t items, const uint8_t *msgbuf_ptr, size_t msgbuf_len);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMP_MSG_H_ */
