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
 * This file contains definitions for ARI text CODEC functions.
 */
#ifndef CACE_ARI_TEXT_UTIL_H_
#define CACE_ARI_TEXT_UTIL_H_

#include "cace/cace_data.h"
#include <m-string.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Determine if a text string conforms to the "id-text" rule from
 * Section 4.1 of @cite ietf-dtn-ari-00.
 *
 * @param[in] text The null-terminated text string.
 * @return True if the text matches the identifier rule.
 */
bool cace_ari_text_is_identity(const cace_data_t *text);

/** Encode to URI percent-encoding text form.
 * This is defined in Section 2.1 of RFC 3986 @cite rfc3986.
 * The set of unreserved characters are alpha, digits, and _.-~ characters.
 * in accordance with Section 2.3 of RFC 3986 @cite rfc3986.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param in The input encoded text which is null-terminated.
 * @param safe A set of additional safe characters to not be encoded,
 * which is null-terminated.
 * @return Zero upon success.
 */
int cace_uri_percent_encode(m_string_t out, const cace_data_t *in, const char *safe);

/** Decode from URI percent-encoding text form.
 * This is defined in Section 2.1 of RFC 3986 @cite rfc3986.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param[in] in The input encoded text which may be null-terminated.
 * @return Zero upon success.
 */
int cace_uri_percent_decode(m_string_t out, const cace_data_t *in);

/** Encode an unsigned integer to text form.
 * The sign will be prepended as necessary.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param in The input encoded text which may be null-terminated.
 * @param base The base to encode to, which must be one of: 2, 10, 16.
 * @return Zero upon success.
 */
int cace_ari_uint64_encode(m_string_t out, uint64_t value, int base);

int cace_ari_uint64_decode(uint64_t *out, const m_string_t in);

/** Encode a floating point number to text form.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param value The value to encode.
 * @param form The encoding form as one of:
 *  'f'
 *  'g'
 *  'x'
 * @return Zero upon success.
 */
int cace_ari_float64_encode(m_string_t out, double value, char form);

/** Encode a decimal fraction to text form.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param in The input fractional value with least digit 1e-9
 * @return Zero upon success.
 */
int cace_decfrac_encode(m_string_t out, const struct timespec *in);

/** Decode a decimal fraction from text.
 *
 * @param[out] out The fractional value with least digit 1e-9
 * @param[in] in The input buffer to read, which may be null terminated.
 * @return Zero upon success.
 * Or 1 to indicate an argument error, 2 to indicate a numeric error,
 * 3 to indicate extra text at the end, and 4 to indicate value underflow.
 */
int cace_decfrac_decode(struct timespec *out, const cace_data_t *in);

/** Determine if data contains only valid UTF-8 code points.
 *
 * @param[in] in The data to check.
 * @return True if the data contains no null codepoints and only valid UTF-8
 * codepoints.
 */
bool cace_data_is_utf8(const cace_data_t *in);

/** Escape backslashes in tstr or bstr text form.
 * This is defined in Section G.2 of RFC 8610 @cite rfc8610
 * and Section 7 of RFC 8259 @cite rfc8259.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param in The input buffer to read, which must be null terminated.
 * @param quote The character used to quote the string.
 * @return Zero upon success.
 */
int cace_slash_escape(m_string_t out, const cace_data_t *in, const char quote);

/** Unescape backslashes in tstr/bstr text form.
 * This is defined in Section G.2 of RFC8610 @cite rfc8610.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param in The input buffer to read, which may be null terminated.
 * @return Zero upon success.
 */
int cace_slash_unescape(m_string_t out, const cace_data_t *in);

/** Remove whitespace characters from a text string.
 * This is based on isspace() inspection.
 *
 * @param[out] out The output buffer, which will be replaced.
 * @param[in] in The input text to read.
 * @param in_len The length of text not including null terminator.
 */
void cace_strip_space(m_string_t out, const char *in, size_t in_len);

/** Convert a text string to lowercase.
 * This is written to work on byte strings, not unicode.
 *
 * @param[out] out The output buffer, which will be replaced.
 */
void cace_string_tolower(m_string_t out);

/** Convert a text string to uppercase.
 * This is written to work on byte strings, not unicode.
 *
 * @param[out] out The output buffer, which will be replaced.
 */
void cace_string_toupper(m_string_t out);

/** Encode to base16 text form.
 * This is defined in Section 8 of RFC 4648 @cite rfc4648.
 *
 * @param[out] out The output buffer, which will be appended to.
 * @param[in] in The input buffer to read.
 * @param uppercase True to use upper-case letters, false to use lower-case.
 * @return Zero upon success.
 */
int cace_base16_encode(m_string_t out, const cace_data_t *in, bool uppercase);

/** Decode base16 text form.
 * This is defined in Section 8 of RFC 4648 @cite rfc4648.
 *
 * @param[out] out The output buffer, which will be sized to its data.
 * @param[in] in The input buffer to read, which must be null terminated.
 * Whitespace in the input must have already been removed with strip_space().
 * @return Zero upon success.
 */
int cace_base16_decode(cace_data_t *out, const m_string_t in);

int cace_base64_encode(m_string_t out, const cace_data_t *in, bool useurl);

/** Decode base64 and base64url text forms.
 * These is defined in Section 4 and 5 of RFC 4648 @cite rfc4648.
 *
 * @param[out] out The output buffer, which will be sized to its data.
 * @param[in] in The input buffer to read, which must be null terminated.
 * Whitespace in the input must have already been removed with strip_space().
 * @return Zero upon success.
 */
int cace_base64_decode(cace_data_t *out, const m_string_t in);

/** Encode to a UTC time in text form.
 * This is defined in Appendix A of RFC 3339 @cite rfc3339.
 * @note The DTN epoch is 2000-01-01T00:00:00Z from @cite rfc9171.
 *
 * @param[out] out The output buffer, which will be sized to its text and
 * null terminated.
 * @param in The time from DTN epoch.
 * @param usesep If true component separators will be added, if false
 * the concise form is used.
 * @return Zero upon success.
 */
int cace_utctime_encode(m_string_t out, const struct timespec *in, bool usesep);

/** Decode a UTC time from text form.
 * This is defined in Appendix A of RFC 3339 @cite rfc3339 with the addition
 * of an optional leading sign character.
 *
 * @param[out] out The decoded time from DTN epoch.
 * @param in The input buffer to read, which may be null terminated.
 * @return Zero upon success.
 */
int cace_utctime_decode(struct timespec *out, const cace_data_t *in);

/** Encode a signed time period time in text form.
 * This is defined in Appendix A of RFC 3339 @cite rfc3339 with the addition
 * of an optional leading sign character.
 *
 * @param[out] out The output buffer, which will be sized to its text and
 * null terminated.
 * @param in The relative time.
 * @return Zero upon success.
 */
int cace_timeperiod_encode(m_string_t out, const struct timespec *in);

/** Decode a signed time period from text form.
 * This is defined in Appendix A of RFC 3339 @cite rfc3339 with the addition
 * of an optional leading sign character.
 *
 * @param[out] out The decoded relative time.
 * @param in The input buffer to read, which may be null terminated.
 * @return Zero upon success.
 */
int cace_timeperiod_decode(struct timespec *out, const cace_data_t *in);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_TEXT_UTIL_H_ */
