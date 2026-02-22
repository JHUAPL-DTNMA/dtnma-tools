/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
 * This file contains definitions for ARI text CODEC functions.
 */
#ifndef CACE_ARI_TEXT_H_
#define CACE_ARI_TEXT_H_

#include "base.h"
#include "containers.h"
#include "cace/config.h"
#include <m-string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Options for URI scheme presence
enum cace_ari_text_scheme_prefix_e
{
    /// Never prefix with a scheme
    CACE_ARI_TEXT_SCHEME_NONE,
    /// Prefix only the outer-most value
    CACE_ARI_TEXT_SCHEME_FIRST,
    /// Prefix all values, including container contents
    CACE_ARI_TEXT_SCHEME_ALL,
};

/// Options for encoding ARITYPE values
enum cace_ari_text_aritype_e
{
    /// Show whatever the original decoding was
    CACE_ARI_TEXT_ARITYPE_ORIG,
    /// Always show the text name
    CACE_ARI_TEXT_ARITYPE_TEXT,
    /// Always show the integer enumeration
    CACE_ARI_TEXT_ARITYPE_INT,
};

/// Options for encoding integer bases
enum cace_ari_int_base_e
{
    /// Binary with 0b prefix
    CACE_ARI_TEXT_INT_BASE2 = 2,
    /// Decimal
    CACE_ARI_TEXT_INT_BASE10 = 10,
    /// Hexadecimal with 0x prefix
    CACE_ARI_TEXT_INT_BASE16 = 16,
};
/// Options for encoding byte strings
enum cace_ari_bstr_form_e
{
    /// Attempt to output as text
    CACE_ARI_TEXT_BSTR_RAW,
    /// Base16 according to Section 8 of RFC 4648 @cite rfc4648
    CACE_ARI_TEXT_BSTR_BASE16,
    // Base64 according to Section 5 of RFC 4648 @cite rfc4648
    CACE_ARI_TEXT_BSTR_BASE64URL,
};
/** Encode just an object path, which can be useful for debugging output.
 *
 */
int cace_ari_text_encode_objpath(m_string_t text, const cace_ari_objpath_t *path, enum cace_ari_text_aritype_e show);

/** Parameters for ARI text encoding.
 * Use ::CACE_ARI_TEXT_ENC_OPTS_DEFAULT to initialize these contents.
 */
typedef struct
{
    /** True if the scheme is present at the start of the text.
     * This defaults to ::CACE_ARI_TEXT_SCHEME_FIRST.
     */
    enum cace_ari_text_scheme_prefix_e scheme_prefix;
    /** Determine how to show cace_ari_type_t values.
     * This defaults to ::CACE_ARI_TEXT_ARITYPE_TEXT.
     */
    enum cace_ari_text_aritype_e show_ari_type;
    /** Desired base of integer values.
     * This defaults to 10 (decimal).
     */
    enum cace_ari_int_base_e int_base;
    /** One of 'f', 'g', or 'e' for decimal format, or 'a' for hexadecimal.
     */
    char float_form;
    /** True if specific text can be left unquoted.
     */
    bool text_identity;
    /** Desired encoding form for byte string values.
     * This defaults to base-16 (hexadecimal strings).
     */
    enum cace_ari_bstr_form_e bstr_form;
    /** True if time values should be in text form.
     * False to use decimal fraction form.
     */
    bool time_text;
} cace_ari_text_enc_opts_t;

#define CACE_ARI_TEXT_ENC_OPTS_DEFAULT                                                            \
    (cace_ari_text_enc_opts_t)                                                                    \
    {                                                                                             \
        .scheme_prefix = CACE_ARI_TEXT_SCHEME_FIRST, .show_ari_type = CACE_ARI_TEXT_ARITYPE_TEXT, \
        .int_base = CACE_ARI_TEXT_INT_BASE10, .float_form = 'g', .text_identity = true,           \
        .bstr_form = CACE_ARI_TEXT_BSTR_BASE16, .time_text = true,                                \
    }

/** Encode an ARI to text form.
 *
 * @param[out] text The data buffer to modify and write the result into.
 * It will contain a null-terminated UTF-8 string if successful.
 * @param ari The ARI to encode from.
 * @param opts Encoding parameters.
 * @return Zero upon success.
 */
int cace_ari_text_encode(m_string_t text, const cace_ari_t *ari, cace_ari_text_enc_opts_t opts);

#if defined(ARI_TEXT_PARSE)

/** Decode an ARI from text form.
 *
 * @param[out] ari The struct to decode into.
 * @param text A null-terminated UTF-8 text string.
 * @param[out] errm If non-null, this will be set to a specific error message
 * associated with any failure.
 * When the return code is non-zero, if the pointed-to pointer is non-null it
 * must be freed using ::CACE_FREE().
 * @return Zero upon success.
 */
int cace_ari_text_decode(cace_ari_t *ari, const m_string_t text, char **errm);

/** @overload
 * Decode from a bare C string.
 *
 * @param[out] ari The struct to decode into.
 * @param[in] text_ptr A null-terminated UTF-8 text string.
 * @param text_len The length of text, including including the null terminator.
 * @param[out] errm If non-null, this will be set to a specific error message
 * associated with any failure.
 * When the return code is non-zero, if the pointed-to pointer is non-null it
 * must be freed using ::CACE_FREE().
 * @return Zero upon success.
 */
int cace_ari_text_decode_cstr(cace_ari_t *ari, const char *text_ptr, size_t text_len, char **errm);

#endif /* ARI_TEXT_PARSE */

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_TEXT_H_ */
