/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
 * This file contains the definitions, prototypes, constants, and
 * other information necessary for the identification and
 * processing of AMM Resource Identifiers (ARIs). Every object in
 * the AMM can be uniquely identified using an ARI.
 */
#ifndef ARI_ARI_DATA_H_
#define ARI_ARI_DATA_H_

#include "ari_config.h"
#include <sys/types.h>
#include <m-core.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Data pointer for ari_data_t
typedef uint8_t *ari_data_ptr_t;

/** Heap data for TSTR and BSTR types.
 */
typedef struct ari_data_s
{
    /// True if this data is a copy
    bool owned;
    /// Pointer to the front of the buffer
    ari_data_ptr_t ptr;
    /// Size of the data buffer
    size_t len;
} ari_data_t;

/** Static initializer for a data store.
 * @sa ari_data_init()
 */
#define ARI_DATA_INIT_NULL                    \
    {                                         \
        .owned = false, .ptr = NULL, .len = 0 \
    }

/** Initialize an empty data struct.
 *
 * @param[in,out] data The data to initialize, which must not be NULL.
 * @return Zero upon success.
 * @sa ARI_DATA_INIT_NULL
 */
int ari_data_init(ari_data_t *data);

/** Initialize a data struct as an overlay on optional external data.
 *
 * @param[in,out] data The data to initialize, which must not be NULL.
 * @param len The total length to allocate, which may be zero.
 * @param src An optional source buffer to point to.
 * @return Zero upon success.
 */
int ari_data_init_view(ari_data_t *data, size_t len, ari_data_ptr_t src);

int ari_data_init_set(ari_data_t *data, const ari_data_t *src);

/** De-initialize a data struct, freeing if necessary.
 *
 * @param[in,out] data The data to de-initialize, which must not be NULL.
 * @return Zero upon success.
 * @post The struct must be initialized before using again.
 */
int ari_data_deinit(ari_data_t *data);

/** Clear the data, freeing if necessary.
 *
 * @param[in,out] data The data to clear, which must not be NULL.
 * @return Zero upon success.
 */
int ari_data_clear(ari_data_t *data);

/** Resize the data, copying if necessary.
 *
 * @param[in,out] data The data to resize, which must not be NULL.
 * @param len The new total size.
 * @return Zero upon success.
 */
int ari_data_resize(ari_data_t *data, size_t len);

/** Alter the size at the back of the array by a difference value.
 *
 * @param[in,out] data The data to resize, which must not be NULL.
 * @param extra The difference of the desired size from the current size.
 * This may be negative to shrink the data.
 * @return Zero upon success.
 */
static inline int ari_data_extend_back(ari_data_t *data, ssize_t extra)
{
    return ari_data_resize(data, data->len + extra);
}

/** Alter the size at the front of the array by a difference value.
 *
 * @param[in,out] data The data to resize, which must not be NULL.
 * @param extra The difference of the desired size from the current size.
 * This may be negative to shrink the data.
 * @return Zero upon success.
 */
int ari_data_extend_front(ari_data_t *data, ssize_t extra);

/** Set an initialized data struct to a given size.
 *
 * @param[in,out] data The data to copy into, which must not be NULL.
 * @param len The total length to allocate, which may be non-zero.
 * @param src An optional source buffer to copy from, from which @c len
 * bytes will be copied.
 * @return Zero upon success.
 */
int ari_data_copy_from(ari_data_t *data, size_t len, ari_data_ptr_t src);

/** Append an initialized data struct with a given size.
 *
 * @param[in,out] data The data to copy into, which must not be NULL.
 * @param len The total length to allocate, which may be non-zero.
 * @param src An optional source buffer to copy from, from which @c len
 * bytes will be copied.
 * @return Zero upon success.
 */
int ari_data_append_from(ari_data_t *data, size_t len, ari_data_ptr_t src);

/** @overload
 */
int ari_data_append_byte(ari_data_t *data, uint8_t val);

/** Copy between two data structs, both already initialized.
 *
 * @param[in,out] data The data to copy to, which must not be NULL.
 * @param src The data to copy from, which must not be NULL.
 * @return Zero upon success.
 */
int ari_data_copy(ari_data_t *data, const ari_data_t *src);

/** Swap between two data structs, both already initialized.
 *
 * @param[in,out] data The data to swap to, which must not be NULL.
 * @param[in,out] other The data to swap with, which must not be NULL.
 * @return Zero upon success.
 */
int ari_data_swap(ari_data_t *data, ari_data_t *other);

size_t ari_data_hash(const ari_data_t *data);

int ari_data_cmp(const ari_data_t *lt, const ari_data_t *rt);

bool   ari_data_equal(const ari_data_t *lt, const ari_data_t *rt);

struct m_string_s;
/** Copy a text C-string from an M-STRING object.
 * The copy includes a terminating null.
 *
 * @param[in,out] data The data to copy to, which must not be NULL.
 * @param src The string to copy from, which must not be NULL.
 * @return Zero upon success.
 */
int ari_data_from_m_string(ari_data_t *data, const struct m_string_s *src);

/** A definition to use length-one-array trick for store-by-value and
 * pass-by-reference semantics.
 */
typedef struct ari_data_s ari_data_a1_t[1];

#define ARI_DATA_OPLIST                                                                                  \
    M_OPEXTEND(M_EMPTY_OPLIST, INIT(ari_data_init), INIT_SET(ari_data_init_set), CLEAR(ari_data_deinit), \
               SET(ari_data_copy), HASH(ari_data_hash), EQUAL(ari_data_equal), )

#ifdef __cplusplus
}
#endif

#endif /* ARI_ARI_DATA_H_ */
