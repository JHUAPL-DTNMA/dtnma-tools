/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "util/ari.h"
#include "util/agent.h"
#include "util/runctx.h"
#include <refda/register.h>
#include <refda/binding.h>
#include <refda/valprod.h>
#include <refda/adm/ietf.h>
#include <refda/adm/ietf_amm_base.h>
#include <refda/adm/ietf_amm_semtype.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/adm/ietf_dtnma_agent_acl.h>
#include <refda/amm/const.h>
#include <refda/amm/var.h>
#include <refda/amm/edd.h>
#include <cace/amm/semtype.h>
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
// Object number for VAR in ADM
#define EXAMPLE_VAR_ENUM 1
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
    refda_binding_ctx_t bind_ctx = {
        .store = &(agent.objs),
        .ns    = ex_adm,
    };

    {
        cace_amm_obj_desc_t *obj;
        { // For ./VAR/test
            refda_amm_var_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_var_desc_t));
            refda_amm_var_desc_init(objdata);
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_INT);
                cace_amm_type_set_use_ref_move(&objdata->val_type, &name);
            }
            cace_ari_set_int(&objdata->init_val, 100);

            obj = refda_register_var(ex_adm, cace_amm_idseg_ref_withenum("test", EXAMPLE_VAR_ENUM), objdata);
            // no parameters
        }
        assert(NULL != obj);
        int res = refda_binding_obj(&bind_ctx, CACE_ARI_TYPE_VAR, obj);
        assert(0 == res);
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

// clang-format off
// ari://1/1/EDD/0 -> ari:%22JHU%2FAPL%22
TEST_CASE("8401012300", 0, CACE_ARI_PRIM_TSTR, 0)
// ari://1/1/EDD/1 -> ari:%220.0.0%22
TEST_CASE("8401012301", 0, CACE_ARI_PRIM_TSTR, 0)
// ari://1/1/EDD/2 -> ari:/TBL/c=4;
TEST_CASE("8401012302", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/typedef-list -> ari:/TBL/c=1;
TEST_CASE("840101230A", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/typedef-list(true) -> ari:/TBL/c=1;(...)
TEST_CASE("850101230A81F5", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/var-list -> ari:/TBL/c=2;
TEST_CASE("840101230B", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/var-list(true) -> ari:/TBL/c=2;
TEST_CASE("850101230B81F5", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/const-list -> ari:/TBL/c=2;
TEST_CASE("840101230E", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/const-list(true) -> ari:/TBL/c=2;
TEST_CASE("850101230E81F5", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/sbr-list -> ari:/TBL/c=6;
TEST_CASE("840101230C", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/sbr-list(true) -> ari:/TBL/c=6;
TEST_CASE("850101230C81F5", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/tbr-list -> ari:/TBL/c=6;
TEST_CASE("840101230D", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// ari://1/1/EDD/tbr-list(true) -> ari:/TBL/c=6;
TEST_CASE("850101230D81F5", 0, CACE_ARI_PRIM_OTHER, CACE_ARI_TYPE_TBL)
// clang-format on
void test_refda_adm_ietf_dtnma_agent_edd_produce(const char *targethex, int expect_res,
                                                 enum cace_ari_prim_type_e expect_prim, cace_ari_type_t expect_type)
{
    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    TEST_ASSERT_EQUAL_INT(0, cace_amm_lookup_deref(&deref, &(agent.objs), &target));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));
    refda_valprod_ctx_t prodctx;
    refda_valprod_ctx_init(&prodctx, &runctx, &target, &deref);

    int res = refda_valprod_run(&prodctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_valprod_run() disagrees");

    // verify produced value
    {
        m_string_t buf;
        TEST_ASSERT_EQUAL_INT(0, test_util_ari_encode(buf, &(prodctx.value)));
        TEST_PRINTF("Produced value %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    TEST_ASSERT_FALSE(prodctx.value.is_ref);
    TEST_ASSERT_EQUAL_INT(expect_prim, prodctx.value.as_lit.prim_type);
    if (expect_prim == CACE_ARI_PRIM_OTHER)
    {
        TEST_ASSERT_TRUE(prodctx.value.as_lit.has_ari_type);
        TEST_ASSERT_EQUAL_INT(expect_type, prodctx.value.as_lit.ari_type);
    }

    refda_valprod_ctx_deinit(&prodctx);
    refda_runctx_deinit(&runctx);
    cace_amm_lookup_deinit(&deref);
    cace_ari_deinit(&target);
}

void test_refda_adm_ietf_dtnma_agent_ctrl_ensure_var(void)
{
    const cace_ari_int_id_t test_var_enum = 1;

    // new VAR object creation and initialization
    {
        cace_ari_t ctrl_ref = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ref_t *ref = cace_ari_set_objref_path_intid(
                &ctrl_ref, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_CTRL,
                REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_VAR);

            cace_ari_list_t params;
            cace_ari_list_init(params);
            { // namespace
                cace_ari_t *param = cace_ari_list_push_back_new(params);
                cace_ari_set_nsref_path_intid(param, EXAMPLE_ORG_ENUM, EXAMPLE_ODM_ENUM);
            }
            { // obj-name
                cace_ari_t *param = cace_ari_list_push_back_new(params);
                cace_ari_set_tstr(param, "newvar", true);
            }
            { // obj-enum
                cace_ari_t *param = cace_ari_list_push_back_new(params);
                cace_ari_set_vast(param, test_var_enum);
            }
            { // type
                cace_ari_t     *param       = cace_ari_list_push_back_new(params);
                cace_ari_ref_t *semtype_ref = cace_ari_set_objref_path_intid(
                    param, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_ADM, CACE_ARI_TYPE_IDENT,
                    REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TYPE_USE);

                cace_ari_list_t semtype_params;
                cace_ari_list_init(semtype_params);

                // use of ari://ietf/amm-base/typedef/rptt
                cace_ari_t *semtype_param = cace_ari_list_push_back_new(semtype_params);
                cace_ari_set_objref_path_intid(semtype_param, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_AMM_BASE_ENUM_ADM,
                                               CACE_ARI_TYPE_TYPEDEF, REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPTT);
                cace_ari_params_set_ac(&(semtype_ref->params), semtype_params);
            }
            { // value
                cace_ari_t *param = cace_ari_list_push_back_new(params);

                cace_ari_ac_t rptt;
                cace_ari_ac_init(&rptt);
                {
                    cace_ari_t *item = cace_ari_list_push_back_new(rptt.items);
                    // can reference a not-yet-existant EDD
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_EDD, 100);
                }
                cace_ari_set_ac(param, &rptt);
            }
            cace_ari_params_set_ac(&(ref->params), params);
        }

        test_util_agent_check_execute(&agent, &ctrl_ref);
        cace_ari_deinit(&ctrl_ref);
    }

#if 0
    // view new state
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, &runctx, &var_ref, &deref);

        int res = refda_valprod_run(&prodctx);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_valprod_run() failed");

        cace_ari_int got;
        TEST_ASSERT_EQUAL_INT(0, cace_ari_get_int(&prodctx.value, &got));
        TEST_ASSERT_EQUAL_INT(100, got);
        refda_valprod_ctx_deinit(&prodctx);
    }
#endif
}

void test_refda_adm_ietf_dtnma_agent_ctrl_var_store_reset(void)
{
    cace_ari_t var_ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&var_ref, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_VAR, EXAMPLE_VAR_ENUM);

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));
    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    TEST_ASSERT_EQUAL_INT(0, cace_amm_lookup_deref(&deref, &(agent.objs), &var_ref));

    // initial state
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, &runctx, &var_ref, &deref);

        int res = refda_valprod_run(&prodctx);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_valprod_run() failed");

        TEST_ASSERT_TRUE(cace_ari_is_undefined(&prodctx.value));
        refda_valprod_ctx_deinit(&prodctx);
    }

    // store specific value
    {
        cace_ari_t ctrl_ref = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ref_t *ref = cace_ari_set_objref_path_intid(
                &ctrl_ref, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_CTRL,
                REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_STORE);

            cace_ari_list_t params;
            cace_ari_list_init(params);
            {
                cace_ari_t *param = cace_ari_list_push_back_new(params);
                cace_ari_set_objref_path_intid(param, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_VAR,
                                               EXAMPLE_VAR_ENUM);
            }
            {
                cace_ari_t *param = cace_ari_list_push_back_new(params);
                cace_ari_set_int(param, -40);
            }
            cace_ari_params_set_ac(&(ref->params), params);
        }

        test_util_agent_check_execute(&agent, &ctrl_ref);
        cace_ari_deinit(&ctrl_ref);
    }

    // view stored state
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, &runctx, &var_ref, &deref);

        int res = refda_valprod_run(&prodctx);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_valprod_run() failed");

        cace_ari_int got;
        TEST_ASSERT_EQUAL_INT(0, cace_ari_get_int(&prodctx.value, &got));
        TEST_ASSERT_EQUAL_INT(-40, got);
        refda_valprod_ctx_deinit(&prodctx);
    }

    // reset to initializer
    {
        cace_ari_t ctrl_ref = CACE_ARI_INIT_UNDEFINED;
        {
            cace_ari_ref_t *ref = cace_ari_set_objref_path_intid(
                &ctrl_ref, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_CTRL,
                REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_RESET);

            cace_ari_list_t params;
            cace_ari_list_init(params);
            {
                cace_ari_t *param = cace_ari_list_push_back_new(params);
                cace_ari_set_objref_path_intid(param, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_VAR,
                                               EXAMPLE_VAR_ENUM);
            }
            cace_ari_params_set_ac(&(ref->params), params);
        }

        test_util_agent_check_execute(&agent, &ctrl_ref);
        cace_ari_deinit(&ctrl_ref);
    }

    // view reset state
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, &runctx, &var_ref, &deref);

        int res = refda_valprod_run(&prodctx);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_valprod_run() failed");

        cace_ari_int got;
        TEST_ASSERT_EQUAL_INT(0, cace_ari_get_int(&prodctx.value, &got));
        TEST_ASSERT_EQUAL_INT(100, got);
        refda_valprod_ctx_deinit(&prodctx);
    }

    cace_amm_lookup_deinit(&deref);
    refda_runctx_deinit(&runctx);
    cace_ari_deinit(&var_ref);
}
