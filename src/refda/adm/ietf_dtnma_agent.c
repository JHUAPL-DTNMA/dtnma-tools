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
 * This is the compilation unit for the implementation of the
 * ADM module "ietf-dtnma-agent" for the C-language reference DA.
 * This contains definitions of every AMM object instance in the ADM and
 * file-local callback functions for all EDDs, CTRLs, and OPERs.
 */

#include "ietf_dtnma_agent.h"
#include "refda/agent.h"
#include "refda/register.h"
#include "refda/edd_prod_ctx.h"
#include "refda/ctrl_exec_ctx.h"
#include "refda/oper_eval_ctx.h"
#include "refda/exec.h"
#include "refda/reporting.h"
#include <cace/amm/semtype.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
#include "refda/eval.h"
#include "cace/amm/numeric.h"
#include "cace/ari/text_util.h"
#include <timespec.h>
#include <math.h>
/*   STOP CUSTOM INCLUDES HERE  */

/*   START CUSTOM FUNCTIONS HERE */

#define NANOS_IN_SEC 1000000000

/** Return code that indicates normal completion. */
#define RET_PASS 0

/** Return code that indicates the input resolves to an undefined result. */
#define RET_FAIL_UNDEFINED 1

/** Return code that indicates the input resolves to an unexpected/abnormal state. */
#define RET_FAIL_UNEXPECTED 2

/**
 * Ensure that the specified timespec is normalized. Normalization consists of the absolute value of the timespec nano
 * field being less than 1 billion.
 *
 * @param[inout] target The timespec that is to be normalized.
 */
static void timespec_normalize(struct timespec *target)
{
    // Turns out there is already timespec library code which performs this normalization. The timespec
    // normalization code may be slower due to looping (for example when multiplied by large scalars).
    //
    // TODO: The 2 lines below allow for a comparison against certain high CPU edge cases
    // *target = timespec_normalise(*target);
    // return;

    // Adjust the seconds and nanos such that the absolute value of the nanos field is < 1 billion
    long adj_sec = target->tv_nsec / NANOS_IN_SEC;
    target->tv_sec += adj_sec;

    target->tv_nsec -= (adj_sec * NANOS_IN_SEC);

    // Negative nanoseconds isn't valid according to POSIX. Decrement tv_sec and roll tv_nsec over.
    if (target->tv_nsec < 0)
    {
        --(target->tv_sec);
        target->tv_nsec = (NANOS_IN_SEC + target->tv_nsec);
    }
}

static void refda_adm_ietf_dtnma_agent_ctrl_wait_finished(refda_ctrl_exec_ctx_t *ctx)
{
    atomic_store(&(ctx->item->execution_stage), REFDA_EXEC_COMPLETE);
    refda_ctrl_exec_ctx_set_result_null(ctx);
}

static void refda_adm_ietf_dtnma_agent_ctrl_wait_cond_check(refda_ctrl_exec_ctx_t *ctx)
{
    const cace_ari_t *cond = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    if (!cond)
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    int        res    = refda_eval_target(ctx->runctx, &result, cond);
    if (res)
    {
        CACE_LOG_ERR("failed to evaluate condition, error %d", res);
        cace_ari_set_bool(&result, false);
    }

    cace_ari_t             as_bool = CACE_ARI_INIT_UNDEFINED;
    const cace_amm_type_t *typeobj = cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL);
    res                            = cace_amm_type_convert(typeobj, &as_bool, &result);
    if (res)
    {
        CACE_LOG_ERR("failed to get bool state, error %d", res);
        cace_ari_set_bool(&as_bool, false);
    }

    bool truthy;
    if (cace_ari_get_bool(&as_bool, &truthy))
    {
        CACE_LOG_ERR("failed to get bool value");
        truthy = false;
    }

    if (truthy)
    {
        refda_ctrl_exec_ctx_set_result_copy(ctx, &result);
    }
    else
    {
        struct timespec nowtime;

        int res = clock_gettime(CLOCK_REALTIME, &nowtime);
        if (res)
        {
            // handled as failure
            CACE_LOG_ERR("Failed clock_gettime()");
        }
        else
        {
            // check again in 1s
            refda_timeline_event_t event = {
                .purpose       = REFDA_TIMELINE_EXEC,
                .ts            = timespec_add(nowtime, timespec_from_ms(1000)),
                .exec.item     = ctx->item,
                .exec.callback = refda_adm_ietf_dtnma_agent_ctrl_wait_cond_check,
            };
            refda_ctrl_exec_ctx_set_waiting(ctx, &event);
        }
    }

    cace_ari_deinit(&as_bool);
    cace_ari_deinit(&result);
}

static void refda_adm_ietf_dtnma_agent_set_objpath(cace_ari_objpath_t *path, const cace_amm_obj_ns_t *ns,
                                                   cace_ari_type_t obj_type, const cace_amm_obj_desc_t *obj)
{
    if (ns->org_id.has_intenum)
    {
        path->org_id.form   = CACE_ARI_IDSEG_INT;
        path->org_id.as_int = ns->org_id.intenum;
    }
    else
    {
        path->org_id.form = CACE_ARI_IDSEG_TEXT;
        string_init_set(path->org_id.as_text, ns->org_id.name);
    }

    if (ns->model_id.has_intenum)
    {
        path->model_id.form   = CACE_ARI_IDSEG_INT;
        path->model_id.as_int = ns->model_id.intenum;
    }
    else
    {
        path->model_id.form = CACE_ARI_IDSEG_TEXT;
        string_init_set(path->model_id.as_text, ns->model_id.name);
    }

    path->type_id.form   = CACE_ARI_IDSEG_INT;
    path->type_id.as_int = obj_type;
    path->has_ari_type   = true;
    path->ari_type       = obj_type;

    if (obj->obj_id.has_intenum)
    {
        path->obj_id.form   = CACE_ARI_IDSEG_INT;
        path->obj_id.as_int = obj->obj_id.intenum;
    }
    else
    {
        path->obj_id.form = CACE_ARI_IDSEG_TEXT;
        string_init_set(path->obj_id.as_text, obj->obj_id.name);
    }
}

static cace_ari_uvast numeric_add_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left + right;
}
static cace_ari_vast numeric_add_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left + right;
}
static cace_ari_real64 numeric_add_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left + right;
}

static cace_ari_uvast numeric_sub_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left - right;
}
static cace_ari_vast numeric_sub_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left - right;
}
static cace_ari_real64 numeric_sub_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left - right;
}

static cace_ari_uvast numeric_mul_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left * right;
}
static cace_ari_vast numeric_mul_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left * right;
}
static cace_ari_real64 numeric_mul_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left * right;
}

static cace_ari_uvast numeric_div_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left / right;
}
static cace_ari_vast numeric_div_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left / right;
}
static cace_ari_real64 numeric_div_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left / right;
}

static cace_ari_uvast numeric_mod_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left % right;
}
static cace_ari_vast numeric_mod_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left % right;
}
static cace_ari_real64 numeric_mod_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return fmod(left, right);
}

static cace_ari_uvast numeric_gt_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left > right;
}
static cace_ari_vast numeric_gt_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left > right;
}
static cace_ari_real64 numeric_gt_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left > right;
}

static cace_ari_uvast numeric_gte_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left >= right;
}
static cace_ari_vast numeric_gte_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left >= right;
}
static cace_ari_real64 numeric_gte_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left >= right;
}

static cace_ari_uvast numeric_lt_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left < right;
}
static cace_ari_vast numeric_lt_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left < right;
}
static cace_ari_real64 numeric_lt_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left < right;
}

static cace_ari_uvast numeric_lte_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left <= right;
}
static cace_ari_vast numeric_lte_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left <= right;
}
static cace_ari_real64 numeric_lte_real64(cace_ari_real64 left, cace_ari_real64 right)
{
    return left <= right;
}

static cace_ari_uvast bitwise_and_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left & right;
}
static cace_ari_vast bitwise_and_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left & right;
}
static cace_ari_uvast bitwise_or_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left | right;
}
static cace_ari_vast bitwise_or_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left | right;
}
static cace_ari_uvast bitwise_xor_uvast(cace_ari_uvast left, cace_ari_uvast right)
{
    return left ^ right;
}
static cace_ari_vast bitwise_xor_vast(cace_ari_vast left, cace_ari_vast right)
{
    return left ^ right;
}

static int timespec_numeric_add(cace_ari_t *result, const cace_ari_t *valueA, const cace_ari_t *valueB)
{
    CHKERR1(result);
    CHKERR1(valueA);
    CHKERR1(valueB);

    cace_ari_type_t typeA = cace_eqiv_ari_type(&(valueA->as_lit));
    cace_ari_type_t typeB = cace_eqiv_ari_type(&(valueB->as_lit));

    // Addition of TD and TD results in a TD value
    bool is_result_TD = false;
    is_result_TD |= typeA == CACE_ARI_TYPE_TD && typeB == CACE_ARI_TYPE_TD;

    // Addition of TP and TD results in a TP value
    bool is_result_TP = false;
    is_result_TP |= typeA == CACE_ARI_TYPE_TD && typeB == CACE_ARI_TYPE_TP;
    is_result_TP |= typeA == CACE_ARI_TYPE_TP && typeB == CACE_ARI_TYPE_TD;

    // Determine the cace_ari_type_t of the result
    cace_ari_type_t ari_type;
    if (is_result_TD == true)
        ari_type = CACE_ARI_TYPE_TD;
    else if (is_result_TP == true)
        ari_type = CACE_ARI_TYPE_TP;
    else
        // Bail if the result will not be a TD or TP
        return RET_FAIL_UNDEFINED;

    // Delegate the addition
    struct timespec valueA_TS = valueA->as_lit.value.as_timespec;
    struct timespec valueB_TS = valueB->as_lit.value.as_timespec;
    struct timespec result_TS = timespec_add(valueA_TS, valueB_TS);

    // Store the result
    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    res_lit->has_ari_type      = true;
    res_lit->ari_type          = ari_type;
    res_lit->prim_type         = CACE_ARI_PRIM_TIMESPEC;
    res_lit->value.as_timespec = result_TS;

    // Success
    return RET_PASS;
}

static int timespec_numeric_sub(cace_ari_t *result, const cace_ari_t *valueA, const cace_ari_t *valueB)
{
    CHKERR1(result);
    CHKERR1(valueA);
    CHKERR1(valueB);

    cace_ari_type_t typeA = cace_eqiv_ari_type(&(valueA->as_lit));
    cace_ari_type_t typeB = cace_eqiv_ari_type(&(valueB->as_lit));

    // Note the following will result in TD:
    // - Subtraction of TP from TP
    // - Subtraction of TD from TD
    bool is_result_TD = false;
    is_result_TD |= typeA == CACE_ARI_TYPE_TP && typeB == CACE_ARI_TYPE_TP;
    is_result_TD |= typeA == CACE_ARI_TYPE_TD && typeB == CACE_ARI_TYPE_TD;
    if (is_result_TD == false)
        // Bail if the result will not be a TD
        return RET_FAIL_UNDEFINED;

    // Delegate the subtraction
    struct timespec valueA_TS = valueA->as_lit.value.as_timespec;
    struct timespec valueB_TS = valueB->as_lit.value.as_timespec;
    // fprintf(stdout, "\nDEBUG--->   [A]: (%ld sec, %ld nano)   ", valueA_TS.tv_sec, valueA_TS.tv_nsec);
    // fprintf(stdout, "|   [B]: (%ld sec, %ld nano)", valueB_TS.tv_sec, valueB_TS.tv_nsec);
    struct timespec result_TS = timespec_sub(valueA_TS, valueB_TS);
    // fprintf(stdout, "   ---ANSWER: (%ld sec, %ld nano)\n", result_TS.tv_sec, result_TS.tv_nsec);

    // Store the result
    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    res_lit->has_ari_type      = true;
    res_lit->ari_type          = CACE_ARI_TYPE_TD;
    res_lit->prim_type         = CACE_ARI_PRIM_TIMESPEC;
    res_lit->value.as_timespec = result_TS;

    // Success
    return RET_PASS;
}

static int timespec_numeric_mul(cace_ari_t *result, const cace_ari_t *valueA, const cace_ari_t *valueB)
{
    CHKERR1(result);
    CHKERR1(valueA);
    CHKERR1(valueB);

    cace_ari_type_t typeA              = cace_eqiv_ari_type(&(valueA->as_lit));
    bool            typeB_is_primitive = cace_has_numeric_prim_type(valueB);

    // Note the following will (typically) result in TD:
    // - Multiplication of TD with a scalar (numeric primitive)
    bool is_result_TD = false;
    is_result_TD |= typeA == CACE_ARI_TYPE_TD && typeB_is_primitive == true;
    if (is_result_TD == false)
        // Bail if the result will not be a TD
        return RET_FAIL_UNDEFINED;

    time_t valueA_sec  = valueA->as_lit.value.as_timespec.tv_sec;
    long   valueA_nano = valueA->as_lit.value.as_timespec.tv_nsec;

    struct timespec result_TS;
    switch (valueB->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            uint64_t scalar_uint64 = valueB->as_lit.value.as_uint64;
            result_TS.tv_sec       = valueA_sec * scalar_uint64;
            result_TS.tv_nsec      = valueA_nano * scalar_uint64;
            break;
        }
        case CACE_ARI_PRIM_INT64:
        {
            int64_t scalar_int64 = valueB->as_lit.value.as_int64;
            result_TS.tv_sec     = valueA_sec * scalar_int64;
            result_TS.tv_nsec    = valueA_nano * scalar_int64;
            break;
        }
        case CACE_ARI_PRIM_FLOAT64:
        {
            // Bail if scalar is infinite or NaN
            double scalar_double = valueB->as_lit.value.as_float64;
            int    scalar_class  = fpclassify(scalar_double);
            if (scalar_class == FP_INFINITE || scalar_class == FP_NAN)
                return RET_FAIL_UNDEFINED;

            // Calculate the seconds (as a double) and extract the integral and the fraction parts out.
            // Note we assign the integral part to result_TS.tv_sec, and the fractional part is sent to
            // result_TS.tv_nsec
            double sec_double = valueA_sec * scalar_double;
            double sec_int;
            double sec_frac  = modf(sec_double, &sec_int);
            result_TS.tv_sec = sec_int;

            result_TS.tv_nsec = valueA_nano * scalar_double;
            // Adjust nanos to take into account the factional second component
            result_TS.tv_nsec += (sec_frac * NANOS_IN_SEC);
            break;
        }
        default:
            // This should never happen - perhaps throw an exception instead
            return RET_FAIL_UNEXPECTED;
    }

    // Normalize the result_TS
    timespec_normalize(&result_TS);

    // Store the result
    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    res_lit->has_ari_type      = true;
    res_lit->ari_type          = CACE_ARI_TYPE_TD;
    res_lit->prim_type         = CACE_ARI_PRIM_TIMESPEC;
    res_lit->value.as_timespec = result_TS;

    // Success
    return RET_PASS;
}

static int timespec_numeric_div(cace_ari_t *result, const cace_ari_t *valueA, const cace_ari_t *valueB)
{
    CHKERR1(result);
    CHKERR1(valueA);
    CHKERR1(valueB);

    cace_ari_type_t typeA              = cace_eqiv_ari_type(&(valueA->as_lit));
    bool            typeB_is_primitive = cace_has_numeric_prim_type(valueB);

    // Note the following will (typically) result in TD:
    // - Division of TD with a scalar (numeric primitive)
    bool is_result_TD = false;
    is_result_TD |= typeA == CACE_ARI_TYPE_TD && typeB_is_primitive == true;
    if (is_result_TD == false)
        // Bail if the result will not be a TD
        return RET_FAIL_UNDEFINED;

    time_t valueA_sec  = valueA->as_lit.value.as_timespec.tv_sec;
    long   valueA_nano = valueA->as_lit.value.as_timespec.tv_nsec;

    struct timespec result_TS;
    switch (valueB->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
        {
            // Bail if scalar is 0
            uint64_t scalar_uint64 = valueB->as_lit.value.as_uint64;
            if (scalar_uint64 == 0)
                return RET_FAIL_UNDEFINED;

            result_TS.tv_sec  = valueA_sec / scalar_uint64;
            result_TS.tv_nsec = valueA_nano / scalar_uint64;
            // Adjust nanos to take into account the factional second component
            result_TS.tv_nsec += ((valueA_sec % scalar_uint64) * NANOS_IN_SEC) / scalar_uint64;
            break;
        }
        case CACE_ARI_PRIM_INT64:
        {
            // Bail if scalar is 0
            int64_t scalar_int64 = valueB->as_lit.value.as_int64;
            if (scalar_int64 == 0)
                return RET_FAIL_UNDEFINED;

            result_TS.tv_sec  = valueA_sec / scalar_int64;
            result_TS.tv_nsec = valueA_nano / scalar_int64;
            // Adjust nanos to take into account the factional second component
            result_TS.tv_nsec += ((valueA_sec % scalar_int64) * NANOS_IN_SEC) / scalar_int64;
            break;
        }
        case CACE_ARI_PRIM_FLOAT64:
        {
            // Bail if scalar is 0 or NaN
            double scalar_double = valueB->as_lit.value.as_float64;
            int    scalar_class  = fpclassify(scalar_double);
            if (scalar_class == FP_ZERO || scalar_class == FP_NAN)
                return RET_FAIL_UNDEFINED;

            result_TS.tv_sec  = valueA_sec / scalar_double;
            result_TS.tv_nsec = valueA_nano / scalar_double;
            // Adjust nanos to take into account the factional second component. Note do not adjust if the
            // scalar is infinite (since the above 2 lines will resolve to 0) but the line below may not.
            if (scalar_class != FP_INFINITE)
                result_TS.tv_nsec += ((valueA_sec - (result_TS.tv_sec * scalar_double)) * NANOS_IN_SEC) / scalar_double;
            break;
        }
        default:
            // This should never happen - perhaps throw an exception instead
            return RET_FAIL_UNEXPECTED;
    }

    // Normalize the result_TS
    timespec_normalize(&result_TS);

    // Store the result
    cace_ari_deinit(result);
    cace_ari_lit_t *res_lit = cace_ari_init_lit(result);

    res_lit->has_ari_type      = true;
    res_lit->ari_type          = CACE_ARI_TYPE_TD;
    res_lit->prim_type         = CACE_ARI_PRIM_TIMESPEC;
    res_lit->value.as_timespec = result_TS;

    // Success
    return RET_PASS;
}

static int timespec_numeric_mod(cace_ari_t *_U_, const cace_ari_t *_U_, const cace_ari_t *_U_)
{
    // Calculating the remainder associated with timespec objects is unsupported
    return RET_FAIL_UNDEFINED;
}

/*   STOP CUSTOM FUNCTIONS HERE  */

/* Name: sw-vendor
 * Description:
 *   The vendor for this Agent implementation.
 *
 * Parameters: none
 *
 * Produced type: use of ari:/ARITYPE/TEXTSTR
 */
static void refda_adm_ietf_dtnma_agent_edd_sw_vendor(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_vendor BODY
     * +-------------------------------------------------------------------------+
     */
    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tstr(&result, "JHU/APL", false);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_vendor BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: sw-version
 * Description:
 *   The version for this Agent implementation.
 *
 * Parameters: none
 *
 * Produced type: use of ari:/ARITYPE/TEXTSTR
 */
static void refda_adm_ietf_dtnma_agent_edd_sw_version(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_version BODY
     * +-------------------------------------------------------------------------+
     */
    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tstr(&result, "0.0.0", false);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_version BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: capability
 * Description:
 *   A table to indicate the ADM capability of the sending agent.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 6 columns (use of ari:/ARITYPE/TEXTSTR, use of ari:/ARITYPE/VAST, use of
 * ari:/ARITYPE/TEXTSTR, use of ari:/ARITYPE/VAST, use of ari:/ARITYPE/TEXTSTR, ulist of use of ari:/ARITYPE/TEXTSTR)
 */
static void refda_adm_ietf_dtnma_agent_edd_capability(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_capability BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 6, 0);

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        const cace_amm_obj_ns_t      *ns     = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if (ns->model_id.intenum < 0)
        {
            // ignore ODMs
            continue;
        }

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, 6);

        cace_ari_set_tstr(cace_ari_array_get(row, 0), m_string_get_cstr(ns->org_id.name), true);
        if (ns->org_id.has_intenum)
        {
            cace_ari_set_vast(cace_ari_array_get(row, 1), ns->org_id.intenum);
        }

        cace_ari_set_tstr(cace_ari_array_get(row, 2), m_string_get_cstr(ns->model_id.name), true);
        if (ns->model_id.has_intenum)
        {
            cace_ari_set_vast(cace_ari_array_get(row, 3), ns->model_id.intenum);
        }

        if (ns->model_rev.valid)
        {
            m_string_t buf;
            m_string_init(buf);
            cace_date_encode(buf, &(ns->model_rev.parts), true);
            cace_ari_set_tstr(cace_ari_array_get(row, 4), m_string_get_cstr(buf), true);
            m_string_clear(buf);
        }
        {
            cace_ari_t   *col = cace_ari_array_get(row, 5);
            cace_ari_ac_t list;
            cace_ari_ac_init(&list);

            string_tree_set_it_t feat_it;
            for (string_tree_set_it(feat_it, ns->feature_supp); !string_tree_set_end_p(feat_it);
                 string_tree_set_next(feat_it))
            {
                const m_string_t *feat = string_tree_set_cref(feat_it);
                cace_ari_t       *item = cace_ari_list_push_back_new(list.items);
                cace_ari_set_tstr(item, m_string_get_cstr(*feat), true);
            }

            cace_ari_set_ac(col, &list);
        }

        // append the row
        cace_ari_tbl_move_row_array(&table, row);
        cace_ari_array_clear(row);
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_capability BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-rx
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_rx(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent  = ctx->prodctx->parent->agent;
    cace_ari_t     result = CACE_ARI_INIT_UNDEFINED;
    atomic_ullong  val    = atomic_load(&agent->instr.num_execset_recv);
    cace_ari_set_uvast(&result, val);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-rx-failed
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent  = ctx->prodctx->parent->agent;
    cace_ari_t     result = CACE_ARI_INIT_UNDEFINED;
    atomic_ullong  val    = atomic_load(&agent->instr.num_execset_recv_failure);
    cace_ari_set_uvast(&result, val);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-tx
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_tx(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent  = ctx->prodctx->parent->agent;
    cace_ari_t     result = CACE_ARI_INIT_UNDEFINED;
    atomic_ullong  val    = atomic_load(&agent->instr.num_rptset_sent);
    cace_ari_set_uvast(&result, val);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-msg-tx-failed
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_tx_failed(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx_failed BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx_failed BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-exec-started
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_started(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_started BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent  = ctx->prodctx->parent->agent;
    cace_ari_t     result = CACE_ARI_INIT_UNDEFINED;
    atomic_ullong  val    = atomic_load(&agent->instr.num_ctrls_run);
    cace_ari_set_uvast(&result, val);
    refda_edd_prod_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_started BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-exec-succeeded
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: num-exec-failed
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: use of ari://ietf/amm-base/TYPEDEF/counter64
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_failed(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_failed BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_failed BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: exec-running
 * Description MISSING
 *
 * Parameters: none
 *
 * Produced type: TBLT with 3 columns (use of ari:/ARITYPE/UVAST, use of ari://ietf/amm-base/TYPEDEF/ANY, use of
 * ari:/ARITYPE/BYTE)
 */
static void refda_adm_ietf_dtnma_agent_edd_exec_running(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_exec_running BODY
     * +-------------------------------------------------------------------------+
     */

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    if (pthread_mutex_lock(&(agent->exec_state_mutex)))
    {
        CACE_LOG_ERR("failed to lock exec_state_mutex");
        return;
    }

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 3, 0);

    refda_exec_seq_list_it_t seq_it;
    for (refda_exec_seq_list_it(seq_it, agent->exec_state); !refda_exec_seq_list_end_p(seq_it);
         refda_exec_seq_list_next(seq_it))
    {
        const refda_exec_seq_t *seq = refda_exec_seq_list_ref(seq_it);
        if (refda_exec_item_list_empty_p(seq->items))
        {
            // intermediate state
            continue;
        }
        const refda_exec_item_t *front = refda_exec_item_list_front(seq->items);

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, 3);

        cace_ari_set_uvast(cace_ari_array_get(row, 0), seq->pid);

        cace_ari_set_copy(cace_ari_array_get(row, 1), &(front->ref));
        {
            int state;
            if (atomic_load(&(front->execution_stage)) == REFDA_EXEC_WAITING)
            {
                state = REFDA_EXEC_WAITING;
            }
            else
            {
                state = REFDA_EXEC_RUNNING;
            }
            cace_ari_set_int(cace_ari_array_get(row, 2), state);
        }

        // append the row
        cace_ari_tbl_move_row_array(&table, row);
        cace_ari_array_clear(row);
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    if (pthread_mutex_unlock(&(agent->exec_state_mutex)))
    {
        CACE_LOG_ERR("failed to unlock exec_state_mutex");
        return;
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_exec_running BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: odm-list
 * Description:
 *   A table of ODM within the agent.
 *
 * Parameters: none
 *
 * Produced type: TBLT with 5 columns (use of ari://ietf/amm-base/TYPEDEF/id-text, use of
 * ari://ietf/amm-base/TYPEDEF/id-int, use of ari://ietf/amm-base/TYPEDEF/id-text, use of
 * ari://ietf/amm-base/TYPEDEF/id-int, use of ari:/ARITYPE/TEXTSTR)
 */
static void refda_adm_ietf_dtnma_agent_edd_odm_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_odm_list BODY
     * +-------------------------------------------------------------------------+
     */
    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 5, 0);

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {

        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if ((ns->model_id.intenum >= 0))
        {
            // ignore ADMs
            continue;
        }

        if (!ns->org_id.has_intenum || !ns->model_id.has_intenum || !ns->model_rev.valid)
        {
            continue;
        }

        if (ns->obsolete)
        {
            CACE_LOG_DEBUG("ODM %s %d / %s %d marked as obsolete, skipping", ns->org_id.name, ns->org_id.intenum,
                           ns->model_id.name, ns->model_id.intenum);
            continue;
        }

        cace_ari_array_t row;
        cace_ari_array_init(row);
        cace_ari_array_resize(row, 5);

        cace_ari_set_tstr(cace_ari_array_get(row, 0), m_string_get_cstr(ns->org_id.name), true);
        cace_ari_set_int(cace_ari_array_get(row, 1), ns->org_id.intenum);
        cace_ari_set_tstr(cace_ari_array_get(row, 2), m_string_get_cstr(ns->model_id.name), true);
        cace_ari_set_int(cace_ari_array_get(row, 3), ns->model_id.intenum);
        {
            m_string_t buf;
            m_string_init(buf);
            cace_date_encode(buf, &(ns->model_rev.parts), true);
            cace_ari_set_tstr(cace_ari_array_get(row, 4), m_string_get_cstr(buf), true);
            m_string_clear(buf);
        }

        // append the row
        cace_ari_tbl_move_row_array(&table, row);
        cace_ari_array_clear(row);
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_odm_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: typedef-list
 * Description:
 *   A table of TYPEDEF within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 1 columns (use of ari:/ARITYPE/TYPEDEF)
 */
static void refda_adm_ietf_dtnma_agent_edd_typedef_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_typedef_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm;
    if (cace_ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 1, 0);
    const cace_ari_type_t obj_type = CACE_ARI_TYPE_TYPEDEF;

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {

        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if ((ns->model_id.intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            cace_ari_array_t row;
            cace_ari_array_init(row);
            cace_ari_array_resize(row, 1);

            {
                cace_ari_ref_t *ref = cace_ari_set_objref(cace_ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }

            // append the row
            cace_ari_tbl_move_row_array(&table, row);
            cace_ari_array_clear(row);
        }
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_typedef_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: const-list
 * Description:
 *   A table of CONST within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 2 columns (use of ari:/ARITYPE/CONST, use of ari://ietf/amm-semtype/TYPEDEF/semtype)
 */
static void refda_adm_ietf_dtnma_agent_edd_const_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_const_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm;
    if (cace_ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 2, 0);
    const cace_ari_type_t obj_type = CACE_ARI_TYPE_CONST;

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if ((ns->model_id.intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            cace_ari_array_t row;
            cace_ari_array_init(row);
            cace_ari_array_resize(row, 2);

            {
                cace_ari_ref_t *ref = cace_ari_set_objref(cace_ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }

            const refda_amm_const_desc_t *cnst = obj->app_data.ptr;
            if (cnst)
            {
                cace_amm_type_get_name(&(cnst->val_type), cace_ari_array_get(row, 1));
            }

            // append the row
            cace_ari_tbl_move_row_array(&table, row);
            cace_ari_array_clear(row);
        }
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_const_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: var-list
 * Description:
 *   A table of VAR within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 2 columns (use of ari:/ARITYPE/VAR, use of ari://ietf/amm-semtype/TYPEDEF/semtype)
 */
static void refda_adm_ietf_dtnma_agent_edd_var_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_var_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm;
    if (cace_ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 2, 0);
    const cace_ari_type_t obj_type = CACE_ARI_TYPE_VAR;

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if ((ns->model_id.intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            cace_ari_array_t row;
            cace_ari_array_init(row);
            cace_ari_array_resize(row, 2);

            {
                cace_ari_ref_t *ref = cace_ari_set_objref(cace_ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }

            const refda_amm_var_desc_t *var = obj->app_data.ptr;
            if (var)
            {
                cace_amm_type_get_name(&(var->val_type), cace_ari_array_get(row, 1));
            }

            // append the row
            cace_ari_tbl_move_row_array(&table, row);
            cace_ari_array_clear(row);
        }
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_var_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: sbr-list
 * Description:
 *   A table of SBR within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 7 columns (use of ari:/ARITYPE/SBR, use of ari://ietf/amm-base/TYPEDEF/MAC, use of
 * ari://ietf/amm-base/TYPEDEF/EXPR, use of ari:/ARITYPE/TD, use of ari:/ARITYPE/UVAST, use of ari:/ARITYPE/BOOL, use of
 * ari:/ARITYPE/BOOL)
 */
static void refda_adm_ietf_dtnma_agent_edd_sbr_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sbr_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm = true;
    if (cace_ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 6, 0);
    const cace_ari_type_t obj_type = CACE_ARI_TYPE_SBR;

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if ((ns->model_id.intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            cace_ari_array_t row;
            cace_ari_array_init(row);
            cace_ari_array_resize(row, 6);

            {
                cace_ari_ref_t *ref = cace_ari_set_objref(cace_ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }

            const refda_amm_sbr_desc_t *sbr = obj->app_data.ptr;
            if (sbr)
            {
                cace_ari_set_copy(cace_ari_array_get(row, 1), &(sbr->action));
                cace_ari_set_copy(cace_ari_array_get(row, 2), &(sbr->condition));
                cace_ari_set_copy(cace_ari_array_get(row, 3), &(sbr->min_interval));
                cace_ari_set_uvast(cace_ari_array_get(row, 4), sbr->max_exec_count);
                cace_ari_set_bool(cace_ari_array_get(row, 5), sbr->enabled);
            }

            // append the row
            cace_ari_tbl_move_row_array(&table, row);
            cace_ari_array_clear(row);
        }
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sbr_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: tbr-list
 * Description:
 *   A table of TBR within the agent.
 *
 * Parameters list:
 *  * Index 0, name "include-adm", type: use of ari:/ARITYPE/BOOL
 *
 * Produced type: TBLT with 7 columns (use of ari:/ARITYPE/TBR, use of ari://ietf/amm-base/TYPEDEF/MAC, use of
 * ari://ietf/amm-base/TYPEDEF/TIME, use of ari:/ARITYPE/TD, use of ari:/ARITYPE/UVAST, use of ari:/ARITYPE/BOOL, use of
 * ari:/ARITYPE/BOOL)
 */
static void refda_adm_ietf_dtnma_agent_edd_tbr_list(refda_edd_prod_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_tbr_list BODY
     * +-------------------------------------------------------------------------+
     */
    bool include_adm = true;
    if (cace_ari_get_bool(refda_edd_prod_ctx_get_aparam_index(ctx, 0), &include_adm))
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->prodctx->parent->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_ari_tbl_t table;
    cace_ari_tbl_init(&table, 6, 0);
    const cace_ari_type_t obj_type = CACE_ARI_TYPE_TBR;

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);
        if ((ns->model_id.intenum >= 0) && !include_adm)
        {
            // ignore ADMs
            continue;
        }

        cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
        if (!ctr_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

        cace_amm_obj_desc_list_it_t obj_it;
        for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
             cace_amm_obj_desc_list_next(obj_it))
        {
            const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));

            cace_ari_array_t row;
            cace_ari_array_init(row);
            cace_ari_array_resize(row, 6);

            {
                cace_ari_ref_t *ref = cace_ari_set_objref(cace_ari_array_get(row, 0));
                refda_adm_ietf_dtnma_agent_set_objpath(&(ref->objpath), ns, obj_type, obj);
            }

            const refda_amm_tbr_desc_t *tbr = obj->app_data.ptr;
            if (tbr)
            {
                cace_ari_set_copy(cace_ari_array_get(row, 1), &(tbr->action));
                cace_ari_set_copy(cace_ari_array_get(row, 2), &(tbr->start_time));
                cace_ari_set_copy(cace_ari_array_get(row, 3), &(tbr->period));
                cace_ari_set_uvast(cace_ari_array_get(row, 4), tbr->max_exec_count);
                cace_ari_set_bool(cace_ari_array_get(row, 5), tbr->enabled);
            }

            // append the row
            cace_ari_tbl_move_row_array(&table, row);
            cace_ari_array_clear(row);
        }
    }

    cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tbl(&result, &table);
    refda_edd_prod_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );

    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_tbr_list BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: if-then-else
 * Description:
 *   Evaluate an expression and follow one of two branches of further
 *   evaluation.
 *
 * Parameters list:
 *  * Index 0, name "condition", type: use of ari://ietf/amm-base/TYPEDEF/eval-tgt
 *  * Index 1, name "on-truthy", type: union of 2 types (use of ari://ietf/amm-base/TYPEDEF/exec-tgt, use of
 * ari:/ARITYPE/NULL)
 *  * Index 2, name "on-falsy", type: union of 2 types (use of ari://ietf/amm-base/TYPEDEF/exec-tgt, use of
 * ari:/ARITYPE/NULL)
 *
 * Result name "branch", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_ctrl_if_then_else(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_if_then_else BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_if_then_else BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: catch
 * Description:
 *   Attempt to execute a target, and if there is some failure catch it and
 *   execute an alternative target.
 *
 * Parameters list:
 *  * Index 0, name "try", type: use of ari://ietf/amm-base/TYPEDEF/exec-tgt
 *  * Index 1, name "on-failure", type: union of 2 types (use of ari://ietf/amm-base/TYPEDEF/exec-tgt, use of
 * ari:/ARITYPE/NULL)
 *
 * Result name "try-success", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_ctrl_catch(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_catch BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_catch BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: wait-for
 * Description:
 *   This control causes the execution to pause for a given amount of time.
 *   This is intended to be used within a macro to separate controls in
 *   time.
 *
 * Parameters list:
 *  * Index 0, name "duration", type: use of ari:/ARITYPE/TD
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_wait_for(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_for BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_duration = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    struct timespec nowtime, duration;
    if (cace_ari_get_td(p_duration, &duration))
    {
        CACE_LOG_ERR("No time duration given");
        return;
    }

    int res = clock_gettime(CLOCK_REALTIME, &nowtime);
    if (res)
    {
        // handled as failure
        CACE_LOG_ERR("Failed clock_gettime()");
        return;
    }

    refda_timeline_event_t event = {
        .purpose       = REFDA_TIMELINE_EXEC,
        .ts            = timespec_add(nowtime, duration),
        .exec.item     = ctx->item,
        .exec.callback = refda_adm_ietf_dtnma_agent_ctrl_wait_finished,
    };
    refda_ctrl_exec_ctx_set_waiting(ctx, &event);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_for BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: wait-until
 * Description:
 *   This control causes the execution to pause until a specific absolute
 *   time point. This is intended to be used within a macro to separate
 *   controls in time or as a first macro item to delay execution after the
 *   time of reception.
 *
 * Parameters list:
 *  * Index 0, name "time", type: use of ari:/ARITYPE/TP
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_wait_until(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_until BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *p_abstime = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    struct timespec abstime;
    if (cace_ari_get_tp_posix(p_abstime, &abstime))
    {
        CACE_LOG_ERR("No time point given");
        return;
    }

    refda_timeline_event_t event = {
        .purpose       = REFDA_TIMELINE_EXEC,
        .ts            = abstime,
        .exec.item     = ctx->item,
        .exec.callback = refda_adm_ietf_dtnma_agent_ctrl_wait_finished,
    };
    refda_ctrl_exec_ctx_set_waiting(ctx, &event);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_until BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: wait-cond
 * Description:
 *   This control causes the execution to pause until a condition
 *   expression evaluates to truthy. This is intended to be used within a
 *   macro to separate controls in time or as a first macro item to delay
 *   execution until the condition is met.
 *
 * Parameters list:
 *  * Index 0, name "condition", type: use of ari://ietf/amm-base/TYPEDEF/eval-tgt
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_wait_cond(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_cond BODY
     * +-------------------------------------------------------------------------+
     */
    // initial check and kickoff timers
    refda_adm_ietf_dtnma_agent_ctrl_wait_cond_check(ctx);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_cond BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: inspect
 * Description:
 *   Produce a result value to inspect the agent state. This does not
 *   perform any EXPR evaluation or RPTT handling.
 *
 * Parameters list:
 *  * Index 0, name "ref", type: use of ari://ietf/amm-base/TYPEDEF/VALUE-OBJ
 *
 * Result name "val", type: use of ari://ietf/amm-base/TYPEDEF/ANY
 */
static void refda_adm_ietf_dtnma_agent_ctrl_inspect(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_inspect BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *ref = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    // mutex-serialize object store access
    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(ctx->runctx->agent->objs), ref);

    REFDA_AGENT_UNLOCK(agent, );

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode_objpath(buf, &(ref->as_ref.objpath), CACE_ARI_TEXT_ARITYPE_TEXT);
        CACE_LOG_DEBUG("Lookup reference to %s", string_get_cstr(buf));
        string_clear(buf);
    }
    if (res)
    {
        CACE_LOG_WARNING("lookup failed with status %d", res);
    }
    else
    {
        refda_valprod_ctx_t prodctx;
        refda_valprod_ctx_init(&prodctx, ctx->runctx, ref, &deref);

        res = refda_valprod_run(&prodctx);
        if (res)
        {
            // not setting a result will be treated as failure
            CACE_LOG_WARNING("production failed with status %d", res);
        }
        else
        {
            // result of the CTRL is the produced value
            refda_ctrl_exec_ctx_set_result_move(ctx, &prodctx.value);
        }

        refda_valprod_ctx_deinit(&prodctx);
    }

    cace_amm_lookup_deinit(&deref);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_inspect BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: report-on
 * Description:
 *   Agent-wide control to generate a report from a report template (RPTT).
 *   The parameter is a single RPTT list that would be produced by an
 *   object. If used for more than one-shot diagnostics, defining a RPTT
 *   (e.g. in a CONST within an ODM) is more efficient because the RPTT
 *   value would not need be present in the EXECSET or RPTSET.
 *
 * Parameters list:
 *  * Index 0, name "template", type: use of ari://ietf/amm-base/TYPEDEF/rpt-tgt
 *  * Index 1, name "destinations", type: union of 2 types (use of ari://ietf/network-base/TYPEDEF/endpoint-or-uri, use
 * of ari:/ARITYPE/NULL)
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_report_on(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_report_on BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *tgt = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    if (!tgt)
    {
        CACE_LOG_ERR("no parameter");
        return;
    }

    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    // ignore return code because failure cannot be handled here
    refda_reporting_target(ctx->runctx, tgt);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_report_on BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-odm
 * Description:
 *   Ensure a specific ODM is present.
 *
 * Parameters list:
 *  * Index 0, name "org-name", type: use of ari://ietf/amm-base/TYPEDEF/id-text
 *  * Index 1, name "org-id", type: use of ari://ietf/amm-base/TYPEDEF/id-int
 *  * Index 2, name "model-name", type: use of ari://ietf/amm-base/TYPEDEF/id-text
 *  * Index 3, name "model-id", type: use of ari://ietf/amm-base/TYPEDEF/id-int
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_odm(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_odm BODY
     * +-------------------------------------------------------------------------+
     */

    const cace_ari_t *ari_org_name   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *ari_org_id     = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);
    const cace_ari_t *ari_model_name = refda_ctrl_exec_ctx_get_aparam_index(ctx, 2);
    const cace_ari_t *ari_model_id   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 3);

    int res;

    cace_ari_int org_id, model_id;
    char        *org_name, *model_name;

    if (cace_ari_get_int(ari_org_id, &org_id))
    {
        CACE_LOG_ERR("Unable to retrieve org ID");
        return;
    }

    if (cace_ari_get_int(ari_model_id, &model_id))
    {
        CACE_LOG_ERR("Unable to retrieve model ID");
        return;
    }

    const cace_data_t *org = cace_ari_cget_tstr(ari_org_name);
    if (org == NULL || org->ptr == NULL)
    {
        CACE_LOG_ERR("Unable to retrieve org name");
        return;
    }

    org_name = CACE_MALLOC(strlen((char *)org->ptr) + 1);
    strcpy(org_name, (char *)org->ptr);

    const cace_data_t *model = cace_ari_cget_tstr(ari_model_name);
    if (model == NULL || model->ptr == NULL)
    {
        CACE_LOG_ERR("Unable to retrieve model name");
        return;
    }

    model_name = CACE_MALLOC(strlen((char *)model->ptr) + 1);
    strcpy(model_name, (char *)model->ptr);

    if (model_id >= 0)
    {
        CACE_LOG_ERR("Invalid ODM ID %d", model_id);
        return;
    }

    char *rev_date_cstr = NULL;
    {
        m_string_t rev_date;
        m_string_init(rev_date);
        time_t     now         = time(NULL);      // Get current time as time_t
        struct tm *currentTime = localtime(&now); // Convert to local time as struct tm
        cace_date_encode(rev_date, currentTime, false);
        rev_date_cstr = CACE_MALLOC(m_string_size(rev_date) + 1);
        strncpy(rev_date_cstr, m_string_get_cstr(rev_date), 12);
        m_string_clear(rev_date);
    }

    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_obj_ns_t *odm =
        cace_amm_obj_store_add_ns(&(agent->objs), cace_amm_idseg_ref_withenum(org_name, org_id),
                                  cace_amm_idseg_ref_withenum(model_name, model_id), rev_date_cstr);

    if (odm)
    {
        CACE_LOG_ERR("ensure-odm ODM created");
    }
    else
    {
        CACE_LOG_ERR("ensure-odm found existing ODM");
    }

    cace_ari_t result = CACE_ARI_INIT_NULL;
    refda_ctrl_exec_ctx_set_result_move(ctx, &result);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_odm BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: obsolete-odm
 * Description:
 *   Mark a specific ODM as obsolete if it is present.
 *
 * Parameters list:
 *  * Index 0, name "odm-ns", type: use of ari:/ARITYPE/NAMESPACE
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_obsolete_odm(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_odm BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *odm_ns = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    refda_agent_t    *agent  = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_obj_ns_t *odm = cace_amm_obj_store_find_ns(&(agent->objs), odm_ns);

    if (odm)
    {
        CACE_LOG_DEBUG("ODM found, marking as obsolete");
        odm->obsolete = true;

        cace_ari_t result = CACE_ARI_INIT_NULL; // Indicate successful result
        refda_ctrl_exec_ctx_set_result_move(ctx, &result);
    }
    else
    {
        CACE_LOG_ERR("ODM not found");
    }

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_odm BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: var-reset
 * Description:
 *   Modify a VAR state to its default value.
 *
 * Parameters list:
 *  * Index 0, name "target", type: use of ari:/ARITYPE/VAR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_var_reset(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_reset BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *target = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    // mutex-serialize object store access
    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(agent->objs), target);

    if (res)
    {
        CACE_LOG_WARNING("lookup failed with status %d", res);
    }
    else
    {
        refda_amm_var_desc_t *var = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (var && !cace_ari_is_undefined(&(var->init_val)))
        {
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                string_t buf;
                string_init(buf);
                cace_ari_text_encode_objpath(buf, &(target->as_ref.objpath), CACE_ARI_TEXT_ARITYPE_TEXT);
                CACE_LOG_DEBUG("resetting state of %s", string_get_cstr(buf));
                string_clear(buf);
            }
            cace_ari_set_copy(&(var->value), &(var->init_val));
        }
    }
    cace_amm_lookup_deinit(&deref);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_reset BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: var-store
 * Description:
 *   Modify a VAR state to a specific value.
 *
 * Parameters list:
 *  * Index 0, name "target", type: use of ari:/ARITYPE/VAR
 *  * Index 1, name "value", type: use of ari://ietf/amm-base/TYPEDEF/ANY
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_var_store(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_store BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *target = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *value  = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);

    // mutex-serialize object store access
    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(agent->objs), target);

    if (res)
    {
        CACE_LOG_WARNING("lookup failed with status %d", res);
    }
    else
    {
        refda_amm_var_desc_t *var = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (var)
        {
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                string_t buf;
                string_init(buf);
                cace_ari_text_encode_objpath(buf, &(target->as_ref.objpath), CACE_ARI_TEXT_ARITYPE_TEXT);
                CACE_LOG_DEBUG("setting state of %s", string_get_cstr(buf));
                string_clear(buf);
            }
            cace_ari_set_copy(&(var->value), value);
        }
    }
    cace_amm_lookup_deinit(&deref);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_store BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-const
 * Description:
 *   Ensure a specific CONST is present in an ODM.
 *
 * Parameters list:
 *  * Index 0, name "namespace", type: use of ari:/ARITYPE/NAMESPACE
 *  * Index 1, name "obj-name", type: use of ari://ietf/amm-base/TYPEDEF/id-text
 *  * Index 2, name "obj-enum", type: use of ari://ietf/amm-base/TYPEDEF/id-int
 *  * Index 3, name "type", type: use of ari://ietf/amm-semtype/TYPEDEF/semtype
 *  * Index 4, name "init", type: use of ari://ietf/amm-base/TYPEDEF/EXPR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_const(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_const BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_const BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: obsolete-const
 * Description:
 *   Mark a specific CONST as obsolete if it is present in an ODM.
 *
 * Parameters list:
 *  * Index 0, name "obj", type: use of ari:/ARITYPE/CONST
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_obsolete_const(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_const BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_const BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-var
 * Description:
 *   Ensure a specific VAR is present in an ODM.
 *
 * Parameters list:
 *  * Index 0, name "namespace", type: use of ari:/ARITYPE/NAMESPACE
 *  * Index 1, name "obj-name", type: use of ari://ietf/amm-base/TYPEDEF/id-text
 *  * Index 2, name "obj-enum", type: use of ari://ietf/amm-base/TYPEDEF/id-int
 *  * Index 3, name "type", type: use of ari://ietf/amm-semtype/TYPEDEF/semtype
 *  * Index 4, name "init", type: union of 2 types (use of ari:/ARITYPE/NULL, use of ari://ietf/amm-base/TYPEDEF/EXPR)
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_var(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_var BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_var BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: obsolete-var
 * Description:
 *   Mark a specific VAR as obsolete if it is present in an ODM.
 *
 * Parameters list:
 *  * Index 0, name "obj", type: use of ari:/ARITYPE/VAR
 *
 * Result: none
 */
static void refda_adm_ietf_dtnma_agent_ctrl_obsolete_var(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_var BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_var BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-sbr
 * Description:
 *   Ensure that a state-based rule with the given identifiers exists
 *   within an ODM.
 *
 * Parameters list:
 *  * Index 0, name "namespace", type: use of ari:/ARITYPE/NAMESPACE
 *  * Index 1, name "obj-name", type: use of ari://ietf/amm-base/TYPEDEF/id-text
 *  * Index 2, name "obj-enum", type: use of ari://ietf/amm-base/TYPEDEF/id-int
 *  * Index 3, name "action", type: use of ari://ietf/amm-base/TYPEDEF/MAC
 *  * Index 4, name "start-time", type: use of ari://ietf/amm-base/TYPEDEF/TIME
 *  * Index 5, name "condition", type: use of ari://ietf/amm-base/TYPEDEF/EXPR
 *  * Index 6, name "min-interval", type: use of ari:/ARITYPE/TD
 *  * Index 7, name "max-count", type: use of ari:/ARITYPE/UVAST
 *  * Index 8, name "init-enabled", type: use of ari:/ARITYPE/BOOL
 *
 * Result name "res", type: use of ari:/ARITYPE/UINT
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_sbr(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_sbr BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_sbr BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-tbr
 * Description:
 *   Ensure that a time-based rule with the given identifiers exists within
 *   an ODM.
 *
 * Parameters list:
 *  * Index 0, name "namespace", type: use of ari:/ARITYPE/NAMESPACE
 *  * Index 1, name "obj-name", type: use of ari://ietf/amm-base/TYPEDEF/id-text
 *  * Index 2, name "obj-enum", type: use of ari://ietf/amm-base/TYPEDEF/id-int
 *  * Index 3, name "action", type: use of ari://ietf/amm-base/TYPEDEF/MAC
 *  * Index 4, name "start-time", type: use of ari://ietf/amm-base/TYPEDEF/TIME
 *  * Index 5, name "period", type: use of ari:/ARITYPE/TD
 *  * Index 6, name "max-count", type: use of ari:/ARITYPE/UVAST
 *  * Index 7, name "max-count", type: use of ari:/ARITYPE/UVAST
 *  * Index 8, name "init-enabled", type: use of ari:/ARITYPE/BOOL
 *
 * Result name "res", type: use of ari:/ARITYPE/UINT
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_tbr(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_tbr BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *odm_ns           = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *ari_org_name     = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);
    const cace_ari_t *ari_obj_name     = refda_ctrl_exec_ctx_get_aparam_index(ctx, 2);
    const cace_ari_t *ari_obj_enum     = refda_ctrl_exec_ctx_get_aparam_index(ctx, 3);
    const cace_ari_t *ari_action       = refda_ctrl_exec_ctx_get_aparam_index(ctx, 4);
    const cace_ari_t *ari_start_time   = refda_ctrl_exec_ctx_get_aparam_index(ctx, 5);
    const cace_ari_t *ari_period       = refda_ctrl_exec_ctx_get_aparam_index(ctx, 6);
    const cace_ari_t *ari_max_count    = refda_ctrl_exec_ctx_get_aparam_index(ctx, 7);
    const cace_ari_t *ari_init_enabled = refda_ctrl_exec_ctx_get_aparam_index(ctx, 9);

    cace_ari_t ari_result;
    cace_ari_init(&ari_result);
    cace_ari_set_uint(&ari_result, 0);

    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_obj_ns_t *odm = cace_amm_obj_store_find_ns(&(agent->objs), odm_ns);

    if (!odm)
    {
        CACE_LOG_INFO("ODM not found");
        return;
    }

    if (odm->model_id.intenum >= 0)
    {
        CACE_LOG_ERR("Invalid model ID, cannot modify an ADM");
        return;
    }

    const cace_data_t *obj_name = cace_ari_cget_tstr(ari_obj_name);
    if (obj_name == NULL || obj_name->ptr == NULL)
    {
        CACE_LOG_ERR("Unable to retrieve obj name");
        return;
    }

    cace_ari_int obj_id;
    if (cace_ari_get_int(ari_obj_enum, &obj_id))
    {
        CACE_LOG_ERR("Unable to retrieve object ID");
        return;
    }

    {
        if (NULL != cace_amm_obj_ns_find_obj_name(odm, CACE_ARI_TYPE_TBR, (const char *)obj_name->ptr))
        {
            // FIXME: update fields on existing TBR in this case, instead of just returning??
            CACE_LOG_INFO("TBR already exists");
            refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
            return;
        }

        if (NULL != cace_amm_obj_ns_find_obj_enum(odm, CACE_ARI_TYPE_TBR, obj_id))
        {
            // FIXME: same comment as above
            CACE_LOG_INFO("TBR already exists");
            refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
            return;
        }
    }

    { // For ./TBR/tbr_rule
        cace_amm_obj_desc_t  *obj;
        refda_amm_tbr_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_tbr_desc_t));
        refda_amm_tbr_desc_init(objdata);
        // action
        {
            struct cace_ari_ac_s *action_ac = cace_ari_get_ac((cace_ari_t *)ari_action);
            if (action_ac == NULL)
            {
                CACE_LOG_ERR("Invalid ARI received for action");
                return;
            }
            cace_ari_set_ac(&(objdata->action), action_ac);
        }
        // period
        {
            struct timespec ts;
            if (cace_ari_get_td(ari_period, &ts))
            {
                CACE_LOG_ERR("Invalid ARI received for period");
                return;
            }
            cace_ari_set_td(&(objdata->period), ts);
        }
        // start_time
        {
            struct timespec ts;
            int             rv;

            rv = cace_ari_get_td(ari_start_time, &ts);
            if (rv)
            {
                rv = cace_ari_get_tp(ari_start_time, &ts);
            }

            if (rv)
            {
                CACE_LOG_ERR("Invalid ARI received for start time");
                return;
            }

            cace_ari_set_tp(&(objdata->start_time), ts);
        }
        //  init_enabled
        {
            cace_ari_bool init_enabled;
            if (cace_ari_get_bool(ari_init_enabled, &init_enabled))
            {
                CACE_LOG_ERR("Invalid ARI received for init enabled");
                return;
            }
            objdata->init_enabled = init_enabled;
        }
        // max_exec_count
        {
            cace_ari_uvast max_exec_count;
            if (cace_ari_get_uvast(ari_max_count, &max_exec_count))
            {
                CACE_LOG_ERR("Invalid ARI received for max exec count");
                return;
            }
            objdata->max_exec_count = max_exec_count;
        }

        obj = refda_register_tbr(odm, cace_amm_idseg_ref_withenum((char *)obj_name->ptr, obj_id), objdata);

        if (obj && obj->app_data.ptr)
        {
            refda_amm_tbr_desc_t *desc = obj->app_data.ptr;
            if (desc->init_enabled)
            {
                refda_exec_tbr_enable(agent, desc);
            }
        }
    }

    refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_tbr BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: ensure-rule-enabled
 * Description:
 *   Ensure rule is enabled or disabled.
 *
 * Parameters list:
 *  * Index 0, name "obj-id", type: union of 2 types (use of ari:/ARITYPE/SBR, use of ari:/ARITYPE/TBR)
 *  * Index 1, name "enabled", type: use of ari:/ARITYPE/BOOL
 *
 * Result name "res", type: use of ari:/ARITYPE/UINT
 */
static void refda_adm_ietf_dtnma_agent_ctrl_ensure_rule_enabled(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_rule_enabled BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *target      = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);
    const cace_ari_t *ari_enabled = refda_ctrl_exec_ctx_get_aparam_index(ctx, 1);

    cace_ari_t ari_result;
    cace_ari_init(&ari_result);
    cace_ari_set_uint(&ari_result, 0);

    cace_ari_bool enabled;
    if (cace_ari_get_bool(ari_enabled, &enabled))
    {
        CACE_LOG_ERR("Invalid ARI for enabled");
        return;
    }

    // mutex-serialize object store access
    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(agent->objs), target);

    if (res)
    {
        CACE_LOG_WARNING("lookup failed with status %d", res);
    }
    else if (deref.obj_type == CACE_ARI_TYPE_SBR)
    {
        refda_amm_sbr_desc_t *sbr = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (sbr && sbr->enabled != enabled)
        {
            CACE_LOG_DEBUG("setting enabled state of %d", enabled);
            sbr->enabled = enabled;

            if (enabled)
            {
                refda_exec_sbr_enable(agent, sbr);
            }
            else
            {
                refda_exec_sbr_disable(agent, sbr);
            }
        }
        refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    }
    else if (deref.obj_type == CACE_ARI_TYPE_TBR)
    {
        refda_amm_tbr_desc_t *tbr = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (tbr && tbr->enabled != enabled)
        {
            CACE_LOG_DEBUG("setting enabled state of %d", enabled);
            tbr->enabled = enabled;

            if (enabled)
            {
                refda_exec_tbr_enable(agent, tbr);
            }
            else
            {
                refda_exec_tbr_disable(agent, tbr);
            }
        }
        refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    }
    cace_amm_lookup_deinit(&deref);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_ensure_rule_enabled BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: reset-rule-enabled
 * Description:
 *   Reset rule enabled to initial setting.
 *
 * Parameters list:
 *  * Index 0, name "obj-id", type: union of 2 types (use of ari:/ARITYPE/SBR, use of ari:/ARITYPE/TBR)
 *
 * Result name "res", type: use of ari:/ARITYPE/UINT
 */
static void refda_adm_ietf_dtnma_agent_ctrl_reset_rule_enabled(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_reset_rule_enabled BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *target = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    cace_ari_t ari_result;
    cace_ari_init(&ari_result);
    cace_ari_set_uint(&ari_result, 0);

    // mutex-serialize object store access
    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(agent->objs), target);

    if (res)
    {
        CACE_LOG_WARNING("lookup failed with status %d", res);
    }
    else if (deref.obj_type == CACE_ARI_TYPE_SBR)
    {
        refda_amm_sbr_desc_t *sbr = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (sbr && sbr->enabled != sbr->init_enabled)
        {
            CACE_LOG_DEBUG("setting back to init_enabled state of %d", sbr->init_enabled);
            sbr->enabled = sbr->init_enabled;

            if (sbr->enabled)
            {
                refda_exec_sbr_enable(agent, sbr);
            }
            else
            {
                refda_exec_sbr_disable(agent, sbr);
            }
        }
        refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    }
    else if (deref.obj_type == CACE_ARI_TYPE_TBR)
    {
        refda_amm_tbr_desc_t *tbr = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (tbr && tbr->enabled != tbr->init_enabled)
        {
            CACE_LOG_DEBUG("setting back to init_enabled state of %d", tbr->init_enabled);
            tbr->enabled = tbr->init_enabled;

            if (tbr->enabled)
            {
                refda_exec_tbr_enable(agent, tbr);
            }
            else
            {
                refda_exec_tbr_disable(agent, tbr);
            }
        }
        refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    }
    cace_amm_lookup_deinit(&deref);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_reset_rule_enabled BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: obsolete-rule
 * Description:
 *   Mark a specific SBR or TBR as obsolete if it is present in an ODM.
 *
 * Parameters list:
 *  * Index 0, name "obj-id", type: union of 2 types (use of ari:/ARITYPE/SBR, use of ari:/ARITYPE/TBR)
 *
 * Result name "success", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_ctrl_obsolete_rule(refda_ctrl_exec_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_rule BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *target = refda_ctrl_exec_ctx_get_aparam_index(ctx, 0);

    cace_ari_t ari_result;
    cace_ari_init(&ari_result);
    cace_ari_set_bool(&ari_result, true);

    // mutex-serialize object store access
    refda_agent_t *agent = ctx->runctx->agent;
    REFDA_AGENT_LOCK(agent, );

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);
    int res = cace_amm_lookup_deref(&deref, &(agent->objs), target);

    if (res)
    {
        CACE_LOG_WARNING("lookup failed with status %d", res);
    }
    else if (!cace_amm_obj_ns_is_odm(deref.ns))
    {
        CACE_LOG_WARNING("unable to obsolete an ADM object");
    }
    else if (deref.obj_type == CACE_ARI_TYPE_SBR)
    {
        refda_amm_sbr_desc_t *sbr = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (sbr)
        {
            CACE_LOG_DEBUG("Marking SBR as obsolete");
            sbr->obsolete = true;

            if (sbr->enabled)
            {
                CACE_LOG_INFO("Disabling obsolete SBR");
                refda_exec_sbr_disable(agent, sbr);
            }
        }
        refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    }
    else if (deref.obj_type == CACE_ARI_TYPE_TBR)
    {
        refda_amm_tbr_desc_t *tbr = deref.obj->app_data.ptr;
        // FIXME need agent access control

        if (tbr)
        {
            CACE_LOG_DEBUG("Marking TBR as obsolete");
            tbr->obsolete = true;

            if (tbr->enabled)
            {
                CACE_LOG_INFO("Disabling obsolete TBR");
                refda_exec_tbr_disable(agent, tbr);
            }
        }
        refda_ctrl_exec_ctx_set_result_move(ctx, &ari_result);
    }

    cace_amm_lookup_deinit(&deref);

    REFDA_AGENT_UNLOCK(agent, );
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_obsolete_rule BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: negate
 * Description:
 *   Negate a value. This is equivalent to multiplying by -1 but a shorter
 *   expression.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_negate(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_negate BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *val    = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    int               retval = 0;
    switch (val->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            result.as_lit.value.as_uint64 = val->as_lit.value.as_uint64 * -1;
            break;
        case CACE_ARI_PRIM_INT64:
            result.as_lit.value.as_int64 = val->as_lit.value.as_int64 * -1;
            break;
        case CACE_ARI_PRIM_FLOAT64:
            result.as_lit.value.as_float64 = val->as_lit.value.as_float64 * -1.0;
            break;
        default:
            // leave lit as default undefined
            CACE_LOG_WARNING("negate: Unhandled numeric type %d", val->as_lit.prim_type);
            retval = 3;
            break;
    }

    if (!retval)
    {
        result.as_lit.prim_type    = val->as_lit.prim_type;
        result.as_lit.has_ari_type = true;
        result.as_lit.ari_type     = val->as_lit.ari_type;
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_negate BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: add
 * Description:
 *   Add two numeric values. The operands are cast to the least compatible
 *   numeric type before the arithmetic.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_add(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_add BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_operator(&result, lt_val, rt_val, numeric_add_uvast, numeric_add_vast, numeric_add_real64,
                                      timespec_numeric_add))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_add BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: sub
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_sub(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_sub BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_operator(&result, lt_val, rt_val, numeric_sub_uvast, numeric_sub_vast, numeric_sub_real64,
                                      timespec_numeric_sub))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_sub BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: multiply
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_multiply(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_multiply BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_operator(&result, lt_val, rt_val, numeric_mul_uvast, numeric_mul_vast, numeric_mul_real64,
                                      timespec_numeric_mul))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_multiply BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: divide
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_divide(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_divide BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;

    if (!cace_numeric_is_zero(rt_val)
        && !cace_numeric_binary_operator(&result, lt_val, rt_val, numeric_div_uvast, numeric_div_vast,
                                         numeric_div_real64, timespec_numeric_div))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_divide BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: remainder
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/NUMERIC
 */
static void refda_adm_ietf_dtnma_agent_oper_remainder(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_remainder BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;

    if (!cace_numeric_is_zero(rt_val)
        && !cace_numeric_binary_operator(&result, lt_val, rt_val, numeric_mod_uvast, numeric_mod_vast,
                                         numeric_mod_real64, timespec_numeric_mod))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_remainder BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-not
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_not(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_not BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *val    = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    int               retval = 0;
    switch (val->as_lit.prim_type)
    {
        case CACE_ARI_PRIM_UINT64:
            result.as_lit.value.as_uint64 = ~val->as_lit.value.as_uint64;
            break;
        case CACE_ARI_PRIM_INT64:
            result.as_lit.value.as_int64 = ~val->as_lit.value.as_int64;
            break;
        default:
            // leave lit as default undefined
            CACE_LOG_WARNING("bit-not: Unhandled numeric type %d", val->as_lit.prim_type);
            retval = 3;
            break;
    }

    if (!retval)
    {
        result.as_lit.prim_type    = val->as_lit.prim_type;
        result.as_lit.has_ari_type = true;
        result.as_lit.ari_type     = val->as_lit.ari_type;
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_not BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-and
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_and(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_and BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_integer_binary_operator(&result, lt_val, rt_val, bitwise_and_uvast, bitwise_and_vast))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_and BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-or
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_or(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_or BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_integer_binary_operator(&result, lt_val, rt_val, bitwise_or_uvast, bitwise_or_vast))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_or BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bit-xor
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari://ietf/amm-base/TYPEDEF/INTEGER
 */
static void refda_adm_ietf_dtnma_agent_oper_bit_xor(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_xor BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_integer_binary_operator(&result, lt_val, rt_val, bitwise_xor_uvast, bitwise_xor_vast))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_xor BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-not
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_not(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_not BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    cace_ari_bool     raw_val;

    if (!cace_ari_get_bool(val, &raw_val))
    {
        cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_bool(&result, !raw_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_not BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-and
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_and(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_and BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_bool     l_val, r_val;

    if (!cace_ari_get_bool(left, &l_val) && !cace_ari_get_bool(right, &r_val))
    {
        cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_bool(&result, l_val && r_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_and BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-or
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_or(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_or BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_bool     l_val, r_val;

    if (!cace_ari_get_bool(left, &l_val) && !cace_ari_get_bool(right, &r_val))
    {
        cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_bool(&result, l_val || r_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_or BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: bool-xor
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_bool_xor(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_xor BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *left  = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *right = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_bool     l_val, r_val;

    if (!cace_ari_get_bool(left, &l_val) && !cace_ari_get_bool(right, &r_val))
    {
        cace_ari_t result = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_bool(&result, l_val != r_val);

        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_xor BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-eq
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_eq(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_eq BODY
     * +-------------------------------------------------------------------------+
     */

    // FIXME: handle non-numeric types

    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;

    bool val = cace_ari_equal(lt_val, rt_val);
    cace_ari_set_bool(&result, val);
    refda_oper_eval_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_eq BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-ne
 * Description MISSING
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_ne(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ne BODY
     * +-------------------------------------------------------------------------+
     */

    // FIXME: handle non-numeric types

    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;

    bool val = cace_ari_equal(lt_val, rt_val);
    cace_ari_set_bool(&result, !val);
    refda_oper_eval_ctx_set_result_move(ctx, &result);
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ne BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-gt
 * Description:
 *   Compare two numbers by value. The result is true if the left value is
 *   greater than the right. The operands are cast to the least compatible
 *   numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_gt(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_gt BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_comparison_operator(&result, lt_val, rt_val, numeric_gt_uvast, numeric_gt_vast,
                                                 numeric_gt_real64))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_gt BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-ge
 * Description:
 *   Compare two numbers by value. The result is true if the left value is
 *   greater than or equal to the right. The operands are cast to the least
 *   compatible numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_ge(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ge BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_comparison_operator(&result, lt_val, rt_val, numeric_gte_uvast, numeric_gte_vast,
                                                 numeric_gte_real64))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ge BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-lt
 * Description:
 *   Compare two operands by value. The result is true if the left value is
 *   less than the right. The operands are cast to the least compatible
 *   numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_lt(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_lt BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_comparison_operator(&result, lt_val, rt_val, numeric_lt_uvast, numeric_lt_vast,
                                                 numeric_lt_real64))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_lt BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: compare-le
 * Description:
 *   Compare two operands by value. The result is true if the left value is
 *   less than or equal to the right. The operands are cast to the least
 *   compatible numeric type before the comparison.
 *
 * Parameters: none
 *
 * Result name "result", type: use of ari:/ARITYPE/BOOL
 */
static void refda_adm_ietf_dtnma_agent_oper_compare_le(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_le BODY
     * +-------------------------------------------------------------------------+
     */
    const cace_ari_t *lt_val = refda_oper_eval_ctx_get_operand_index(ctx, 0);
    const cace_ari_t *rt_val = refda_oper_eval_ctx_get_operand_index(ctx, 1);
    cace_ari_t        result = CACE_ARI_INIT_UNDEFINED;
    if (!cace_numeric_binary_comparison_operator(&result, lt_val, rt_val, numeric_lte_uvast, numeric_lte_vast,
                                                 numeric_lte_real64))
    {
        refda_oper_eval_ctx_set_result_move(ctx, &result);
    }
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_le BODY
     * +-------------------------------------------------------------------------+
     */
}

/* Name: tbl-filter
 * Description:
 *   Filter a table first by rows and then by columns.
 *
 * Parameters list:
 *  * Index 0, name "row-match", type: ulist of use of ari://ietf/amm-base/TYPEDEF/EXPR
 *  * Index 1, name "columns", type: ulist of use of ari://ietf/dtnma-agent/TYPEDEF/column-id
 *
 * Result name "out", type: use of ari:/ARITYPE/TBL
 */
static void refda_adm_ietf_dtnma_agent_oper_tbl_filter(refda_oper_eval_ctx_t *ctx)
{
    /*
     * +-------------------------------------------------------------------------+
     * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_tbl_filter BODY
     * +-------------------------------------------------------------------------+
     */
    /*
     * +-------------------------------------------------------------------------+
     * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_tbl_filter BODY
     * +-------------------------------------------------------------------------+
     */
}

int refda_adm_ietf_dtnma_agent_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: "
                   "ietf-dtnma-agent");
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(
        &(agent->objs), cace_amm_idseg_ref_withenum("ietf", 1),
        cace_amm_idseg_ref_withenum("dtnma-agent", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM), "2025-07-03");
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/column-id
            refda_amm_typedef_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }

            obj = refda_register_typedef(
                adm, cace_amm_idseg_ref_withenum("column-id", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_TYPEDEF_COLUMN_ID),
                objdata);
            // no parameters possible
        }

        /**
         * Register CONST objects
         */
        { // For ./CONST/hello
            refda_amm_const_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_const_desc_t));
            refda_amm_const_desc_init(objdata);
            // constant value:
            {
                cace_ari_ac_t acinit0;
                cace_ari_ac_init(&acinit0);
                {
                    cace_ari_t *item0 = cace_ari_list_push_back_new(acinit0.items);
                    // ari://ietf/dtnma-agent/EDD/sw-vendor
                    cace_ari_set_objref_path_intid(item0, 1, 1, CACE_ARI_TYPE_EDD, 0);
                }
                {
                    cace_ari_t *item0 = cace_ari_list_push_back_new(acinit0.items);
                    // ari://ietf/dtnma-agent/EDD/sw-version
                    cace_ari_set_objref_path_intid(item0, 1, 1, CACE_ARI_TYPE_EDD, 1);
                }
                {
                    cace_ari_t *item0 = cace_ari_list_push_back_new(acinit0.items);
                    // ari://ietf/dtnma-agent/EDD/capability
                    cace_ari_set_objref_path_intid(item0, 1, 1, CACE_ARI_TYPE_EDD, 2);
                }
                cace_ari_set_ac(&(objdata->value), &acinit0);
            }

            obj = refda_register_const(
                adm, cace_amm_idseg_ref_withenum("hello", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CONST_HELLO), objdata);
            // no parameters
        }

        /**
         * Register EDD objects
         */
        { // For ./EDD/sw-vendor
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_vendor;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("sw-vendor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VENDOR),
                objdata);
            // no parameters
        }
        { // For ./EDD/sw-version
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_version;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("sw-version", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VERSION),
                objdata);
            // no parameters
        }
        { // For ./EDD/capability
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 6);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "org-name");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "org-enum");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAST);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "model-name");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "model-enum");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAST);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "revision");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 5);
                    m_string_set_cstr(col->name, "features");
                    {
                        cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(col->typeobj));
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                            cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                        }
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_capability;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("capability", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_CAPABILITY),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-rx
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_rx;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("num-msg-rx", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-rx-failed
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("num-msg-rx-failed",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX_FAILED),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-tx
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_tx;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("num-msg-tx", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_TX),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-msg-tx-failed
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_tx_failed;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("num-msg-tx-failed",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_TX_FAILED),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-exec-started
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_started;

            obj =
                refda_register_edd(adm,
                                   cace_amm_idseg_ref_withenum(
                                       "num-exec-started", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_STARTED),
                                   objdata);
            // no parameters
        }
        { // For ./EDD/num-exec-succeeded
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("num-exec-succeeded",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_SUCCEEDED),
                objdata);
            // no parameters
        }
        { // For ./EDD/num-exec-failed
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/counter64
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 12);
                cace_amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_failed;

            obj = refda_register_edd(adm,
                                     cace_amm_idseg_ref_withenum(
                                         "num-exec-failed", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_FAILED),
                                     objdata);
            // no parameters
        }
        { // For ./EDD/exec-running
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 3);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "pid");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "target");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/ANY
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "state");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BYTE);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_exec_running;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("exec-running", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_EXEC_RUNNING),
                objdata);
            // no parameters
        }
        { // For ./EDD/odm-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 5);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "org-name");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/id-text
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "org-enum");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/id-int
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "model-name");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/id-text
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "model-enum");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/id-int
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "revision");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TEXTSTR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_odm_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("odm-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_ODM_LIST),
                objdata);
            // no parameters
        }
        { // For ./EDD/typedef-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 1);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TYPEDEF);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_typedef_list;

            obj = refda_register_edd(
                adm,
                cace_amm_idseg_ref_withenum("typedef-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TYPEDEF_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                cace_ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/const-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 2);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_CONST);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "type");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-semtype/TYPEDEF/semtype
                        cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_const_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("const-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_CONST_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                cace_ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/var-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 2);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "type");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-semtype/TYPEDEF/semtype
                        cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_var_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("var-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_VAR_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                cace_ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/sbr-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 7);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_SBR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "action");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/MAC
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 21);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "condition");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/EXPR
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "min-interval");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TD);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "max-count");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 5);
                    m_string_set_cstr(col->name, "init-enabled");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 6);
                    m_string_set_cstr(col->name, "enabled");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sbr_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("sbr-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SBR_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                cace_ari_set_bool(&(fparam->defval), false);
            }
        }
        { // For ./EDD/tbr-list
            refda_amm_edd_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&(objdata->prod_type), 7);
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBR);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "action");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/MAC
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 21);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "start-time");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/TIME
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 5);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "period");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_TD);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "max-count");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 5);
                    m_string_set_cstr(col->name, "init-enabled");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 6);
                    m_string_set_cstr(col->name, "enabled");
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                        cace_amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_tbr_list;

            obj = refda_register_edd(
                adm, cace_amm_idseg_ref_withenum("tbr-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TBR_LIST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                cace_ari_set_bool(&(fparam->defval), false);
            }
        }

        /**
         * Register CTRL objects
         */
        { // For ./CTRL/if-then-else
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_if_then_else;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("if-then-else", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_IF_THEN_ELSE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/eval-tgt
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 16);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-truthy");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/amm-base/TYPEDEF/exec-tgt
                            cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 19);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-falsy");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/amm-base/TYPEDEF/exec-tgt
                            cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 19);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
        }
        { // For ./CTRL/catch
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_catch;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("catch", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_CATCH), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "try");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/exec-tgt
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 19);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-failure");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/amm-base/TYPEDEF/exec-tgt
                            cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 19);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
        }
        { // For ./CTRL/wait-for
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_for;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("wait-for", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_FOR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "duration");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_TD);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/wait-until
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_until;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("wait-until", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_UNTIL),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "time");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_TP);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/wait-cond
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_cond;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("wait-cond", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_COND),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/eval-tgt
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 16);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/inspect
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/ANY
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_inspect;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("inspect", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_INSPECT),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "ref");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/VALUE-OBJ
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 9);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/report-on
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_report_on;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("report-on", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_REPORT_ON),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "template");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/rpt-tgt
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 22);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "destinations");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/network-base/TYPEDEF/endpoint-or-uri
                            cace_ari_set_objref_path_intid(&name, 1, 26, CACE_ARI_TYPE_TYPEDEF, 3);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
            }
        }
        { // For ./CTRL/ensure-odm
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_odm;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("ensure-odm", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_ODM),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "org-name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-text
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "org-id");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-int
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "model-name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-text
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "model-id");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-int
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/obsolete-odm
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_obsolete_odm;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("obsolete-odm", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_OBSOLETE_ODM),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "odm-ns");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_NAMESPACE);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/var-reset
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_var_reset;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("var-reset", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_RESET),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "target");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAR);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/var-store
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_var_store;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("var-store", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_STORE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "target");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAR);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "value");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-const
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_const;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("ensure-const", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_CONST),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "namespace");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_NAMESPACE);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-text
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-enum");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-int
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/EXPR
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/obsolete-const
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_obsolete_const;

            obj = refda_register_ctrl(adm,
                                      cace_amm_idseg_ref_withenum(
                                          "obsolete-const", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_OBSOLETE_CONST),
                                      objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_CONST);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-var
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_var;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("ensure-var", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_VAR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "namespace");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_NAMESPACE);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-text
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-enum");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-int
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "type");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-semtype/TYPEDEF/semtype
                    cace_ari_set_objref_path_intid(&name, 1, 24, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_NULL);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            // ari://ietf/amm-base/TYPEDEF/EXPR
                            cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                cace_ari_set_null(&(fparam->defval));
            }
        }
        { // For ./CTRL/obsolete-var
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_obsolete_var;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("obsolete-var", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_OBSOLETE_VAR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_VAR);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-sbr
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_sbr;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("ensure-sbr", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_SBR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "namespace");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_NAMESPACE);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-text
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-enum");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-int
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "action");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/MAC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 21);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "start-time");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/TIME
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 5);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/EXPR
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "min-interval");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_TD);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "max-count");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init-enabled");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-tbr
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_tbr;

            obj = refda_register_ctrl(
                adm, cace_amm_idseg_ref_withenum("ensure-tbr", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_TBR),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "namespace");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_NAMESPACE);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-name");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-text
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 25);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-enum");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/id-int
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 26);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "action");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/MAC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 21);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "start-time");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/TIME
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 5);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "period");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_TD);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "max-count");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "max-count");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_UVAST);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init-enabled");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/ensure-rule-enabled
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_rule_enabled;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("ensure-rule-enabled",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_RULE_ENABLED),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-id");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_SBR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "enabled");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
        }
        { // For ./CTRL/reset-rule-enabled
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_UINT);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_reset_rule_enabled;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("reset-rule-enabled",
                                            REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_RESET_RULE_ENABLED),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-id");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_SBR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
            }
        }
        { // For ./CTRL/obsolete-rule
            refda_amm_ctrl_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_obsolete_rule;

            obj = refda_register_ctrl(
                adm,
                cace_amm_idseg_ref_withenum("obsolete-rule", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_OBSOLETE_RULE),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj-id");
                {
                    // union
                    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_SBR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
                        {
                            cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                            cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBR);
                            cace_amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
            }
        }

        /**
         * Register OPER objects
         */
        { // For ./OPER/negate
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 1);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "val");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/NUMERIC
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_negate;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("negate", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_NEGATE), objdata);
            // no parameters
        }
        { // For ./OPER/add
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/ANY
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_add;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("add", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_ADD), objdata);
            // no parameters
        }
        { // For ./OPER/sub
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/ANY
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_sub;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("sub", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_SUB), objdata);
            // no parameters
        }
        { // For ./OPER/multiply
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/ANY
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_multiply;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("multiply", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_MULTIPLY),
                objdata);
            // no parameters
        }
        { // For ./OPER/divide
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/ANY
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_divide;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("divide", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_DIVIDE), objdata);
            // no parameters
        }
        { // For ./OPER/remainder
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/NUMERIC
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_remainder;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("remainder", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_REMAINDER),
                objdata);
            // no parameters
        }
        { // For ./OPER/bit-not
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 1);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "val");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/INTEGER
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_not;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bit-not", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_NOT),
                objdata);
            // no parameters
        }
        { // For ./OPER/bit-and
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/INTEGER
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_and;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bit-and", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_AND),
                objdata);
            // no parameters
        }
        { // For ./OPER/bit-or
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/INTEGER
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_or;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bit-or", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_OR), objdata);
            // no parameters
        }
        { // For ./OPER/bit-xor
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/INTEGER
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                // ari://ietf/amm-base/TYPEDEF/INTEGER
                cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 1);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_xor;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bit-xor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_XOR),
                objdata);
            // no parameters
        }
        { // For ./OPER/bool-not
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 1);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "val");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_not;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bool-not", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_NOT),
                objdata);
            // no parameters
        }
        { // For ./OPER/bool-and
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_and;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bool-and", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_AND),
                objdata);
            // no parameters
        }
        { // For ./OPER/bool-or
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_or;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bool-or", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_OR),
                objdata);
            // no parameters
        }
        { // For ./OPER/bool-xor
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_xor;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("bool-xor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_XOR),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-eq
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_eq;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("compare-eq", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_EQ),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-ne
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/ANY
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 8);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_ne;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("compare-ne", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_NE),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-gt
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_gt;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("compare-gt", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GT),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-ge
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_ge;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("compare-ge", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GE),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-lt
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_lt;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("compare-lt", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LT),
                objdata);
            // no parameters
        }
        { // For ./OPER/compare-le
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 2);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "left");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 1);
                string_set_str(operand->name, "right");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    // ari://ietf/amm-base/TYPEDEF/NUMERIC
                    cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 3);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_BOOL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_le;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("compare-le", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LE),
                objdata);
            // no parameters
        }
        { // For ./OPER/tbl-filter
            refda_amm_oper_desc_t *objdata = CACE_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // operands:
            cace_amm_named_type_array_resize(objdata->operand_types, 1);
            {
                cace_amm_named_type_t *operand = cace_amm_named_type_array_get(objdata->operand_types, 0);
                string_set_str(operand->name, "in");
                {
                    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                    cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBL);
                    cace_amm_type_set_use_ref_move(&(operand->typeobj), &name);
                }
            }
            // result type:
            {
                cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                cace_ari_set_aritype(&name, CACE_ARI_TYPE_TBL);
                cace_amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_tbl_filter;

            obj = refda_register_oper(
                adm, cace_amm_idseg_ref_withenum("tbl-filter", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_TBL_FILTER),
                objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "row-match");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/amm-base/TYPEDEF/EXPR
                        cace_ari_set_objref_path_intid(&name, 1, 25, CACE_ARI_TYPE_TYPEDEF, 18);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "columns");
                {
                    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&(fparam->typeobj));
                    {
                        cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
                        // ari://ietf/dtnma-agent/TYPEDEF/column-id
                        cace_ari_set_objref_path_intid(&name, 1, 1, CACE_ARI_TYPE_TYPEDEF, 1);
                        cace_amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
        }
    }
    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}
