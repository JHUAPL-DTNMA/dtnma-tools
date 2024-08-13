/*
 * Copyright (c) 2012-2023 The Johns Hopkins University Applied Physics
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
 ** File Name: rda.h
 **
 ** Description: This implements the Remote Data Aggregator (RDA)
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  01/10/13  E. Birrane     Initial Implementation (JHU/APL)
 **  10/04/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 **  11/23/21  E. Birrane     Added table sets (JHU/APL)
 *****************************************************************************/

#ifndef RDA_H_
#define RDA_H_

#include "../shared/primitives/rules.h"
#include "../shared/primitives/report.h"
#include "../shared/msg/msg.h"
#include "nmagent.h"


#ifdef __cplusplus
extern "C" {
#endif


#define RDA_DEF_NUM_RPTS 8
#define RDA_DEF_NUM_TBLS 8
#define RDA_DEF_NUM_TBRS 8
#define RDA_DEF_NUM_SBRS 8


/*
 * TODO: Sort these vectors by time to execute.
 */
typedef struct
{
	vector_t rpt_msgs; /* of type (msg_rpt_t *)  */
	vector_t tbl_msgs; /* of type (msg_tbl_t *)  */
	vector_t tbrs;    /* of type (rule_t *) */
	vector_t sbrs;    /* of type (rule_t *) */
} agent_db_t;

extern agent_db_t gAgentDb;

int rda_init();

void rda_signal_shutdown();
void         rda_cleanup();

msg_rpt_t*   rda_get_msg_rpt(eid_t recipient);
msg_tbl_t*   rda_get_msg_tbl(eid_t recipient);

OS_time_t rda_earliest_ctrl();
int rda_process_ctrls(OS_time_t nowtime);
void * rda_ctrls(void *arg);

void rda_scan_tbrs_cb(rh_elt_t *elt, void *tag);
void rda_scan_sbrs_cb(rh_elt_t *elt, void *tag);

OS_time_t rda_earliest_rule();
int rda_process_rules (OS_time_t nowtime);
void * rda_rules(void *arg);


int          rda_send_reports(nmagent_t *agent);
int          rda_send_tables(nmagent_t *agent);
void * rda_reports(void *arg);
void * rda_tables(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* RDA_H_ */
