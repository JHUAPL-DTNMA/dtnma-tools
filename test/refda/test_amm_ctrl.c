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
#include <refda/amm/ctrl.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/ari/text.h>
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

static ari_t mock_result_store;

static int mock_ctrl_exec_none(const refda_amm_ctrl_desc_t *obj _U_, refda_amm_exec_ctx_t *ctx)
{
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &mock_result_store, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution with mock result %s", string_get_cstr(buf));
        string_clear(buf);
    }
    ari_set_copy(&(ctx->result), &mock_result_store);
    return 0;
}

static int mock_ctrl_exec_one_int(const refda_amm_ctrl_desc_t *obj _U_, refda_amm_exec_ctx_t *ctx)
{
    const ari_t *val = ari_list_get(ctx->aparams.ordered, 0);
    CHKERR1(val)
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, val, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }
    ari_set_copy(&(ctx->result), val);
    return 0;
}

static void ari_convert(ari_t *ari, const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = base16_decode(&indata, intext);
    string_clear(intext);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "base16_decode() failed");

    res = ari_cbor_decode(ari, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");
}

// refda_amm_obj_desc_t obj;
// refda_amm_obj_desc_init(&obj);
// refda_amm_user_data_set_from(&obj.app_data, &ctrl, (refda_amm_user_data_deinit_f)&refda_amm_ctrl_desc_deinit);

static void check_execute(ari_t *result, const refda_amm_ctrl_desc_t *obj, const cace_amm_formal_param_list_t fparams,
                          const char *refhex, const char *outhex, int expect_res)
{
    ari_t inref = ARI_INIT_UNDEFINED;
    ari_convert(&inref, refhex);
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    ari_t outval = ARI_INIT_UNDEFINED;
    ari_convert(&outval, outhex);
    TEST_ASSERT_EQUAL_INT(0, ari_set_copy(&mock_result_store, &outval));

    refda_amm_exec_ctx_t ctx;
    int                 res = refda_amm_exec_ctx_init(&ctx, fparams, &inref);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_amm_exec_ctx_init() disagrees");

    res = refda_amm_ctrl_desc_execute(obj, &ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_amm_ctrl_desc_execute() failed");

    TEST_ASSERT_TRUE_MESSAGE(ari_equal(&outval, &(ctx.result)), "result value mismatch");

    if (result)
    {
        // move out produced value
        TEST_ASSERT_EQUAL_INT(0, ari_set_move(result, &(ctx.result)));
    }

    refda_amm_exec_ctx_deinit(&ctx);
    ari_deinit(&outval);
    ari_deinit(&inref);
}

void setUp(void)
{
    ari_init(&mock_result_store);
}

void tearDown(void)
{
    ari_deinit(&mock_result_store);
}

// References are based on ari://2/CONST/4
TEST_CASE(ARI_TYPE_NULL, "83022104", "F6", 0)
TEST_CASE(ARI_TYPE_INT, "83022104", "0A", 0)
void test_ctrl_execute_param_none(ari_type_t restype, const char *refhex, const char *outhex, int expect_res)
{
    refda_amm_ctrl_desc_t obj;
    refda_amm_ctrl_desc_init(&obj);
    // leave formal parameter list empty
    amm_type_set_use_direct(&obj.res_type, amm_type_get_builtin(restype));
    obj.execute = mock_ctrl_exec_none;

    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);

    ari_t result = ARI_INIT_UNDEFINED;
    check_execute(&result, &obj, fparams, refhex, outhex, expect_res);

    ari_deinit(&result);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_ctrl_desc_init(&obj);
}

// References are based on ari://2/CONST/4
TEST_CASE("83022104", "820403", 0)           // no parameters, default value
TEST_CASE("84022104810A", "82040A", 0)       // pass through parameter
TEST_CASE("84022104A1000A", "82040A", 0)     // pass through parameter by index
TEST_CASE("84022104A16268690A", "82040A", 0) // pass through parameter by name
TEST_CASE("8402210481F7", "F7", 0)           // pass through undefined parameter
void test_ctrl_execute_param_one_int(const char *refhex, const char *outhex, int expect_res)
{
    refda_amm_ctrl_desc_t obj;
    refda_amm_ctrl_desc_init(&obj);
    // result is same type as parameter
    amm_type_set_use_direct(&obj.res_type, amm_type_get_builtin(ARI_TYPE_INT));
    obj.execute = mock_ctrl_exec_one_int;

    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);
        fparam->index              = 0;
        string_set_str(fparam->name, "hi");
        fparam->typeobj = amm_type_get_builtin(ARI_TYPE_INT);
        ari_set_int(&(fparam->defval), 3); // arbitrary default
    }

    ari_t result = ARI_INIT_UNDEFINED;
    check_execute(&result, &obj, fparams, refhex, outhex, expect_res);

    ari_deinit(&result);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_ctrl_desc_init(&obj);
}
