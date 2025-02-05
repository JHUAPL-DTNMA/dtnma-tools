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
#include "util/ari.h"
#include "util/runctx.h"
#include <refda/eval.h>
#include <refda/register.h>
#include <refda/edd_prod_ctx.h>
#include <refda/oper_eval_ctx.h>
#include <refda/adm/ietf_amm.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/amm/const.h>
#include <refda/amm/edd.h>
#include <cace/amm/semtype.h>
#include <cace/amm/numeric.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

void suiteSetUp(void)
{
    cace_openlog();
}

int suiteTearDown(int failures)
{
    cace_closelog();
    return failures;
}

#define EXAMPLE_ADM_ENUM 65536

/// Agent context for testing
static refda_agent_t agent;

static void test_reporting_edd_one_int(refda_edd_prod_ctx_t *ctx)
{
    const ari_t *param = refda_edd_prod_ctx_get_aparam_index(ctx, 0);
    CHKVOID(param);
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, param, ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("EDD production with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }
    refda_edd_prod_ctx_set_result_copy(ctx, param);
}

static int ari_numeric_add(ari_t *result, const ari_t *lt_val, const ari_t *rt_val)
{
    ari_type_t promote;
    if (cace_amm_numeric_promote_type(&promote, lt_val, rt_val))
    {
        return 2;
    }

    const amm_type_t *amm_promote = amm_type_get_builtin(promote);
    ari_t             lt_prom     = ARI_INIT_UNDEFINED;
    ari_t             rt_prom     = ARI_INIT_UNDEFINED;
    amm_type_convert(amm_promote, &lt_prom, lt_val);
    amm_type_convert(amm_promote, &rt_prom, rt_val);

    ari_deinit(result);
    ari_lit_t *res_lit = ari_init_lit(result);

    int retval = 0;
    switch (lt_prom.as_lit.prim_type)
    {
        case ARI_PRIM_UINT64:
            res_lit->value.as_uint64 = lt_prom.as_lit.value.as_uint64 + rt_prom.as_lit.value.as_uint64;
            break;
        case ARI_PRIM_INT64:
            res_lit->value.as_int64 = lt_prom.as_lit.value.as_int64 + rt_prom.as_lit.value.as_int64;
            break;
        case ARI_PRIM_FLOAT64:
            res_lit->value.as_float64 = lt_prom.as_lit.value.as_float64 + rt_prom.as_lit.value.as_float64;
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = lt_prom.as_lit.prim_type;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = promote;
    }

    ari_deinit(&lt_prom);
    ari_deinit(&rt_prom);
    return retval;
}

static void test_reporting_oper_add(refda_oper_eval_ctx_t *ctx)
{
    const ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);

    const ari_t *more = refda_oper_eval_ctx_get_aparam_index(ctx, 0);
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, more, ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("OPER evaluation with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }

    int   retval = 0;
    ari_t tmp1   = ARI_INIT_UNDEFINED;
    ari_t tmp2   = ARI_INIT_UNDEFINED;
    if (ari_numeric_add(&tmp1, left, right))
    {
        retval = 2;
    }
    if (!retval)
    {
        if (ari_numeric_add(&tmp2, &tmp1, more))
        {
            retval = 2;
        }
    }
    ari_deinit(&tmp1);

    if (!retval)
    {
        refda_oper_eval_ctx_set_result_move(ctx, &tmp2);
    }
    else
    {
        ari_deinit(&tmp2);
    }
}

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_dtnma_agent_init(&agent));

    {
        // ADM for this test fixture
        cace_amm_obj_ns_t   *adm = cace_amm_obj_store_add_ns(&(agent.objs), "example-adm", "", true, EXAMPLE_ADM_ENUM);
        cace_amm_obj_desc_t *obj;

        /**
         * Register CONST objects
         */
        {
            refda_amm_const_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_const_desc_t));
            refda_amm_const_desc_init(objdata);
            {
                ari_ac_t acinit;
                ari_ac_init(&acinit);
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    ari_set_objref_path_intid(item, EXAMPLE_ADM_ENUM, ARI_TYPE_EDD, 2); // ari://example-adm/EDD/edd1
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    ari_set_objref_path_intid(item, EXAMPLE_ADM_ENUM, ARI_TYPE_VAR, 1); // ari://example-adm/VAR/var1
                }

                ari_set_ac(&(objdata->value), &acinit);
            }

            obj = refda_register_const(adm, cace_amm_obj_id_withenum("rptt1", 1), objdata);
            // no parameters
        }

        /**
         * Register VAR objects
         */
        {
            refda_amm_var_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_var_desc_t));
            refda_amm_var_desc_init(objdata);
            amm_type_set_use_direct(&(objdata->val_type), amm_type_get_builtin(ARI_TYPE_VAST));
            ari_set_vast(&(objdata->value), 123456);

            obj = refda_register_var(adm, cace_amm_obj_id_withenum("var1", 1), objdata);
            // no parameters
        }

        /**
         * Register EDD objects
         */
        {
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            amm_type_set_use_direct(&(objdata->prod_type), amm_type_get_builtin(ARI_TYPE_VAST));
            objdata->produce = test_reporting_edd_one_int;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("edd2", 2), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "val");
                amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_VAST));
            }
        }

        /**
         * Register OPER objects
         */
        {
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);

            amm_named_type_array_resize(objdata->operand_types, 2);
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                amm_type_set_use_direct(&(operand->typeobj), amm_type_get_builtin(ARI_TYPE_VAST));
            }
            {
                amm_named_type_t *operand = amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                amm_type_set_use_direct(&(operand->typeobj), amm_type_get_builtin(ARI_TYPE_VAST));
            }
            amm_type_set_use_direct(&(objdata->res_type), amm_type_get_builtin(ARI_TYPE_VAST));

            objdata->evaluate = test_reporting_oper_add;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("oper1", 1), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "more");
                amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_VAST));
            }
        }
    }

    int res = refda_agent_bindrefs(&agent);
    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    refda_agent_deinit(&agent);
}

// direct EXPR ari:/AC/(/VAST/1) -> /VAST/1
TEST_CASE("821181820601", "820601")
// ref EXPR ari:/AC/(//65536/EDD/2(10)) -> /VAST/10
TEST_CASE("821181841A000100002302810A", "82060A")
// ari:/AC/(/VAST/3,/VAST/5,//65536/OPER/1(10)) -> /VAST/18
TEST_CASE("821183820603820605841A000100002501810A", "820612")
// ari:/AC/(//65536/EDD/2(10),//65536/VAR/1,//65536/OPER/1(10)) -> /VAST/123476
TEST_CASE("821183841A000100002302810A831A000100002A01841A000100002501810A", "82061A0001E254")

// ari:/AC/(/BOOL/false,//ietf-dtnma-agent/OPER/bool-not) -> /BOOL/true
TEST_CASE("8211828201F48370696574662D64746E6D612D6167656E742568626F6F6C2D6E6F74", "8201F5")
// ari:/AC/(/BOOL/true,//1/OPER/bool-not) -> /BOOL/false
TEST_CASE("8211828201F583012568626F6F6C2D6E6F74", "8201F4")
//  ari:/AC/(/VAST/0,//1/OPER/bool-not) -> /BOOL/true
TEST_CASE("82118282060083012568626F6F6C2D6E6F74", "8201F5")
// ari:/AC/(/TEXTSTR/test,//1/OPER/bool-not) -> /BOOL/false
TEST_CASE("821182820A647465737483012568626F6F6C2D6E6F74", "8201F4")

// ari:/AC/(/BOOL/true,/BOOL/false,//ietf-dtnma-agent/OPER/bool-and) -> /BOOL/false
TEST_CASE("8211838201F58201F48370696574662D64746E6D612D6167656E742568626F6F6C2D616E64", "8201F4")
// ari:/AC/(/BOOL/false,/BOOL/false,//1/OPER/bool-and) -> /BOOL/false
TEST_CASE("8211838201F48201F483012568626F6F6C2D616E64", "8201F4")
// ari:/AC/(/BOOL/false,/VAST/0,//1/OPER/bool-and) -> /BOOL/false
TEST_CASE("8211838201F482060083012568626F6F6C2D616E64", "8201F4")
// ari:/AC/(/BOOL/true,/TEXTSTR/test,//1/OPER/bool-and) -> /BOOL/true
TEST_CASE("8211838201F5820A647465737483012568626F6F6C2D616E64", "8201F5")

// ari:/AC/(/BOOL/true,/BOOL/false,//ietf-dtnma-agent/OPER/bool-or) -> /BOOL/true
TEST_CASE("8211838201F58201F48370696574662D64746E6D612D6167656E742567626F6F6C2D6F72", "8201F5")
// ari:/AC/(/BOOL/false,/BOOL/false,//1/OPER/bool-or) -> /BOOL/false
TEST_CASE("8211838201F48201F483012567626F6F6C2D6F72", "8201F4")
// ari:/AC/(/BOOL/false,/VAST/0,//1/OPER/bool-or) -> /BOOL/false
TEST_CASE("8211838201F482060083012567626F6F6C2D6F72", "8201F4")
// ari:/AC/(/BOOL/true,/TEXTSTR/test,//1/OPER/bool-or) -> /BOOL/true
TEST_CASE("8211838201F5820A647465737483012567626F6F6C2D6F72", "8201F5")

// ari:/AC/(/BOOL/true,/BOOL/false,//ietf-dtnma-agent/OPER/bool-xor) -> /BOOL/true
TEST_CASE("8211838201F58201F48370696574662D64746E6D612D6167656E742568626F6F6C2D786F72", "8201F5")
// ari:/AC/(/BOOL/false,/BOOL/false,//1/OPER/bool-xor) -> /BOOL/false
TEST_CASE("8211838201F48201F483012568626F6F6C2D786F72", "8201F4")
// ari:/AC/(/BOOL/false,/VAST/0,//1/OPER/bool-xor) -> /BOOL/false
TEST_CASE("8211838201F482060083012568626F6F6C2D786F72", "8201F4")
// ari:/AC/(/BOOL/true,/TEXTSTR/test,//1/OPER/bool-xor) -> /BOOL/false
TEST_CASE("8211838201F5820A647465737483012568626F6F6C2D786F72", "8201F4")
void test_refda_eval_target_valid(const char *targethex, const char *expectloghex)
{
    ari_t target = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    ari_t expect_result = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&expect_result, expectloghex));
    TEST_ASSERT_FALSE(ari_is_undefined(&expect_result));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    ari_t result = ARI_INIT_UNDEFINED;
    int   res    = refda_eval_target(&runctx, &result, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_eval_target() disagrees");

    // verify result value
    const bool equal = ari_equal(&expect_result, &result);
    TEST_ASSERT_TRUE_MESSAGE(equal, "result ARI is different");

    ari_deinit(&result);
    refda_runctx_deinit(&runctx);
    ari_deinit(&expect_result);
    ari_deinit(&target);
}

// ari:/AC/(/BOOL/false,undefined,//1/OPER/bool-and) -> undefined
TEST_CASE("8211838201F4F783012568626F6F6C2D616E64", "F7")
void test_refda_eval_target_undefined(const char *targethex, const char *expectloghex)
{
    ari_t target = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    ari_t expect_result = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&expect_result, expectloghex));
    TEST_ASSERT_TRUE(ari_is_undefined(&expect_result));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    ari_t result = ARI_INIT_UNDEFINED;
    int   res    = refda_eval_target(&runctx, &result, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_eval_target() disagrees");

    // verify result value
    const bool equal = ari_equal(&expect_result, &result);
    TEST_ASSERT_TRUE_MESSAGE(equal, "result ARI is different");

    ari_deinit(&result);
    refda_runctx_deinit(&runctx);
    ari_deinit(&expect_result);
    ari_deinit(&target);
}

TEST_CASE("821180", 6)             // Empty stack ari:/AC/()
TEST_CASE("821182820601820602", 6) // Extra stack ari:/AC/(/VAST/1,/VAST/2)
void test_refda_eval_target_failure(const char *targethex, int expect_res)
{

    ari_t target = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    ari_t result = ARI_INIT_UNDEFINED;
    int   res    = refda_eval_target(&runctx, &result, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_eval_target() disagrees");

    refda_runctx_deinit(&runctx);
    ari_deinit(&result);
    ari_deinit(&target);
}
