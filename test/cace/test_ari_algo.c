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
/** @file
 * Test the ari_algo.h interfaces.
 */
#include <cace/ari/algo.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <m-dict.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

M_BPTREE_DEF2(test_seen_ari, 4, string_t, STRING_OPLIST, cace_ari_t, M_OPL_cace_ari_t())

/// A collection of unique ARIs in binary form for testing comparisons
static const char *different_aris[] = {
    "F7",             // ari:undefined
    "F6",             // ari:null
    "F4",             // ari:false
    "1864",           // ari:100
    "F90000",         // ari:0.0
    "F97E00",         // ari:NaN
    "40",             // ari:h''
    "426869",         // ari:h'6869'
    "60",             // ari:%22%22
    "626869",         // ari:%22hi%22
    "8201F4",         // ari:/bool/false
    "8201F5",         // ari:/bool/true
    "82021864",       // ari:/byte/100
    "82041864",       // ari:/int/100
    "82051864",       // ari:/uint/100
    "82061864",       // ari:/vast/100
    "82071864",       // ari:/uvast/100
    "8208F95640"      // ari:/real32/100.0
    "8209F95640"      // ari:/real64/100.0
    "821180",         // ari:/AC/()
    "821182F6820417", // ari:/AC/(null,/INT/23)
    "8212A0",         // ari:/AM/()
    "8212A101F5",     // ari:/AM/(1=true)
    "8212A11864F5",   // ari:/AM/(100=true)
    "8212A1F90000F5", // ari:/AM/(0.0=true)
    "8212A1F97E00F5", // ari:/AM/(NaN=true)
    "82138103",       // ari:/TBL/c=3;
    "82138403010203", // ari:/TBL/c=3;(1,2,3)

    "85676578616D706C656474657374646374726C6474686174811822", // ari://test/ctrl/that(34)
    "8519FFFF02220481626869",                                 // ari://65535/2/-3/4(hi)
    NULL,
};

/// Cached decoding
static test_seen_ari_t decode_cache;

void suiteSetUp(void)
{
    cace_openlog();

    test_seen_ari_init(decode_cache);
    for (const char **curs = different_aris; *curs != NULL; ++curs)
    {
        TEST_PRINTF("Adding value %s", *curs);

        string_t intext;
        string_init_set_str(intext, *curs);
        cace_data_t indata;
        cace_data_init(&indata);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_decode(&indata, intext), "cace_base16_decode() failed");

        cace_ari_t ari;
        cace_ari_init(&ari);
        int res = cace_ari_cbor_decode(&ari, &indata, NULL, NULL);
        cace_data_deinit(&indata);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

        {
            cace_ari_t *found = test_seen_ari_get(decode_cache, intext);
            TEST_ASSERT_NULL(found); // no duplicates
        }

        // insert with move
        cace_ari_t *val = test_seen_ari_safe_get(decode_cache, intext);
        TEST_ASSERT_NOT_NULL(val);
        cace_ari_set_move(val, &ari);
        string_clear(intext);
    }
}

int suiteTearDown(int failures)
{
    test_seen_ari_clear(decode_cache);
    cace_closelog();
    return failures;
}

void test_ari_hash(void)
{
    test_seen_ari_it_t it;
    for (test_seen_ari_it(it, decode_cache); !test_seen_ari_end_p(it); test_seen_ari_next(it))
    {
        const test_seen_ari_itref_t *pair = test_seen_ari_cref(it);
        TEST_ASSERT_NOT_NULL(pair);
        TEST_PRINTF("Hashing value %s", m_string_get_cstr(*(pair->key_ptr)));
        const size_t val_hash = cace_ari_hash(pair->value_ptr);

        {
            // alternate decoding has identical hash
            cace_data_t indata;
            cace_data_init(&indata);
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_decode(&indata, *(pair->key_ptr)),
                                          "cace_base16_decode() failed");

            cace_ari_t ari_b;
            cace_ari_init(&ari_b);
            int res = cace_ari_cbor_decode(&ari_b, &indata, NULL, NULL);
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

            const size_t hash_b = cace_ari_hash(&ari_b);
            TEST_ASSERT_EQUAL_INT_MESSAGE(val_hash, hash_b, "hashes from same data differ");
            cace_ari_deinit(&ari_b);
            cace_data_deinit(&indata);
        }

        // different than all other values so far
        test_seen_ari_it_t oth_it;
        for (test_seen_ari_it(oth_it, decode_cache); !test_seen_ari_it_equal_p(oth_it, it); test_seen_ari_next(oth_it))
        {
            const test_seen_ari_itref_t *oth_pair = test_seen_ari_cref(oth_it);

            const size_t oth_hash = cace_ari_hash(oth_pair->value_ptr);
            TEST_ASSERT_NOT_EQUAL_INT(val_hash, oth_hash);
        }
    }
}

void test_ari_cmp(void)
{
    test_seen_ari_it_t it;
    for (test_seen_ari_it(it, decode_cache); !test_seen_ari_end_p(it); test_seen_ari_next(it))
    {
        const test_seen_ari_itref_t *pair = test_seen_ari_cref(it);
        TEST_ASSERT_NOT_NULL(pair);
        TEST_PRINTF("Comparing value %s", m_string_get_cstr(*pair->key_ptr));

        {
            // alternate decoding has identical hash
            cace_data_t indata;
            cace_data_init(&indata);
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_decode(&indata, *pair->key_ptr),
                                          "cace_base16_decode() failed");

            cace_ari_t ari_b;
            cace_ari_init(&ari_b);
            int res = cace_ari_cbor_decode(&ari_b, &indata, NULL, NULL);
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

            TEST_ASSERT_EQUAL_INT(0, cace_ari_cmp(pair->value_ptr, &ari_b));
            cace_ari_deinit(&ari_b);
            cace_data_deinit(&indata);
        }

        // different than all other values so far
        test_seen_ari_it_t oth_it;
        for (test_seen_ari_it(oth_it, decode_cache); !test_seen_ari_it_equal_p(oth_it, it); test_seen_ari_next(oth_it))
        {
            const test_seen_ari_itref_t *oth_pair = test_seen_ari_cref(oth_it);

            TEST_ASSERT_NOT_EQUAL_INT(0, cace_ari_cmp(pair->value_ptr, oth_pair->value_ptr));
        }
    }
}

void test_ari_equal(void)
{
    test_seen_ari_it_t it;
    for (test_seen_ari_it(it, decode_cache); !test_seen_ari_end_p(it); test_seen_ari_next(it))
    {
        const test_seen_ari_itref_t *pair = test_seen_ari_cref(it);
        TEST_ASSERT_NOT_NULL(pair);
        TEST_PRINTF("Comparing value %s", m_string_get_cstr(*pair->key_ptr));

        {
            // alternate decoding has identical hash
            cace_data_t indata;
            cace_data_init(&indata);
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_decode(&indata, *pair->key_ptr),
                                          "cace_base16_decode() failed");

            cace_ari_t ari_b;
            cace_ari_init(&ari_b);
            int res = cace_ari_cbor_decode(&ari_b, &indata, NULL, NULL);
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

            TEST_ASSERT_TRUE(cace_ari_equal(pair->value_ptr, &ari_b));
            cace_ari_deinit(&ari_b);
            cace_data_deinit(&indata);
        }

        // different than all other values so far
        test_seen_ari_it_t oth_it;
        for (test_seen_ari_it(oth_it, decode_cache); !test_seen_ari_it_equal_p(oth_it, it); test_seen_ari_next(oth_it))
        {
            const test_seen_ari_itref_t *oth_pair = test_seen_ari_cref(oth_it);

            TEST_ASSERT_FALSE(cace_ari_equal(pair->value_ptr, oth_pair->value_ptr));
        }
    }
}
