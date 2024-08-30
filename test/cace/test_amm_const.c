/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
#include <cace/amm/const.h>
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

static void check_produce(ari_t *value, const cace_amm_const_desc_t *cnst, const ari_formal_param_list_t fparams,
                          const char *refhex, const char *outhex, int expect_res)
{
    ari_t inref = ARI_INIT_UNDEFINED;
    ari_convert(&inref, refhex);
    TEST_ASSERT_TRUE_MESSAGE(inref.is_ref, "invalid reference");

    cace_amm_valprod_ctx_t ctx;

    int res = cace_amm_valprod_ctx_init(&ctx, fparams, &inref);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "cace_amm_valprod_ctx_init() disagrees");

    res = cace_amm_const_desc_produce(cnst, &ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_const_desc_produce() failed");

    ari_t outval = ARI_INIT_UNDEFINED;
    ari_convert(&outval, outhex);

    TEST_ASSERT_TRUE_MESSAGE(ari_equal(&outval, &(ctx.value)), "produced value mismatch");

    // move out produced value
    TEST_ASSERT_EQUAL_INT(0, ari_set_move(value, &(ctx.value)));

    cace_amm_valprod_ctx_deinit(&ctx);
    ari_deinit(&outval);
    ari_deinit(&inref);
}

// References are based on ari://2/CONST/4
TEST_CASE("0A", "83022104", "0A", 0)
TEST_CASE("0A", "84022104810A", "0A", 2)   // [10] extra given param
TEST_CASE("0A", "84022104A1000A", "0A", 2) // {0:10} extra given param
void test_const_produce_param_none(const char *valhex, const char *refhex, const char *outhex, int expect_res)
{
    cace_amm_const_desc_t obj;
    cace_amm_const_desc_init(&obj);

    // leave formal parameter list empty
    ari_formal_param_list_t fparams;
    ari_formal_param_list_init(fparams);

    // initial state
    ari_convert(&(obj.value), valhex);

    ari_t value = ARI_INIT_UNDEFINED;
    check_produce(&value, &obj, fparams, refhex, outhex, expect_res);

    ari_deinit(&value);
    ari_formal_param_list_clear(fparams);
    cace_amm_const_desc_deinit(&obj);
}

// References are based on ari://2/CONST/4
TEST_CASE("0A", "83022104", "0A", 0)
TEST_CASE("0A", "84022104810A", "0A", 0)   // [10] not used, but not an error
TEST_CASE("0A", "84022104A1000A", "0A", 0) // {0:10} not used
TEST_CASE("0A", "84022104A1010A", "0A", 2) // {1:10} extra given param
// FIXME: TEST_CASE("820E00", "84022104810A", "0A", 0)     // [10] label substituted by index
// FIXME: TEST_CASE("820E626869", "84022104810A", "0A", 0) // [10] label substituted by name
void test_const_produce_param_one_int(const char *valhex, const char *refhex, const char *outhex, int expect_res)
{
    cace_amm_const_desc_t obj;
    cace_amm_const_desc_init(&obj);

    ari_formal_param_list_t fparams;
    ari_formal_param_list_init(fparams);
    {
        ari_formal_param_t *fparam = ari_formal_param_list_push_back_new(fparams);
        fparam->index              = 0;
        string_set_str(fparam->name, "hi");
        fparam->typeobj = amm_type_get_builtin(ARI_TYPE_INT);
    }

    // initial state
    ari_convert(&(obj.value), valhex);

    ari_t value = ARI_INIT_UNDEFINED;
    check_produce(&value, &obj, fparams, refhex, outhex, expect_res);

    ari_deinit(&value);
    ari_formal_param_list_clear(fparams);
    cace_amm_const_desc_deinit(&obj);
}
