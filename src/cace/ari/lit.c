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
 */
#include "lit.h"
#include "containers.h"
#include "cace/util/defs.h"
#include <inttypes.h>

const time_t cace_ari_dtn_epoch = CACE_ARI_DTN_EPOCH;

const struct timespec cace_ari_dtn_epoch_timespec = { .tv_sec = CACE_ARI_DTN_EPOCH, .tv_nsec = 0 };

int ari_lit_deinit(ari_lit_t *obj)
{
    switch (obj->prim_type)
    {
        case ARI_PRIM_TSTR:
        case ARI_PRIM_BSTR:
            if (cace_data_deinit(&(obj->value.as_data)))
            {
                return 2;
            }
            break;
        default:
            // handle ARI type
            break;
    }

    if (obj->has_ari_type)
    {
        switch (obj->ari_type)
        {
            case ARI_TYPE_AC:
                CHKERR1(obj->value.as_ac);
                ari_ac_deinit(obj->value.as_ac);
                M_MEMORY_DEL(obj->value.as_ac);
                break;
            case ARI_TYPE_AM:
                CHKERR1(obj->value.as_am);
                ari_am_deinit(obj->value.as_am);
                M_MEMORY_DEL(obj->value.as_am);
                break;
            case ARI_TYPE_TBL:
                CHKERR1(obj->value.as_tbl);
                ari_tbl_deinit(obj->value.as_tbl);
                M_MEMORY_DEL(obj->value.as_tbl);
                break;
            case ARI_TYPE_EXECSET:
                CHKERR1(obj->value.as_execset);
                ari_execset_deinit(obj->value.as_execset);
                M_MEMORY_DEL(obj->value.as_execset);
                break;
            case ARI_TYPE_RPTSET:
                CHKERR1(obj->value.as_rptset);
                ari_rptset_deinit(obj->value.as_rptset);
                M_MEMORY_DEL(obj->value.as_rptset);
                break;
            default:
                // do nothing
                break;
        }
    }
    return 0;
}

int ari_lit_copy(ari_lit_t *lit, const ari_lit_t *src)
{
    lit->has_ari_type = src->has_ari_type;
    lit->ari_type     = src->ari_type;
    lit->prim_type    = src->prim_type;

    switch (src->prim_type)
    {
        case ARI_PRIM_TSTR:
        case ARI_PRIM_BSTR:
            if (cace_data_init_set(&(lit->value.as_data), &(src->value.as_data)))
            {
                return 2;
            }
            break;
        case ARI_PRIM_OTHER:
            // copied below based on ari_type
            break;
        default:
            // simple copy
            lit->value = src->value;
            break;
    }

    if (src->has_ari_type)
    {
        switch (src->ari_type)
        {
            case ARI_TYPE_TP:
            case ARI_TYPE_TD:
                lit->value = src->value;
                break;
            case ARI_TYPE_AC:
            {
                ari_ac_t *ctr    = M_MEMORY_ALLOC(ari_ac_t);
                lit->value.as_ac = ctr;
                ari_list_init_set(ctr->items, src->value.as_ac->items);
                break;
            }
            case ARI_TYPE_AM:
            {
                ari_am_t *ctr    = M_MEMORY_ALLOC(ari_am_t);
                lit->value.as_am = ctr;
                ari_tree_init_set(ctr->items, src->value.as_am->items);
                break;
            }
            case ARI_TYPE_TBL:
            {
                ari_tbl_t *ctr    = M_MEMORY_ALLOC(ari_tbl_t);
                lit->value.as_tbl = ctr;
                ctr->ncols        = src->value.as_tbl->ncols;
                ari_array_init_set(ctr->items, src->value.as_tbl->items);
                break;
            }
            case ARI_TYPE_EXECSET:
            {
                ari_execset_t *ctr    = M_MEMORY_ALLOC(ari_execset_t);
                lit->value.as_execset = ctr;
                ari_init_copy(&(ctr->nonce), &(src->value.as_execset->nonce));
                ari_list_init_set(ctr->targets, src->value.as_execset->targets);
                break;
            }
            case ARI_TYPE_RPTSET:
            {
                ari_rptset_t *ctr    = M_MEMORY_ALLOC(ari_rptset_t);
                lit->value.as_rptset = ctr;
                ari_init_copy(&(ctr->nonce), &(src->value.as_rptset->nonce));
                ari_init_copy(&(ctr->reftime), &(src->value.as_rptset->reftime));
                ari_report_list_init_set(ctr->reports, src->value.as_rptset->reports);
                break;
            }
            default:
                break;
        }
    }

    return 0;
}
