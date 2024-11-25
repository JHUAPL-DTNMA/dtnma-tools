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
#include <refda/amm/const.h>
#include <cace/amm/semtype.h>
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

static void check_produce(ari_t *value, const refda_amm_const_desc_t *cnst, const cace_amm_formal_param_list_t fparams,
                          const char *refhex, const char *outhex, int expect_res)
{
    ari_t inref = ARI_INIT_UNDEFINED;
    ari_convert(&inref, refhex);
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_actual_param_set_populate(&(deref.aparams), fparams, &(inref.as_ref.params));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_actual_param_set_populate() failed");

    refda_valprod_ctx_t ctx;
    refda_valprod_ctx_init(&ctx, NULL, &deref);

    res = refda_amm_const_desc_produce(cnst, &ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_amm_const_desc_produce() mismatch");

    ari_t outval = ARI_INIT_UNDEFINED;
    ari_convert(&outval, outhex);

    TEST_ASSERT_TRUE_MESSAGE(ari_equal(&outval, &(ctx.value)), "produced value mismatch");

    // move out produced value
    TEST_ASSERT_EQUAL_INT(0, ari_set_move(value, &(ctx.value)));

    refda_valprod_ctx_deinit(&ctx);
    cace_amm_lookup_deinit(&deref);

    ari_deinit(&outval);
    ari_deinit(&inref);
}

// References are based on ari://2/CONST/4
TEST_CASE("0A", "83022104", "0A", 0)
void test_const_produce_param_none(const char *valhex, const char *refhex, const char *outhex, int expect_res)
{
    refda_amm_const_desc_t obj;
    refda_amm_const_desc_init(&obj);

    // leave formal parameter list empty
    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);

    // initial state
    ari_convert(&(obj.value), valhex);

    ari_t value = ARI_INIT_UNDEFINED;
    check_produce(&value, &obj, fparams, refhex, outhex, expect_res);

    ari_deinit(&value);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_const_desc_deinit(&obj);
}

// References are based on ari://2/CONST/4
TEST_CASE("0A", "83022104", "0A", 0)
TEST_CASE("0A", "84022104810A", "0A", 0)   // [10] not used, but not an error
// FIXME: TEST_CASE("820E00", "84022104810A", "0A", 0)     // [10] label substituted by index
// FIXME: TEST_CASE("820E626869", "84022104810A", "0A", 0) // [10] label substituted by name
void test_const_produce_param_one_int(const char *valhex, const char *refhex, const char *outhex, int expect_res)
{
    refda_amm_const_desc_t obj;
    refda_amm_const_desc_init(&obj);

    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);

        fparam->index              = 0;
        string_set_str(fparam->name, "hi");

        amm_type_set_use_direct(&(fparam->typeobj), amm_type_get_builtin(ARI_TYPE_INT));
    }

    // initial state
    ari_convert(&(obj.value), valhex);

    ari_t value = ARI_INIT_UNDEFINED;
    check_produce(&value, &obj, fparams, refhex, outhex, expect_res);

    ari_deinit(&value);
    cace_amm_formal_param_list_clear(fparams);
    refda_amm_const_desc_deinit(&obj);
}
