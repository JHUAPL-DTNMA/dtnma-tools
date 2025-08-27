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
 */
#include "cace_data.h"
#include "cace/util/defs.h"
#include <m-core.h>
#include <m-string.h>
#include <string.h>

static void cace_data_int_reset(cace_data_t *data)
{
    data->owned = false;
    data->ptr   = NULL;
    data->len   = 0;
}

static void cace_data_int_free(cace_data_t *data)
{
    if (data->owned && data->ptr)
    {
        CACE_FREE(data->ptr);
    }
}

int cace_data_init(cace_data_t *data)
{
    CHKERR1(data);
    cace_data_int_reset(data);
    return 0;
}

int cace_data_init_view(cace_data_t *data, size_t len, const cace_data_ptr_t src)
{
    CHKERR1(data);
    data->owned = false;
    data->ptr   = src;
    data->len   = len;
    return 0;
}

int cace_data_init_view_cstr(cace_data_t *data, const char *src)
{
    CHKERR1(data);
    CHKERR1(src);
    return cace_data_init_view(data, strlen(src) + 1, (cace_data_ptr_t)src);
}

int cace_data_init_set(cace_data_t *data, const cace_data_t *src)
{
    CHKERR1(data);
    cace_data_int_reset(data);
    return cace_data_copy_from(data, src->len, src->ptr);
}

int cace_data_deinit(cace_data_t *data)
{
    CHKERR1(data);
    cace_data_int_free(data);
    return 0;
}

int cace_data_copy_from(cace_data_t *data, size_t len, cace_data_ptr_t src)
{
    CHKERR1(data);

    if (len)
    {
        if (!data->owned)
        {
            data->ptr   = NULL;
            data->owned = true;
        }
        if (cace_data_resize(data, len))
        {
            return 2;
        }
    }
    else
    {
        cace_data_int_reset(data);
    }

    if (data->ptr && src)
    {
        memcpy(data->ptr, src, len);
    }

    return 0;
}

int cace_data_copy_from_cstr(cace_data_t *data, const char *src)
{
    CHKERR1(data);
    CHKERR1(src);
    return cace_data_copy_from(data, strlen(src) + 1, (cace_data_ptr_t)src);
}

int cace_data_copy(cace_data_t *data, const cace_data_t *src)
{
    CHKERR1(data);
    CHKERR1(src);

    cace_data_int_free(data);
    return cace_data_copy_from(data, src->len, src->ptr);
}

void cace_data_move(cace_data_t *data, cace_data_t *src)
{
    CHKVOID(data);
    CHKVOID(src);

    cace_data_int_free(data);
    *data = *src; // shallow copy
    cace_data_int_reset(src);
}

void cace_data_swap(cace_data_t *data, cace_data_t *other)
{
    CHKVOID(data);
    CHKVOID(other);
    cace_data_t tmp = *data;
    *data           = *other;
    *other          = tmp;
}

bool cace_data_is_empty(const cace_data_t *data)
{
    if (!data)
    {
        return true;
    }
    return data->len == 0;
}

size_t cace_data_hash(const cace_data_t *data)
{
    CHKRET(data, 0);
    size_t accum = M_HASH_DEFAULT(data->len);
    if (data->ptr)
    {
        accum ^= m_core_hash(data->ptr, data->len);
    }
    return accum;
}

int cace_data_cmp(const cace_data_t *lt, const cace_data_t *rt)
{
    CHKRET(lt, -2);
    CHKRET(lt, -2);

    int part_cmp = M_CMP_BASIC(lt->len, rt->len);
    if (part_cmp)
    {
        return part_cmp;
    }

    if (!lt->ptr && !rt->ptr)
    {
        // both null are equal
        return 0;
    }
    if (!lt->ptr || !rt->ptr)
    {
        // null ptr ordered before non-null
        return lt->ptr ? 1 : -1;
    }
    return memcmp(lt->ptr, rt->ptr, lt->len);
}

bool cace_data_equal(const cace_data_t *lt, const cace_data_t *rt)
{
    CHKFALSE(lt);
    CHKFALSE(rt);

    if (lt->len != rt->len)
    {
        return false;
    }

    if (!lt->ptr && !rt->ptr)
    {
        return true;
    }
    if (!lt->ptr || !rt->ptr)
    {
        return false;
    }
    return (memcmp(lt->ptr, rt->ptr, lt->len) == 0);
}

int cace_data_clear(cace_data_t *data)
{
    CHKERR1(data);
    cace_data_int_free(data);
    cace_data_int_reset(data);
    return 0;
}

int cace_data_resize(cace_data_t *data, size_t len)
{
    CHKERR1(data);

    if (len == data->len)
    {
        return 0;
    }
    else if (len == 0)
    {
        cace_data_int_free(data);
        cace_data_int_reset(data);
        return 0;
    }

    if (!data->owned)
    {
        data->ptr = NULL;
    }
    cace_data_ptr_t got = CACE_REALLOC(data->ptr, len);
    if (UNLIKELY(!got))
    {
        cace_data_int_reset(data);
        return 2;
    }
    data->owned = true;
    data->ptr   = got;
    data->len   = len;
    return 0;
}

int cace_data_extend_front(cace_data_t *data, ssize_t extra)
{
    CHKERR1(data);
    if (extra == 0)
    {
        return 0;
    }

    size_t origlen = data->len;
    if (extra > 0)
    {
        // adding size, reposition to front
        if (cace_data_resize(data, origlen + extra))
        {
            return 2;
        }
        memmove(data->ptr, data->ptr + extra, origlen);
    }
    else
    {
        // removing size, reposition first
        memmove(data->ptr, data->ptr - extra, origlen + extra);
        if (cace_data_resize(data, origlen + extra))
        {
            return 2;
        }
    }
    return 0;
}

int cace_data_append_from(cace_data_t *data, size_t len, cace_data_ptr_t src)
{
    CHKERR1(data);
    if (cace_data_extend_back(data, len))
    {
        return 2;
    }
    memcpy(data->ptr + data->len - len, src, len);
    return 0;
}

int cace_data_append_byte(cace_data_t *data, uint8_t val)
{
    CHKERR1(data);
    if (cace_data_extend_back(data, 1))
    {
        return 2;
    }
    *(data->ptr + data->len - 1) = val;
    return 0;
}

int cace_data_from_m_string(cace_data_t *data, const struct m_string_s *src)
{
    const size_t len = m_string_size(src) + 1;
    cace_data_copy_from(data, len, (cace_data_ptr_t)m_string_get_cstr(src));
    return 0;
}
