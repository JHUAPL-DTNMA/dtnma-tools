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
#include "util/runctx.h"
#include <refda/reporting.h>
#include <refda/register.h>
#include <refda/edd_prod_ctx.h>
#include <refda/adm/ietf_amm.h>
#include <refda/adm/ietf_amm_base.h>
#include <refda/adm/ietf_amm_semtype.h>
#include <refda/adm/ietf_network_base.h>
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

#define EXAMPLE_ORG_ENUM 65535
#define EXAMPLE_ADM_ENUM 10

/// Agent context for testing
static refda_agent_t agent;

static atomic_int edd_one_state = ATOMIC_VAR_INIT(0);

static void test_reporting_edd_int(refda_edd_prod_ctx_t *ctx)
{
    int oldval = atomic_fetch_add(&edd_one_state, 1);
    TEST_PRINTF("EDD production to counter %d", oldval);
    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_int(&result, oldval);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
}

static void test_reporting_edd_one_int(refda_edd_prod_ctx_t *ctx)
{
    const cace_ari_t *param = refda_edd_prod_ctx_get_aparam_index(ctx, 0);
    CHKVOID(param)
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, param, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("EDD production with parameter %s", string_get_cstr(buf));
        string_clear(buf);
    }
    refda_edd_prod_ctx_set_result_copy(ctx, param);
}

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_base_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_semtype_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_network_base_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_dtnma_agent_init(&agent));

    atomic_store(&edd_one_state, 1);

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
                    // ari://example/adm/EDD/edd1
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_EDD, 1);
                }
                {
                    cace_ari_t *item = cace_ari_list_push_back_new(acinit.items);
                    // ari://example/adm/VAR/var2
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_VAR, 1);
                }

                cace_ari_set_ac(&(objdata->value), &acinit);
            }

            obj = refda_register_const(adm, cace_amm_idseg_ref_withenum("rptt1", 1), objdata);
            // no parameters
        }

        /**
         * Register VAR objects
         */
        {
            refda_amm_var_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_var_desc_t));
            refda_amm_var_desc_init(objdata);
            TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(objdata->val_type), CACE_ARI_TYPE_VAST));
            cace_ari_set_vast(&(objdata->value), 123456);

            obj = refda_register_var(adm, cace_amm_idseg_ref_withenum("var1", 1), objdata);
            // no parameters
        }

        /**
         * Register EDD objects
         */
        {
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(objdata->prod_type), CACE_ARI_TYPE_INT));
            objdata->produce = test_reporting_edd_int;

            obj = refda_register_edd(adm, cace_amm_idseg_ref_withenum("edd1", 1), objdata);
            // no parameters
        }
        {
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(objdata->prod_type), CACE_ARI_TYPE_INT));
            objdata->produce = test_reporting_edd_one_int;

            obj = refda_register_edd(adm, cace_amm_idseg_ref_withenum("edd2", 2), objdata);
            // no parameters
        }
    }

    int res = refda_agent_bindrefs(&agent);
    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    refda_agent_deinit(&agent);
}

static cace_ari_report_t *assert_rptset_items(cace_ari_t *val)
{
    TEST_ASSERT_FALSE(val->is_ref);
    TEST_ASSERT_TRUE(val->as_lit.has_ari_type);
    TEST_ASSERT_EQUAL(CACE_ARI_TYPE_RPTSET, val->as_lit.ari_type);
    cace_ari_report_list_t *rpts = &(val->as_lit.value.as_rptset->reports);
    TEST_ASSERT_EQUAL_INT(1, cace_ari_report_list_size(*rpts));
    return cace_ari_report_list_front(*rpts);
}

// clang-format off
// direct RPTT ari:/AC/(//65535/10/EDD/1,//65535/10/VAR/1) -> (/INT/1,/VAST/123456)
TEST_CASE("8211828419FFFF0A23018419FFFF0A2A01", 0, "821182""820401""82061A0001E240")
// indirect RPTT ari://65535/10/CONST/1 -> (/INT/1,/VAST/123456)
TEST_CASE("8419FFFF0A2101", 0, "821182""820401""82061A0001E240")
// direct with simple (one-item) expressions
// ari:/AC/(/AC/(//65535/10/EDD/1),/AC/(//65535/10/VAR/1)) -> (/INT/1,/VAST/123456)
TEST_CASE("8211828211818419FFFF0A23018211818419FFFF0A2A01", 0, "821182""820401""82061A0001E240")
// clang-format on
void test_refda_reporting_target(const char *targethex, int expect_res, const char *expectloghex)
{
    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    cace_ari_t destination = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tstr(&destination, "data:foo", false);

    cace_ari_t expect_rpt_items = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&expect_rpt_items, expectloghex));
    cace_ari_ac_t *expect_seq = cace_ari_get_ac(&expect_rpt_items);
    TEST_ASSERT_NOT_NULL(expect_seq);

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    int res = refda_reporting_target(&runctx, &target, &destination);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_exec_target() disagrees");

    // extract agent state
    TEST_ASSERT_EQUAL_INT(1, refda_msgdata_queue_size(agent.rptgs));
    refda_msgdata_t got_rptset;
    TEST_ASSERT_TRUE(refda_msgdata_queue_pop(&got_rptset, agent.rptgs));
    cace_ari_report_t *rpt = assert_rptset_items(&got_rptset.value);
    TEST_ASSERT_NOT_NULL(rpt);

    // verify RPTSET result
    cace_ari_list_it_t expect_it;
    cace_ari_list_it(expect_it, expect_seq->items);
    cace_ari_list_it_t got_it;
    cace_ari_list_it(got_it, rpt->items);
    TEST_ASSERT_EQUAL_INT_MESSAGE(cace_ari_list_size(expect_seq->items), cace_ari_list_size(rpt->items),
                                  "RPT size mismatch");
    size_t item_ix = 0;
    for (; !cace_ari_list_end_p(expect_it) && !cace_ari_list_end_p(got_it);
         cace_ari_list_next(expect_it), cace_ari_list_next(got_it))
    {
        TEST_PRINTF("Checking ARI %zu", item_ix++);
        const bool equal = cace_ari_equal(cace_ari_list_cref(expect_it), cace_ari_list_cref(got_it));
        TEST_ASSERT_TRUE_MESSAGE(equal, "RPT ARI is different");
    }

    refda_msgdata_deinit(&got_rptset);
    refda_runctx_deinit(&runctx);
    cace_ari_deinit(&expect_rpt_items);
    cace_ari_deinit(&destination);
    cace_ari_deinit(&target);
}
