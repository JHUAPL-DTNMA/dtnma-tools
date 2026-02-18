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
#include "alarms.h"
#include "agent.h"
#include "eval.h"
#include "cace/ari/time_util.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void refda_alarms_history_item_init(refda_alarms_history_item_t *obj)
{
    cace_ari_init(&obj->timestamp);
    obj->severity = REFDA_ALARMS_SEVERITY_CLEARED;
}

void refda_alarms_history_item_deinit(refda_alarms_history_item_t *obj)
{
    cace_ari_deinit(&obj->timestamp);
}

void refda_alarms_entry_init(refda_alarms_entry_t *obj)
{
    refda_amm_ident_base_init(&obj->resource);
    refda_amm_ident_base_init(&obj->category);
    obj->severity = REFDA_ALARMS_SEVERITY_CLEARED;
    cace_ari_init(&obj->created_at);
    cace_ari_init(&obj->updated_at);
    refda_alarms_history_list_init(obj->history);
    obj->mgr_state = REFDA_ALARMS_MGR_STATE_NONE;
    obj->mgr_ident = CACE_ARI_INIT_NULL;
    obj->mgr_time  = CACE_ARI_INIT_NULL;
}

void refda_alarms_entry_deinit(refda_alarms_entry_t *obj)
{
    cace_ari_deinit(&obj->mgr_time);
    cace_ari_deinit(&obj->mgr_ident);
    refda_alarms_history_list_clear(obj->history);
    cace_ari_deinit(&obj->updated_at);
    cace_ari_deinit(&obj->created_at);
    refda_amm_ident_base_deinit(&obj->category);
    refda_amm_ident_base_deinit(&obj->resource);
}

int refda_alarms_entry_key_cmp(const refda_alarms_entry_key_t *left, const refda_alarms_entry_key_t *right)
{
    int res = M_CMP_DEFAULT(left->resource, right->resource);
    if (res)
    {
        return res;
    }
    res = M_CMP_DEFAULT(left->category, right->category);
    return res;
}

void refda_alarms_init(refda_alarms_t *obj)
{
    refda_alarms_entry_list_init(obj->alarm_list);
    refda_alarms_entry_index_init(obj->alarm_index);
    pthread_mutex_init(&(obj->alarm_mutex), NULL);
}

void refda_alarms_deinit(refda_alarms_t *obj)
{
    pthread_mutex_destroy(&(obj->alarm_mutex));
    refda_alarms_entry_index_clear(obj->alarm_index);
    refda_alarms_entry_list_clear(obj->alarm_list);
}

void refda_alarms_set_refs(refda_agent_t *agent, const cace_ari_t *resource, const cace_ari_t *category,
                           refda_alarms_severity_t severity)
{
    refda_amm_ident_base_t res_ref, cat_ref;
    refda_amm_ident_base_init(&res_ref);
    refda_amm_ident_base_init(&cat_ref);

    // validate lookup fields only
    REFDA_AGENT_LOCK(agent, );
    refda_amm_ident_base_populate(&res_ref, resource, &agent->objs);
    if (!cace_ari_is_null(category))
    {
        refda_amm_ident_base_populate(&cat_ref, category, &agent->objs);
    }
    else
    {
        cace_ari_set_null(&cat_ref.name);
    }
    REFDA_AGENT_UNLOCK(agent, );

    if (!res_ref.ident)
    {
        CACE_LOG_ERR("invalid resource reference");
        refda_amm_ident_base_deinit(&res_ref);
        refda_amm_ident_base_deinit(&cat_ref);
        return;
    }
    if (!cace_ari_is_null(&cat_ref.name) && !cat_ref.ident)
    {
        CACE_LOG_ERR("invalid category reference");
        refda_amm_ident_base_deinit(&res_ref);
        refda_amm_ident_base_deinit(&cat_ref);
        return;
    }

    if (pthread_mutex_lock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return;
    }

    refda_alarms_entry_key_t entry_key = {
        .resource = res_ref.ident,
        .category = cat_ref.ident,
    };

    refda_alarms_entry_t  *entry = NULL;
    refda_alarms_entry_t **entry_found;
    entry_found = refda_alarms_entry_index_get(agent->alarms.alarm_index, entry_key);
    if (entry_found)
    {
        entry = *entry_found;
        // done with key
        refda_amm_ident_base_deinit(&res_ref);
        refda_amm_ident_base_deinit(&cat_ref);
    }
    else
    {
        refda_alarms_entry_ptr_t *entry_ptr = *refda_alarms_entry_list_push_new(agent->alarms.alarm_list);
        // new entry with default non-key fields
        entry = refda_alarms_entry_ptr_ref(entry_ptr);
        refda_amm_ident_base_set_move(&entry->resource, &res_ref);
        refda_amm_ident_base_set_move(&entry->category, &cat_ref);

        // key on new instance
        refda_alarms_entry_index_set_at(agent->alarms.alarm_index, entry_key, entry);
    }

    // update main state
    entry->severity = severity;
    cace_get_system_time(&entry->updated_at);
    if (cace_ari_is_undefined(&entry->created_at))
    {
        // ensure first timestamp matches exactly
        cace_ari_set_copy(&entry->created_at, &entry->updated_at);
    }

    { // record history at the front
        refda_alarms_history_item_t *hist_item = refda_alarms_history_list_push_front_new(entry->history);
        hist_item->severity                    = entry->severity;
        cace_ari_set_copy(&hist_item->timestamp, &entry->updated_at);
    }

    if (pthread_mutex_unlock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
    }
}

/// Table column index
enum alarm_table_col_e
{
    ALARM_TABLE_COL_RESOURCE,
    ALARM_TABLE_COL_CATEGORY,
    ALARM_TABLE_COL_SEVERITY,
    ALARM_TABLE_COL_CREATED_AT,
    ALARM_TABLE_COL_UPDATED_AT,
    ALARM_TABLE_COL_HISTORY,
    ALARM_TABLE_COL_MGR_STATE,
    ALARM_TABLE_COL_MGR_IDENT,
    ALARM_TABLE_COL_MGR_TIME,
};

/**
 * Translation helper function to substitute any LABEL value in the expression with
 * corresponding data from an ./edd/alarm-list entry.
 *
 * Assumes the LABEL contains a name or index of the column which will substitute data.
 */
static cace_ari_translate_result_t alarm_list_filter_sub_label(cace_ari_t *out, const cace_ari_t *in,
                                                               const cace_ari_translate_ctx_t *ctx)
{
    if (cace_ari_is_lit_typed(in, CACE_ARI_TYPE_LABEL))
    {
        const refda_alarms_entry_t *entry = (refda_alarms_entry_t *)ctx->user_data;

        const char  *as_text = NULL;
        cace_ari_int as_int;
        if ((as_text = cace_ari_cget_tstr_cstr(in)))
        {
            if (0 == strcasecmp("resource", as_text))
            {
                as_int = ALARM_TABLE_COL_RESOURCE;
            }
            else if (0 == strcasecmp("category", as_text))
            {
                as_int = ALARM_TABLE_COL_CATEGORY;
            }
            else if (0 == strcasecmp("severity", as_text))
            {
                as_int = ALARM_TABLE_COL_SEVERITY;
            }
            else if (0 == strcasecmp("time-created", as_text))
            {
                as_int = ALARM_TABLE_COL_CREATED_AT;
            }
            else if (0 == strcasecmp("time-updated", as_text))
            {
                as_int = ALARM_TABLE_COL_UPDATED_AT;
            }
            else
            {
                CACE_LOG_ERR("invalid alarm column label: %s", as_text);
                return CACE_ARI_TRANSLATE_FAILURE;
            }
        }
        else if (!cace_ari_get_int(in, &as_int))
        {
            // nothing else to do here
        }
        else
        {
            CACE_LOG_ERR("invalid LABEL primitive type");
            return CACE_ARI_TRANSLATE_FAILURE;
        }

        switch (as_int)
        {
            case ALARM_TABLE_COL_RESOURCE:
                cace_ari_set_copy(out, &(entry->resource.name));
                break;
            case ALARM_TABLE_COL_CATEGORY:
                cace_ari_set_copy(out, &(entry->category.name));
                break;
            case ALARM_TABLE_COL_SEVERITY:
                cace_ari_set_int(out, entry->severity);
                break;
            case ALARM_TABLE_COL_CREATED_AT:
                cace_ari_set_copy(out, &(entry->created_at));
                break;
            case ALARM_TABLE_COL_UPDATED_AT:
                cace_ari_set_copy(out, &(entry->updated_at));
                break;
            case ALARM_TABLE_COL_MGR_STATE:
                cace_ari_set_uint(out, entry->mgr_state);
                break;
            case ALARM_TABLE_COL_MGR_IDENT:
                cace_ari_set_copy(out, &entry->mgr_ident);
                break;
            case ALARM_TABLE_COL_MGR_TIME:
                cace_ari_set_copy(out, &entry->mgr_time);
                break;
            default:
                return CACE_ARI_TRANSLATE_FAILURE;
        }
        return CACE_ARI_TRANSLATE_FINAL;
    }
    return CACE_ARI_TRANSLATE_DEFAULT;
}

int refda_alarms_get_table(refda_runctx_t *runctx, cace_ari_t *out)
{
    refda_agent_t *agent = runctx->agent;
    if (pthread_mutex_lock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return 2;
    }

    cace_ari_tbl_t *table = cace_ari_set_tbl(out, NULL);
    cace_ari_tbl_reset(table, 9, 0);

    // table is naturally sorted
    refda_alarms_entry_list_it_t entry_it;
    for (refda_alarms_entry_list_it(entry_it, agent->alarms.alarm_list); !refda_alarms_entry_list_end_p(entry_it);
         refda_alarms_entry_list_next(entry_it))
    {
        const refda_alarms_entry_t *entry = refda_alarms_entry_ptr_cref(*refda_alarms_entry_list_cref(entry_it));

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, table->ncols);

        cace_ari_set_copy(cace_ari_array_get(row, ALARM_TABLE_COL_RESOURCE), &entry->resource.name);
        cace_ari_set_copy(cace_ari_array_get(row, ALARM_TABLE_COL_CATEGORY), &entry->category.name);
        cace_ari_set_uint(cace_ari_array_get(row, ALARM_TABLE_COL_SEVERITY), entry->severity);
        cace_ari_set_copy(cace_ari_array_get(row, ALARM_TABLE_COL_CREATED_AT), &entry->created_at);
        cace_ari_set_copy(cace_ari_array_get(row, ALARM_TABLE_COL_UPDATED_AT), &entry->updated_at);
        {
            cace_ari_tbl_t *hist_tbl = cace_ari_set_tbl(cace_ari_array_get(row, ALARM_TABLE_COL_HISTORY), NULL);
            cace_ari_tbl_reset(hist_tbl, 2, 0);

            refda_alarms_history_list_it_t hist_it;
            for (refda_alarms_history_list_it(hist_it, entry->history); !refda_alarms_history_list_end_p(hist_it);
                 refda_alarms_history_list_next(hist_it))
            {
                const refda_alarms_history_item_t *hist_item = refda_alarms_history_list_cref(hist_it);

                cace_ari_array_t hist_row;
                cace_ari_array_init(hist_row);
                cace_ari_array_resize(hist_row, hist_tbl->ncols);

                cace_ari_set_copy(cace_ari_array_get(hist_row, 0), &hist_item->timestamp);
                cace_ari_set_uint(cace_ari_array_get(hist_row, 1), hist_item->severity);

                cace_ari_tbl_move_row_array(hist_tbl, hist_row);
            }
        }
        cace_ari_set_uint(cace_ari_array_get(row, ALARM_TABLE_COL_MGR_STATE), entry->mgr_state);
        cace_ari_set_copy(cace_ari_array_get(row, ALARM_TABLE_COL_MGR_IDENT), &entry->mgr_ident);
        cace_ari_set_copy(cace_ari_array_get(row, ALARM_TABLE_COL_MGR_TIME), &entry->mgr_time);

        // append the row
        cace_ari_tbl_move_row_array(table, row);
    }

    if (pthread_mutex_unlock(&(agent->alarms.alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
        return 2;
    }
    return 0;
}

size_t refda_alarms_purge(refda_runctx_t *runctx, const cace_ari_t *filter)
{
    CHKRET(runctx, 0);
    CHKRET(filter, 0);

    refda_alarms_t *alarms = &(runctx->agent->alarms);
    if (pthread_mutex_lock(&(alarms->alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return 0;
    }

    size_t affected = 0;

    cace_ari_translator_t translator = { .map_ari = alarm_list_filter_sub_label };

    refda_alarms_entry_list_it_t entry_it;
    for (refda_alarms_entry_list_it(entry_it, alarms->alarm_list); !refda_alarms_entry_list_end_p(entry_it);)
    {
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*refda_alarms_entry_list_cref(entry_it));

        // Substitute entry values for LABEL items within filter EXPR
        cace_ari_t expr = CACE_ARI_INIT_UNDEFINED;
        {
            int res = cace_ari_translate(&expr, filter, &translator, entry);
            if (res)
            {
                CACE_LOG_ERR("Unable to translate filter, error %d", res);
                cace_ari_deinit(&expr); // No longer needed at this point
                refda_alarms_entry_list_next(entry_it);
                continue;
            }
        }

        // Evaluate the filter EXPR
        cace_ari_t eval_result = CACE_ARI_INIT_UNDEFINED;
        int        res         = refda_eval_target(runctx, &eval_result, &expr);
        cace_ari_deinit(&expr); // No longer needed at this point
        if (res)
        {
            CACE_LOG_ERR("failed to evaluate condition, error %d", res);
            cace_ari_deinit(&eval_result);
            refda_alarms_entry_list_next(entry_it);
            continue;
        }

        // True result indicates entry is purged
        if (cace_amm_ari_is_truthy(&eval_result))
        {
            refda_alarms_entry_key_t entry_key = {
                .resource = entry->resource.ident,
                .category = entry->category.ident,
            };
            refda_alarms_entry_index_erase(alarms->alarm_index, entry_key);

            refda_alarms_entry_list_remove(alarms->alarm_list, entry_it);
            affected += 1;
        }
        else
        {
            // keep and check next
            refda_alarms_entry_list_next(entry_it);
        }
    }

    if (pthread_mutex_unlock(&(alarms->alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
    }
    return affected;
}

size_t refda_alarms_compress(refda_runctx_t *runctx, const cace_ari_t *filter)
{
    CHKRET(runctx, 0);
    CHKRET(filter, 0);

    refda_alarms_t *alarms = &(runctx->agent->alarms);
    if (pthread_mutex_lock(&(alarms->alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return 0;
    }

    size_t affected = 0;

    cace_ari_translator_t translator = { .map_ari = alarm_list_filter_sub_label };

    refda_alarms_entry_list_it_t entry_it;
    for (refda_alarms_entry_list_it(entry_it, alarms->alarm_list); !refda_alarms_entry_list_end_p(entry_it);
         refda_alarms_entry_list_next(entry_it))
    {
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*refda_alarms_entry_list_cref(entry_it));

        // Substitute entry values for LABEL items within filter EXPR
        cace_ari_t expr = CACE_ARI_INIT_UNDEFINED;
        {
            int res = cace_ari_translate(&expr, filter, &translator, entry);
            if (res)
            {
                CACE_LOG_ERR("Unable to translate filter, error %d", res);
                cace_ari_deinit(&expr); // No longer needed at this point
                continue;
            }
        }

        // Evaluate the filter EXPR
        cace_ari_t eval_result = CACE_ARI_INIT_UNDEFINED;
        int        res         = refda_eval_target(runctx, &eval_result, &expr);
        cace_ari_deinit(&expr); // No longer needed at this point
        if (res)
        {
            CACE_LOG_ERR("failed to evaluate condition, error %d", res);
            cace_ari_deinit(&eval_result);
            continue;
        }

        // True result indicates entry is commpressed
        if (cace_amm_ari_is_truthy(&eval_result))
        {
            size_t hist_size = refda_alarms_history_list_size(entry->history);
            if (hist_size > 1)
            {
                while (--hist_size > 0)
                {
                    refda_alarms_history_list_pop_back(NULL, entry->history);
                }
                affected += 1;
            }
        }
    }

    if (pthread_mutex_unlock(&(alarms->alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
    }
    return affected;
}

size_t refda_alarms_mgr_state(refda_runctx_t *runctx, const cace_ari_t *filter, refda_alarms_mgr_state_t state)
{
    CHKRET(runctx, 0);
    CHKRET(filter, 0);

    refda_alarms_t *alarms = &(runctx->agent->alarms);
    if (pthread_mutex_lock(&(alarms->alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to lock alarm_mutex");
        return 0;
    }

    int    res;
    size_t affected = 0;

    cace_ari_translator_t translator = { .map_ari = alarm_list_filter_sub_label };

    refda_alarms_entry_list_it_t entry_it;
    for (refda_alarms_entry_list_it(entry_it, alarms->alarm_list); !refda_alarms_entry_list_end_p(entry_it);
         refda_alarms_entry_list_next(entry_it))
    {
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*refda_alarms_entry_list_cref(entry_it));

        // Substitute entry values for LABEL items within filter EXPR
        cace_ari_t expr = CACE_ARI_INIT_UNDEFINED;
        {
            res = cace_ari_translate(&expr, filter, &translator, entry);
            if (res)
            {
                CACE_LOG_ERR("Unable to translate filter, error %d", res);
                cace_ari_deinit(&expr); // No longer needed at this point
                continue;
            }
        }

        // Evaluate the filter EXPR
        cace_ari_t eval_result = CACE_ARI_INIT_UNDEFINED;
        res                    = refda_eval_target(runctx, &eval_result, &expr);
        cace_ari_deinit(&expr); // No longer needed at this point
        if (res)
        {
            CACE_LOG_ERR("failed to evaluate condition, error %d", res);
            cace_ari_deinit(&eval_result);
            continue;
        }

        // True result indicates entry is commpressed
        if (cace_amm_ari_is_truthy(&eval_result))
        {
            // special logic when setting to shelved
            if (state == REFDA_ALARMS_MGR_STATE_SHELVED)
            {
                // TODO shelf entry
            }
            else if (entry->mgr_state != state)
            {
                entry->mgr_state = state;
                cace_ari_set_copy(&entry->mgr_ident, &runctx->mgr_ident);
                cace_get_system_time(&entry->mgr_time);
                affected += 1;
            }
        }
    }

    if (pthread_mutex_unlock(&(alarms->alarm_mutex)))
    {
        CACE_LOG_CRIT("failed to unlock alarm_mutex");
    }
    return affected;
}
