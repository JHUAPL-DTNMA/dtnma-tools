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
#include <refda/ctrl_exec_ctx.h>
#include <refda/exec_seq.h>
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

static cace_ari_t mock_result_store;

static void mock_ctrl_exec_none(refda_ctrl_exec_ctx_t *ctx)
{
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &mock_result_store, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution with mock result %s", string_get_cstr(buf));
        string_clear(buf);
    }
    refda_ctrl_exec_ctx_set_result_copy(ctx, &mock_result_store);
}

static void mock_ctrl_exec_one_int(refda_ctrl_exec_ctx_t *ctx)
{
    const cace_ari_t *val = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    CHKVOID(val)
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, val, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }
    refda_ctrl_exec_ctx_set_result_copy(ctx, val);
}

static void check_execute(cace_ari_t *result, const refda_amm_ctrl_desc_t *ctrl,
                          const cace_amm_formal_param_list_t fparams, const char *refhex, const char *outhex)
{
    cace_amm_obj_desc_t obj;
    cace_amm_obj_desc_init(&obj);
    cace_amm_user_data_set_from(&(obj.app_data), (void *)ctrl, false, NULL);

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

    // fudge these contents
    cace_ari_t inref = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&inref, refhex));
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    cace_ari_t outval = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&outval, outhex));
    TEST_ASSERT_EQUAL_INT(0, cace_ari_set_copy(&mock_result_store, &outval));

    eitem.deref.obj_type = CACE_ARI_TYPE_CTRL;
    eitem.deref.obj      = &obj;
    int res              = cace_amm_actual_param_set_populate(&(eitem.deref.aparams), fparams, &(inref.as_ref.params));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_actual_param_set_populate() failed");

    refda_ctrl_exec_ctx_t ctx;
    refda_ctrl_exec_ctx_init(&ctx, &eitem);
    (ctrl->execute)(&ctx);
    refda_ctrl_exec_ctx_deinit(&ctx);

    TEST_ASSERT_TRUE_MESSAGE(cace_ari_equal(&outval, &(eitem.result)), "result value mismatch");

    if (result)
    {
        // move out result value
        TEST_ASSERT_EQUAL_INT(0, cace_ari_set_move(result, &(eitem.result)));
    }

    refda_exec_item_deinit(&eitem);
    refda_exec_seq_deinit(&eseq);
    refda_runctx_ptr_clear(ctxptr);
    cace_amm_obj_desc_deinit(&obj);

    cace_ari_deinit(&outval);
    cace_ari_deinit(&inref);
}

void setUp(void)
{
    cace_ari_init(&mock_result_store);
}

void tearDown(void)
{
    cace_ari_deinit(&mock_result_store);
}

// References are based on ari://2/2/CONST/4
TEST_CASE(CACE_ARI_TYPE_NULL, "8402022104", "8200F6")
TEST_CASE(CACE_ARI_TYPE_INT, "8402022104", "82040A")
void test_ctrl_execute_param_none(cace_ari_type_t restype, const char *refhex, const char *outhex)
{
    refda_amm_ctrl_desc_t obj;
    refda_amm_ctrl_desc_init(&obj);
    // leave formal parameter list empty
    cace_amm_type_set_use_direct(&obj.res_type, cace_amm_type_get_builtin(restype));
    obj.execute = mock_ctrl_exec_none;

    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    check_execute(&result, &obj, fparams, refhex, outhex);

    cace_ari_deinit(&result);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_ctrl_desc_deinit(&obj);
}

// References are based on ari://2/2/CONST/4
TEST_CASE("8402022104", "820403")           // no parameters, default value
TEST_CASE("8502022104810A", "82040A")       // pass through parameter
TEST_CASE("8502022104A1000A", "82040A")     // pass through parameter by index
TEST_CASE("8502022104A16268690A", "82040A") // pass through parameter by name
TEST_CASE("850202210481F7", "F7")           // pass through undefined parameter
void test_ctrl_execute_param_one_int(const char *refhex, const char *outhex)
{
    refda_amm_ctrl_desc_t obj;
    refda_amm_ctrl_desc_init(&obj);
    // result is same type as parameter
    cace_amm_type_set_use_direct(&obj.res_type, cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
    obj.execute = mock_ctrl_exec_one_int;

    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);

        fparam->index = 0;
        string_set_str(fparam->name, "hi");

        cace_amm_type_set_use_direct(&(fparam->typeobj), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
        cace_ari_set_int(&(fparam->defval), 3); // arbitrary default
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    check_execute(&result, &obj, fparams, refhex, outhex);

    cace_ari_deinit(&result);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_ctrl_desc_deinit(&obj);
}
