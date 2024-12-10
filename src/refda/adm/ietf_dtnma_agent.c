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
#include "ietf_dtnma_agent.h"
#include "refda/agent.h"
#include "refda/register.h"
#include "refda/valprod.h"
#include "refda/reporting.h"
#include <cace/amm/semtype.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*   STOP CUSTOM INCLUDES HERE  */

/*   START CUSTOM FUNCTIONS HERE */
/*   STOP CUSTOM FUNCTIONS HERE  */


/* Name: sw-vendor
 * Description:
 *   The vendor for this Agent implementation.
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_sw_vendor(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_vendor BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_vendor BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: sw-version
 * Description:
 *   The version for this Agent implementation.
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_sw_version(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_version BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sw_version BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: capability
 * Description:
 *   A table to indicate the ADM capability of the sending agent.
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_capability(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_capability BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_capability BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: num-msg-rx
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_rx(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: num-msg-rx-failed
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: num-msg-tx
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_num_msg_tx(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_msg_tx BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: num-exec-started
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_started(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_started BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_num_exec_started BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: num-exec-succeeded
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
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
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_num_exec_failed(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
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
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_exec_running(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_exec_running BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_exec_running BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: typedef-list
 * Description:
 *   A table of TYPEDEF within the agent.
 * Parameters::
 * include-adm
 */
static void refda_adm_ietf_dtnma_agent_edd_typedef_list(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_typedef_list BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_typedef_list BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: var-list
 * Description:
 *   A table of VAR within the agent.
 * Parameters::
 * include-adm
 */
static void refda_adm_ietf_dtnma_agent_edd_var_list(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_var_list BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_var_list BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: sbr-list
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_sbr_list(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sbr_list BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_sbr_list BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: tbr-list
 * Description:
 *   
 *   
 * Parameters: none
 */
static void refda_adm_ietf_dtnma_agent_edd_tbr_list(const refda_amm_edd_desc_t *obj _U_, refda_valprod_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_tbr_list BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_edd_tbr_list BODY
	 * +-------------------------------------------------------------------------+
	 */

}


/* Name: if-then-else
 * Description:
 *   Evaluate an expression and follow one of two branches of
 * further evaluation.
 * Parameters::
 * condition
 * on-truthy
 * on-falsy
 */
static int refda_adm_ietf_dtnma_agent_ctrl_if_then_else(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
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

    return 0; //FIXME
}


/* Name: catch
 * Description:
 *   Attempt to execute a target, and if there is some failure catch it
 * and execute an alternative target.
 * Parameters::
 * try
 * on-failure
 */
static int refda_adm_ietf_dtnma_agent_ctrl_catch(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
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

    return 0; //FIXME
}


/* Name: wait-for
 * Description:
 *   This control causes the execution to pause for a given amount of time.
 * This is intended to be used within a macro to separate controls
 * in time.
 * Parameters::
 * duration
 */
static int refda_adm_ietf_dtnma_agent_ctrl_wait_for(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_for BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_for BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: wait-until
 * Description:
 *   This control causes the execution to pause until a specific absolute
 * time point.
 * This is intended to be used within a macro to separate controls
 * in time or as a first macro item to delay execution after the time
 * of reception.
 * Parameters::
 * time
 */
static int refda_adm_ietf_dtnma_agent_ctrl_wait_until(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_until BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_until BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: wait-cond
 * Description:
 *   This control causes the execution to pause until a condition expression
 * evaluates to truthy.
 * This is intended to be used within a macro to separate controls
 * in time or as a first macro item to delay execution until the
 * condition is met.
 * Parameters::
 * condition
 */
static int refda_adm_ietf_dtnma_agent_ctrl_wait_cond(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_cond BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_wait_cond BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: inspect
 * Description:
 *   Produce a result value to inspect the agent state.
 * This does not perform any EXPR evaluation or RPTT handling.
 * Parameters::
 * ref
 */
static int refda_adm_ietf_dtnma_agent_ctrl_inspect(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_inspect BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_inspect BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: report-on
 * Description:
 *   Generate a report on an object without needing to define an object.
 * The parameter is a single RPTT list that would be produced by an object.
 * If used for more than one-shot diagnostics, defining a RPTT
 * (e.g. in a VAR) is more efficient because the RPTT item would not
 * be present in the report.
 * Parameters::
 * rptt
 */
static int refda_adm_ietf_dtnma_agent_ctrl_report_on(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_report_on BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_report_on BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: var-reset
 * Description:
 *   Modify a VAR state to its default value.
 * Parameters::
 * target
 */
static int refda_adm_ietf_dtnma_agent_ctrl_var_reset(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_reset BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_reset BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: var-store
 * Description:
 *   Modify a VAR state to a specific value.
 * Parameters::
 * target
 * value
 */
static int refda_adm_ietf_dtnma_agent_ctrl_var_store(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_store BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_var_store BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: ensure-var
 * Description:
 *   Ensure a specific VAR is present.
 * Parameters::
 * obj
 * type
 * init
 */
static int refda_adm_ietf_dtnma_agent_ctrl_ensure_var(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
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

    return 0; //FIXME
}


/* Name: discard-var
 * Description:
 *   Discard a specific VAR if it is present.
 * Parameters::
 * obj
 */
static int refda_adm_ietf_dtnma_agent_ctrl_discard_var(const refda_amm_ctrl_desc_t *obj _U_, refda_exec_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_discard_var BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_ctrl_discard_var BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: negate
 * Description:
 *   Negate a value.
 * This is equivalent to multiplying by -1 but a shorter
 * expression.
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_negate(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_negate BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_negate BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: add
 * Description:
 *   Add two numeric values.
 * The operands are cast to the least compatible numeric type
 * before the arithmetic.
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_add(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_add BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_add BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: sub
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_sub(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_sub BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_sub BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: multiply
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_multiply(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_multiply BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_multiply BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: divide
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_divide(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_divide BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_divide BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: remainder
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_remainder(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_remainder BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_remainder BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bit-not
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bit_not(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_not BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_not BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bit-and
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bit_and(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_and BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_and BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bit-or
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bit_or(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_or BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_or BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bit-xor
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bit_xor(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_xor BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bit_xor BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bool-not
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bool_not(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_not BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_not BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bool-and
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bool_and(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_and BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_and BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bool-or
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bool_or(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_or BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_or BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: bool-xor
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_bool_xor(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_xor BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_bool_xor BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: compare-eq
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_compare_eq(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_eq BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_eq BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: compare-ne
 * Description:
 *   
 *   
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_compare_ne(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ne BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ne BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: compare-gt
 * Description:
 *   Compare two numbers by value.
 * The result is true if the left value is greater than the right.
 * The operands are cast to the least compatible numeric type
 * before the comparison.
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_compare_gt(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_gt BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_gt BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: compare-ge
 * Description:
 *   Compare two numbers by value.
 * The result is true if the left value is greater than or equal
 * to the right.
 * The operands are cast to the least compatible numeric type
 * before the comparison.
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_compare_ge(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ge BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_ge BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: compare-lt
 * Description:
 *   Compare two operands by value.
 * The result is true if the left value is less than the right.
 * The operands are cast to the least compatible numeric type
 * before the comparison.
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_compare_lt(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_lt BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_lt BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: compare-le
 * Description:
 *   Compare two operands by value.
 * The result is true if the left value is less than or
 * equal to the right.
 * The operands are cast to the least compatible numeric type
 * before the comparison.
 * Parameters: none
 */
static int refda_adm_ietf_dtnma_agent_oper_compare_le(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
{
	/*
	 * +-------------------------------------------------------------------------+
	 * |START CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_le BODY
	 * +-------------------------------------------------------------------------+
	 */
	/*
	 * +-------------------------------------------------------------------------+
	 * |STOP CUSTOM FUNCTION refda_adm_ietf_dtnma_agent_oper_compare_le BODY
	 * +-------------------------------------------------------------------------+
	 */

    return 0; //FIXME
}


/* Name: tbl-filter
 * Description:
 *   Filter a table first by rows and then by columns.
 * Parameters::
 * row-match
 * columns
 */
static int refda_adm_ietf_dtnma_agent_oper_tbl_filter(const refda_amm_oper_desc_t *obj _U_, refda_eval_ctx_t *ctx _U_)
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

    return 0; //FIXME
}

int refda_adm_ietf_dtnma_agent_init(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_DEBUG("Registering ADM: " "ietf-dtnma-agent");
    REFDA_AGENT_LOCK(agent);

    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(&(agent->objs), "ietf-dtnma-agent", true, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM);
    if (adm)
    {
        cace_amm_obj_desc_t *obj;

        /**
         * Register TYPEDEF objects
         */
        { // For ./TYPEDEF/hellotyp
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_BYTE);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UINT);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("hellotyp", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_TYPEDEF_HELLOTYP), objdata);
            // no parameters possible
        }
        { // For ./TYPEDEF/column-id
            refda_amm_typedef_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
            refda_amm_typedef_desc_init(objdata);
            // named type:
            {
                // union
                amm_semtype_union_t *semtype = amm_type_set_union_size(&(objdata->typeobj), 2);
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
                {
                    amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(choice, &name);
                    }
                }
            }
            obj = refda_register_typedef(adm, cace_amm_obj_id_withenum("column-id", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_TYPEDEF_COLUMN_ID), objdata);
            // no parameters possible
        }

        /**
         * Register CONST objects
         */
        { // For ./CONST/hello
            refda_amm_const_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_const_desc_t));
            refda_amm_const_desc_init(objdata);
            // constant value:
            {
                ari_ac_t acinit;
                ari_ac_init(&acinit);
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    // ari://ietf-dtnma-agent/EDD/sw-vendor
                    ari_set_objref_path_intid(item, 1, ARI_TYPE_EDD, 0);
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    // ari://ietf-dtnma-agent/EDD/sw-version
                    ari_set_objref_path_intid(item, 1, ARI_TYPE_EDD, 1);
                }
                {
                    ari_t *item = ari_list_push_back_new(acinit.items);
                    // ari://ietf-dtnma-agent/EDD/capability
                    ari_set_objref_path_intid(item, 1, ARI_TYPE_EDD, 2);
                }
                ari_set_ac(&(objdata->value), &acinit);
            }
            obj = refda_register_const(adm, cace_amm_obj_id_withenum("hello", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CONST_HELLO), objdata);
            // no parameters

        }

        /**
         * Register EDD objects
         */
        { // For ./EDD/sw-vendor
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_vendor;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("sw-vendor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VENDOR), objdata);
            // no parameters

        }
        { // For ./EDD/sw-version
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sw_version;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("sw-version", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VERSION), objdata);
            // no parameters

        }
        { // For ./EDD/capability
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 4);
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "adm-name");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "enum");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_VAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "revision");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "features");
                    {
                        amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(col->typeobj));
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_TEXTSTR);
                            amm_type_set_use_ref_move(&(semtype->item_type), &name);
                        }
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_capability;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("capability", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_CAPABILITY), objdata);
            // no parameters

        }
        { // For ./EDD/num-msg-rx
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_rx;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("num-msg-rx", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX), objdata);
            // no parameters

        }
        { // For ./EDD/num-msg-rx-failed
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_rx_failed;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("num-msg-rx-failed", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX_FAILED), objdata);
            // no parameters

        }
        { // For ./EDD/num-msg-tx
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_msg_tx;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("num-msg-tx", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_TX), objdata);
            // no parameters

        }
        { // For ./EDD/num-exec-started
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_started;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("num-exec-started", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_STARTED), objdata);
            // no parameters

        }
        { // For ./EDD/num-exec-succeeded
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_succeeded;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("num-exec-succeeded", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_SUCCEEDED), objdata);
            // no parameters

        }
        { // For ./EDD/num-exec-failed
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/counter64
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 12);
                amm_type_set_use_ref_move(&(objdata->prod_type), &name);
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_num_exec_failed;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("num-exec-failed", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_FAILED), objdata);
            // no parameters

        }
        { // For ./EDD/exec-running
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 3);
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "pid");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "target");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/ANY
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "state");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_BYTE);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_exec_running;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("exec-running", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_EXEC_RUNNING), objdata);
            // no parameters

        }
        { // For ./EDD/typedef-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 1);
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TYPEDEF);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_typedef_list;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("typedef-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TYPEDEF_LIST), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                
            }

        }
        { // For ./EDD/var-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 2);
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_VAR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "type");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TYPE-REF
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_var_list;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("var-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_VAR_LIST), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "include-adm");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_BOOL);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
                
            }

        }
        { // For ./EDD/sbr-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 6);
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_SBR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "action");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/MAC
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 21);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "start-time");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TIME
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 5);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "condition");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/EXPR
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "min-interval");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TD);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 5);
                    m_string_set_cstr(col->name, "max-count");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_sbr_list;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("sbr-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SBR_LIST), objdata);
            // no parameters

        }
        { // For ./EDD/tbr-list
            refda_amm_edd_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
            refda_amm_edd_desc_init(objdata);
            // produced type
            {
                // table template
                amm_semtype_tblt_t *semtype = amm_type_set_tblt_size(&(objdata->prod_type), 5);
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 0);
                    m_string_set_cstr(col->name, "obj");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TBR);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 1);
                    m_string_set_cstr(col->name, "action");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/MAC
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 21);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 2);
                    m_string_set_cstr(col->name, "start-time");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/TIME
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 5);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 3);
                    m_string_set_cstr(col->name, "period");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_TD);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
                {
                    amm_semtype_tblt_col_t *col = amm_semtype_tblt_col_array_get(semtype->columns, 4);
                    m_string_set_cstr(col->name, "max-count");
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        ari_set_aritype(&name, ARI_TYPE_UVAST);
                        amm_type_set_use_ref_move(&(col->typeobj), &name);
                    }
                }
            }
            // callback:
            objdata->produce = refda_adm_ietf_dtnma_agent_edd_tbr_list;

            obj = refda_register_edd(adm, cace_amm_obj_id_withenum("tbr-list", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TBR_LIST), objdata);
            // no parameters

        }


        /**
         * Register CTRL objects
         */
        { // For ./CTRL/if-then-else
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_if_then_else;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("if-then-else", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_IF_THEN_ELSE), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/eval-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 16);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-truthy");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/exec-tgt
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-falsy");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/exec-tgt
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                
            }

        }
        { // For ./CTRL/catch
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_catch;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("catch", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_CATCH), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "try");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/exec-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "on-failure");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/exec-tgt
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 19);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                
            }

        }
        { // For ./CTRL/wait-for
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_for;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("wait-for", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_FOR), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "duration");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_TD);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/wait-until
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_until;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("wait-until", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_UNTIL), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "time");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_TP);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/wait-cond
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_wait_cond;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("wait-cond", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_COND), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "condition");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/eval-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 16);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/inspect
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/ANY
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_inspect;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("inspect", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_INSPECT), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "ref");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/VALUE-OBJ
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 9);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/report-on
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_report_on;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("report-on", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_REPORT_ON), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "rptt");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/rpt-tgt
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 22);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/var-reset
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_var_reset;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("var-reset", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_RESET), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "target");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/var-store
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_var_store;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("var-store", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_STORE), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "target");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "value");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/ANY
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 8);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }
        { // For ./CTRL/ensure-var
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_ensure_var;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("ensure-var", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_VAR), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "type");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    // ari://ietf-amm/TYPEDEF/TYPE-REF
                    ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 0);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "init");
                {
                    // union
                    amm_semtype_union_t *semtype = amm_type_set_union_size(&(fparam->typeobj), 2);
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 0);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            ari_set_aritype(&name, ARI_TYPE_NULL);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                    {
                        amm_type_t *choice = amm_type_array_get(semtype->choices, 1);
                        {
                            ari_t name = ARI_INIT_UNDEFINED;
                            // ari://ietf-amm/TYPEDEF/EXPR
                            ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                            amm_type_set_use_ref_move(choice, &name);
                        }
                    }
                }
                
            }

        }
        { // For ./CTRL/discard-var
            refda_amm_ctrl_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
            refda_amm_ctrl_desc_init(objdata);
            // no result type
            // callback:
            objdata->execute = refda_adm_ietf_dtnma_agent_ctrl_discard_var;

            obj = refda_register_ctrl(adm, cace_amm_obj_id_withenum("discard-var", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_DISCARD_VAR), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "obj");
                {
                    ari_t name = ARI_INIT_UNDEFINED;
                    ari_set_aritype(&name, ARI_TYPE_VAR);
                    amm_type_set_use_ref_move(&(fparam->typeobj), &name);
                }
            }

        }


        /**
         * Register OPER objects
         */
        { // For ./OPER/negate
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_negate;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("negate", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_NEGATE), objdata);
            // no parameters

        }
        { // For ./OPER/add
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_add;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("add", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_ADD), objdata);
            // no parameters

        }
        { // For ./OPER/sub
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_sub;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("sub", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_SUB), objdata);
            // no parameters

        }
        { // For ./OPER/multiply
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_multiply;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("multiply", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_MULTIPLY), objdata);
            // no parameters

        }
        { // For ./OPER/divide
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_divide;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("divide", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_DIVIDE), objdata);
            // no parameters

        }
        { // For ./OPER/remainder
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/NUMERIC
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 3);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_remainder;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("remainder", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_REMAINDER), objdata);
            // no parameters

        }
        { // For ./OPER/bit-not
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_not;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bit-not", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_NOT), objdata);
            // no parameters

        }
        { // For ./OPER/bit-and
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_and;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bit-and", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_AND), objdata);
            // no parameters

        }
        { // For ./OPER/bit-or
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_or;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bit-or", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_OR), objdata);
            // no parameters

        }
        { // For ./OPER/bit-xor
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                // ari://ietf-amm/TYPEDEF/INTEGER
                ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 1);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bit_xor;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bit-xor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_XOR), objdata);
            // no parameters

        }
        { // For ./OPER/bool-not
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_not;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bool-not", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_NOT), objdata);
            // no parameters

        }
        { // For ./OPER/bool-and
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_and;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bool-and", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_AND), objdata);
            // no parameters

        }
        { // For ./OPER/bool-or
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_or;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bool-or", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_OR), objdata);
            // no parameters

        }
        { // For ./OPER/bool-xor
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_bool_xor;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("bool-xor", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_XOR), objdata);
            // no parameters

        }
        { // For ./OPER/compare-eq
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_eq;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("compare-eq", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_EQ), objdata);
            // no parameters

        }
        { // For ./OPER/compare-ne
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_ne;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("compare-ne", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_NE), objdata);
            // no parameters

        }
        { // For ./OPER/compare-gt
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_gt;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("compare-gt", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GT), objdata);
            // no parameters

        }
        { // For ./OPER/compare-ge
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_ge;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("compare-ge", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GE), objdata);
            // no parameters

        }
        { // For ./OPER/compare-lt
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_lt;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("compare-lt", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LT), objdata);
            // no parameters

        }
        { // For ./OPER/compare-le
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_BOOL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_compare_le;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("compare-le", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LE), objdata);
            // no parameters

        }
        { // For ./OPER/tbl-filter
            refda_amm_oper_desc_t *objdata = ARI_MALLOC(sizeof(refda_amm_oper_desc_t));
            refda_amm_oper_desc_init(objdata);
            // result type
            {
                ari_t name = ARI_INIT_UNDEFINED;
                ari_set_aritype(&name, ARI_TYPE_TBL);
                amm_type_set_use_ref_move(&(objdata->res_type), &name);
            }
            // callback:
            objdata->evaluate = refda_adm_ietf_dtnma_agent_oper_tbl_filter;

            obj = refda_register_oper(adm, cace_amm_obj_id_withenum("tbl-filter", REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_TBL_FILTER), objdata);
            // parameters:
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "row-match");
                {
                    amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(fparam->typeobj));
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-amm/TYPEDEF/EXPR
                        ari_set_objref_path_intid(&name, 0, ARI_TYPE_TYPEDEF, 18);
                        amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }
            {
                cace_amm_formal_param_t *fparam = refda_register_add_param(obj, "columns");
                {
                    amm_semtype_ulist_t *semtype = amm_type_set_ulist(&(fparam->typeobj));
                    {
                        ari_t name = ARI_INIT_UNDEFINED;
                        // ari://ietf-dtnma-agent/TYPEDEF/column-id
                        ari_set_objref_path_intid(&name, 1, ARI_TYPE_TYPEDEF, 1);
                        amm_type_set_use_ref_move(&(semtype->item_type), &name);
                    }
                }
            }

        }

    }
    REFDA_AGENT_UNLOCK(agent);
    return 0;
}
