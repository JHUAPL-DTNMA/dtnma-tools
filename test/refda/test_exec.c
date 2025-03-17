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

#define EXAMPLE_ORG_ENUM 65535
#define EXAMPLE_ADM_ENUM 10

/// Agent context for testing
static refda_agent_t agent;

/// Sequence of executions
static cace_ari_list_t exec_log;

static void test_exec_ctrl_exec_one_int(refda_ctrl_exec_ctx_t *ctx)
{
    const cace_ari_t *val = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    CHKVOID(val)
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, val, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("execution with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }

    // record this execution
    cace_ari_list_push_back(exec_log, ctx->item->ref);

    refda_ctrl_exec_ctx_set_result_copy(ctx, val);
}

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_dtnma_agent_init(&agent));

    cace_ari_list_init(exec_log);

    {
        // ADM for this test fixture
        cace_amm_obj_ns_t *adm =
            cace_amm_obj_store_add_ns(&(agent.objs), cace_amm_idseg_ref_withenum("example", EXAMPLE_ORG_ENUM),
                                      cace_amm_idseg_ref_withenum("adm", EXAMPLE_ADM_ENUM), "2025-02-10");
        cace_amm_obj_desc_t *obj;

        /**
         * Register CONST objects
         */
        {
            refda_amm_const_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_const_desc_t));
            refda_amm_const_desc_init(objdata);
            {
                cace_ari_ac_t acinit;
                cace_ari_ac_init(&acinit);
                {
                    cace_ari_t *item = cace_ari_list_push_back_new(acinit.items);
                    // ari://example/adm/CTRL/ctrl1
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_CTRL, 1);
                }
                {
                    cace_ari_t *item = cace_ari_list_push_back_new(acinit.items);
                    // ari://example/adm/CTRL/ctrl2
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_CTRL, 2);
                }

                cace_ari_set_ac(&(objdata->value), &acinit);
            }

            obj = refda_register_const(adm, cace_amm_idseg_ref_withenum("mac1", 1), objdata);
            // no parameters
        }

        /**
         * Register CTRL objects
         */
        {
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            cace_amm_type_set_use_direct(&(objdata->res_type), cace_amm_type_get_builtin(CACE_ARI_TYPE_VAST));
            objdata->execute = test_exec_ctrl_exec_one_int;

            obj = refda_register_ctrl(adm, cace_amm_idseg_ref_withenum("ctrl1", 1), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "one");

                cace_amm_type_set_use_direct(&(fparam->typeobj), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
            }
        }
        {
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            cace_amm_type_set_use_direct(&(objdata->res_type), cace_amm_type_get_builtin(CACE_ARI_TYPE_VAST));
            objdata->execute = test_exec_ctrl_exec_one_int;

            obj = refda_register_ctrl(adm, cace_amm_idseg_ref_withenum("ctrl2", 2), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "one");

                cace_amm_type_set_use_direct(&(fparam->typeobj), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
            }
        }
    }

    int res = refda_agent_bindrefs(&agent);
    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    cace_ari_list_clear(exec_log);
    refda_agent_deinit(&agent);
}

static void check_execute(const cace_ari_t *target, int expect_exp, int wait_limit, int wait_ms[])
{
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, target, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("execution target %s", string_get_cstr(buf));
        string_clear(buf);
    }

    refda_runctx_ptr_t ctxptr;
    refda_runctx_ptr_init_new(ctxptr);
    // no nonce for test
    refda_runctx_from(refda_runctx_ptr_ref(ctxptr), &agent, NULL);

    refda_exec_seq_t eseq;
    refda_exec_seq_init(&eseq);

    int res = refda_exec_exp_target(&eseq, ctxptr, target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_exp, res, "refda_exec_exp_target() failed");

    bool success = false;
    if (expect_exp)
    {
        // don't actually run, but clean up after
        success = true;
    }

    // limit test scale
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
            // if there are more items the first must be waiting
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
                cace_timeperiod_encode(buf, &remain);
                CACE_LOG_DEBUG("remaining time %s", string_get_cstr(buf));
                string_clear(buf);
            }

            // absolute difference within 20ms of expected
            TEST_ASSERT_GREATER_OR_EQUAL(wait_ms[ix] - 30, timespec_to_ms(remain));
            TEST_ASSERT_LESS_OR_EQUAL(wait_ms[ix] + 20, timespec_to_ms(remain));

            // manual sleep
            nanosleep(&remain, NULL);

            TEST_ASSERT_EQUAL_INT(1, refda_timeline_size(agent.exec_timeline));
            refda_timeline_it(tl_it, agent.exec_timeline);
            if (!refda_timeline_end_p(tl_it))
            {
                const refda_timeline_event_t *next = refda_timeline_cref(tl_it);

                refda_ctrl_exec_ctx_t ctx;
                refda_ctrl_exec_ctx_init(&ctx, next->exec.item);
                (next->exec.callback)(&ctx);
                refda_ctrl_exec_ctx_deinit(&ctx);

                if (!atomic_load(&(next->exec.item->waiting)))
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
// direct ref ari://65535/10/CTRL/1
TEST_CASE("8419FFFF0A2201", 0, "821181""8419FFFF0A2201")
// bad deref ref ari://65535/10/CTRL/-1
TEST_CASE("8419FFFF0A2220", 4, "821180")
// direct MAC ari:/AC/(//65535/10/CTRL/1,//65535/10/CTRL/2)
TEST_CASE("8211828419FFFF0A22018419FFFF0A2202", 0, "821182""8419FFFF0A2201""8419FFFF0A2202")
// indirect MAC ari://65535/10/CONST/1
TEST_CASE("8419FFFF0A2101", 0, "821182""8419FFFF0A2201""8419FFFF0A2202")
// recursive MAC ari:/AC/(//65535/10/CONST/1)
TEST_CASE("8211818419FFFF0A2101", 0, "821182""8419FFFF0A2201""8419FFFF0A2202")
// clang-format on
void test_refda_exec_target(const char *targethex, int expect_exp, const char *expectloghex)
{
    CACE_LOG_DEBUG("target %s", targethex);
    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    cace_ari_t expect_log = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&expect_log, expectloghex));
    {
        const cace_amm_type_t *ac_type = cace_amm_type_get_builtin(CACE_ARI_TYPE_AC);
        TEST_ASSERT_NOT_NULL(ac_type);
        TEST_ASSERT_TRUE_MESSAGE(cace_amm_type_match(ac_type, &expect_log), "invalid log ARI");
    }
    cace_ari_list_t *expect_seq = &(expect_log.as_lit.value.as_ac->items);

    int wait_limit = expect_exp == 0 ? 1 : 0;
    int wait_ms[]  = { 0 };
    check_execute(&target, expect_exp, wait_limit, wait_ms);

    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(agent.exec_state_mutex)));
    TEST_ASSERT_TRUE(refda_exec_seq_list_empty_p(agent.exec_state));
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(agent.exec_state_mutex)));

    // verify execution sequence
    cace_ari_list_it_t expect_it;
    cace_ari_list_it(expect_it, *expect_seq);
    cace_ari_list_it_t got_it;
    cace_ari_list_it(got_it, exec_log);
    TEST_ASSERT_EQUAL_INT_MESSAGE(cace_ari_list_size(*expect_seq), cace_ari_list_size(exec_log),
                                  "exec_log size mismatch");
    for (; !cace_ari_list_end_p(expect_it) && !cace_ari_list_end_p(got_it);
         cace_ari_list_next(expect_it), cace_ari_list_next(got_it))
    {
        // TEST_MESSAGE()
        const bool equal = cace_ari_equal(cace_ari_list_cref(expect_it), cace_ari_list_cref(got_it));
        TEST_ASSERT_TRUE_MESSAGE(equal, "exec_log ARI is different");
    }

    // no remaining state
    TEST_ASSERT_TRUE(refda_exec_seq_list_empty_p(agent.exec_state));
    TEST_ASSERT_TRUE(refda_timeline_empty_p(agent.exec_timeline));

    cace_ari_deinit(&expect_log);
    cace_ari_deinit(&target);
}

TEST_CASE(1000)
void test_refda_exec_wait_for(int delay_ms)
{
    // synthesize the target
    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    {
        cace_ari_ref_t *ref = cace_ari_set_objref(&target);
        // ari://ietf/dtnma-agent/CTRL/wait-for
        cace_ari_objpath_set_intid(&(ref->objpath), 1, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_CTRL,
                                   REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_FOR);

        cace_ari_list_t params;
        cace_ari_list_init(params);
        {
            cace_ari_t *param = cace_ari_list_push_back_new(params);
            cace_ari_set_td(param, timespec_from_ms(delay_ms));
        }
        cace_ari_params_set_ac(&(ref->params), params);
    }

    int wait_ms[] = { delay_ms, 0 };
    check_execute(&target, 0, 2, wait_ms);

    cace_ari_deinit(&target);
}

TEST_CASE(1000)
void test_refda_exec_wait_until(int delay_ms)
{
    struct timespec nowtime;
    int             res = clock_gettime(CLOCK_REALTIME, &nowtime);
    TEST_ASSERT_EQUAL_INT(0, res);
    struct timespec abstime = timespec_add(timespec_from_ms(delay_ms), nowtime);

    // synthesize the target
    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    {
        cace_ari_ref_t *ref = cace_ari_set_objref(&target);
        // ari://ietf/dtnma-agent/CTRL/wait-until
        cace_ari_objpath_set_intid(&(ref->objpath), 1, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_CTRL,
                                   REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_UNTIL);

        cace_ari_list_t params;
        cace_ari_list_init(params);
        {
            cace_ari_t *param = cace_ari_list_push_back_new(params);
            cace_ari_set_tp_posix(param, abstime);
        }
        cace_ari_params_set_ac(&(ref->params), params);
    }

    int wait_ms[] = { delay_ms, 0 };
    check_execute(&target, 0, 2, wait_ms);

    cace_ari_deinit(&target);
}

TEST_CASE(1000)
void test_refda_exec_wait_cond(int delay_ms)
{
    // synthesize the target
    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    {
        cace_ari_ref_t *ref = cace_ari_set_objref(&target);
        cace_ari_objpath_set_intid(&(ref->objpath), 1, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_CTRL,
                                   REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_COND);

        cace_ari_list_t params;
        cace_ari_list_init(params);
        {
            cace_ari_t *param = cace_ari_list_push_back_new(params);

            cace_ari_ac_t expr;
            cace_ari_ac_init(&expr);
            {
                cace_ari_t     *expr_item = cace_ari_list_push_back_new(expr.items);
                cace_ari_ref_t *pref      = cace_ari_set_objref(expr_item);
                // will always evaluate truthy
                cace_ari_objpath_set_intid(&(pref->objpath), 1, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM, CACE_ARI_TYPE_EDD,
                                           REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VERSION);
            }
            cace_ari_set_ac(param, &expr);
        }
        cace_ari_params_set_ac(&(ref->params), params);
    }

    int wait_ms[] = { 0 };
    check_execute(&target, 0, 1, wait_ms);

    cace_ari_deinit(&target);
}
