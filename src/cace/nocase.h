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

#ifndef CACE_NOCASE_H_
#define CACE_NOCASE_H_

#include <m-string.h>
#include <strings.h>

/// Case-insensitive equality comparison
#define M_CSTR_NOCASE_EQUAL(a, b) (strcasecmp((a), (b)) == 0)

/** M*LIB OPLIST for case-insenstive C-string values.
 * This is intended to be used as dict/tree keys of type "const char *" with
 * external memory management.
 */
#define M_CSTR_NOCASE_OPLIST M_OPEXTEND(M_CSTR_OPLIST, EQUAL(M_CSTR_NOCASE_EQUAL), CMP(strcasecmp))

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

/** M*LIB OPLIST for case-insenstive M-STRING values.
 * This is intended to be used as dict/tree keys of type "m_string_t" with
 * external memory management.
 */
#define M_STRING_NOCASE_OPLIST M_OPEXTEND(M_STRING_OPLIST, EQUAL(m_string_nocase_eq), CMP(m_string_nocase_cmp))

#endif /* CACE_NOCASE_H_ */
