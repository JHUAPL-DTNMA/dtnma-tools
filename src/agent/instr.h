/*
 * Copyright (c) 2013-2023 The Johns Hopkins University Applied Physics
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

/*****************************************************************************
 **
 ** \file instr.h
 **
 **
 ** Description: DTNMP Instrumentation headers.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  07/04/13  E. Birrane     Initial Implementation (JHU/APL)
 **  11/23/21  E. Birrane     Added counting of sent table sets (JHU/APL)
 *****************************************************************************/


#ifndef _INSTR_H_
#define _INSTR_H_


#include "../shared/utils/nm_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * +--------------------------------------------------------------------------+
 * |							  CONSTANTS  								  +
 * +--------------------------------------------------------------------------+
 */



/*
 * +--------------------------------------------------------------------------+
 * |							  	MACROS  								  +
 * +--------------------------------------------------------------------------+
 */


/*
 * +--------------------------------------------------------------------------+
 * |							  DATA TYPES  								  +
 * +--------------------------------------------------------------------------+
 */


typedef struct {
	unsigned long num_sent_rpts;
	unsigned long num_sent_tbls;
	unsigned long num_tbrs;
	unsigned long num_tbrs_run;
	unsigned long num_sbrs;
	unsigned long num_sbrs_run;
	unsigned long num_macros_run;
	unsigned long num_ctrls_run;
} agent_instr_t;



/*
 * +--------------------------------------------------------------------------+
 * |						  FUNCTION PROTOTYPES  							  +
 * +--------------------------------------------------------------------------+
 */


void agent_instr_init();
void agent_instr_clear();

extern agent_instr_t gAgentInstr;


#ifdef __cplusplus
}
#endif

#endif /* _INSTR_H_ */
