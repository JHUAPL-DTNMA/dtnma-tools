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

#ifndef CACE_NOCASE_H_
#define CACE_NOCASE_H_

#include <m-string.h>
#include <strings.h>

/* HASH function for a C-string (to be used within oplist)
 * We cannot use m_core_hash due to the alignment constraint,
 * and it avoids computing the size before computing the hash.
 */
static inline size_t m_core_cstr_nocase_hash(const char str[])
{
    M_HASH_DECL(hash);
    while (*str)
    {
        unsigned long u = tolower((unsigned char)*str++);
        M_HASH_UP(hash, u);
    }
    return M_HASH_FINAL(hash);
}

#define M_CSTR_NOCASE_HASH(s) (m_core_cstr_nocase_hash(s))

/// Case-insensitive equality comparison
#define M_CSTR_NOCASE_EQUAL(a, b) (strcasecmp((a), (b)) == 0)

/** M*LIB OPLIST for case-insensitive C-string values.
 * This is intended to be used as dict/tree keys of type "const char *" with
 * external memory management.
 */
#define M_CSTR_NOCASE_OPLIST \
    M_OPEXTEND(M_CSTR_OPLIST, HASH(M_CSTR_NOCASE_HASH), EQUAL(M_CSTR_NOCASE_EQUAL), CMP(strcasecmp))

/// Case-insensitive comparison
static inline int m_string_nocase_cmp(const m_string_t v1, const m_string_t v2)
{
    M_ASSERT(v1 != NULL);
    M_ASSERT(v2 != NULL);
    return strcasecmp(m_string_get_cstr(v1), m_string_get_cstr(v2));
}

/// Case-insensitive equality comparison
static inline int m_string_nocase_eq(const m_string_t v1, const m_string_t v2)
{
    M_ASSERT(v1 != NULL);
    M_ASSERT(v2 != NULL);
    return (m_string_size(v1) == m_string_size(v2)) && (strcasecmp(m_string_get_cstr(v1), m_string_get_cstr(v2)) == 0);
}

/** M*LIB OPLIST for case-insensitive M-STRING values.
 * This is intended to be used as dict/tree keys of type "m_string_t" with
 * external memory management.
 */
#define M_STRING_NOCASE_OPLIST M_OPEXTEND(M_STRING_OPLIST, EQUAL(m_string_nocase_eq), CMP(m_string_nocase_cmp))

#endif /* CACE_NOCASE_H_ */
