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
#include "access.h"
#include "cace/util/defs.h"
#include <timespec.h>

bool cace_ari_is_undefined(const cace_ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return false;
    }
    return ari->as_lit.prim_type == CACE_ARI_PRIM_UNDEFINED;
}

bool cace_ari_not_undefined(const cace_ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return true;
    }
    return ari->as_lit.prim_type != CACE_ARI_PRIM_UNDEFINED;
}

void cace_ari_set_undefined(cace_ari_t *ari)
{
    cace_ari_deinit(ari);
}

bool cace_ari_is_null(const cace_ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return false;
    }
    return ari->as_lit.prim_type == CACE_ARI_PRIM_NULL;
}

void cace_ari_set_null(cace_ari_t *ari)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) {
        .has_ari_type = false, .prim_type = CACE_ARI_PRIM_NULL,
        // no associated value
    };
}

bool cace_ari_is_bool(const cace_ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return false;
    }
    return ari->as_lit.prim_type == CACE_ARI_PRIM_BOOL;
}

int cace_ari_get_bool(const cace_ari_t *ari, bool *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    if (ari->as_lit.prim_type != CACE_ARI_PRIM_BOOL)
    {
        return 2;
    }
    *out = ari->as_lit.value.as_bool;
    return 0;
}

void cace_ari_set_prim_bool(cace_ari_t *ari, cace_ari_bool src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) =
        (cace_ari_lit_t) { .has_ari_type = false, .prim_type = CACE_ARI_PRIM_BOOL, .value = { .as_bool = src } };
}

void cace_ari_set_prim_uint64(cace_ari_t *ari, uint64_t src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) =
        (cace_ari_lit_t) { .has_ari_type = false, .prim_type = CACE_ARI_PRIM_UINT64, .value = { .as_uint64 = src } };
}

void cace_ari_set_prim_int64(cace_ari_t *ari, int64_t src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) =
        (cace_ari_lit_t) { .has_ari_type = false, .prim_type = CACE_ARI_PRIM_INT64, .value = { .as_int64 = src } };
}

void cace_ari_set_prim_float64(cace_ari_t *ari, cace_ari_real64 src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) =
        (cace_ari_lit_t) { .has_ari_type = false, .prim_type = CACE_ARI_PRIM_FLOAT64, .value = { .as_float64 = src } };
}

int cace_ari_get_int(const cace_ari_t *ari, cace_ari_int *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    switch (ari->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            const uint64_t *val = &(ari->as_lit.value.as_uint64);
            if (*val > INT32_MAX)
            {
                return 3;
            }
            *out = *val;
            break;
        }
        case CACE_ARI_PRIM_INT64:
        {
            const int64_t *val = &(ari->as_lit.value.as_int64);
            if ((*val < INT32_MIN) || (*val > INT32_MAX))
            {
                return 3;
            }
            *out = *val;
            break;
        }
        default:
            return 2;
    }
    return 0;
}

int cace_ari_get_uint(const cace_ari_t *ari, cace_ari_uint *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    switch (ari->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            const uint64_t *val = &(ari->as_lit.value.as_uint64);
            if (*val > UINT32_MAX)
            {
                return 3;
            }
            *out = *val;
            break;
        }
        case CACE_ARI_PRIM_INT64:
        {
            const int64_t *val = &(ari->as_lit.value.as_int64);
            if ((*val < 0) || (*val > UINT32_MAX))
            {
                return 3;
            }
            *out = *val;
            break;
        }
        default:
            return 2;
    }
    return 0;
}

int cace_ari_get_byte(const cace_ari_t *ari, cace_ari_byte *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    switch (ari->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            const uint64_t *val = &(ari->as_lit.value.as_uint64);
            if (*val > UINT8_MAX)
            {
                return 3;
            }
            *out = *val;
            break;
        }
        case CACE_ARI_PRIM_INT64:
        {
            const int64_t *val = &(ari->as_lit.value.as_int64);
            if ((*val < 0) || (*val > UINT8_MAX))
            {
                return 3;
            }
            *out = *val;
            break;
        }
        default:
            return 2;
    }
    return 0;
}

int cace_ari_get_vast(const cace_ari_t *ari, cace_ari_vast *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    switch (ari->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            const uint64_t *val = &(ari->as_lit.value.as_uint64);
            if (*val > INT64_MAX)
            {
                return 3;
            }
            *out = *val;
            break;
        }
        case CACE_ARI_PRIM_INT64:
            *out = ari->as_lit.value.as_int64;
            break;
        default:
            return 2;
    }
    return 0;
}

int cace_ari_get_uvast(const cace_ari_t *ari, cace_ari_uvast *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    switch (ari->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            *out = ari->as_lit.value.as_uint64;
            break;
        case CACE_ARI_PRIM_INT64:
        {
            const int64_t *val = &(ari->as_lit.value.as_int64);
            if (*val < 0)
            {
                return 3;
            }
            *out = *val;
            break;
        }
        default:
            return 2;
    }
    return 0;
}

void cace_ari_set_bool(cace_ari_t *ari, cace_ari_bool src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_BOOL,
                                                 .prim_type    = CACE_ARI_PRIM_BOOL,
                                                 .value        = { .as_bool = src } };
}

void cace_ari_set_byte(cace_ari_t *ari, cace_ari_byte src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_BYTE,
                                                 .prim_type    = CACE_ARI_PRIM_INT64,
                                                 .value        = { .as_int64 = src } };
}

void cace_ari_set_int(cace_ari_t *ari, cace_ari_int src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_INT,
                                                 .prim_type    = CACE_ARI_PRIM_INT64,
                                                 .value        = { .as_int64 = src } };
}

void cace_ari_set_uint(cace_ari_t *ari, cace_ari_uint src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_UINT,
                                                 .prim_type    = CACE_ARI_PRIM_UINT64,
                                                 .value        = { .as_uint64 = src } };
}

void cace_ari_set_vast(cace_ari_t *ari, cace_ari_vast src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_VAST,
                                                 .prim_type    = CACE_ARI_PRIM_INT64,
                                                 .value        = { .as_int64 = src } };
}

void cace_ari_set_uvast(cace_ari_t *ari, cace_ari_uvast src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_UVAST,
                                                 .prim_type    = CACE_ARI_PRIM_UINT64,
                                                 .value        = { .as_uint64 = src } };
}

void cace_ari_set_real32(cace_ari_t *ari, cace_ari_real32 src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_REAL32,
                                                 .prim_type    = CACE_ARI_PRIM_FLOAT64,
                                                 .value        = { .as_float64 = src } };
}

void cace_ari_set_real64(cace_ari_t *ari, cace_ari_real64 src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_REAL64,
                                                 .prim_type    = CACE_ARI_PRIM_FLOAT64,
                                                 .value        = { .as_float64 = src } };
}

const cace_data_t *cace_ari_cget_tstr(const cace_ari_t *ari)
{
    if (ari->is_ref || (ari->as_lit.prim_type != CACE_ARI_PRIM_TSTR))
    {
        return NULL;
    }
    return &(ari->as_lit.value.as_data);
}

void cace_ari_set_tstr(cace_ari_t *ari, const char *buf, bool copy)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);

    cace_data_t data;
    if (buf)
    {
        const size_t len = strlen(buf) + 1;
        if (copy)
        {
            cace_data_init(&data);
            cace_data_copy_from(&data, len, (cace_data_ptr_t)buf);
        }
        else
        {
            cace_data_init_view(&data, len, (cace_data_ptr_t)buf);
        }
    }
    else
    {
        cace_data_init(&data);
    }

    *cace_ari_init_lit(ari) =
        (cace_ari_lit_t) { .has_ari_type = false, .prim_type = CACE_ARI_PRIM_TSTR, .value = { .as_data = data } };
}

const cace_data_t *cace_ari_cget_bstr(const cace_ari_t *ari)
{
    if (ari->is_ref || (ari->as_lit.prim_type != CACE_ARI_PRIM_BSTR))
    {
        return NULL;
    }
    return &(ari->as_lit.value.as_data);
}

void cace_ari_set_bstr(cace_ari_t *ari, cace_data_t *src, bool copy)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);

    cace_data_t data;
    cace_data_init(&data);
    if (copy)
    {
        cace_data_copy(&data, src);
    }
    else
    {
        cace_data_swap(&data, src);
    }

    *cace_ari_init_lit(ari) =
        (cace_ari_lit_t) { .has_ari_type = false, .prim_type = CACE_ARI_PRIM_BSTR, .value = { .as_data = data } };
}

int cace_ari_get_tp(const cace_ari_t *ari, struct timespec *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_TP))
    {
        return 2;
    }
    *out = ari->as_lit.value.as_timespec;
    return 0;
}

int cace_ari_get_tp_posix(const cace_ari_t *ari, struct timespec *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_TP))
    {
        return 2;
    }
    *out = timespec_add(ari->as_lit.value.as_timespec, cace_ari_dtn_epoch_timespec);
    return 0;
}

void cace_ari_set_tp(cace_ari_t *ari, struct timespec dtntime)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_TP,
                                                 .prim_type    = CACE_ARI_PRIM_TIMESPEC,
                                                 .value        = { .as_timespec = dtntime } };
}

void cace_ari_set_tp_posix(cace_ari_t *ari, struct timespec ptime)
{
    struct timespec dtntime = timespec_sub(ptime, cace_ari_dtn_epoch_timespec);
    cace_ari_set_tp(ari, dtntime);
}

int cace_ari_get_td(const cace_ari_t *ari, struct timespec *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_TD))
    {
        return 2;
    }
    *out = ari->as_lit.value.as_timespec;
    return 0;
}

void cace_ari_set_td(cace_ari_t *ari, struct timespec delta)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_TD,
                                                 .prim_type    = CACE_ARI_PRIM_TIMESPEC,
                                                 .value        = { .as_timespec = delta } };
}

void cace_ari_set_aritype(cace_ari_t *ari, cace_ari_type_t type)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);
    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_ARITYPE,
                                                 .prim_type    = CACE_ARI_PRIM_INT64,
                                                 .value        = { .as_int64 = type } };
}

void cace_ari_set_aritype_text(cace_ari_t *ari, cace_ari_type_t type)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);

    const char *buf = cace_ari_type_to_name(type);

    cace_data_t  data;
    const size_t len = strlen(buf) + 1;
    cace_data_init_view(&data, len, (cace_data_ptr_t)buf);

    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_ARITYPE,
                                                 .prim_type    = CACE_ARI_PRIM_TSTR,
                                                 .value        = { .as_data = data } };
}

bool cace_ari_is_lit_typed(const cace_ari_t *ari, cace_ari_type_t typ)
{
    return (ari && !(ari->is_ref) && ari->as_lit.has_ari_type && (ari->as_lit.ari_type == typ));
}

const int64_t *cace_ari_get_aritype(const cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_ARITYPE))
    {
        return NULL;
    }
    if (ari->as_lit.prim_type != CACE_ARI_PRIM_INT64)
    {
        return NULL;
    }
    return &(ari->as_lit.value.as_int64);
}

struct cace_ari_ac_s *cace_ari_get_ac(cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_AC))
    {
        return NULL;
    }
    return ari->as_lit.value.as_ac;
}

const struct cace_ari_ac_s *cace_ari_cget_ac(const cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_AC))
    {
        return NULL;
    }
    return ari->as_lit.value.as_ac;
}

void cace_ari_set_ac(cace_ari_t *ari, struct cace_ari_ac_s *src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);

    cace_ari_ac_t *ctr = CACE_MALLOC(sizeof(cace_ari_ac_t));
    cace_ari_ac_init(ctr);
    if (src)
    {
        cace_ari_list_move(ctr->items, src->items);
    }

    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_AC,
                                                 .prim_type    = CACE_ARI_PRIM_OTHER,
                                                 .value        = {
                                                            .as_ac = ctr,
                                                 } };
}

struct cace_ari_am_s *cace_ari_get_am(cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_AM))
    {
        return NULL;
    }
    return ari->as_lit.value.as_am;
}

const struct cace_ari_am_s *cace_ari_cget_am(const cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_AM))
    {
        return NULL;
    }
    return ari->as_lit.value.as_am;
}

void cace_ari_set_am(cace_ari_t *ari, struct cace_ari_am_s *src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);

    cace_ari_am_t *ctr = CACE_MALLOC(sizeof(cace_ari_am_t));
    cace_ari_am_init(ctr);
    if (src)
    {
        cace_ari_tree_move(ctr->items, src->items);
    }

    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_AM,
                                                 .prim_type    = CACE_ARI_PRIM_OTHER,
                                                 .value        = {
                                                            .as_am = ctr,
                                                 } };
}

struct cace_ari_tbl_s *cace_ari_get_tbl(cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_TBL))
    {
        return NULL;
    }
    return ari->as_lit.value.as_tbl;
}

const struct cace_ari_tbl_s *cace_ari_cget_tbl(const cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_TBL))
    {
        return NULL;
    }
    return ari->as_lit.value.as_tbl;
}

void cace_ari_set_tbl(cace_ari_t *ari, struct cace_ari_tbl_s *src)
{
    CHKVOID(ari);
    cace_ari_deinit(ari);

    cace_ari_tbl_t *ctr = CACE_MALLOC(sizeof(cace_ari_tbl_t));
    cace_ari_tbl_init(ctr, 0, 0);
    if (src)
    {
        ctr->ncols = src->ncols;
        cace_ari_array_move(ctr->items, src->items);
    }

    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_TBL,
                                                 .prim_type    = CACE_ARI_PRIM_OTHER,
                                                 .value        = {
                                                            .as_tbl = ctr,
                                                 } };
}

struct cace_ari_execset_s *cace_ari_get_execset(cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_EXECSET))
    {
        return NULL;
    }
    return ari->as_lit.value.as_execset;
}

const struct cace_ari_execset_s *cace_ari_cget_execset(const cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_EXECSET))
    {
        return NULL;
    }
    return ari->as_lit.value.as_execset;
}

struct cace_ari_execset_s *cace_ari_set_execset(cace_ari_t *ari)
{
    CHKNULL(ari);
    cace_ari_deinit(ari);

    cace_ari_execset_t *ctr = CACE_MALLOC(sizeof(cace_ari_execset_t));
    cace_ari_execset_init(ctr);

    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_EXECSET,
                                                 .prim_type    = CACE_ARI_PRIM_OTHER,
                                                 .value        = {
                                                            .as_execset = ctr,
                                                 } };

    return ctr;
}

struct cace_ari_rptset_s *cace_ari_get_rptset(cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_RPTSET))
    {
        return NULL;
    }
    return ari->as_lit.value.as_rptset;
}

const struct cace_ari_rptset_s *cace_ari_cget_rptset(const cace_ari_t *ari)
{
    if (!cace_ari_is_lit_typed(ari, CACE_ARI_TYPE_RPTSET))
    {
        return NULL;
    }
    return ari->as_lit.value.as_rptset;
}

struct cace_ari_rptset_s *cace_ari_set_rptset(cace_ari_t *ari)
{
    CHKNULL(ari);
    cace_ari_deinit(ari);

    cace_ari_rptset_t *ctr = CACE_MALLOC(sizeof(cace_ari_rptset_t));
    cace_ari_rptset_init(ctr);

    *cace_ari_init_lit(ari) = (cace_ari_lit_t) { .has_ari_type = true,
                                                 .ari_type     = CACE_ARI_TYPE_RPTSET,
                                                 .prim_type    = CACE_ARI_PRIM_OTHER,
                                                 .value        = {
                                                            .as_rptset = ctr,
                                                 } };

    return ctr;
}

const cace_ari_ref_t *cace_ari_cget_ref(const cace_ari_t *ari)

{
    if (!ari || !(ari->is_ref))
    {
        return NULL;
    }
    return &ari->as_ref;
}

const cace_ari_objpath_t *cace_ari_cget_ref_objpath(const cace_ari_t *ari)
{
    const cace_ari_ref_t *ref = cace_ari_cget_ref(ari);
    return ref ? &ref->objpath : NULL;
}
