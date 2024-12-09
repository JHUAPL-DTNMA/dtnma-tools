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
#include <refda/reporting.h>
#include <refda/register.h>
#include <refda/adm/ietf.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/amm/const.h>
#include <refda/amm/edd.h>
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

#define EXAMPLE_ADM_ENUM 65536

/// Agent context for testing
static refda_agent_t agent;
/// Manager identity
static cace_data_t mgr;

static atomic_int edd_one_state = ATOMIC_VAR_INIT(0);

static void test_reporting_edd_int(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx)
{
    int oldval = atomic_fetch_add(&edd_one_state, 1);
    TEST_PRINTF("EDD production to counter %d", oldval);
    ari_set_int(&(ctx->value), oldval);
}

static void test_reporting_edd_one_int(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx)
{
    const ari_t *val = refda_valprod_ctx_get_aparam_index(ctx, 0);
    CHKVOID(val)
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, val, ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("EDD production with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }
    ari_set_copy(&(ctx->value), val);
}

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_dtnma_agent_init(&agent));

    cace_data_init(&mgr);

    static const char *data = "test";
    // CACE data does not include terminating null
    cace_data_copy_from(&mgr, strlen(data), (cace_data_ptr_t)data);

    atomic_store(&edd_one_state, 1);

    {
        // ADM for this test fixture
        cace_amm_obj_ns_t   *adm = cace_amm_obj_store_add_ns(&(agent.objs), "example-adm", true, EXAMPLE_ADM_ENUM);
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
                    ari_set_objref_path_intid(item, EXAMPLE_ADM_ENUM, ARI_TYPE_EDD, 1); // ari://example-adm/EDD/edd1
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    ari_set_objref_path_intid(item, EXAMPLE_ADM_ENUM, ARI_TYPE_VAR, 1); // ari://example-adm/VAR/var2
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
            objdata->produce = test_reporting_edd_int;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("edd1", 1), objdata);
            // no parameters
        }
        {
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            amm_type_set_use_direct(&(objdata->prod_type), amm_type_get_builtin(ARI_TYPE_VAST));
            objdata->produce = test_reporting_edd_one_int;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("edd2", 2), objdata);
            // no parameters
        }
    }

    int res = refda_agent_bindrefs(&agent);
    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    cace_data_deinit(&mgr);
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

static ari_report_t *assert_rptset_items(ari_t *val)
{
    TEST_ASSERT_FALSE(val->is_ref);
    TEST_ASSERT_TRUE(val->as_lit.has_ari_type);
    TEST_ASSERT_EQUAL(ARI_TYPE_RPTSET, val->as_lit.ari_type);
    ari_report_list_t *rpts = &(val->as_lit.value.as_rptset->reports);
    TEST_ASSERT_EQUAL_INT(1, ari_report_list_size(*rpts));
    return ari_report_list_front(*rpts);
}

// clang-format off
// direct RPTT ari:/AC/(//65536/EDD/1,//65536/VAR/1) -> (/VAST/1,/VAST/123456)
TEST_CASE("821182831A000100002301831A000100002A01", 0, "821182""820601""82061A0001E240")
// indirect RPTT ari://65536/CONST/1 -> (/VAST/1,/VAST/123456)
TEST_CASE("831A000100002101", 0, "821182""820601""82061A0001E240")
// direct with simple (one-item) expressions
// ari:/AC/(/AC/(//65536/EDD/1),/AC/(//65536/VAR/1)) -> (/VAST/1,/VAST/123456)
TEST_CASE("821182821181831A000100002301821181831A000100002A01", 0, "821182""820601""82061A0001E240")
// clang-format on
void test_refda_reporting_target(const char *targethex, int expect_res, const char *expectloghex)
{
    ari_t target = ARI_INIT_UNDEFINED;
    ari_convert(&target, targethex);

    ari_t expect_rpt_items = ARI_INIT_UNDEFINED;
    ari_convert(&expect_rpt_items, expectloghex);
    ari_ac_t *expect_seq = ari_get_ac(&expect_rpt_items);
    TEST_ASSERT_NOT_NULL(expect_seq);

    refda_runctx_t runctx;
    refda_runctx_init(&runctx);
    // no nonce for test
    refda_runctx_from(&runctx, &agent, NULL);
    cace_data_copy(&runctx.mgr_ident, &mgr);

    int res = refda_reporting_target(&runctx, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_exec_target() disagrees");

    // extract agent state
    TEST_ASSERT_EQUAL_INT(1, refda_msgdata_queue_size(agent.rptgs));
    refda_msgdata_t got_rptset;
    TEST_ASSERT_TRUE(refda_msgdata_queue_pop(&got_rptset, agent.rptgs));
    ari_report_t *rpt = assert_rptset_items(&got_rptset.value);
    TEST_ASSERT_NOT_NULL(rpt);

    // verify RPTSET result
    ari_list_it_t expect_it;
    ari_list_it(expect_it, expect_seq->items);
    ari_list_it_t got_it;
    ari_list_it(got_it, rpt->items);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ari_list_size(expect_seq->items), ari_list_size(rpt->items), "RPT size mismatch");
    size_t item_ix = 0;
    for (; !ari_list_end_p(expect_it) && !ari_list_end_p(got_it); ari_list_next(expect_it), ari_list_next(got_it))
    {
        TEST_PRINTF("Checking ARI %u", item_ix++);
        const bool equal = ari_equal(ari_list_cref(expect_it), ari_list_cref(got_it));
        TEST_ASSERT_TRUE_MESSAGE(equal, "RPT ARI is different");
    }

    refda_msgdata_deinit(&got_rptset);
    refda_runctx_deinit(&runctx);
    ari_deinit(&expect_rpt_items);
    ari_deinit(&target);
}
