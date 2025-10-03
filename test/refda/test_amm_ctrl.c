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
#include "util/ari.h"
#include "util/agent.h"
#include "util/runctx.h"
#include <refda/ctrl_exec_ctx.h>
#include <refda/exec.h>
#include <cace/amm/semtype.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/ari/text.h>
#include <cace/util/defs.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

// Agent context for testing
static refda_agent_t agent;
// Initialize the test #agent
static void suite_adms_init(refda_agent_t *agent);

void suiteSetUp(void)
{
    cace_openlog();

    refda_agent_init(&agent);
    test_util_agent_crit_adms(&agent);
    suite_adms_init(&agent);
    test_util_agent_permission(&agent, REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PRODUCE);
}

int suiteTearDown(int failures)
{
    refda_agent_deinit(&agent);

    cace_closelog();
    return failures;
}

#define EXAMPLE_ORG_ENUM 65535
#define EXAMPLE_ADM_ENUM 10

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

static void suite_adms_init(refda_agent_t *agent)
{
    // ADM for this test fixture
    cace_amm_obj_ns_t *adm =
        cace_amm_obj_store_add_ns(&(agent->objs), cace_amm_idseg_ref_withenum("example", EXAMPLE_ORG_ENUM),
                                  cace_amm_idseg_ref_withenum("adm", EXAMPLE_ADM_ENUM), "2025-02-10");
    cace_amm_obj_desc_t *obj;

    /**
     * Register CTRL objects
     */
    {
        refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
        refda_amm_ctrl_desc_init(objdata);
        objdata->execute = mock_ctrl_exec_none;

        obj = refda_register_ctrl(adm, cace_amm_idseg_ref_withenum("noparam", 4), objdata);
        assert(NULL != obj);
        // no parameters
    }
    {
        refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
        refda_amm_ctrl_desc_init(objdata);
        assert(0 == cace_amm_type_set_use_builtin(&(objdata->res_type), CACE_ARI_TYPE_INT));
        objdata->execute = mock_ctrl_exec_one_int;

        obj = refda_register_ctrl(adm, cace_amm_idseg_ref_withenum("one_int", 5), objdata);
        assert(NULL != obj);
        // parameters:
        {
            cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "val");
            {
                cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
                // use of ari:/ARITYPE/INT
                cace_ari_set_aritype(&typeref, CACE_ARI_TYPE_INT);
                cace_amm_type_set_use_ref_move(&(fparam->typeobj), &typeref);
            }
        }
    }

    int res = refda_agent_bindrefs(agent);
    assert(0 == res);
}

static void check_execute(const char *refhex, const char *outhex)
{
    cace_ari_t inref = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&inref, refhex));
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&agent.objs_mutex));
    int res = cace_amm_lookup_deref(&deref, &agent.objs, &inref);
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&agent.objs_mutex));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_lookup_deref() failed");

    refda_runctx_ptr_t ctxptr;
    refda_runctx_ptr_init_new(ctxptr);
    // no nonce for test
    refda_runctx_from(refda_runctx_ptr_ref(ctxptr), &agent, NULL);

    refda_exec_seq_t eseq;
    refda_exec_seq_init(&eseq);
    refda_runctx_ptr_set(eseq.runctx, ctxptr);

    refda_exec_item_t *eitem = refda_exec_item_list_push_back_new(eseq.items);
    eitem->seq               = &eseq;
    cace_ari_set_move(&eitem->ref, &inref);
    cace_amm_lookup_set_move(&eitem->deref, &deref);

    cace_ari_t outval = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&outval, outhex));
    TEST_ASSERT_EQUAL_INT(0, cace_ari_set_copy(&mock_result_store, &outval));

    refda_exec_run_seq(&eseq);

#if 0
    TEST_ASSERT_TRUE_MESSAGE(cace_ari_equal(&outval, &(eitem.result)), "result value mismatch");
    if (result)
    {
        // move out result value
        TEST_ASSERT_EQUAL_INT(0, cace_ari_set_move(result, &(eitem.result)));
    }
#endif

    refda_exec_seq_deinit(&eseq);
    refda_runctx_ptr_clear(ctxptr);

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

// References are based on ari://65535/10/CONST/4
TEST_CASE(CACE_ARI_TYPE_NULL, "8419FFFF0A2204", "8200F6")
TEST_CASE(CACE_ARI_TYPE_INT, "8419FFFF0A2204", "82040A")
void test_ctrl_execute_noparam(cace_ari_type_t restype, const char *refhex, const char *outhex)
{
    check_execute(refhex, outhex);
}

// References are based on ari://65535/10/CTRL/5
TEST_CASE("8419FFFF0A2205", "820403")             // no parameters, default value
TEST_CASE("8519FFFF0A2205810A", "82040A")         // given parameter array
TEST_CASE("8519FFFF0A2205A1000A", "82040A")       // given parameter map by index
TEST_CASE("8519FFFF0A2205A16376616C0A", "82040A") // given parameter map by name
TEST_CASE("8519FFFF0A220581F7", "F7")             // given undefined parameter
void test_ctrl_execute_param_one_int(const char *refhex, const char *outhex)
{
    check_execute(refhex, outhex);
}
