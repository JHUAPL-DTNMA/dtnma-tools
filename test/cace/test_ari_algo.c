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
/** @file
 * Test the ari_algo.h interfaces.
 */
#include <cace/ari/algo.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/defs.h>
#include <m-dict.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

M_DICT_DEF2(test_seen_ari, string_t, STRING_OPLIST, ari_t, M_OPL_ari_t())

M_DICT_DEF2(test_seen_hash, string_t, STRING_OPLIST, size_t, M_BASIC_OPLIST)

/// A collection of unique ARIs in binary form for testing comparisons
static const char *different_aris[] = {
    "F7",     // ari:undefined
    "F6",     // ari:null
    "8201F4", // ari:false
    "8201F5", // ari:true
    "82041864",
    "82051864",
    "82061864",
    "82071864",
    "821180",                                 // ari:/AC/()
    "821182F6820417",                         // ari:/AC/(null,/INT/23)
    "8212A0",                                 // ari:/AM/()
    "8212A101F5",                             // ari:/AM/(1=true)
    "82138103",                               // ari:/TBL/c=3;
    "82138403010203",                         // ari:/TBL/c=3;(1,2,3)
    "84647465737464746869736474686174811822", // ari://test/this/that(34)
    "8402030481626869",                       // ari://2/3/4(hi)
    NULL,
};

void test_ari_hash()
{
    test_seen_hash_t hash_history;
    test_seen_hash_init(hash_history);

    for (const char **curs = different_aris; *curs != NULL; ++curs)
    {
        TEST_PRINTF("Adding value %s", *curs);

        string_t intext;
        string_init_set_str(intext, *curs);
        cace_data_t indata;
        cace_data_init(&indata);
        TEST_ASSERT_EQUAL_INT(0, base16_decode(&indata, intext));

        ari_t ari_a;
        ari_init(&ari_a);
        int   res = ari_cbor_decode(&ari_a, &indata, NULL, NULL);
        ari_t ari_b;
        ari_init(&ari_b);
        res += ari_cbor_decode(&ari_b, &indata, NULL, NULL);
        cace_data_deinit(&indata);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

        const size_t hash_a = ari_hash(&ari_a);
        const size_t hash_b = ari_hash(&ari_b);
        TEST_ASSERT_EQUAL_INT_MESSAGE(hash_a, hash_b, "hashes from same data differ");

        {
            size_t *found = test_seen_hash_get(hash_history, intext);
            TEST_ASSERT_NULL(found); // no duplicates
        }
        {
            test_seen_hash_it_t it;
            for (test_seen_hash_it(it, hash_history); !test_seen_hash_end_p(it); test_seen_hash_next(it))
            {
                test_seen_hash_itref_t *pair = test_seen_hash_ref(it);
                TEST_ASSERT_NOT_EQUAL_INT(hash_a, pair->value);
            }
        }
        test_seen_hash_set_at(hash_history, intext, hash_a);

        string_clear(intext);
        ari_deinit(&ari_a);
        ari_deinit(&ari_b);
    }

    test_seen_hash_clear(hash_history);
}

void test_ari_equal()
{
    static test_seen_ari_t ari_history;
    test_seen_ari_init(ari_history);

    for (const char **curs = different_aris; *curs != NULL; ++curs)
    {
        TEST_PRINTF("Adding value %s", *curs);

        string_t intext;
        string_init_set_str(intext, *curs);
        cace_data_t indata;
        cace_data_init(&indata);
        TEST_ASSERT_EQUAL_INT(0, base16_decode(&indata, intext));

        ari_t ari_a;
        ari_init(&ari_a);
        int   res = ari_cbor_decode(&ari_a, &indata, NULL, NULL);
        ari_t ari_b;
        ari_init(&ari_b);
        res += ari_cbor_decode(&ari_b, &indata, NULL, NULL);
        cace_data_deinit(&indata);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

        TEST_ASSERT_TRUE_MESSAGE(ari_equal(&ari_a, &ari_b), "ari_equal() differs");
        {
            ari_t *found = test_seen_ari_get(ari_history, intext);
            TEST_ASSERT_NULL(found); // no duplicates
        }
        {
            test_seen_ari_it_t it;
            for (test_seen_ari_it(it, ari_history); !test_seen_ari_end_p(it); test_seen_ari_next(it))
            {
                test_seen_ari_itref_t *pair = test_seen_ari_ref(it);
                TEST_ASSERT_FALSE(ari_equal(&ari_a, &(pair->value)));
            }
        }
        ari_t *val = test_seen_ari_safe_get(ari_history, intext);
        ari_set_move(val, &ari_a);

        string_clear(intext);
        ari_deinit(&ari_b);
    }

    test_seen_ari_clear(ari_history);
}
