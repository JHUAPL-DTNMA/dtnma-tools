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
 * @ingroup ari
 * This file contains definitions for ARI CBOR CODEC functions.
 */
#ifndef CACE_ARI_CBOR_H_
#define CACE_ARI_CBOR_H_

#include "base.h"
#include "containers.h"
#include "cace/cace_data.h"
#include <qcbor/qcbor_encode.h>
#include <qcbor/qcbor_decode.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Encode an ARI to binary form.
 *
 * @param[out] buf The data buffer to modify and write the result into.
 * It will contain a well-formed CBOR item if successful.
 * @param[in] ari The ARI to encode from.
 * @return Zero upon success.
 */
int cace_ari_cbor_encode(cace_data_t *buf, const cace_ari_t *ari);

/** Lower-level stream encoding interface.
 *
 * @param[in] encoder The existing encoder to write with.
 * @param[in] ari The ARI to encode from.
 * @return Zero upon success.
 */
int cace_ari_cbor_encode_stream(QCBOREncodeContext *encoder, const cace_ari_t *ari);

/** Decode an ARI from binary form.
 *
 * @note When @c used is non-null, extra data after the ARI is not considered
 * to be a decoding failure and the caller is assumed to compare the @c used
 * value against the buffer size itself.
 *
 * @param[out] ari The ARI to decode into.
 * The struct must already be initialized.
 * @param[in] buf The buffer to decode from.
 * @param[out] used If non-null, the size of used data for this decoding
 * is placed here.
 * This is set even if decoding fails to allow skipping well-formed CBOR which
 * is not a valid ARI.
 * @param[out] errm If non-null, this will be set to a specific error message
 * associated with any failure.
 * Regardless of the return code, if the pointed-to pointer is non-null it
 * must be freed using CACE_FREE().
 * @return Zero upon success.
 */
int cace_ari_cbor_decode(cace_ari_t *ari, const cace_data_t *buf, size_t *used, const char **errm);

/** Lower-level stream decoding interface.
 *
 * @param[in] decoder The existing decoder to write with.
 * @param[out] ari The ARI to decode into.
 * The struct must already be initialized.
 * @return Zero upon success.
 */
int cace_ari_cbor_decode_stream(QCBORDecodeContext *decoder, cace_ari_t *ari);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_CBOR_H_ */
