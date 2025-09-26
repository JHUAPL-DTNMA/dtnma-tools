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
#include "lit.h"
#include "containers.h"
#include "cace/util/defs.h"
#include <inttypes.h>

const time_t cace_ari_dtn_epoch = CACE_ARI_DTN_EPOCH;

const struct timespec cace_ari_dtn_epoch_timespec = { .tv_sec = CACE_ARI_DTN_EPOCH, .tv_nsec = 0 };

int cace_ari_lit_deinit(cace_ari_lit_t *obj)
{
    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_TSTR:
        case CACE_ARI_PRIM_BSTR:
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
            case CACE_ARI_TYPE_AC:
                CHKERR1(obj->value.as_ac);
                cace_ari_ac_deinit(obj->value.as_ac);
                CACE_FREE(obj->value.as_ac);
                break;
            case CACE_ARI_TYPE_AM:
                CHKERR1(obj->value.as_am);
                cace_ari_am_deinit(obj->value.as_am);
                CACE_FREE(obj->value.as_am);
                break;
            case CACE_ARI_TYPE_TBL:
                CHKERR1(obj->value.as_tbl);
                cace_ari_tbl_deinit(obj->value.as_tbl);
                CACE_FREE(obj->value.as_tbl);
                break;
            case CACE_ARI_TYPE_EXECSET:
                CHKERR1(obj->value.as_execset);
                cace_ari_execset_deinit(obj->value.as_execset);
                CACE_FREE(obj->value.as_execset);
                break;
            case CACE_ARI_TYPE_RPTSET:
                CHKERR1(obj->value.as_rptset);
                cace_ari_rptset_deinit(obj->value.as_rptset);
                CACE_FREE(obj->value.as_rptset);
                break;
            default:
                // do nothing
                break;
        }
    }
    return 0;
}

int cace_ari_lit_copy(cace_ari_lit_t *lit, const cace_ari_lit_t *src)
{
    lit->has_ari_type = src->has_ari_type;
    lit->ari_type     = src->ari_type;
    lit->prim_type    = src->prim_type;

    switch (src->prim_type)
    {
        case CACE_ARI_PRIM_TSTR:
        case CACE_ARI_PRIM_BSTR:
            if (cace_data_init_set(&(lit->value.as_data), &(src->value.as_data)))
            {
                return 2;
            }
            break;
        case CACE_ARI_PRIM_OTHER:
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
            case CACE_ARI_TYPE_TP:
            case CACE_ARI_TYPE_TD:
                lit->value = src->value;
                break;
            case CACE_ARI_TYPE_AC:
            {
                cace_ari_ac_t *ctr = CACE_MALLOC(sizeof(cace_ari_ac_t));
                cace_ari_ac_init(ctr);
                cace_ari_list_set(ctr->items, src->value.as_ac->items);
                lit->value.as_ac = ctr;
                break;
            }
            case CACE_ARI_TYPE_AM:
            {
                cace_ari_am_t *ctr = CACE_MALLOC(sizeof(cace_ari_am_t));
                cace_ari_am_init(ctr);
                cace_ari_tree_set(ctr->items, src->value.as_am->items);
                lit->value.as_am = ctr;
                break;
            }
            case CACE_ARI_TYPE_TBL:
            {
                cace_ari_tbl_t *ctr = CACE_MALLOC(sizeof(cace_ari_tbl_t));
                cace_ari_tbl_init(ctr);
                cace_ari_tbl_reset(ctr, src->value.as_tbl->ncols, 0);
                cace_ari_array_set(ctr->items, src->value.as_tbl->items);
                lit->value.as_tbl = ctr;
                break;
            }
            case CACE_ARI_TYPE_EXECSET:
            {
                cace_ari_execset_t *ctr = CACE_MALLOC(sizeof(cace_ari_execset_t));
                cace_ari_execset_init(ctr);
                cace_ari_set_copy(&(ctr->nonce), &(src->value.as_execset->nonce));
                cace_ari_list_set(ctr->targets, src->value.as_execset->targets);
                lit->value.as_execset = ctr;
                break;
            }
            case CACE_ARI_TYPE_RPTSET:
            {
                cace_ari_rptset_t *ctr = CACE_MALLOC(sizeof(cace_ari_rptset_t));
                cace_ari_rptset_init(ctr);
                cace_ari_set_copy(&(ctr->nonce), &(src->value.as_rptset->nonce));
                cace_ari_set_copy(&(ctr->reftime), &(src->value.as_rptset->reftime));
                cace_ari_report_list_set(ctr->reports, src->value.as_rptset->reports);
                lit->value.as_rptset = ctr;
                break;
            }
            default:
                break;
        }
    }

    return 0;
}
