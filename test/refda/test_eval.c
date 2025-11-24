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
#include <refda/eval.h>
#include <refda/register.h>
#include <refda/edd_prod_ctx.h>
#include <refda/oper_eval_ctx.h>
#include <refda/amm/const.h>
#include <refda/amm/edd.h>
#include <cace/amm/semtype.h>
#include <cace/amm/numeric.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text.h>
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

static void test_reporting_edd_one_int(refda_edd_prod_ctx_t *ctx)
{
    const cace_ari_t *param = refda_edd_prod_ctx_get_aparam_index(ctx, 0);
    CHKVOID(param);
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, param, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("EDD production with parameter %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    refda_edd_prod_ctx_set_result_copy(ctx, param);
}

static int ari_numeric_add(cace_ari_t *result, const cace_ari_t *lt_val, const cace_ari_t *rt_val)
{
    cace_ari_type_t promote;
    if (cace_amm_numeric_promote_type(&promote, lt_val, rt_val))
    {
        return 2;
    }

    const cace_amm_type_t *amm_promote = cace_amm_type_get_builtin(promote);
    cace_ari_t             lt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_ari_t             rt_prom     = CACE_ARI_INIT_UNDEFINED;
    cace_amm_type_convert(amm_promote, &lt_prom, lt_val);
    cace_amm_type_convert(amm_promote, &rt_prom, rt_val);

    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    int retval = 0;
    switch (lt_prom.as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            res_lit->value.as_uint64 = lt_prom.as_lit.value.as_uint64 + rt_prom.as_lit.value.as_uint64;
            break;
        case CACE_ARI_PRIM_INT64:
            res_lit->value.as_int64 = lt_prom.as_lit.value.as_int64 + rt_prom.as_lit.value.as_int64;
            break;
        case CACE_ARI_PRIM_FLOAT64:
            res_lit->value.as_float64 = lt_prom.as_lit.value.as_float64 + rt_prom.as_lit.value.as_float64;
            break;
        default:
            // leave lit as default undefined
            retval = 3;
            break;
    }

    if (!retval)
    {
        res_lit->prim_type    = lt_prom.as_lit.prim_type;
        res_lit->has_ari_type = true;
        res_lit->ari_type     = promote;
    }

    cace_ari_deinit(&lt_prom);
    cace_ari_deinit(&rt_prom);
    return retval;
}

static void test_reporting_oper_add(refda_oper_eval_ctx_t *ctx)
{
    const cace_ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);

    const cace_ari_t *more = refda_oper_eval_ctx_get_aparam_index(ctx, 0);
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, more, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("OPER evaluation with parameter %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    int        retval = 0;
    cace_ari_t tmp1   = CACE_ARI_INIT_UNDEFINED;
    cace_ari_t tmp2   = CACE_ARI_INIT_UNDEFINED;
    if (ari_numeric_add(&tmp1, left, right))
    {
        retval = 2;
    }
    if (!retval)
    {
        if (ari_numeric_add(&tmp2, &tmp1, more))
        {
            retval = 2;
        }
    }
    cace_ari_deinit(&tmp1);

    if (!retval)
    {
        refda_oper_eval_ctx_set_result_move(ctx, &tmp2);
    }
    else
    {
        cace_ari_deinit(&tmp2);
    }
}

static void suite_adms_init(refda_agent_t *agent)
{
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
            {
                cace_ari_ac_t *acinit = cace_ari_set_ac(&(objdata->value), NULL);
                {
                    cace_ari_t *item = cace_ari_list_push_back_new(acinit->items);
                    // ari://example/adm/EDD/edd1
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_EDD, 2);
                }
                {
                    cace_ari_t *item = cace_ari_list_push_back_new(acinit->items);
                    // ari://example/adm/VAR/var1
                    cace_ari_set_objref_path_intid(item, EXAMPLE_ORG_ENUM, EXAMPLE_ADM_ENUM, CACE_ARI_TYPE_VAR, 1);
                }
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
            assert(0 == cace_amm_type_set_use_builtin(&(objdata->val_type), CACE_ARI_TYPE_VAST));
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
            assert(0 == cace_amm_type_set_use_builtin(&(objdata->prod_type), CACE_ARI_TYPE_VAST));
            objdata->produce = test_reporting_edd_one_int;

            obj = refda_register_edd(adm, cace_amm_idseg_ref_withenum("edd2", 2), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "val");
                assert(0 == cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_VAST));
            }
        }

        /**
         * Register OPER objects
         */
        {
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);

            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                m_string_set_cstr(operand->name, "left");
                assert(0 == cace_amm_type_set_use_builtin(&(operand->typeobj), CACE_ARI_TYPE_VAST));
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                m_string_set_cstr(operand->name, "right");
                assert(0 == cace_amm_type_set_use_builtin(&(operand->typeobj), CACE_ARI_TYPE_VAST));
            }
            assert(0 == cace_amm_type_set_use_builtin(&(objdata->res_type), CACE_ARI_TYPE_VAST));

            objdata->evaluate = test_reporting_oper_add;

            obj = refda_register_oper(adm, cace_amm_idseg_ref_withenum("oper1", 1), objdata);
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "more");
                assert(0 == cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_VAST));
            }
        }
    }

    int res = refda_agent_bindrefs(agent);
    assert(0 == res);
}

// ari:/AC/(/TBL/c=3;(1,1,1)(2,2,2),//1/1/OPER/tbl-filter(/AC/(false),/AC/())) -> /TBL/c=0;
TEST_CASE("82118282138703010101020202850101256A74626C2D66696C74657282821181F4821180", "82138100")

// ari:/AC/(/TBL/c=3;(1,1,1)(2,2,2),//1/1/OPER/tbl-filter(/AC/(1),/AC/())) -> /TBL/c=0;
// ari:/AC/(/TBL/c=3;(1,1,1)(2,2,2),//1/1/OPER/tbl-filter(/AC/(true),/AC/(0,1,2))) -> /TBL/c=3;(1,1,1)(2,2,2)

// ari:/AC/(/TBL/c=3;(1,1,1)(2,2,2),//1/1/OPER/tbl-filter(/AC/(),/AC/())) -> /TBL/c=3;(1,1,1)(2,2,2)
// TEST_CASE("82118282138703010101020202850101256A74626C2D66696C74657282821180821180", "82138703010101020202")
/*
// direct EXPR ari:/AC/(/VAST/1) -> /VAST/1
TEST_CASE("821181820601", "820601")
// ref EXPR ari:/AC/(//65535/10/EDD/2(10)) -> /VAST/10
TEST_CASE("8211818519FFFF0A2302810A", "82060A")
// ari:/AC/(/VAST/3,/VAST/5,//65535/10/OPER/1(10)) -> /VAST/18
TEST_CASE("8211838206038206058519FFFF0A2501810A", "820612")
// ari:/AC/(//65535/10/EDD/2(10),//65535/10/VAR/1,//65535/10/OPER/1(10)) -> /VAST/123476
TEST_CASE("8211838519FFFF0A2302810A8419FFFF0A2A018519FFFF0A2501810A", "82061A0001E254")

// ari:/AC/(/BOOL/false,//ietf/dtnma-agent/OPER/bool-not) -> /BOOL/true
TEST_CASE("8211828201F48464696574666B64746E6D612D6167656E742568626F6F6C2D6E6F74", "8201F5")
// ari:/AC/(/BOOL/true,//1/1/OPER/bool-not) -> /BOOL/false
TEST_CASE("8211828201F58401012568626F6F6C2D6E6F74", "8201F4")
//  ari:/AC/(/VAST/0,//1/1/OPER/bool-not) -> /BOOL/true
TEST_CASE("8211828206008401012568626F6F6C2D6E6F74", "8201F5")
// ari:/AC/(/TEXTSTR/test,//1/1/OPER/bool-not) -> /BOOL/false
TEST_CASE("821182820A64746573748401012568626F6F6C2D6E6F74", "8201F4")

// ari:/AC/(/BOOL/true,/BOOL/false,//ietf/dtnma-agent/OPER/bool-and) -> /BOOL/false
TEST_CASE("8211838201F58201F48464696574666B64746E6D612D6167656E742568626F6F6C2D616E64", "8201F4")
// ari:/AC/(/BOOL/false,/BOOL/false,//1/1/OPER/bool-and) -> /BOOL/false
TEST_CASE("8211838201F48201F48401012568626F6F6C2D616E64", "8201F4")
// ari:/AC/(/BOOL/false,/VAST/0,//1/1/OPER/bool-and) -> /BOOL/false
TEST_CASE("8211838201F48206008401012568626F6F6C2D616E64", "8201F4")
// ari:/AC/(/BOOL/true,/TEXTSTR/test,//1/1/OPER/bool-and) -> /BOOL/true
TEST_CASE("8211838201F5820A64746573748401012568626F6F6C2D616E64", "8201F5")

// ari:/AC/(/BOOL/true,/BOOL/false,//ietf/dtnma-agent/OPER/bool-or) -> /BOOL/true
TEST_CASE("8211838201F58201F48464696574666B64746E6D612D6167656E742567626F6F6C2D6F72", "8201F5")
// ari:/AC/(/BOOL/false,/BOOL/false,//1/1/OPER/bool-or) -> /BOOL/false
TEST_CASE("8211838201F48201F48401012567626F6F6C2D6F72", "8201F4")
// ari:/AC/(/BOOL/false,/VAST/0,//1/1/OPER/bool-or) -> /BOOL/false
TEST_CASE("8211838201F48206008401012567626F6F6C2D6F72", "8201F4")
// ari:/AC/(/BOOL/true,/TEXTSTR/test,//1/1/OPER/bool-or) -> /BOOL/true
TEST_CASE("8211838201F5820A64746573748401012567626F6F6C2D6F72", "8201F5")

// ari:/AC/(/BOOL/true,/BOOL/false,//ietf/dtnma-agent/OPER/bool-xor) -> /BOOL/true
TEST_CASE("8211838201F58201F48464696574666B64746E6D612D6167656E742568626F6F6C2D786F72", "8201F5")
// ari:/AC/(/BOOL/false,/BOOL/false,//1/1/OPER/bool-xor) -> /BOOL/false
TEST_CASE("8211838201F48201F48401012568626F6F6C2D786F72", "8201F4")
// ari:/AC/(/BOOL/false,/VAST/0,//1/1/OPER/bool-xor) -> /BOOL/false
TEST_CASE("8211838201F48206008401012568626F6F6C2D786F72", "8201F4")
// ari:/AC/(/BOOL/true,/TEXTSTR/test,//1/1/OPER/bool-xor) -> /BOOL/false
TEST_CASE("8211838201F5820A64746573748401012568626F6F6C2D786F72", "8201F4")

// ari:/AC/(/INT/5,/INT/0xF,//1/1/OPER/bit-and) -> /INT/5
TEST_CASE("82118382040582040F84010125676269742D616E64", "820405")
// ari:/AC/(/INT/5,/UVAST/0xF,//1/1/OPER/bit-and) -> /VAST/5
TEST_CASE("82118382040582070F84010125676269742D616E64", "820605")

// ari:/AC/(/INT/5,/INT/0xF,//1/1/OPER/bit-or) -> /INT/15
TEST_CASE("82118382040582040F84010125666269742D6F72", "82040F")
// ari:/AC/(/INT/5,/UVAST/0xF,//1/1/OPER/bit-or) -> /VAST/15
TEST_CASE("82118382040582070F84010125666269742D6F72", "82060F")

// ari:/AC/(/INT/5,/INT/0xF,//1/1/OPER/bit-xor) -> /INT/10
TEST_CASE("82118382040582040F84010125676269742D786F72", "82040A")
// ari:/AC/(/INT/5,/UVAST/0xF,//1/1/OPER/bit-xor) -> /VAST/10
TEST_CASE("82118382040582070F84010125676269742D786F72", "82060A")

// ari:/AC/(/INT/0xF,//1/1/OPER/bit-not) -> /INT/0xFFFFFFF0
TEST_CASE("82118282040F84010125676269742D6E6F74", "82042F")
// FIXME:
//// ari:/AC/(/UVAST/0xF,//1/1/OPER/bit-not) -> /UVAST/0xFFFFFFF0
// TEST_CASE("82118282070F84010125676269742D6E6F74", "82072F")

// ari:/AC/(/INT/0x1,/INT/2,//1/1/OPER/add) -> /INT/3
TEST_CASE("8211838204018204028401012563616464", "820403")
// ari:/AC/(/INT/0x1,/UVAST/16,//1/1/OPER/add) -> /VAST/17
TEST_CASE("8211838204018207108401012563616464", "820611")

// ari:/AC/(/REAL64/32,/UVAST/16,//1/1/OPER/add) -> /REAL64/48
TEST_CASE("8211838209F950008207108401012563616464", "8209F95200")

// ari:/AC/(/BYTE/0x4,/UVAST/16,//1/1/OPER/sub) -> /VAST/12
TEST_CASE("8211838202048207108401012563737562", "82070C")
// ari:/AC/(/INT/0x1,/UVAST/16,//1/1/OPER/sub) -> /VAST/15
TEST_CASE("8211838204018207108401012563737562", "82060F")
// ari:/AC/(/INT/0x1,/UVAST/16,//1/1/OPER/multiply) -> /VAST/16
TEST_CASE("82118382040182071084010125686D756C7469706C79", "820610")
// ari:/AC/(/INT/0x10,/UVAST/1,//1/1/OPER/divide) -> /VAST/0
TEST_CASE("8211838204108207018401012566646976696465", "820600")

// ari:/AC/(/INT/0x10,/UVAST/1,//1/1/OPER/remainder) -> /VAST/1
// TEST_CASE("8211838204108207018301256972656D61696E646572", "820601")
// ari:/AC/(/INT/3,/UVAST/7,//1/OPER/remainder) -> /VAST/1
TEST_CASE("821183820410820701840101256972656D61696E646572", "820601")

// ari:/AC/(/INT/3,//1/1/OPER/negate) -> /INT/-3
TEST_CASE("82118282040384010125666E6567617465", "820422")
// ari:/AC/(/UINT/3,//1/1/OPER/negate) -> /INT/-3
TEST_CASE("82118282050384010125666E6567617465", "820422")
// ari:/AC/(/REAL32/3,//1/1/OPER/negate) -> /REAL32/-3
TEST_CASE("8211828208F9420084010125666E6567617465", "8208F9C200")

// ari:/AC/(/INT/2,/INT/2,//1/1/OPER/compare-eq) -> /BOOL/true
TEST_CASE("821183820402820402840101256A636F6D706172652D6571", "8201F5")
// ari:/AC/(/INT/2,/INT/3,//1/1/OPER/compare-ne) -> /BOOL/true
TEST_CASE("821183820402820403840101256A636F6D706172652D6E65", "8201F5")
// ari:/AC/(/INT/2,/INT/46,//1/1/OPER/compare-gt) -> /BOOL/true
TEST_CASE("8211838204028204182E840101256A636F6D706172652D6774", "8201F5")
// ari:/AC/(/REAL32/2,/VAST/46,//1/1/OPER/compare-gt) -> /BOOL/true
// FIXME: need to add builtin_float32_convert in typing.c
TEST_CASE("8211838208F940008206182E840101256A636F6D706172652D6774", "8201F5")
// ari:/AC/(/INT/2,/INT/2,//1/1/OPER/compare-ge) -> /BOOL/true
TEST_CASE("821183820402820402840101256A636F6D706172652D6765", "8201F5")
// ari:/AC/(/INT/46,/UVAST/2,//1/1/OPER/compare-lt) -> /BOOL/true
TEST_CASE("8211838204182E820702840101256A636F6D706172652D6C74", "8201F5")
// ari:/AC/(/INT/46,/UINT/46,//1/1/OPER/compare-le) -> /BOOL/true
TEST_CASE("8211838204182E8205182E840101256A636F6D706172652D6C65", "8201F5")

// ari:/AC/(/BOOL/false,undefined,//1/1/OPER/bool-and) -> undefined
TEST_CASE("8211838201F4F78401012568626F6F6C2D616E64", "F7")
// ari:/AC/(/INT/0x0,/UVAST/1,//1/1/OPER/divide) -> undefined
TEST_CASE("8211838204008207018401012566646976696465", "F7")
// ari:/AC/(/INT/0x0,/UVAST/1,//1/1/OPER/remainder) -> undefined
TEST_CASE("821183820400820701840101256972656D61696E646572", "F7")

// Test addition of timespec related logic:
//
// --- Addition of TP and TD value (in either order) ---
// ari:/AC/(/TD/1000,/TP/20000101T001640Z,//1/1/OPER/add) -> /TP/20000101T003320Z
TEST_CASE("821183820D1903E8820C1903E88401012563616464", "820C1907D0")
// ari:/AC/(/TP/20000101T001640Z,/TD/1000,//1/1/OPER/add) -> /TP/20000101T003320Z
TEST_CASE("821183820C1903E8820D1903E88401012563616464", "820C1907D0")
//
// --- Addition of 2 TD values ---
// ari:/AC/(/TD/5000,/TD/7000,//1/1/OPER/add) -> /TD/12000
TEST_CASE("821183820D191388820D191B588401012563616464", "820D192EE0")
// ari:/AC/(/TD/7000,/TD/5000,//1/1/OPER/add) -> /TD/12000
TEST_CASE("821183820D191B58820D1913888401012563616464", "820D192EE0")
// ari:/AC/(/TD/4.75,/TD/2.75,//1/1/OPER/add) -> /TD/7.5
TEST_CASE("821183820D82211901DB820D82211901138401012563616464", "820D8220184B")

// Test subtraction of timespec related logic:
//
// --- Subtraction of 2 TP values ---
// ari:/AC/(/TP/20000101T001640Z,/TP/20000101T001640Z,//1/1/OPER/sub) -> /TD/0
TEST_CASE("821183820C1903E8820C1903E88401012563737562", "820D00")
// ari:/AC/(/TP/20000101T001600Z,/TP/20000101T001640Z,//1/1/OPER/sub) -> /TD/40
TEST_CASE("821183820C1903C0820C1903E88401012563737562", "820D1828")
// ari:/AC/(/TP/20000101T001640Z,/TP/20000101T001600Z,//1/1/OPER/sub) -> /TD/-40
TEST_CASE("821183820C1903E8820C1903C08401012563737562", "820D3827")
//
// --- Subtraction of 2 TD values ---
// ari:/AC/(/TD/4000,/TD/7000,//1/1/OPER/sub) -> /TD/3000
TEST_CASE("821183820D190FA0820D191B588401012563737562", "820D190BB8")
// ari:/AC/(/TD/7000,/TD/4000,//1/1/OPER/sub) -> /TD/-3000
TEST_CASE("821183820D191B58820D190FA08401012563737562", "820D390BB7")
// ari:/AC/(/TD/0.25,/TD/3.75,//1/1/OPER/sub) -> /TD/3.5
TEST_CASE("821183820D82211819820D82211901778401012563737562", "820D82201823")
//
// --- Subtraction of TD from TP value ---
// ari:/AC/(/TD/1000,/TP/20000101T001640Z,//1/1/OPER/sub) -> /TP/20000101T000000Z
TEST_CASE("821183820D1903E8820C1903E88401012563737562", "820C00")
//
//-----------------------------------------------------------------------------------------------------------
// TODO: The 2 unit tests below fail and it may be due to bad parsing of negative fractional TD values.
// // ari:/AC/(/TD/-2.25,/TD/0.75,//1/1/OPER/sub) -> /TD/3
// TEST_CASE("821183820D822138AE820D8221184B8401012563737562", "820D03")
// // ari:/AC/(/TD/3.25,/TD/0.75,//1/1/OPER/sub) -> /TD/-2.5
// TEST_CASE("821183820D8221190145820D8221184B8401012563737562", "820D82202E")

// Test scaling (multiplication / division) of TD with a primitive (int or float):
//
// --- Multiply of TD value with primitive (int or float) ---
// ari:/AC/(/INT/0,/TD/7000,//1/1/OPER/multiply) -> /TD/0
TEST_CASE("821183820400820D191B5884010125686D756C7469706C79", "820D00")
// ari:/AC/(/INT/1,/TD/7000,//1/1/OPER/multiply) -> /TD/7000
TEST_CASE("821183820401820D191B5884010125686D756C7469706C79", "820D191B58")
// ari:/AC/(/INT/7,/TD/7000,//1/1/OPER/multiply) -> /TD/49000
TEST_CASE("821183820407820D191B5884010125686D756C7469706C79", "820D19BF68")
// ari:/AC/(/REAL32/3.5,/TD/7000,//1/1/OPER/multiply) -> /TD/24500
TEST_CASE("8211838208F94300820D191B5884010125686D756C7469706C79", "820D195FB4")
// ari:/AC/(/REAL32/INFINITY,/TD/7000,//1/1/OPER/multiply) -> undefined
TEST_CASE("8211838208F97C00820D191B5884010125686D756C7469706C79", "F7") // Undefined Behavoir
// ari:/AC/(/INT/4,/TD/0.25,//1/1/OPER/multiply) -> /TD/1
TEST_CASE("821183820404820D8221181984010125686D756C7469706C79", "820D01")
// ari:/AC/(/INT/15,/TD/0.25,//1/1/OPER/multiply) -> /TD/3.75
TEST_CASE("82118382040F820D8221181984010125686D756C7469706C79", "820D8221190177")
// ari:/AC/(/REAL32/2.50,/TD/2.50,//1/1/OPER/multiply) -> /TD/6.25
TEST_CASE("8211838208F94100820D8220181984010125686D756C7469706C79", "820D8221190271")
// ari:/AC/(/REAL32/0.75,/TD/1.00,//1/1/OPER/multiply) -> /TD/0.75
TEST_CASE("8211838208F93A00820D0184010125686D756C7469706C79", "820D8221184B")
// ari:/AC/(/REAL32/9e6,/TD/1.99,//1/1/OPER/multiply) -> /TD/17910000
TEST_CASE("8211838208FA4B095440820D822118C784010125686D756C7469706C79", "820D1A011148F0")
//
//-----------------------------------------------------------------------------------------------------------
// TODO: The test cases below result in very high CPU usage if timespec_normalise() method is used over the
// TODO: custom timespec_normalize() method. The 2nd test cases below will also fail (unrelated to performance)
// TODO: and it appears to be due to integer/long overflow.
// ari:/AC/(/VAST/1234567890,/TD/0.9999,//1/1/OPER/multiply) -> /TD/1234444433.211
TEST_CASE("82118382061A499602D2820D822319270F84010125686D756C7469706C79", "820D82221B0000011F6A9F373B")
// ari:/AC/(/VAST/12345678909,/TD/0.9999,//1/1/OPER/multiply) -> /TD/12344444341.1091
// TEST_CASE("82118382061B00000002DFDC1C3D820D822319270F84010125686D756C7469706C79", "820D82283B54AFB946829C721F")

//
// --- Division of TD value with primitive (int or float) ---
// ari:/AC/(/INT/1,/TD/7000,//1/1/OPER/divide) -> /TD/7000
TEST_CASE("821183820401820D191B588401012566646976696465", "820D191B58")
// ari:/AC/(/INT/7000,/TD/49000,//1/1/OPER/divide) -> /TD/7
TEST_CASE("8211838204191B58820D19BF688401012566646976696465", "820D07")
// ari:/AC/(/INT/2,/TD/7,//1/1/OPER/divide) -> /TD/3.5
TEST_CASE("821183820402820D078401012566646976696465", "820D82201823")
// ari:/AC/(/REAL32/2,/TD/7,//1/1/OPER/divide) -> /TD/3.5
TEST_CASE("8211838208F94000820D078401012566646976696465", "820D82201823")
// ari:/AC/(/REAL32/3.5,/TD/7000,//1/1/OPER/divide) -> /TD/2000
TEST_CASE("8211838208F94300820D191B588401012566646976696465", "820D1907D0")
// ari:/AC/(/REAL32/0.0,/TD/7000,//1/1/OPER/divide) -> undefined
TEST_CASE("8211838208F90000820D191B588401012566646976696465", "F7") // Undefined Behavoir
// ari:/AC/(/REAL32/-INFINITY,/TD/7000,//1/1/OPER/divide) -> /TD/0
TEST_CASE("8211838208F9FC00820D191B588401012566646976696465", "820D00")

// Test various undefined behavoir involving TD and TP values
//
// --- Multiplication of primitive (int or float) with a TD value ---
// ari:/AC/(/TD/7000,/INT/1,//1/1/OPER/multiply) -> undefined
TEST_CASE("821183820D191B5882040184010125686D756C7469706C79", "F7")
// ari:/AC/(/TD/7000,/REAL32/3.5,//1/1/OPER/multiply) -> undefined
TEST_CASE("821183820D191B588208F9430084010125686D756C7469706C79", "F7")
//
// --- Division of primitive (int or float) with a TD value ---
// ari:/AC/(/TD/7000,/INT/1,//1/1/OPER/divide) -> undefined
TEST_CASE("821183820D191B588204018401012566646976696465", "F7")
// ari:/AC/(/TD/7000,/REAL32/3.5,//1/1/OPER/divide) -> undefined
TEST_CASE("821183820D191B588208F943008401012566646976696465", "F7")
//
// --- Addition,Subtraction,Multiplication,Division of TP value with a primitive (int) (in either order) ---
// ari:/AC/(/TP/20000101T001640Z,/INT/1000,//1/1/OPER/add) -> undefined
TEST_CASE("821183820C1903E882041903E88401012563616464", "F7")
// ari:/AC/(/INT/1000,/TP/20000101T001640Z,//1/1/OPER/add) -> undefined
TEST_CASE("82118382041903E8820C1903E88401012563616464", "F7")
// ari:/AC/(/TP/20000101T001640Z,/INT/1000,//1/1/OPER/sub) -> undefined
TEST_CASE("821183820C1903E882041903E88401012563737562", "F7")
// ari:/AC/(/INT/1000,/TP/20000101T001640Z,//1/1/OPER/sub) -> undefined
TEST_CASE("82118382041903E8820C1903E88401012563737562", "F7")
// ari:/AC/(/TP/20000101T001640Z,/INT/1000,//1/1/OPER/multiply) -> undefined
TEST_CASE("821183820C1903E882041903E884010125686D756C7469706C79", "F7")
// ari:/AC/(/INT/1000,/TP/20000101T001640Z,//1/1/OPER/multiply) -> undefined
TEST_CASE("82118382041903E8820C1903E884010125686D756C7469706C79", "F7")
// ari:/AC/(/TP/20000101T001640Z,/INT/1000,//1/1/OPER/divide) -> undefined
TEST_CASE("821183820C1903E882041903E88401012566646976696465", "F7")
// ari:/AC/(/INT/1000,/TP/20000101T001640Z,//1/1/OPER/divide) -> undefined
TEST_CASE("82118382041903E8820C1903E88401012566646976696465", "F7")
*/
void test_refda_eval_target_check(const char *targethex, const char *expectloghex)
{
    // TODO: Later, post version 2.0 release, consider passing ARI text instead of ARI hex

    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    cace_ari_t expect_result = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&expect_result, expectloghex));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    int        res    = refda_eval_target(&runctx, &result, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "refda_eval_target() disagrees");

    // verify result value
    const bool equal = cace_ari_equal(&expect_result, &result);
    TEST_ASSERT_TRUE_MESSAGE(equal, "result ARI is different");

    cace_ari_deinit(&result);
    refda_runctx_deinit(&runctx);
    cace_ari_deinit(&expect_result);
    cace_ari_deinit(&target);
}

TEST_CASE("821180", 6)             // Empty stack ari:/AC/()
TEST_CASE("821182820601820602", 6) // Extra stack ari:/AC/(/VAST/1,/VAST/2)
void test_refda_eval_target_failure(const char *targethex, int expect_res)
{

    cace_ari_t target = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_EQUAL_INT(0, test_util_ari_decode(&target, targethex));

    refda_runctx_t runctx;
    TEST_ASSERT_EQUAL_INT(0, test_util_runctx_init(&runctx, &agent));

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    int        res    = refda_eval_target(&runctx, &result, &target);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "refda_eval_target() disagrees");

    refda_runctx_deinit(&runctx);
    cace_ari_deinit(&result);
    cace_ari_deinit(&target);
}
