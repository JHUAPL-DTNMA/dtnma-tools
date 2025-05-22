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
 * @ingroup ari
 * This file contains the definitions, prototypes, constants, and
 * other information necessary for the identification and
 * processing of AMM Resource Identifiers (ARIs). Every object in
 * the AMM can be uniquely identified using an ARI.
 */
#ifndef CACE_CACE_DATA_H_
#define CACE_CACE_DATA_H_

#include "cace/config.h"
#include <sys/types.h>
#include <m-core.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Data pointer for cace_data_t
typedef uint8_t *cace_data_ptr_t;

/** Heap data for TSTR and BSTR types.
 */
typedef struct cace_data_s
{
    /// True if this data is a copy
    bool owned;
    /// Pointer to the front of the buffer
    cace_data_ptr_t ptr;
    /// Size of the data buffer
    size_t len;
} cace_data_t;

/** Static initializer for a data store.
 * @sa cace_data_init()
 */
#define CACE_DATA_INIT_NULL { .owned = false, .ptr = NULL, .len = 0 }

/** Initialize an empty data struct.
 *
 * @param[in,out] data The data to initialize, which must not be NULL.
 * @return Zero upon success.
 * @sa CACE_DATA_INIT_NULL
 */
int cace_data_init(cace_data_t *data);

/** Initialize a data struct as an overlay on optional external data.
 *
 * @param[in,out] data The data to initialize, which must not be NULL.
 * @param len The total length to allocate, which may be zero.
 * @param src An optional source buffer to point to.
 * @return Zero upon success.
 */
int cace_data_init_view(cace_data_t *data, size_t len, cace_data_ptr_t src);
/// @overload
int cace_data_init_view_cstr(cace_data_t *data, const char *src);

int cace_data_init_set(cace_data_t *data, const cace_data_t *src);

/** De-initialize a data struct, freeing if necessary.
 *
 * @param[in,out] data The data to de-initialize, which must not be NULL.
 * @return Zero upon success.
 * @post The struct must be initialized before using again.
 */
int cace_data_deinit(cace_data_t *data);

/** Clear the data, freeing if necessary.
 *
 * @param[in,out] data The data to clear, which must not be NULL.
 * @return Zero upon success.
 */
int cace_data_clear(cace_data_t *data);

/** Resize the data, copying if necessary.
 *
 * @param[in,out] data The data to resize, which must not be NULL.
 * @param len The new total size.
 * @return Zero upon success.
 */
int cace_data_resize(cace_data_t *data, size_t len);

/** Alter the size at the back of the array by a difference value.
 *
 * @param[in,out] data The data to resize, which must not be NULL.
 * @param extra The difference of the desired size from the current size.
 * This may be negative to shrink the data.
 * @return Zero upon success.
 */
static inline int cace_data_extend_back(cace_data_t *data, ssize_t extra)
{
    return cace_data_resize(data, data->len + extra);
}

/** Alter the size at the front of the array by a difference value.
 *
 * @param[in,out] data The data to resize, which must not be NULL.
 * @param extra The difference of the desired size from the current size.
 * This may be negative to shrink the data.
 * @return Zero upon success.
 */
int cace_data_extend_front(cace_data_t *data, ssize_t extra);

/** Set an initialized data struct to a given size.
 *
 * @param[in,out] data The data to copy into, which must not be NULL.
 * @param len The total length to allocate, which may be non-zero.
 * @param src An optional source buffer to copy from, from which @c len
 * bytes will be copied.
 * @return Zero upon success.
 */
int cace_data_copy_from(cace_data_t *data, size_t len, cace_data_ptr_t src);

/// @overload
int cace_data_copy_from_cstr(cace_data_t *data, const char *src);

/** Append an initialized data struct with a given size.
 *
 * @param[in,out] data The data to copy into, which must not be NULL.
 * @param len The total length to allocate, which may be non-zero.
 * @param src An optional source buffer to copy from, from which @c len
 * bytes will be copied.
 * @return Zero upon success.
 */
int cace_data_append_from(cace_data_t *data, size_t len, cace_data_ptr_t src);

/** @overload
 */
int cace_data_append_byte(cace_data_t *data, uint8_t val);

/** Copy between two data structs, both already initialized.
 *
 * @param[in,out] data The data to copy to, which must not be NULL.
 * @param src The data to copy from, which must not be NULL.
 * @return Zero upon success.
 */
int cace_data_copy(cace_data_t *data, const cace_data_t *src);

/** Move between two data structs, both already initialized.
 *
 * @param[in,out] data The data to move to, which must not be NULL.
 * @param[in,out] src The data to move from, which must not be NULL.
 */
void cace_data_move(cace_data_t *data, cace_data_t *src);

/** Swap between two data structs, both already initialized.
 *
 * @param[in,out] data The data to swap to, which must not be NULL.
 * @param[in,out] other The data to swap with, which must not be NULL.
 */
void cace_data_swap(cace_data_t *data, cace_data_t *other);

bool cace_data_is_empty(const cace_data_t *data);

/** Interface for M*LIB items.
 */
size_t cace_data_hash(const cace_data_t *data);

/** Interface for M*LIB items.
 */
int cace_data_cmp(const cace_data_t *lt, const cace_data_t *rt);

/** Interface for M*LIB items.
 */
bool cace_data_equal(const cace_data_t *lt, const cace_data_t *rt);

struct m_string_s;
/** Copy a text C-string from an M-STRING object.
 * The copy includes a terminating null.
 *
 * @param[in,out] data The data to copy to, which must not be NULL.
 * @param src The string to copy from, which must not be NULL.
 * @return Zero upon success.
 */
int cace_data_from_m_string(cace_data_t *data, const struct m_string_s *src);

/** A definition to use length-one-array trick for store-by-value and
 * pass-by-reference semantics.
 */
typedef struct cace_data_s cace_data_a1_t[1];

/// Default OPLIST for cace_data_t
#define M_OPL_cace_data_t()                                                                            \
    (INIT(API_2(cace_data_init)), INIT_SET(API_6(cace_data_init_set)), CLEAR(API_2(cace_data_deinit)), \
     SET(API_2(cace_data_copy)), MOVE(API_6(cace_data_move)), SWAP(API_6(cace_data_swap)),             \
     HASH(API_2(cace_data_hash)), EQUAL(API_6(cace_data_equal)))

#ifdef __cplusplus
}
#endif

#endif /* CACE_CACE_DATA_H_ */
