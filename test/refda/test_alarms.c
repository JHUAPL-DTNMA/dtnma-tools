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
 * Test the internal API for alarm bookkeeping, separate from the ADM
 * exposing some of these data and behaviors.
 */
#include "util/ari.h"
#include "util/agent.h"
#include "util/runctx.h"
#include <refda/register.h>
#include <refda/binding.h>
#include <refda/valprod.h>
#include <refda/exec_proc.h>
#include <refda/adm/ietf.h>
#include <refda/adm/ietf_amm_base.h>
#include <refda/adm/ietf_alarms.h>
#include <refda/amm/ident.h>
#include <refda/amm/const.h>
#include <refda/amm/var.h>
#include <refda/amm/edd.h>
#include <cace/amm/semtype.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

// Agent context for testing
static refda_agent_t agent;
// Example ADM
static cace_amm_obj_ns_t *ex_adm = NULL;
/// Example ODM
static cace_amm_obj_ns_t *ex_odm = NULL;

// Org number for #ex_adm
#define EXAMPLE_ORG_ENUM 65535
// Model number for #ex_adm
#define EXAMPLE_ADM_ENUM 10
// Object number for ./ident/res1 in ADM
#define EXAMPLE_IDENT_RES1_ENUM 1
// Object number for ./ident/cat2 in ADM
#define EXAMPLE_IDENT_CAT2_ENUM 2
// Model number for #ex_odm
#define EXAMPLE_ODM_ENUM -5

void suiteSetUp(void)
{
    cace_openlog();

    refda_agent_init(&agent);
    test_util_agent_crit_adms(&agent);

    ex_adm = cace_amm_obj_store_add_ns(&(agent.objs), cace_amm_idseg_ref_withenum("example", EXAMPLE_ORG_ENUM),
                                       cace_amm_idseg_ref_withenum("adm", EXAMPLE_ADM_ENUM), "2025-01-03");
    assert(NULL != ex_adm);
    {
        cace_amm_obj_desc_t *obj;
        { // For ./ident/res1
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // ari://ietf/alarms/IDENT/resource
                cace_ari_set_objref_path_intid(&(base->name), REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_ALARMS_ENUM_ADM,
                                               CACE_ARI_TYPE_IDENT, REFDA_ADM_IETF_ALARMS_ENUM_OBJID_IDENT_RESOURCE);
            }

            obj = refda_register_ident(ex_adm, cace_amm_idseg_ref_withenum("res1", EXAMPLE_IDENT_RES1_ENUM), objdata);
            // no parameters
        }
        assert(NULL != obj);
        { // For ./ident/cat2
            refda_amm_ident_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ident_desc_t));
            refda_amm_ident_desc_init(objdata);
            // IDENT bases:
            {
                refda_amm_ident_base_t *base = refda_amm_ident_base_list_push_new(objdata->bases);
                // ari://ietf/alarms/IDENT/category
                cace_ari_set_objref_path_intid(&(base->name), REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_ALARMS_ENUM_ADM,
                                               CACE_ARI_TYPE_IDENT, REFDA_ADM_IETF_ALARMS_ENUM_OBJID_IDENT_CATEGORY);
            }

            obj = refda_register_ident(ex_adm, cace_amm_idseg_ref_withenum("cat2", EXAMPLE_IDENT_CAT2_ENUM), objdata);
            // no parameters
        }
        assert(NULL != obj);
    }

    ex_odm = cace_amm_obj_store_add_ns(&(agent.objs), cace_amm_idseg_ref_withenum("example", EXAMPLE_ORG_ENUM),
                                       cace_amm_idseg_ref_withenum("!odm", EXAMPLE_ODM_ENUM), "2025-01-03");
    assert(NULL != ex_adm);

    int res = refda_agent_bindrefs(&agent);
    assert(0 == res);
}

int suiteTearDown(int failures)
{
    ex_odm = NULL;
    ex_adm = NULL;
    refda_agent_deinit(&agent);

    cace_closelog();
    return failures;
}

void setUp(void)
{
    refda_alarms_entry_index_reset(agent.alarms.alarm_index);
    refda_alarms_entry_list_reset(agent.alarms.alarm_list);
}

void test_refda_alarms_set_state_no_category(void)
{
    // no mutex use with single thread
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    cace_ari_t res_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&res_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_RES1_ENUM);

    cace_ari_t cat_ref = CACE_ARI_INIT_NULL;

    // initial state
    refda_alarms_set_refs(&agent, &res_ref, &cat_ref, REFDA_ALARMS_SEVERITY_MINOR);
    TEST_ASSERT_EQUAL_size_t(1, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(1, refda_alarms_entry_index_size(agent.alarms.alarm_index));
    {
        refda_alarms_entry_ptr_t *entry_ptr = *refda_alarms_entry_list_front(agent.alarms.alarm_list);
        refda_alarms_entry_t     *entry     = refda_alarms_entry_ptr_ref(entry_ptr);
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_TRUE(cace_ari_equal(&res_ref, &entry->resource.name));
        TEST_ASSERT_TRUE(cace_ari_equal(&cat_ref, &entry->category.name));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_SEVERITY_MINOR, entry->severity);
        TEST_ASSERT_TRUE(cace_ari_not_undefined(&entry->created_at));
        TEST_ASSERT_TRUE(cace_ari_not_undefined(&entry->updated_at));
        TEST_ASSERT_TRUE(cace_ari_equal(&entry->created_at, &entry->updated_at));
        TEST_ASSERT_EQUAL_size_t(1, refda_alarms_history_list_size(entry->history));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_MGR_STATE_NONE, entry->mgr_state);
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_ident));
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_time));
    }

    // update state
    refda_alarms_set_refs(&agent, &res_ref, &cat_ref, REFDA_ALARMS_SEVERITY_WARNING);
    TEST_ASSERT_EQUAL_size_t(1, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(1, refda_alarms_entry_index_size(agent.alarms.alarm_index));
    {
        refda_alarms_entry_ptr_t **entry_ptr = refda_alarms_entry_list_front(agent.alarms.alarm_list);
        TEST_ASSERT_NOT_NULL(entry_ptr);
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*entry_ptr);
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_TRUE(cace_ari_equal(&res_ref, &entry->resource.name));
        TEST_ASSERT_TRUE(cace_ari_equal(&cat_ref, &entry->category.name));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_SEVERITY_WARNING, entry->severity);
        TEST_ASSERT_TRUE(cace_ari_not_undefined(&entry->created_at));
        TEST_ASSERT_TRUE(cace_ari_not_undefined(&entry->updated_at));
        TEST_ASSERT_TRUE(!cace_ari_equal(&entry->created_at, &entry->updated_at));
        TEST_ASSERT_EQUAL_size_t(2, refda_alarms_history_list_size(entry->history));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_MGR_STATE_NONE, entry->mgr_state);
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_ident));
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_time));
    }

    cace_ari_deinit(&cat_ref);
    cace_ari_deinit(&res_ref);
}

void test_refda_alarms_purge_all(void)
{
    // no mutex use with single thread
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    cace_ari_t null_ref = CACE_ARI_INIT_NULL;
    cace_ari_t res1_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&res1_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_RES1_ENUM);
    cace_ari_t cat2_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&cat2_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_CAT2_ENUM);

    // initial entries: one with 2-item history, one with 1-item history
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_MINOR);
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_WARNING);
    refda_alarms_set_refs(&agent, &res1_ref, &null_ref, REFDA_ALARMS_SEVERITY_MINOR);
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    {
        cace_ari_t filter = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ac_t *filter_ac = cace_ari_set_ac(&filter, NULL);

            // purge all rows
            cace_ari_t *expr_item = cace_ari_list_push_back_new(filter_ac->items);
            cace_ari_set_bool(expr_item, true);
        }

        refda_runctx_t runctx;
        TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));
        size_t affected = refda_alarms_purge(&runctx, &filter);
        refda_runctx_deinit(&runctx);
        cace_ari_deinit(&filter);
        TEST_ASSERT_EQUAL_size_t(2, affected);
    }
    // all gone
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    cace_ari_deinit(&cat2_ref);
    cace_ari_deinit(&res1_ref);
    cace_ari_deinit(&null_ref);
}

void test_refda_alarms_purge_warning(void)
{
    // no mutex use with single thread
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    cace_ari_t null_ref = CACE_ARI_INIT_NULL;
    cace_ari_t res1_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&res1_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_RES1_ENUM);
    cace_ari_t cat2_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&cat2_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_CAT2_ENUM);

    // initial entries: one with 2-item history, one with 1-item history
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_MINOR);
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_WARNING);
    refda_alarms_set_refs(&agent, &res1_ref, &null_ref, REFDA_ALARMS_SEVERITY_MINOR);
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    {
        cace_ari_t filter = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ac_t *filter_ac = cace_ari_set_ac(&filter, NULL);

            // ari:/ac/(/label/severity,2,//ietf/dtnma-agent/oper/eq)
            cace_ari_set_label_tstr(cace_ari_list_push_back_new(filter_ac->items), "severity", false);
            cace_ari_set_int(cace_ari_list_push_back_new(filter_ac->items), REFDA_ALARMS_SEVERITY_WARNING);
            cace_ari_set_objref_path_intid(cace_ari_list_push_back_new(filter_ac->items), REFDA_ADM_IETF_ENUM,
                                           REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_OPER,
                                           REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_EQ);
        }

        refda_runctx_t runctx;
        TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));
        size_t affected = refda_alarms_purge(&runctx, &filter);
        refda_runctx_deinit(&runctx);
        cace_ari_deinit(&filter);
        TEST_ASSERT_EQUAL_size_t(1, affected);
    }
    // one warning entry gone
    TEST_ASSERT_EQUAL_size_t(1, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(1, refda_alarms_entry_index_size(agent.alarms.alarm_index));
    {
        refda_alarms_entry_ptr_t **entry_ptr = refda_alarms_entry_list_front(agent.alarms.alarm_list);
        TEST_ASSERT_NOT_NULL(entry_ptr);
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*entry_ptr);
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_TRUE(cace_ari_equal(&res1_ref, &entry->resource.name));
        TEST_ASSERT_TRUE(cace_ari_equal(&null_ref, &entry->category.name));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_SEVERITY_MINOR, entry->severity);
    }

    cace_ari_deinit(&cat2_ref);
    cace_ari_deinit(&res1_ref);
    cace_ari_deinit(&null_ref);
}

void test_refda_alarms_compress_all(void)
{
    // no mutex use with single thread
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    cace_ari_t null_ref = CACE_ARI_INIT_NULL;
    cace_ari_t res1_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&res1_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_RES1_ENUM);
    cace_ari_t cat2_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&cat2_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_CAT2_ENUM);

    // initial entries: one with 2-item history, one with 1-item history
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_MINOR);
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_WARNING);
    refda_alarms_set_refs(&agent, &res1_ref, &null_ref, REFDA_ALARMS_SEVERITY_MINOR);
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    {
        cace_ari_t filter = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ac_t *filter_ac = cace_ari_set_ac(&filter, NULL);
            // compress all rows
            cace_ari_t *expr_item = cace_ari_list_push_back_new(filter_ac->items);
            cace_ari_set_bool(expr_item, true);
        }

        refda_runctx_t runctx;
        TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));
        size_t affected = refda_alarms_compress(&runctx, &filter);
        refda_runctx_deinit(&runctx);
        cace_ari_deinit(&filter);
        TEST_ASSERT_EQUAL_INT(1, affected);
    }
    // one history compressed
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_index_size(agent.alarms.alarm_index));
    {
        refda_alarms_entry_ptr_t **entry_ptr = refda_alarms_entry_list_front(agent.alarms.alarm_list);
        TEST_ASSERT_NOT_NULL(entry_ptr);
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*entry_ptr);
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_TRUE(cace_ari_equal(&res1_ref, &entry->resource.name));
        TEST_ASSERT_TRUE(cace_ari_equal(&cat2_ref, &entry->category.name));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_SEVERITY_WARNING, entry->severity);
        TEST_ASSERT_EQUAL_size_t(1, refda_alarms_history_list_size(entry->history));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_MGR_STATE_NONE, entry->mgr_state);
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_ident));
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_time));
    }

    cace_ari_deinit(&cat2_ref);
    cace_ari_deinit(&res1_ref);
    cace_ari_deinit(&null_ref);
}

void test_refda_alarms_mgr_state_warning(void)
{
    // no mutex use with single thread
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    cace_ari_t null_ref = CACE_ARI_INIT_NULL;
    cace_ari_t res1_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&res1_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_RES1_ENUM);
    cace_ari_t cat2_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&cat2_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_CAT2_ENUM);

    // initial entries: one with 2-item history, one with 1-item history
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_MINOR);
    refda_alarms_set_refs(&agent, &res1_ref, &cat2_ref, REFDA_ALARMS_SEVERITY_WARNING);
    refda_alarms_set_refs(&agent, &res1_ref, &null_ref, REFDA_ALARMS_SEVERITY_MINOR);
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_index_size(agent.alarms.alarm_index));

    {
        cace_ari_t filter = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ac_t *filter_ac = cace_ari_set_ac(&filter, NULL);
            // ari:/ac/(/label/severity,2,//ietf/dtnma-agent/oper/eq)
            cace_ari_set_label_tstr(cace_ari_list_push_back_new(filter_ac->items), "severity", false);
            cace_ari_set_int(cace_ari_list_push_back_new(filter_ac->items), REFDA_ALARMS_SEVERITY_WARNING);
            cace_ari_set_objref_path_intid(cace_ari_list_push_back_new(filter_ac->items), REFDA_ADM_IETF_ENUM,
                                           REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_OPER,
                                           REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_EQ);
        }

        refda_runctx_t runctx;
        TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));
        size_t affected = refda_alarms_mgr_state(&runctx, &filter, REFDA_ALARMS_MGR_STATE_ACK);
        refda_runctx_deinit(&runctx);
        cace_ari_deinit(&filter);
        TEST_ASSERT_EQUAL_INT(1, affected);
    }
    // one history compressed
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(2, refda_alarms_entry_index_size(agent.alarms.alarm_index));
    refda_alarms_entry_list_it_t alarm_it;
    refda_alarms_entry_list_it(alarm_it, agent.alarms.alarm_list);
    {
        refda_alarms_entry_ptr_t **entry_ptr = refda_alarms_entry_list_ref(alarm_it);
        TEST_ASSERT_NOT_NULL(entry_ptr);
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*entry_ptr);
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_TRUE(cace_ari_equal(&res1_ref, &entry->resource.name));
        TEST_ASSERT_TRUE(cace_ari_equal(&cat2_ref, &entry->category.name));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_SEVERITY_WARNING, entry->severity);
        TEST_ASSERT_EQUAL_size_t(2, refda_alarms_history_list_size(entry->history));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_MGR_STATE_ACK, entry->mgr_state);
        TEST_ASSERT_TRUE(!cace_ari_is_null(&entry->mgr_ident));
        TEST_ASSERT_TRUE(!cace_ari_is_null(&entry->mgr_time));
    }
    refda_alarms_entry_list_next(alarm_it);
    {
        refda_alarms_entry_ptr_t **entry_ptr = refda_alarms_entry_list_ref(alarm_it);
        TEST_ASSERT_NOT_NULL(entry_ptr);
        refda_alarms_entry_t *entry = refda_alarms_entry_ptr_ref(*entry_ptr);
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_TRUE(cace_ari_equal(&res1_ref, &entry->resource.name));
        TEST_ASSERT_TRUE(cace_ari_equal(&null_ref, &entry->category.name));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_SEVERITY_MINOR, entry->severity);
        TEST_ASSERT_EQUAL_size_t(1, refda_alarms_history_list_size(entry->history));
        TEST_ASSERT_EQUAL_INT(REFDA_ALARMS_MGR_STATE_NONE, entry->mgr_state);
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_ident));
        TEST_ASSERT_TRUE(cace_ari_is_null(&entry->mgr_time));
    }

    cace_ari_deinit(&cat2_ref);
    cace_ari_deinit(&res1_ref);
    cace_ari_deinit(&null_ref);
}
