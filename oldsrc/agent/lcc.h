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
 ** File Name: lcc.h
 **
 ** Description: This implements the NM Agent Local Command and Control (LCC).
 **              This applies controls and macros.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  01/22/13  E. Birrane     Update to latest version of DTNMP. Cleanup. (JHU/APL)
 *****************************************************************************/

#ifndef _LCC_H_
#define _LCC_H_

#include "../shared/adm/adm.h"
#include "../shared/utils/nm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCC_MAX_NESTING 5


// Todo: Talk about why we separate out parameters.

int lcc_run_ac(ac_t *ac, tnvc_t *parent_parms);

int lcc_run_ctrl(ctrl_t *ctrl, tnvc_t *parent_parms);

int lcc_run_macro(macdef_t *mac, tnvc_t *parent_parms);



void lcc_send_retval(eid_t *rx, tnv_t *retval, ctrl_t *ctrl, tnvc_t *parms);


#ifdef __cplusplus
}
#endif

#endif  /* _LCC_H_ */

