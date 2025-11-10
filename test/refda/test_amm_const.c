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
#include <refda/amm/const.h>
#include <refda/agent.h>
#include <refda/valprod.h>
#include <cace/amm/semtype.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
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

// No-parameter object
static refda_amm_const_desc_t *obj_noparam = NULL;
// Flow-down parameter object
static refda_amm_const_desc_t *obj_intparam = NULL;

static void suite_adms_init(refda_agent_t *agent)
{
    // ADM for this test fixture
    cace_amm_obj_ns_t *adm =
        cace_amm_obj_store_add_ns(&(agent->objs), cace_amm_idseg_ref_withenum("example", EXAMPLE_ORG_ENUM),
                                  cace_amm_idseg_ref_withenum("adm", EXAMPLE_ADM_ENUM), "2025-02-10");
    cace_amm_obj_desc_t *obj;

    /**
     * Register CONST objects
     */
    {
        refda_amm_const_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_const_desc_t));
        refda_amm_const_desc_init(objdata);
        // initial state
        cace_ari_set_int(&(objdata->value), 10);

        obj = refda_register_const(adm, cace_amm_idseg_ref_withenum("noparam", 4), objdata);
        assert(NULL != obj);
        // no parameters

        obj_noparam = objdata;
    }

    int res = refda_agent_bindrefs(agent);
    assert(0 == res);
}

static void check_produce(const char *refhex, const char *outhex, int expect_res)
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

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    refda_valprod_ctx_t ctx;
    refda_valprod_ctx_init(&ctx, &runctx, NULL, &deref);

    res = refda_valprod_run(&ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_valprod_run() mismatch");

    cace_ari_t outval = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&outval, outhex));

    TEST_ASSERT_TRUE_MESSAGE(cace_ari_equal(&outval, &(ctx.value)), "produced value mismatch");

    refda_valprod_ctx_deinit(&ctx);
    refda_runctx_deinit(&runctx);
    cace_amm_lookup_deinit(&deref);

    cace_ari_deinit(&outval);
    cace_ari_deinit(&inref);
}

// References are based on ari://65535/10/CONST/4
TEST_CASE("8419FFFF0A2104", "0A", 0, "0A")
TEST_CASE("8419FFFF0A2104", "F7", 0, "F7") // undefined state
TEST_CASE("8419FFFF0A2104", "F4", 0, "F4") // mismatched type not checked
void test_const_produce_noparam(const char *refhex, const char *valhex, int expect_res, const char *outhex)
{
    // force value state
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&(obj_noparam->value), valhex));

    check_produce(refhex, outhex, expect_res);
}

// FIXME: TEST_CASE("8519FFFF0A2105810A", "0A", 0) // [10] label substituted by index
// FIXME: TEST_CASE("8519FFFF0A2106810A", "0A", 0) // [10] label substituted by name
