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
#include "util/runctx.h"
#include <refda/valprod.h>
#include <refda/adm/ietf_amm.h>
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

static atomic_int edd_one_state = ATOMIC_VAR_INIT(0);

void setUp(void)
{
    refda_agent_init(&agent);
    // ADM initialization
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_amm_init(&agent));
    TEST_ASSERT_EQUAL_INT(0, refda_adm_ietf_dtnma_agent_init(&agent));

    int res = refda_agent_bindrefs(&agent);
    TEST_ASSERT_EQUAL_INT(0, res);
}

void tearDown(void)
{
    refda_agent_deinit(&agent);
}

// clang-format off
// ari://1/EDD/0 -> ari:%22JHU%2FAPL%22
TEST_CASE("83012300", 0, ARI_PRIM_TSTR, 0)
// ari://1/EDD/1 -> ari:%220.0.0%22
TEST_CASE("83012301", 0, ARI_PRIM_TSTR, 0)
// ari://1/EDD/2 -> ari:/TBL/c=4;
TEST_CASE("83012302", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// ari://1/EDD/typedef-list -> ari:/TBL/c=1;
TEST_CASE("8301230A", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// ari://1/EDD/typedef-list(true) -> ari:/TBL/c=1;(...)
TEST_CASE("8401230A81F5", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// ari://1/EDD/var-list -> ari:/TBL/c=2;
TEST_CASE("8301230B", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// ari://1/EDD/var-list(true) -> ari:/TBL/c=2;
TEST_CASE("8401230B81F5", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// ari://1/EDD/const-list -> ari:/TBL/c=2;
TEST_CASE("8301230E", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// ari://1/EDD/const-list(true) -> ari:/TBL/c=2;
TEST_CASE("8401230E81F5", 0, ARI_PRIM_OTHER, ARI_TYPE_TBL)
// clang-format on
void test_refda_adm_ietf_dtnma_agent_edd_produce(const char *targethex, int expect_res,
                                                 enum ari_prim_type_e expect_prim, ari_type_t expect_type)
{
    ari_t target = ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    TEST_ASSERT_EQUAL_INT(0, cace_amm_lookup_deref(&deref, &(agent.objs), &target));

    refda_valprod_ctx_t prodctx;
    refda_valprod_ctx_init(&prodctx, &runctx, &target, &deref);

    int res = refda_valprod_run(&prodctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_valprod_run() disagrees");

    // verify produced value
    {
        m_string_t buf;
        TEST_ASSERT_EQUAL_INT(0, test_util_ari_encode(buf, &(prodctx.value)));
        TEST_PRINTF("Produced value %s", string_get_cstr(buf));
        m_string_clear(buf);
    }
    TEST_ASSERT_FALSE(prodctx.value.is_ref);
    TEST_ASSERT_EQUAL_INT(expect_prim, prodctx.value.as_lit.prim_type);
    if (expect_prim == ARI_PRIM_OTHER)
    {
        TEST_ASSERT_TRUE(prodctx.value.as_lit.has_ari_type);
        TEST_ASSERT_EQUAL_INT(expect_type, prodctx.value.as_lit.ari_type);
    }

    refda_valprod_ctx_deinit(&prodctx);
    cace_amm_lookup_deinit(&deref);
    refda_runctx_deinit(&runctx);
    ari_deinit(&target);
}
