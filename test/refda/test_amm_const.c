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
#include <refda/amm/const.h>
#include <refda/valprod.h>
#include <cace/amm/semtype.h>
#include <cace/amm/parameters.h>
#include <cace/amm/lookup.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
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

static cace_amm_obj_desc_t    obj;
static refda_amm_const_desc_t desc;

void setUp(void)
{
    cace_amm_obj_desc_init(&obj);

    refda_amm_const_desc_init(&desc);
    obj.app_data.ptr = &desc;

    // leave formal parameter list empty
}

void tearDown(void)
{
    refda_amm_const_desc_deinit(&desc);
    cace_amm_obj_desc_deinit(&obj);
}

static void check_produce(cace_ari_t *value, const char *refhex, const char *outhex, int expect_res)
{
    cace_ari_t inref = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&inref, refhex));
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    deref.obj_type = CACE_ARI_TYPE_CONST;
    deref.obj      = &obj;

    int res = cace_amm_actual_param_set_populate(&(deref.aparams), obj.fparams, &(inref.as_ref.params));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_actual_param_set_populate() failed");

    refda_valprod_ctx_t ctx;
    refda_valprod_ctx_init(&ctx, NULL, NULL, &deref);

    res = refda_valprod_run(&ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_amm_const_desc_produce() mismatch");

    cace_ari_t outval = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&outval, outhex));

    TEST_ASSERT_TRUE_MESSAGE(cace_ari_equal(&outval, &(ctx.value)), "produced value mismatch");

    // move out produced value
    TEST_ASSERT_EQUAL_INT(0, cace_ari_set_move(value, &(ctx.value)));

    refda_valprod_ctx_deinit(&ctx);
    cace_amm_lookup_deinit(&deref);

    cace_ari_deinit(&outval);
    cace_ari_deinit(&inref);
}

// References are based on ari://2/2/CONST/4
TEST_CASE("0A", "8402022104", "0A", 0)
void test_const_produce_param_none(const char *valhex, const char *refhex, const char *outhex, int expect_res)
{
    // initial state
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&(desc.value), valhex));

    cace_ari_t value = CACE_ARI_INIT_UNDEFINED;
    check_produce(&value, refhex, outhex, expect_res);

    cace_ari_deinit(&value);
}

// References are based on ari://2/CONST/4
TEST_CASE("0A", "8402022104", "0A", 0)
TEST_CASE("0A", "8502022104810A", "0A", 0) // [10] not used, but not an error
// FIXME: TEST_CASE("820E00", "8502022104810A", "0A", 0)     // [10] label substituted by index
// FIXME: TEST_CASE("820E626869", "8502022104810A", "0A", 0) // [10] label substituted by name
void test_const_produce_param_one_int(const char *valhex, const char *refhex, const char *outhex, int expect_res)
{
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(obj.fparams);

        fparam->index = 0;
        m_string_set_cstr(fparam->name, "hi");

        TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_INT));
    }

    // initial state
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&(desc.value), valhex));

    cace_ari_t value = CACE_ARI_INIT_UNDEFINED;
    check_produce(&value, refhex, outhex, expect_res);

    cace_ari_deinit(&value);
}
