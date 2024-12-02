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
#include <refda/exec.h>
#include <refda/register.h>
#include <refda/amm/const.h>
#include <refda/amm/ctrl.h>
#include <refda/adm/ietf.h>
#include <cace/amm/semtype.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/ari/text.h>
#include <cace/util/defs.h>
#include <timespec.h>
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

/// Sequence of executions
static ari_list_t exec_log;

static int test_exec_ctrl_exec_one_int(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx)
{
    const ari_t *val = refda_exec_ctx_get_aparam_index(ctx, 0);
    CHKERR1(val)
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, val, ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("execution with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }

    // record this execution
    ari_list_push_back(exec_log, ctx->item->ref);

    ari_set_copy(&(ctx->result), val);
    return 0;
}

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    refda_adm_ietf_amm_init(&agent);
    refda_adm_ietf_dtnma_agent_init(&agent);

    ari_list_init(exec_log);

    {
        // ADM for this test fixture
        cace_amm_obj_ns_t *adm =
            cace_amm_obj_store_add_ns(&(agent.objs), "example-adm", true, EXAMPLE_ADM_ENUM);
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
                    ari_set_objref_path_intid(item, EXAMPLE_ADM_ENUM, ARI_TYPE_CTRL, 1); // ari://example-adm/CTRL/ctrl1
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    ari_set_objref_path_intid(item, EXAMPLE_ADM_ENUM, ARI_TYPE_CTRL, 2); // ari://example-adm/CTRL/ctrl2
                }

                ari_set_ac(&(objdata->value), &acinit);
            }

            obj = refda_register_const(adm, cace_amm_obj_id_withenum("mac1", 1), objdata);
            // no parameters
        }

        /**
         * Register CTRL objects
         */
        {
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            amm_type_set_use_direct(&(objdata->res_type), amm_type_get_builtin(ARI_TYPE_VAST));
            objdata->execute = test_exec_ctrl_exec_one_int;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("ctrl1", 1), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "one");

                amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_INT));
            }
        }
        {
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            amm_type_set_use_direct(&(objdata->res_type), amm_type_get_builtin(ARI_TYPE_VAST));
            objdata->execute = test_exec_ctrl_exec_one_int;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("ctrl2", 2), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "one");

                amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_INT));
            }
        }
    }

    int res = refda_agent_bindrefs(&agent);
    (void)res;
//    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    ari_list_clear(exec_log);
    refda_agent_deinit(&agent);
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

TEST_CASE("831A000100002201", 0, "821181""831A000100002201") // direct ref ari://65536/CTRL/1
TEST_CASE("831A000100002220", 4, "821180") // bad deref ref ari://65536/CTRL/-1
TEST_CASE("821182831A000100002201831A000100002202", 0, "821182""831A000100002201""831A000100002202") // direct MAC ari:/AC/(//65536/CTRL/1,//65536/CTRL/2)
TEST_CASE("831A000100002101", 0, "821182""831A000100002201""831A000100002202") // indirect MAC ari://65536/CONST/1
TEST_CASE("821181831A000100002101", 0, "821182""831A000100002201""831A000100002202") // recursive MAC ari:/AC/(//65536/CONST/1)
void test_refda_exec_target(const char *targethex, int expect_res, const char *expectloghex)
{
    ari_t target = ARI_INIT_UNDEFINED;
    ari_convert(&target, targethex);

    ari_t expect_log = ARI_INIT_UNDEFINED;
    ari_convert(&expect_log, expectloghex);
    {
        const amm_type_t *ac_type = amm_type_get_builtin(ARI_TYPE_AC);
        TEST_ASSERT_NOT_NULL(ac_type);
        TEST_ASSERT_TRUE_MESSAGE(amm_type_match(ac_type, &expect_log), "invalid log ARI");
    }
    ari_list_t *expect_seq = &(expect_log.as_lit.value.as_ac->items);

    refda_runctx_t ctx;
    // no nonce for test
    refda_runctx_init(&ctx, &agent, NULL);

    int res = refda_exec_target(&ctx, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_exec_target() disagrees");

    TEST_ASSERT_TRUE(refda_exec_seq_list_empty_p(agent.exec_state));

    // verify execution sequence
    ari_list_it_t expect_it;
    ari_list_it(expect_it, *expect_seq);
    ari_list_it_t got_it;
    ari_list_it(got_it, exec_log);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ari_list_size(*expect_seq), ari_list_size(exec_log), "exec_log size mismatch");
    for (; !ari_list_end_p(expect_it) && !ari_list_end_p(got_it); ari_list_next(expect_it), ari_list_next(got_it))
    {
        //TEST_MESSAGE()
        const bool equal = ari_equal(ari_list_cref(expect_it), ari_list_cref(got_it));
        TEST_ASSERT_TRUE_MESSAGE(equal, "exec_log ARI is different");
    }

    // no remaining state
    TEST_ASSERT_TRUE(refda_exec_seq_list_empty_p(agent.exec_state));
    TEST_ASSERT_TRUE(refda_timeline_empty_p(agent.exec_timeline));

    ari_deinit(&expect_log);
    ari_deinit(&target);
}

TEST_CASE("8401220281820D01", 1000) // direct ref ari://1/CTRL/2(/TD/1)
TEST_CASE("8401220281820D82200F", 1500) // direct ref ari://1/CTRL/2(/TD/1.5)
void test_refda_exec_wait_for(const char *targethex, int delay_ms)
{
    ari_t target = ARI_INIT_UNDEFINED;
    ari_convert(&target, targethex);

    refda_runctx_t ctx;
    // no nonce for test
    refda_runctx_init(&ctx, &agent, NULL);

    int res = refda_exec_target(&ctx, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_exec_target() disagrees");

    TEST_ASSERT_FALSE(refda_exec_seq_list_empty_p(agent.exec_state));
    {
        TEST_ASSERT_FALSE(refda_exec_seq_list_empty_p(agent.exec_state));
        const refda_exec_seq_t *seq = refda_exec_seq_list_front(agent.exec_state);

        TEST_ASSERT_FALSE(refda_exec_item_list_empty_p(seq->items));
        const refda_exec_item_t *item = refda_exec_item_list_front(seq->items);
        TEST_ASSERT_TRUE(atomic_load(&(item->waiting)));
    }
    TEST_ASSERT_EQUAL_INT(1, refda_timeline_size(agent.exec_timeline));
    {
        refda_timeline_it_t it;
        refda_timeline_it(it, agent.exec_timeline);
        TEST_ASSERT_FALSE(refda_timeline_end_p(it));
        const refda_timeline_event_t *evt = refda_timeline_cref(it);

        struct timespec nowtime;
        int res = clock_gettime(CLOCK_REALTIME, &nowtime);
        TEST_ASSERT_EQUAL_INT(0, res);
        struct timespec remain = timespec_sub(evt->ts, nowtime);

        // absolute difference within 50ms of expected
        TEST_ASSERT_TRUE(timespec_ge(remain, timespec_from_ms(delay_ms - 50)));
        TEST_ASSERT_TRUE(timespec_le(remain, timespec_from_ms(delay_ms + 50)));
    }

    ari_deinit(&target);
}
