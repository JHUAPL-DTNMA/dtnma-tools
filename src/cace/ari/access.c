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
#include "access.h"
#include "cace/util/defs.h"

bool ari_is_undefined(const ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_UNDEFINED;
}

void ari_set_undefined(ari_t *ari)
{
    ari_deinit(ari);
}

bool ari_is_null(const ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_NULL;
}

void ari_set_null(ari_t *ari)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = false, .prim_type = ARI_PRIM_NULL,
        // no associated value
    };
}

bool ari_is_bool(const ari_t *ari)
{
    CHKFALSE(ari);
    if (ari->is_ref)
    {
        return false;
    }
    return ari->as_lit.prim_type == ARI_PRIM_BOOL;
}

int ari_get_bool(const ari_t *ari, bool *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    if (ari->as_lit.prim_type != ARI_PRIM_BOOL)
    {
        return 2;
    }
    *out = ari->as_lit.value.as_bool;
    return 0;
}

void ari_set_bool(ari_t *ari, ari_bool src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = false, .prim_type = ARI_PRIM_BOOL, .value = { .as_bool = src } };
}

void ari_set_prim_uint64(ari_t *ari, uint64_t src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) =
        (ari_lit_t) { .has_ari_type = false, .prim_type = ARI_PRIM_UINT64, .value = { .as_uint64 = src } };
}

void ari_set_prim_int64(ari_t *ari, int64_t src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) =
        (ari_lit_t) { .has_ari_type = false, .prim_type = ARI_PRIM_INT64, .value = { .as_int64 = src } };
}

void ari_set_prim_float64(ari_t *ari, ari_real64 src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) =
        (ari_lit_t) { .has_ari_type = false, .prim_type = ARI_PRIM_FLOAT64, .value = { .as_float64 = src } };
}

int ari_get_int(ari_t *ari, ari_int *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    if (ari->as_lit.prim_type != ARI_PRIM_INT64)
    {
        return 2;
    }
    *out = ari->as_lit.value.as_int64;
    return 0;
}

void ari_set_int(ari_t *ari, ari_int src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = true, .ari_type = ARI_TYPE_INT, .prim_type = ARI_PRIM_INT64, .value = { .as_int64 = src }
    };
}

int ari_get_uint(ari_t *ari, ari_uint *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    if (ari->as_lit.prim_type != ARI_PRIM_UINT64)
    {
        return 2;
    }
    uint64_t *val = &(ari->as_lit.value.as_uint64);
    if (*val > UINT32_MAX)
    {
        return 3;
    }
    *out = *val;
    return 0;
}

void ari_set_uint(ari_t *ari, ari_uint src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = true, .ari_type = ARI_TYPE_UINT, .prim_type = ARI_PRIM_UINT64, .value = { .as_uint64 = src }
    };
}

void ari_set_vast(ari_t *ari, ari_vast src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = true, .ari_type = ARI_TYPE_VAST, .prim_type = ARI_PRIM_INT64, .value = { .as_int64 = src }
    };
}

void ari_set_uvast(ari_t *ari, ari_uvast src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = true, .ari_type = ARI_TYPE_UVAST, .prim_type = ARI_PRIM_UINT64, .value = { .as_uint64 = src }
    };
}

int ari_get_uvast(const ari_t *ari, ari_uvast *out)
{
    CHKERR1(ari);
    CHKERR1(out);
    if (ari->is_ref)
    {
        return 1;
    }
    switch (ari->as_lit.prim_type)
    {
        case ARI_PRIM_UINT64:
            *out = ari->as_lit.value.as_uint64;
            break;
        case ARI_PRIM_INT64:
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

void ari_set_real64(ari_t *ari, ari_real64 src)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = true, .ari_type = ARI_TYPE_REAL64, .prim_type = ARI_PRIM_FLOAT64, .value = { .as_float64 = src }
    };
}

void ari_set_tstr(ari_t *ari, const char *buf, bool copy)
{
    CHKVOID(ari);
    ari_deinit(ari);

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

    *ari_init_lit(ari) =
        (ari_lit_t) { .has_ari_type = false, .prim_type = ARI_PRIM_TSTR, .value = { .as_data = data } };
}

void ari_set_bstr(ari_t *ari, cace_data_t *src, bool copy)
{
    CHKVOID(ari);
    ari_deinit(ari);

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

    *ari_init_lit(ari) =
        (ari_lit_t) { .has_ari_type = false, .prim_type = ARI_PRIM_BSTR, .value = { .as_data = data } };
}

void ari_set_tp(ari_t *ari, struct timespec dtntime)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = true,
                                       .ari_type     = ARI_TYPE_TP,
                                       .prim_type    = ARI_PRIM_TIMESPEC,
                                       .value        = { .as_timespec = dtntime } };
}

void ari_set_td(ari_t *ari, struct timespec delta)
{
    CHKVOID(ari);
    ari_deinit(ari);
    *ari_init_lit(ari) = (ari_lit_t) {
        .has_ari_type = true, .ari_type = ARI_TYPE_TD, .prim_type = ARI_PRIM_TIMESPEC, .value = { .as_timespec = delta }
    };
}

void ari_set_ac(ari_t *ari, struct ari_ac_s *src)
{
    CHKVOID(ari);
    ari_deinit(ari);

    ari_ac_t *ctr = M_MEMORY_ALLOC(ari_ac_t);
    ari_ac_init(ctr);
    if (src)
    {
        ari_list_move(ctr->items, src->items);
    }

    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = true,
                                       .ari_type     = ARI_TYPE_AC,
                                       .prim_type    = ARI_PRIM_OTHER,
                                       .value        = {
                                                  .as_ac = ctr,
                                       } };
}

void ari_set_am(ari_t *ari, struct ari_am_s *src)
{
    CHKVOID(ari);
    ari_deinit(ari);

    ari_am_t *ctr = M_MEMORY_ALLOC(ari_am_t);
    ari_am_init(ctr);
    if (src)
    {
        ari_tree_move(ctr->items, src->items);
    }

    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = true,
                                       .ari_type     = ARI_TYPE_AM,
                                       .prim_type    = ARI_PRIM_OTHER,
                                       .value        = {
                                                  .as_am = ctr,
                                       } };
}

void ari_set_tbl(ari_t *ari, struct ari_tbl_s *src)
{
    CHKVOID(ari);
    ari_deinit(ari);

    ari_tbl_t *ctr = M_MEMORY_ALLOC(ari_tbl_t);
    ari_tbl_init(ctr, 0, 0);
    if (src)
    {
        ctr->ncols = src->ncols;
        ari_array_move(ctr->items, src->items);
    }

    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = true,
                                       .ari_type     = ARI_TYPE_TBL,
                                       .prim_type    = ARI_PRIM_OTHER,
                                       .value        = {
                                                  .as_tbl = ctr,
                                       } };
}

struct ari_execset_s *ari_init_execset(ari_t *ari)
{
    CHKNULL(ari);

    ari_execset_t *ctr = ARI_MALLOC(sizeof(ari_execset_t));
    ari_execset_init(ctr);

    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = true,
                                       .ari_type     = ARI_TYPE_EXECSET,
                                       .prim_type    = ARI_PRIM_OTHER,
                                       .value        = {
                                                  .as_execset = ctr,
                                       } };

    return ctr;
}

struct ari_rptset_s *ari_init_rptset(ari_t *ari)
{
    CHKNULL(ari);

    ari_rptset_t *ctr = ARI_MALLOC(sizeof(ari_rptset_t));
    ari_rptset_init(ctr);

    *ari_init_lit(ari) = (ari_lit_t) { .has_ari_type = true,
                                       .ari_type     = ARI_TYPE_RPTSET,
                                       .prim_type    = ARI_PRIM_OTHER,
                                       .value        = {
                                                  .as_rptset = ctr,
                                       } };

    return ctr;
}

void ari_set_objref_path_textid(ari_t *ari, const char *ns_id, ari_type_t type_id, const char *obj_id)
{
    CHKVOID(ari);
    ari_deinit(ari);

    ari_ref_t *ref = ari_init_objref(ari);
    {
        ref->objpath.ns_id.form = ARI_IDSEG_TEXT;
        string_t *value         = &(ref->objpath.ns_id.as_text);
        string_init_set_str(*value, ns_id);
    }
    {
        // FIXME better way to handle this?
        const char *type_name     = ari_type_to_name(type_id);
        ref->objpath.type_id.form = ARI_IDSEG_TEXT;
        string_t *value           = &(ref->objpath.type_id.as_text);
        string_init_set_str(*value, type_name);
    }
    {
        ref->objpath.obj_id.form = ARI_IDSEG_TEXT;
        string_t *value          = &(ref->objpath.obj_id.as_text);
        string_init_set_str(*value, obj_id);
    }

    ref->objpath.has_ari_type = true;
    ref->objpath.ari_type     = type_id;
}

void ari_set_objref_path_intid(ari_t *ari, int64_t ns_id, ari_type_t type_id, int64_t obj_id)
{
    CHKVOID(ari);
    ari_deinit(ari);

    ari_ref_t *ref = ari_init_objref(ari);
    {
        ref->objpath.ns_id.form   = ARI_IDSEG_INT;
        ref->objpath.ns_id.as_int = ns_id;
    }
    {
        ref->objpath.type_id.form   = ARI_IDSEG_INT;
        ref->objpath.type_id.as_int = type_id;
    }
    {
        ref->objpath.obj_id.form   = ARI_IDSEG_INT;
        ref->objpath.obj_id.as_int = obj_id;
    }

    ref->objpath.has_ari_type = true;
    ref->objpath.ari_type     = type_id;
}

void ari_set_objref_params_ac(ari_t *ari, struct ari_ac_s *src)
{
    CHKVOID(ari);
    CHKVOID(ari->is_ref);

    ari_params_deinit(&(ari->as_ref.params));
    ari->as_ref.params.state = ARI_PARAMS_AC;
    ari->as_ref.params.as_ac = src;
}

void ari_set_objref_params_am(ari_t *ari, struct ari_am_s *src)
{
    CHKVOID(ari);
    CHKVOID(ari->is_ref);

    ari_params_deinit(&(ari->as_ref.params));
    ari->as_ref.params.state = ARI_PARAMS_AM;
    ari->as_ref.params.as_am = src;
}
