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
 */
#include "ari_data.h"
#include "util.h"
#include <m-core.h>
#include <m-string.h>
#include <string.h>

static void ari_data_int_reset(ari_data_t *data)
{
    data->owned = false;
    data->ptr   = NULL;
    data->len   = 0;
}

static void ari_data_int_free(ari_data_t *data)
{
    if (data->owned && data->ptr)
    {
        ARI_FREE(data->ptr);
    }
}

int ari_data_init(ari_data_t *data)
{
    CHKERR1(data);
    ari_data_int_reset(data);
    return 0;
}

int ari_data_init_view(ari_data_t *data, size_t len, const ari_data_ptr_t src)
{
    CHKERR1(data);
    data->owned = false;
    data->ptr   = src;
    data->len   = len;
    return 0;
}

int ari_data_init_set(ari_data_t *data, const ari_data_t *src)
{
    CHKERR1(data);
    ari_data_int_reset(data);
    return ari_data_copy_from(data, src->len, src->ptr);
}

int ari_data_deinit(ari_data_t *data)
{
    CHKERR1(data);
    ari_data_int_free(data);
    return 0;
}

int ari_data_copy_from(ari_data_t *data, size_t len, ari_data_ptr_t src)
{
    CHKERR1(data);

    if (len)
    {
        if (!data->owned)
        {
            data->ptr   = NULL;
            data->owned = true;
        }
        if (ari_data_resize(data, len))
        {
            return 2;
        }
    }
    else
    {
        ari_data_int_reset(data);
    }

    if (data->ptr && src)
    {
        memcpy(data->ptr, src, len);
    }

    return 0;
}

int ari_data_copy(ari_data_t *data, const ari_data_t *src)
{
    CHKERR1(data);
    CHKERR1(src);

    ari_data_int_free(data);
    return ari_data_copy_from(data, src->len, src->ptr);
}

int ari_data_swap(ari_data_t *data, ari_data_t *other)
{
    CHKERR1(data);
    CHKERR1(other);
    ari_data_t tmp = *data;
    *data          = *other;
    *other         = tmp;
    return 0;
}

size_t ari_data_hash(const ari_data_t *data)
{
    CHKRET(data, 0);
    size_t accum = M_HASH_DEFAULT(data->len);
    if (data->ptr)
    {
        accum ^= m_core_hash(data->ptr, data->len);
    }
    return accum;
}

int ari_data_cmp(const ari_data_t *lt, const ari_data_t *rt)
{
    CHKRET(lt, -2);
    CHKRET(lt, -2);

    int part_cmp = M_CMP_BASIC(lt->len, rt->len);
    if (part_cmp)
    {
        return part_cmp;
    }

    if (!lt->ptr || !rt->ptr)
    {
        // null ptr ordered before non-null
        return lt->ptr ? 1 : -1;
    }
    return memcmp(lt->ptr, rt->ptr, lt->len);
}

bool ari_data_equal(const ari_data_t *lt, const ari_data_t *rt)
{
    CHKFALSE(lt);
    CHKFALSE(rt);

    if (lt->len != rt->len)
    {
        return false;
    }

    if (!lt->ptr || !rt->ptr)
    {
        return false;
    }
    return (memcmp(lt->ptr, rt->ptr, lt->len) == 0);
}

int ari_data_clear(ari_data_t *data)
{
    CHKERR1(data);
    ari_data_int_free(data);
    ari_data_int_reset(data);
    return 0;
}

int ari_data_resize(ari_data_t *data, size_t len)
{
    CHKERR1(data);

    if (len == data->len)
    {
        return 0;
    }
    else if (len == 0)
    {
        ari_data_int_free(data);
        ari_data_int_reset(data);
        return 0;
    }

    if (!data->owned)
    {
        data->ptr = NULL;
    }
    ari_data_ptr_t got = ARI_REALLOC(data->ptr, len);
    if (UNLIKELY(!got))
    {
        ari_data_int_reset(data);
        return 2;
    }
    data->owned = true;
    data->ptr   = got;
    data->len   = len;
    return 0;
}

int ari_data_extend_front(ari_data_t *data, ssize_t extra)
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
        if (ari_data_resize(data, origlen + extra))
        {
            return 2;
        }
        memmove(data->ptr, data->ptr + extra, origlen);
    }
    else
    {
        // removing size, reposition first
        memmove(data->ptr, data->ptr - extra, origlen + extra);
        if (ari_data_resize(data, origlen + extra))
        {
            return 2;
        }
    }
    return 0;
}

int ari_data_append_from(ari_data_t *data, size_t len, ari_data_ptr_t src)
{
    CHKERR1(data);
    if (ari_data_extend_back(data, len))
    {
        return 2;
    }
    memcpy(data->ptr + data->len - len, src, len);
    return 0;
}

int ari_data_append_byte(ari_data_t *data, uint8_t val)
{
    CHKERR1(data);
    if (ari_data_extend_back(data, 1))
    {
        return 2;
    }
    *(data->ptr + data->len - 1) = val;
    return 0;
}

int ari_data_from_m_string(ari_data_t *data, const struct m_string_s *src)
{
    const size_t len = string_size(src) + 1;
    ari_data_copy_from(data, len, (ari_data_ptr_t)string_get_cstr(src));
    return 0;
}
