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
#include <refda/exec.h>
#include <refda/register.h>
#include <refda/ctrl_exec_ctx.h>
#include <refda/adm/ietf_amm.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/amm/const.h>
#include <refda/amm/ctrl.h>
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

static void test_exec_ctrl_exec_one_int(refda_ctrl_exec_ctx_t *ctx)
{
    const ari_t *val = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    CHKVOID(val)
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, val, ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("execution with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }

    // record this execution
    ari_list_push_back(exec_log, ctx->item->ref);

    refda_ctrl_exec_ctx_set_result_copy(ctx, val);
}

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_dtnma_agent_init(&agent));

    ari_list_init(exec_log);

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
    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    ari_list_clear(exec_log);
    refda_agent_deinit(&agent);
}

static void check_execute(const ari_t *target, int expect_exp, int wait_limit, int wait_ms[])
{
    refda_runctx_ptr_t ctxptr;
    refda_runctx_ptr_init_new(ctxptr);
    // no nonce for test
    refda_runctx_from(refda_runctx_ptr_ref(ctxptr), &agent, NULL);

    refda_exec_seq_t eseq;
    refda_exec_seq_init(&eseq);

    int res = refda_exec_exp_target(&eseq, ctxptr, target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_exp, res, "refda_exec_exp_target() failed");
    if (expect_exp)
    {
        TEST_ASSERT_EQUAL(0, refda_exec_item_list_empty_p(eseq.items));
        return;
    }

    // limit test scale
    bool success = false;
    for (int ix = 0; !success && (ix < wait_limit); ++ix)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, refda_exec_run_seq(&eseq), "refda_exec_run_seq() failed");

        if (refda_exec_item_list_empty_p(eseq.items))
        {
            CACE_LOG_DEBUG("run break after %d iterations", ix + 1);
            success = true;
            break;
        }
        {
            const refda_exec_item_t *item = refda_exec_item_list_front(eseq.items);
            TEST_ASSERT_TRUE(atomic_load(&(item->waiting)));
        }

        TEST_ASSERT_EQUAL_INT(1, refda_timeline_size(agent.exec_timeline));
        refda_timeline_it_t tl_it;
        refda_timeline_it(tl_it, agent.exec_timeline);
        if (!refda_timeline_end_p(tl_it))
        {
            const refda_timeline_event_t *next = refda_timeline_cref(tl_it);
            TEST_ASSERT_NOT_NULL(next);

            struct timespec nowtime;
            int             res = clock_gettime(CLOCK_REALTIME, &nowtime);
            TEST_ASSERT_EQUAL_INT(0, res);
            struct timespec remain = timespec_sub(next->ts, nowtime);

            {
                string_t buf;
                string_init(buf);
                timeperiod_encode(buf, &remain);
                CACE_LOG_DEBUG("remaining time %s", string_get_cstr(buf));
                string_clear(buf);
            }

            // absolute difference within 20ms of expected
            TEST_ASSERT_TRUE(timespec_ge(remain, timespec_from_ms(wait_ms[ix] - 20)));
            TEST_ASSERT_TRUE(timespec_le(remain, timespec_from_ms(wait_ms[ix] + 20)));

            // manual sleep
            nanosleep(&remain, NULL);

            TEST_ASSERT_EQUAL_INT(1, refda_timeline_size(agent.exec_timeline));
            refda_timeline_it(tl_it, agent.exec_timeline);
            if (!refda_timeline_end_p(tl_it))
            {
                const refda_timeline_event_t *next = refda_timeline_cref(tl_it);

                bool finished = (next->callback)(next->item);
                if (finished)
                {
                    CACE_LOG_DEBUG("callback finished after %d iterations", ix + 1);
                    success = true;
                }

                refda_timeline_remove(agent.exec_timeline, tl_it);
            }
        }
    }

    refda_exec_seq_deinit(&eseq);
    refda_runctx_ptr_clear(ctxptr);
    TEST_ASSERT_TRUE(success);
}

// clang-format off
// direct ref ari://65536/CTRL/1
TEST_CASE("831A000100002201", 0, "821181""831A000100002201")
// bad deref ref ari://65536/CTRL/-1
TEST_CASE("831A000100002220", 4, "821180")
// direct MAC ari:/AC/(//65536/CTRL/1,//65536/CTRL/2)
TEST_CASE("821182831A000100002201831A000100002202", 0, "821182""831A000100002201""831A000100002202")
// indirect MAC ari://65536/CONST/1
TEST_CASE("831A000100002101", 0, "821182""831A000100002201""831A000100002202")
// recursive MAC ari:/AC/(//65536/CONST/1)
TEST_CASE("821181831A000100002101", 0, "821182""831A000100002201""831A000100002202")
// clang-format on
void test_refda_exec_target(const char *targethex, int expect_exp, const char *expectloghex)
{
    CACE_LOG_DEBUG("target %s", targethex);
    ari_t target = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    ari_t expect_log = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&expect_log, expectloghex));
    {
        const amm_type_t *ac_type = amm_type_get_builtin(ARI_TYPE_AC);
        TEST_ASSERT_NOT_NULL(ac_type);
        TEST_ASSERT_TRUE_MESSAGE(amm_type_match(ac_type, &expect_log), "invalid log ARI");
    }
    ari_list_t *expect_seq = &(expect_log.as_lit.value.as_ac->items);

    int wait_limit = expect_exp == 0 ? 1 : 0;
    int wait_ms[]  = { 0 };
    check_execute(&target, expect_exp, wait_limit, wait_ms);

    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(agent.exec_state_mutex)));
    TEST_ASSERT_TRUE(refda_exec_seq_list_empty_p(agent.exec_state));
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(agent.exec_state_mutex)));

    // verify execution sequence
    ari_list_it_t expect_it;
    ari_list_it(expect_it, *expect_seq);
    ari_list_it_t got_it;
    ari_list_it(got_it, exec_log);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ari_list_size(*expect_seq), ari_list_size(exec_log), "exec_log size mismatch");
    for (; !ari_list_end_p(expect_it) && !ari_list_end_p(got_it); ari_list_next(expect_it), ari_list_next(got_it))
    {
        // TEST_MESSAGE()
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
void test_refda_exec_wait_for(const char *targethex, int delay_ms)
{
    ari_t target = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    int wait_ms[] = { delay_ms, 0 };
    check_execute(&target, 0, 2, wait_ms);

    ari_deinit(&target);
}
