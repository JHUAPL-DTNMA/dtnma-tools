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
 * Test the external ADM for alarm bookkeeping.
 * This requires some internal API access to manipulate the Agent state.
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
#include <refda/adm/ietf_amm_semtype.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/adm/ietf_dtnma_agent_acl.h>
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
        //        int res = refda_binding_obj(CACE_ARI_TYPE_IDENT, obj, &agent.objs);
        //        assert(0 == res);
    }

    ex_odm = cace_amm_obj_store_add_ns(&(agent.objs), cace_amm_idseg_ref_withenum("example", EXAMPLE_ORG_ENUM),
                                       cace_amm_idseg_ref_withenum("!odm", EXAMPLE_ODM_ENUM), "2025-01-03");
    assert(NULL != ex_adm);

    int res = refda_agent_bindrefs(&agent);
    assert(0 == res);

    test_util_agent_permission(&agent, REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PRODUCE);
}

int suiteTearDown(int failures)
{
    ex_odm = NULL;
    ex_adm = NULL;
    refda_agent_deinit(&agent);

    cace_closelog();
    return failures;
}

void test_refda_alarms_set_state_no_category(void)
{
    cace_ari_t alarm_list_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&alarm_list_ref, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_ALARMS_ENUM_ADM, CACE_ARI_TYPE_EDD, REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_ALARM_LIST);
    cace_amm_lookup_t alarm_list_deref;
    cace_amm_lookup_init(&alarm_list_deref);
    TEST_ASSERT_EQUAL_INT(0, cace_amm_lookup_deref(&alarm_list_deref, &(agent.objs), &alarm_list_ref));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    // no mutex use with single thread
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_list_size(agent.alarms.alarm_list));
    TEST_ASSERT_EQUAL_size_t(0, refda_alarms_entry_index_size(agent.alarms.alarm_index));
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, &runctx, &alarm_list_ref, &alarm_list_deref);

        int res = refda_valprod_run(&prodctx);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_valprod_run() failed");

        struct cace_ari_tbl_s *tbl =cace_ari_get_tbl(&prodctx.value);
        TEST_ASSERT_NOT_NULL(tbl);
        TEST_ASSERT_EQUAL_size_t(9, tbl->ncols);
        TEST_ASSERT_EQUAL_size_t(0, cace_ari_array_size(tbl->items));

        refda_valprod_ctx_deinit(&prodctx);
    }

    cace_ari_t res_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&res_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_IDENT,
                                   EXAMPLE_IDENT_RES1_ENUM);

    cace_ari_t cat_ref = CACE_ARI_INIT_NULL;

    // initial entry
    refda_alarms_set_refs(&agent, &res_ref, &cat_ref, REFDA_ALARMS_SEVERITY_MINOR);
    // observe the table EDD
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, &runctx, &alarm_list_ref, &alarm_list_deref);

        int res = refda_valprod_run(&prodctx);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_valprod_run() failed");

        struct cace_ari_tbl_s *tbl =cace_ari_get_tbl(&prodctx.value);
        TEST_ASSERT_NOT_NULL(tbl);
        TEST_ASSERT_EQUAL_size_t(9, tbl->ncols);
        TEST_ASSERT_EQUAL_size_t(9, cace_ari_array_size(tbl->items));

        refda_valprod_ctx_deinit(&prodctx);
    }

    cace_ari_deinit(&cat_ref);
    cace_ari_deinit(&res_ref);

    refda_runctx_deinit(&runctx);
    cace_amm_lookup_deinit(&alarm_list_deref);
    cace_ari_deinit(&alarm_list_ref);
}
