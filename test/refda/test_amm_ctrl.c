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
#include <refda/ctrl_exec_ctx.h>
#include <cace/amm/semtype.h>
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

static void mock_ctrl_exec_none(refda_ctrl_exec_ctx_t *ctx)
{
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &mock_result_store, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution with mock result %s", string_get_cstr(buf));
        string_clear(buf);
    }
    refda_ctrl_exec_ctx_set_result_copy(ctx, &mock_result_store);
}

static void mock_ctrl_exec_one_int(refda_ctrl_exec_ctx_t *ctx)
{
    const ari_t *val = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    CHKVOID(val)
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, val, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }
    refda_ctrl_exec_ctx_set_result_copy(ctx, val);
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

static void check_execute(ari_t *result, const refda_amm_ctrl_desc_t *obj, const cace_amm_formal_param_list_t fparams,
                          const char *refhex, const char *outhex)
{
    refda_runctx_ptr_t ctxptr;
    refda_runctx_ptr_init_new(ctxptr);
    // no nonce for test
    refda_runctx_from(refda_runctx_ptr_ref(ctxptr), NULL, NULL);

    refda_exec_seq_t eseq;
    refda_exec_seq_init(&eseq);
    refda_runctx_ptr_set(eseq.runctx, ctxptr);

    refda_exec_item_t eitem;
    refda_exec_item_init(&eitem);
    eitem.seq = &eseq;

    ari_t inref = ARI_INIT_UNDEFINED;
    ari_convert(&inref, refhex);
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    ari_t outval = ARI_INIT_UNDEFINED;
    ari_convert(&outval, outhex);
    TEST_ASSERT_EQUAL_INT(0, ari_set_copy(&mock_result_store, &outval));

    int res = cace_amm_actual_param_set_populate(&(eitem.deref.aparams), fparams, &(inref.as_ref.params));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_actual_param_set_populate() failed");

    refda_ctrl_exec_ctx_t ctx;
    refda_ctrl_exec_ctx_init(&ctx, obj, &eitem);
    (obj->execute)(&ctx);
    refda_ctrl_exec_ctx_deinit(&ctx);

    TEST_ASSERT_TRUE_MESSAGE(ari_equal(&outval, &(eitem.result)), "result value mismatch");

    if (result)
    {
        // move out result value
        TEST_ASSERT_EQUAL_INT(0, ari_set_move(result, &(eitem.result)));
    }

    refda_exec_item_deinit(&eitem);
    refda_exec_seq_deinit(&eseq);
    refda_runctx_ptr_clear(ctxptr);

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
TEST_CASE(ARI_TYPE_NULL, "83022104", "8200F6")
TEST_CASE(ARI_TYPE_INT, "83022104", "82040A")
void test_ctrl_execute_param_none(ari_type_t restype, const char *refhex, const char *outhex)
{
    refda_amm_ctrl_desc_t obj;
    refda_amm_ctrl_desc_init(&obj);
    // leave formal parameter list empty
    amm_type_set_use_direct(&obj.res_type, amm_type_get_builtin(restype));
    obj.execute = mock_ctrl_exec_none;

    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);

    ari_t result = ARI_INIT_UNDEFINED;
    check_execute(&result, &obj, fparams, refhex, outhex);

    ari_deinit(&result);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_ctrl_desc_deinit(&obj);
}

// References are based on ari://2/CONST/4
TEST_CASE("83022104", "820403")           // no parameters, default value
TEST_CASE("84022104810A", "82040A")       // pass through parameter
TEST_CASE("84022104A1000A", "82040A")     // pass through parameter by index
TEST_CASE("84022104A16268690A", "82040A") // pass through parameter by name
TEST_CASE("8402210481F7", "F7")           // pass through undefined parameter
void test_ctrl_execute_param_one_int(const char *refhex, const char *outhex)
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

        fparam->index = 0;
        string_set_str(fparam->name, "hi");

        amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_INT));
        ari_set_int(&(fparam->defval), 3); // arbitrary default
    }

    ari_t result = ARI_INIT_UNDEFINED;
    check_execute(&result, &obj, fparams, refhex, outhex);

    ari_deinit(&result);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_ctrl_desc_deinit(&obj);
}
