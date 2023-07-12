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

/*****************************************************************************
 **
 ** File Name: ldc.h
 **
 ** Description: This implements the NM Agent Local Data Collector (LDC).
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  10/22/11  E. Birrane     Code comments and functional updates. (JHU/APL)
 **  01/10/13  E. Birrane     Update to latest version of DTNMP. Cleanup. (JHU/APL)
 **  10/04/18  E. Birrane     Updaye to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#ifndef _LDC_H_
#define _LDC_H_

#include "../shared/adm/adm.h"

#include "../shared/primitives/report.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LDC_MAX_NESTING (5)


tnv_t* ldc_collect(ari_t *id, tnvc_t *parms);

tnv_t *ldc_collect_cnst(ari_t *id, tnvc_t *parms);
tnv_t *ldc_collect_edd(ari_t *id, tnvc_t *parms);
tnv_t *ldc_collect_lit(ari_t *id);
tnv_t *ldc_collect_rpt(ari_t *id, tnvc_t *parms);
tnv_t *ldc_collect_var(ari_t *id, tnvc_t *parms);

int    ldc_fill_rpt(rpttpl_t *rpttpl, rpt_t *rpt);


#ifdef __cplusplus
}
#endif

#endif  /* _LDC_H_ */

