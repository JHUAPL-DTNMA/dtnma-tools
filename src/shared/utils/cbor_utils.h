/*
 * Copyright (c) 2018-2023 The Johns Hopkins University Applied Physics
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

/*****************************************************************************
 **
 ** File Name: cbor_utils.h
 **
 ** Subsystem:
 **          Shared utilities
 **
 ** Description: This file provides CBOR encoding/decoding functions for
 **              AMP structures.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  08/31/18  E. Birrane     Initial Implementation (JHU/APL)
 *****************************************************************************/
#ifndef __CBOR_UTILS__
#define __CBOR_UTILS__

#include <qcbor/qcbor.h>
#include "nm_types.h"
#include "vector.h"


#ifdef __cplusplus
extern "C" {
#endif


#define CUT_ENC_BUFSIZE 4096

/** Callback function prototype for cut_serialize_vector() and cut_serialize_wrapper() */
typedef QCBORError (*cut_enc_fn)(QCBOREncodeContext *encoder, void *item);

/** Callback function prototype for cut_deserialize_vector() */
typedef void* (*vec_des_fn)(QCBORDecodeContext *decoder, int *success);

int       cut_advance_it(QCBORItem *value);

int cut_enc_bytes(QCBOREncodeContext *encoder, uint8_t *buf, size_t len);
int cut_dec_bytes(QCBORDecodeContext *it, uint8_t *buf, size_t len);
static inline int cut_enc_byte(QCBOREncodeContext *encoder, uint8_t buf)
{
   return cut_enc_bytes(encoder, &buf, sizeof(uint8_t));
}

int cut_enc_uvast(amp_uvast num, blob_t *result);

int cut_get_cbor_numeric_raw(blob_t *data, amp_type_e type, void *val);
int       cut_get_cbor_numeric(QCBORDecodeContext *value, amp_type_e type, void *val);
char *    cut_get_cbor_str(QCBORDecodeContext *value, int *success);

blob_t*   cut_serialize_wrapper(size_t size, void *item, cut_enc_fn encode);


int cut_deserialize_vector(vector_t *vec, QCBORDecodeContext *it, vec_des_fn des_fn);
int cut_serialize_vector(QCBOREncodeContext *encoder, vector_t *vec, cut_enc_fn enc_fn);

void *cut_char_deserialize(QCBORDecodeContext *it, int *success);
int cut_char_serialize(QCBOREncodeContext *encoder, void *item);

int cut_get_cbor_str_ptr(QCBORDecodeContext *it, char *dst, size_t length);

/** Macro to check and report if decoding exited with any errors. These are logged as warnings, but no action is otherwise taken. */
#define cut_decode_finish(qcbor) \
   QCBORError tmperr = QCBORDecode_Finish(qcbor); \
   if (tmperr != QCBOR_SUCCESS) { \
     AMP_DEBUG_WARN(__func__, "Warning: CBOR Decoding finished with err %d", tmperr); \
   }


#ifdef __cplusplus
}
#endif

#endif  /* _CBOR_UTILS_H_ */
