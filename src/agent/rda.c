/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2012 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 **
 ******************************************************************************/
/*****************************************************************************
 ** \file rda.c
 **
 ** File Name: rda.c
 **
 **
 ** Subsystem:
 **          Network Management Utilities: DTNMP Agent
 **
 ** Description: This file implements the DTNMP Agent's Remote Data
 **              Aggregator thread.  Periodically, this thread runs to evaluate
 **              what production rules are queued for execution, runs these
 **              rules, constructs the appropriate data reports, and queues
 **              them for transmission.
 **
 ** Notes:
 **
 ** Assumptions:
 **      1. We assume that this code is not under such tight profiling
 **         constraints that sanity checks are too expensive.
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  09/06/11  M. Reid        Initial Implementation (JHU/APL)
 **  10/21/11  E. Birrane     Code comments and functional updates. (JHU/APL)
 **  06/27/13  E. Birrane     Support persisted rules. (JHU/APL)
 **  08/30/15  E. Birrane     Updated support for SRL/TRL (Secure DTN - NASA: NNX14CS58P)
 **  10/04/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#include "shared/platform.h"
#include "shared/primitives/time.h"

#include "../shared/utils/utils.h"
#include "instr.h"
#include "../shared/primitives/expr.h"

#include "../shared/utils/db.h"
#include "../shared/msg/msg_if.h"

#include "ldc.h"
#include "lcc.h"
#include "rda.h"
#include "nmagent.h"


agent_db_t gAgentDb;

/** Context for active rule scanning. */
typedef struct
{
  /// Scanning time
  OS_time_t nowtime;
  /// Vector of valid rules
  vector_t *vec;
} rda_scan_context_t;

/******************************************************************************
 *
 * \par Function Name: rda_cleanup
 *
 * \par Purpose: Cleans up any resources left over by the RDA when it exits.
 *
 * \retval void
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/21/11  E. Birrane     Initial implementation,
 *  05/20/15  E. Birrane     Switched to global, mutex-protected lysts.
 *  10/04/18  E. Birrane     Updated to AMP V0.5 (JHU/APL)
 *  11/23/21  E. Birrane     Added table sets (JHU/APL)
 *****************************************************************************/

void rda_cleanup()
{
        vec_release(&(gAgentDb.rpt_msgs), 0);
        vec_release(&(gAgentDb.tbl_msgs), 0);
        vec_release(&(gAgentDb.tbrs), 0);
        vec_release(&(gAgentDb.sbrs), 0);
}

int rda_init()
{
        int success;

        gAgentDb.rpt_msgs = vec_create(RDA_DEF_NUM_RPTS, msg_rpt_cb_del_fn, NULL, NULL, 0, &success);
        gAgentDb.tbl_msgs = vec_create(RDA_DEF_NUM_TBLS, msg_tbl_cb_del_fn, NULL, NULL, 0, &success);

        gAgentDb.tbrs = vec_create(RDA_DEF_NUM_TBRS, NULL, NULL, NULL, 0, &success);
        gAgentDb.sbrs = vec_create(RDA_DEF_NUM_SBRS, NULL, NULL, NULL, 0, &success);

        return success;
}

void rda_signal_shutdown()
{
  vector_t *vec = &(gVDB.ctrls);
  pthread_mutex_lock(&vec->lock);
  pthread_cond_broadcast(&vec->cond_ins_mod);
  pthread_mutex_unlock(&vec->lock);

  rhht_t *ht = &(gVDB.rules);
  pthread_mutex_lock(&ht->lock);
  pthread_cond_broadcast(&ht->cond_ins_mod);
  pthread_mutex_unlock(&ht->lock);

  vec = &(gAgentDb.rpt_msgs);
  pthread_mutex_lock(&vec->lock);
  pthread_cond_broadcast(&vec->cond_ins_mod);
  pthread_mutex_unlock(&vec->lock);
}

/******************************************************************************
 *
 * \par Function Name: rda_get_report
 *
 * \par Purpose: Find the data report intended for a given recipient. The
 *               agent will, when possible, combine reports for a single
 *               recipient.
 *
 * \param[in]  recipient     - The recipient for which we are searching for
 *                             a report.
 *
 * \par Notes:
 *
 * \return !NULL - Report for this recipient.
 *         NULL  - Error.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/21/11  E. Birrane     Initial implementation,
 *  05/20/15  E. Birrane     Switched to using global lyst
 *  07/04/15  E. Birrane     Refactored report type and TDC support.
 *****************************************************************************/

msg_rpt_t *rda_get_msg_rpt(eid_t recipient)
{
        vecit_t it;
        int success;
        msg_rpt_t *msg_rpt;

    AMP_DEBUG_ENTRY("rda_get_report","(%s)", recipient.name);

    /* Step 0: Sanity check. */
    if(strlen(recipient.name) <= 0)
    {
        AMP_DEBUG_ERR("rda_get_report","Bad parms.",NULL);
        return NULL;
    }

    /* Step 1: See if we already have a report message going to
     * that recipient. If so, return it.
     */
    for(it = vecit_first(&(gAgentDb.rpt_msgs)); vecit_valid(it); it = vecit_next(it))
    {
        msg_rpt_t *cur = vecit_data(it);

        vec_find(&(cur->rx), recipient.name, &success);
        if(success == AMP_OK)
        {
                return cur;
        }
    }

    /* Step 2; If we get here, create a new report for that recipient. */
    if((msg_rpt = msg_rpt_create(recipient.name)) != NULL)
    {
        if(vec_push(&(gAgentDb.rpt_msgs), msg_rpt) != VEC_OK)
        {
                msg_rpt_release(msg_rpt, 1);
                return NULL;
        }
    }

    return msg_rpt;
}


/******************************************************************************
 *
 * \par Function Name: rda_get_msg_tbl
 *
 * \par Purpose: Find the table set intended for a given recipient. The
 *               agent will, when possible, combine table sets for a single
 *               recipient.
 *
 * \param[in]  recipient     - The recipient for which we are searching for
 *                             a table set.
 *
 * \par Notes:
 *
 * \return !NULL - Report for this recipient.
 *         NULL  - Error.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  11/23/21  E. Birrane     Initial Implementation (JHU/APL)
 *****************************************************************************/

msg_tbl_t *rda_get_msg_tbl(eid_t recipient)
{
    vecit_t it;
    msg_tbl_t *msg_tbl;

    AMP_DEBUG_ENTRY("rda_get_msg_tbl","(%s)", recipient.name);

    /* Step 0: Sanity check. */
    if(strlen(recipient.name) <= 0)
    {
        AMP_DEBUG_ERR("rda_get_msg_tbl","Bad parms.",NULL);
        return NULL;
    }

    /* Step 1: See if we already have a report message going to
     * that recipient. If so, return it.
     */
    for(it = vecit_first(&(gAgentDb.tbl_msgs)); vecit_valid(it); it = vecit_next(it))
    {
        int success;
        msg_tbl_t *cur = vecit_data(it);

        vec_find(&(cur->rx), recipient.name, &success);
        if(success == AMP_OK)
        {
            return cur;
        }
    }

    /* Step 2; If we get here, create a new report for that recipient. */
    if((msg_tbl = msg_tbl_create(recipient.name)) != NULL)
    {
        if(vec_push(&(gAgentDb.tbl_msgs), msg_tbl) != VEC_OK)
        {
            msg_tbl_release(msg_tbl, 1);
            return NULL;
        }
    }

    return msg_tbl;
}


OS_time_t rda_earliest_ctrl()
{
  OS_time_t earliest = OS_TIME_MAX;
  vecit_t it;

  vec_lock(&(gVDB.ctrls));
  for(it = vecit_first(&(gVDB.ctrls)); vecit_valid(it); it = vecit_next(it))
  {
    ctrl_t *ctrl = vecit_data(it);
    if(ctrl == NULL)
    {
      continue;
    }
    earliest = TimeMin(earliest, ctrl->start);
  }
  vec_unlock(&(gVDB.ctrls));
  return earliest;
}


/******************************************************************************
 *
 * \par Function Name: rda_process_ctrls
 *
 * \par Purpose: Run all controls that are ready to run. Once run, remove ctrl.
 *
 * \retval int -  AMP Status Code
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/21/11  E. Birrane     Initial implementation,
 *  10/04/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/
int rda_process_ctrls(OS_time_t nowtime)
{
        vec_idx_t i;
        ctrl_t *ctrl;
    vecit_t it;

        vec_lock(&(gVDB.ctrls));
    for(i = 0, it = vecit_first(&(gVDB.ctrls)); vecit_valid(it); it = vecit_next(it),i++)
        {
        ctrl = vecit_data(it);

                if(ctrl != NULL)
                {
                        if(TimeCompare(nowtime, ctrl->start) >= 0)
                        {
                                lcc_run_ctrl(ctrl, NULL);
//                                db_forget(&(ctrl->desc), gDB.ctrls);

                                /* Don't access ctrl after this call...*/
                                vec_del(&(gVDB.ctrls), i);
                        }
                }
        }
        vec_unlock(&(gVDB.ctrls));
        return AMP_OK;
}


void* rda_ctrls(void *arg)
{
  nmagent_t *agent = arg;
#ifndef mingw
    AMP_DEBUG_ENTRY("rda_ctrls","(0x%X)", (unsigned long) pthread_self()); //threadId);
#endif

    AMP_DEBUG_INFO("rda_ctrls","Running Remote Data Aggregator Thread.", NULL);

    /* While the DTNMP Agent is running...*/
    while(true)
    {
        OS_time_t nowtime;

        if (pthread_mutex_lock(&gVDB.ctrls.lock))
        {
          AMP_DEBUG_ERR("rda_ctrls", "failed mutex %d lock", gVDB.ctrls.lock);
          return NULL;
        }
        if (!daemon_run_get(&agent->running))
        {
          pthread_mutex_unlock(&gVDB.ctrls.lock);
          break;
        }

        OS_GetLocalTime(&nowtime);
        OS_time_t next_ctrl = rda_earliest_ctrl();
        AMP_DEBUG_INFO("rda_ctrls", "next CTRL start at %lld", next_ctrl.ticks);
        if (TimeCompare(next_ctrl, nowtime) > 0)
        {
          int ret;
          OS_time_t delta;
          delta = OS_TimeSubtract(next_ctrl, nowtime);
          AMP_DEBUG_INFO("rda_ctrls", "sleeping up to %lld", delta.ticks);

          const struct timespec abstime = TimeToTimespec(next_ctrl);
          ret = pthread_cond_timedwait(&gVDB.ctrls.cond_ins_mod, &gVDB.ctrls.lock, &abstime);
          // return may have been earlier than the timeout
          OS_GetLocalTime(&nowtime);
          AMP_DEBUG_INFO("rda_ctrls", "running at %lld from %d (%s)", nowtime.ticks, ret, strerror(ret));
        }
        if (pthread_mutex_unlock(&gVDB.ctrls.lock))
        {
          AMP_DEBUG_ERR("rda_ctrls", "failed mutex %p unlock", &gVDB.ctrls.lock);
          return NULL;
        }

        /* Run any ctrls that are due and forget them if they are done. */
        if(rda_process_ctrls(nowtime) != AMP_OK)
        {
                AMP_DEBUG_ERR("rda_ctrls","Problem processing ctrls.", NULL);
        }
    } // end while

    AMP_DEBUG_ALWAYS("rda_ctrls","Shutting Down Remote Data Aggregator Thread.",NULL);
    return NULL;
}


/******************************************************************************
 *
 * \par Function Name: rda_scan_rules
 *
 * \par Purpose: Walks the list of rules defined by this agent, determines
 *               which rules are to be executed, and updates housekeeping
 *               information for each rule.
 *
 * \retval int -  AMP Status Code
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/21/11  E. Birrane     Initial implementation,
 *  05/20/15  E. Birrane     Moved to global, mutex-protected lyst
 *  08/14/15  E. Birrane     Added SRL processing.
 *  10/04/18  E. Birrane     Updated to AMP v0.5 structures. (JHU/APL)
 *****************************************************************************/

void rda_scan_tbrs_cb(rh_elt_t *elt, void *tag)
{
        rule_t *rule;
        rda_scan_context_t *ctx = (rda_scan_context_t *) tag;

        if((elt == NULL) || (elt->value == NULL) || (tag == NULL))
        {
                return;
        }

        rule = (rule_t*) elt->value;

        if(rule->id.type == AMP_TYPE_SBR)
        {
                return;
        }

        /* If rule is inactive or still waiting, skip it. */
        if((RULE_IS_ACTIVE(rule->flags))
           && (TimeCompare(rule->eval_at, ctx->nowtime) <= 0))
        {
                vec_push(ctx->vec, rule);
    }
}



void rda_scan_sbrs_cb(rh_elt_t *elt, void *tag)
{
        rule_t *rule;
    rda_scan_context_t *ctx = (rda_scan_context_t *) tag;

        if((elt == NULL) || (elt->value == NULL) || (tag == NULL))
        {
                return;
        }

        rule = (rule_t*) elt->value;

        if(rule->id.type == AMP_TYPE_TBR)
        {
                return;
        }

        /* If rule is inactive or still waiting, skip it. */
        if((RULE_IS_ACTIVE(rule->flags))
           && (TimeCompare(rule->eval_at, ctx->nowtime) <= 0))
        {
                if(rule->def.as_sbr.max_eval > rule->num_eval)
                {
                        vec_push(ctx->vec, rule);
                }
                else
                {
                        /* Rule is SBR with no evals left. Disable and skip. */
                        RULE_CLEAR_ACTIVE(rule->flags);
                }
    }
}


void rda_scan_earliest_rule(rh_elt_t *elt, void *tag)
{
  rule_t *rule;
  OS_time_t *earliest = (OS_time_t *) tag;

  if((elt == NULL) || (elt->value == NULL) || (tag == NULL))
  {
      return;
  }

  rule = (rule_t*) elt->value;

  if(!RULE_IS_ACTIVE(rule->flags))
  {
      return;
  }

  *earliest = TimeMin(*earliest, rule->eval_at);
}

OS_time_t rda_earliest_rule()
{
  OS_time_t earliest = OS_TIME_MAX;

  rhht_foreach(&(gVDB.rules), rda_scan_earliest_rule, &earliest);

  return earliest;
}


/******************************************************************************
 *
 * \par Function Name: rda_process_rules
 *
 * \par Purpose: Walks the list of rules flagged for evaluation and evaluates
 *               them, taking the appropriate action for each rule.
 *
 * \retval int -  AMP Status Code
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/21/11  E. Birrane     Initial implementation,
 *  05/20/15  E. Birrane     Updated to use global, mutex-protected lysts
 *  10/05/18  E. Birrane     Updated to AMP v0.5.
 *****************************************************************************/

int rda_process_rules(OS_time_t nowtime)
{
    vecit_t it;

    rda_scan_context_t ctx;
    ctx.nowtime = nowtime;

    pthread_mutex_lock(&gVDB.rules.lock);

    ctx.vec = &(gAgentDb.tbrs);
    rhht_foreach(&(gVDB.rules), rda_scan_tbrs_cb, &ctx);

    ctx.vec = &(gAgentDb.sbrs);
    rhht_foreach(&(gVDB.rules), rda_scan_sbrs_cb, &ctx);

    AMP_DEBUG_INFO("rda_process_rules","Checking %d TBRs.", vec_num_entries_ptr(&(gAgentDb.tbrs)));
    for(it = vecit_first(&(gAgentDb.tbrs)); vecit_valid(it); it = vecit_next(it))
    {
        rule_t *rule = vecit_data(it);

                gAgentInstr.num_tbrs_run++;

        lcc_run_ac(&(rule->action), &(rule->id.as_reg.parms));

                rule->num_eval++;
                rule->num_fire++;

                if(rule->num_fire >= rule->def.as_tbr.max_fire && rule->def.as_tbr.max_fire != 0)
                {
                        /* Remove the rule. */
//                        db_forget(&(rule->desc), gDB.rules);
                        RULE_CLEAR_ACTIVE(rule->flags);
                        VDB_DELKEY_RULE(&(rule->id));
                        gAgentInstr.num_tbrs--;
                }
                else
                {
                        rule->eval_at = OS_TimeAdd(ctx.nowtime, rule->def.as_tbr.period);
                        if(db_persist_rule(rule) != AMP_OK)
                        {
                                AMP_DEBUG_ERR("rda_process_rules", "Unable to persist new TBR state.", NULL);
                        }
                }
    }


    AMP_DEBUG_INFO("rda_process_rules","Checking %d SBRs.", vec_num_entries_ptr(&(gAgentDb.sbrs)));
    for(it = vecit_first(&(gAgentDb.sbrs)); vecit_valid(it); it = vecit_next(it))
    {
        rule_t *rule = (rule_t*) vecit_data(it);

        rule->num_eval++;
        rule->eval_at = OS_TimeAdd(rule->eval_at, OS_TimeAssembleFromMilliseconds(1, 0)); // check again in 1s
        if(sbr_should_fire(rule))
        {
                gAgentInstr.num_sbrs_run++;

                lcc_run_ac(&(rule->action), &(rule->id.as_reg.parms));

                rule->num_fire++;
        }

        if((rule->num_eval >= rule->def.as_sbr.max_eval && rule->def.as_sbr.max_eval != 0) ||
           (rule->num_fire >= rule->def.as_sbr.max_fire && rule->def.as_sbr.max_fire != 0))
        {
                /* Remove the rule. */
//                db_forget(&(rule->desc), gDB.rules);
                VDB_DELKEY_RULE(&(rule->id));
                gAgentInstr.num_sbrs--;
        }
    }

    vec_clear(&(gAgentDb.sbrs));
    vec_clear(&(gAgentDb.tbrs));

    pthread_mutex_unlock(&gVDB.rules.lock);

    AMP_DEBUG_EXIT("rda_eval_pending_rules","-> 0", NULL);
    return AMP_OK;
}



/******************************************************************************
 *
 * \par Function Name: rda_send_reports
 *
 * \par Purpose: For each report constructed during this evaluation period,
 *               create a message and send it.
 *
 * \retval int -  0 : Success
 *               -1 : Failure
 *
 * \par Notes:
 *              - When we construct the reports, we build one compound report
 *                per recipient. By the time we get to this function, we should have
 *                one report per recipient, so making one message per report should
 *                not result in multiple messages to the same recipient.
 *
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/21/11  E. Birrane     Initial implementation,
 *  05/20/15  E. Birrane     Updated to use global, mutex-protected lyst
 *  10/05/18  E. Birrane     Update to latest AMP v0.5. (JHU/APL)
 *****************************************************************************/

int rda_send_reports(nmagent_t *agent)
{
    vecit_t it1;
    vecit_t it2;
    OS_time_t nowtime;
    OS_GetLocalTime(&nowtime);

    AMP_DEBUG_ENTRY("rda_send_reports","()", NULL);


    vec_lock(&(gAgentDb.rpt_msgs));

    for(it1 = vecit_first(&(gAgentDb.rpt_msgs)); vecit_valid(it1); it1 = vecit_next(it1))
    {
        msg_rpt_t *msg_rpt = (msg_rpt_t*)vecit_data(it1);

        if(msg_rpt == NULL)
        {
                continue;
        }
        if (vec_num_entries(msg_rpt->rpts) < 1)
        {
            continue;
        }

        for(it2 = vecit_first(&(msg_rpt->rx)); vecit_valid(it2); it2 = vecit_next(it2))
        {
                char *rx = vecit_data(it2);

                if(rx == NULL)
                {
                        AMP_DEBUG_ERR("rda_send_reports", "NULL rx", NULL);
                        continue;
                }
                eid_t destination;
                strncpy(destination.name, rx, AMP_MAX_EID_LEN);
                if(mif_send_msg(&agent->mif, MSG_TYPE_RPT_SET, msg_rpt, &destination, amp_tv_from_ctime(nowtime, NULL)) == AMP_OK)
                {
                        gAgentInstr.num_sent_rpts += vec_num_entries(msg_rpt->rpts);
                }
                else
                {
                        AMP_DEBUG_ERR("rda_send_reports", "Error sending reports to %s", rx);
                }
        }
        /* FIXME: cleanup belongs in the vector state
        msg_rpt_release(msg_rpt, 1);
        */
    }

    /* Sent successfully or not, clear the reports. */
    vec_clear(&(gAgentDb.rpt_msgs));

    vec_unlock(&(gAgentDb.rpt_msgs));

    return AMP_OK;
}


void* rda_reports(void *arg)
{
    nmagent_t *agent = arg;
#ifndef mingw
    AMP_DEBUG_ENTRY("rda_reports","(0x%X)", (unsigned long) pthread_self()); //threadId);
#endif

    AMP_DEBUG_INFO("rda_reports","Running Remote Data Aggregator Thread.", NULL);

    /* While the DTNMP Agent is running...*/
    while(true)
    {
      int ret;
      if (pthread_mutex_lock(&gAgentDb.rpt_msgs.lock))
      {
        AMP_DEBUG_ERR("rda_reports", "failed mutex %p lock", &gAgentDb.rpt_msgs.lock);
        return NULL;
      }
      if (!daemon_run_get(&agent->running))
      {
        pthread_mutex_unlock(&gAgentDb.rpt_msgs.lock);
        break;
      }
      ret = pthread_cond_wait(&gAgentDb.rpt_msgs.cond_ins_mod, &(gAgentDb.rpt_msgs.lock));
      if (pthread_mutex_unlock(&(gAgentDb.rpt_msgs.lock)))
      {
        AMP_DEBUG_ERR("rda_reports", "failed mutex %p unlock", &(gAgentDb.rpt_msgs.lock));
        return NULL;
      }

      AMP_DEBUG_INFO("rda_reports", "processing reports...", NULL);
      if(rda_send_reports(agent) != AMP_OK)
      {
        AMP_DEBUG_ERR("rda_reports","Problem processing reports.", NULL);
      }
    } // end while

    AMP_DEBUG_ALWAYS("rda_reports","Shutting Down Remote Data Aggregator Thread.",NULL);
    return NULL;
}


/******************************************************************************
 *
 * \par Function Name: rda_rules
 *
 * \par Purpose: "Main" function for the remote data aggregator.  This thread
 *               performs the following functions:
 *               1) Collect set of rules that are to be processed
 *               2) Process the rules (data collection, cmd execution)
 *               3) Update statistics and capture outgoing reports
 *               4) Perform rule housekeeping/cleanup.
 *
 * \retval void * - pthread_exit(NULL).
 *
 * \param[in,out]  threadId The thread id for the RDA thread.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  09/06/11  M. Reid        Initial Implementation
 *  10/21/11  E. Birrane     Code comments and functional updates.
 *****************************************************************************/

void* rda_rules(void *arg)
{
  nmagent_t *agent = arg;
#ifndef mingw
    AMP_DEBUG_ENTRY("rda_rules","(0x%X)", (unsigned long) pthread_self()); //threadId);
#endif

    AMP_DEBUG_INFO("rda_rules","Running Remote Data Aggregator Thread.", NULL);

    /* While the DTNMP Agent is running...*/
    while(true)
    {
        OS_time_t nowtime;

        if (pthread_mutex_lock(&(gVDB.rules.lock)))
        {
          AMP_DEBUG_ERR("rda_rules", "failed mutex %p lock", &(gVDB.rules.lock));
          return NULL;
        }
        if (!daemon_run_get(&agent->running))
        {
          pthread_mutex_unlock(&(gVDB.rules.lock));
          break;
        }

        OS_GetLocalTime(&nowtime);
        OS_time_t next_rule = rda_earliest_rule();
        AMP_DEBUG_INFO("rda_rules", "next TBR at %lld from now %lld", next_rule.ticks, nowtime.ticks);
        if (TimeCompare(next_rule, nowtime) > 0)
        {
          int ret;
          OS_time_t delta;
          delta = OS_TimeSubtract(next_rule, nowtime);
          AMP_DEBUG_INFO("rda_rules", "sleeping up to %lld", delta.ticks);

          const struct timespec abstime = TimeToTimespec(next_rule);
          ret = pthread_cond_timedwait(&gVDB.rules.cond_ins_mod, &gVDB.rules.lock, &abstime);
          // return may have been earlier than the timeout
          OS_GetLocalTime(&nowtime);
          AMP_DEBUG_INFO("rda_rules", "running at %lld from %d (%s)", nowtime.ticks, ret, strerror(ret));
        }
        if (pthread_mutex_unlock(&(gVDB.rules.lock)))
        {
          AMP_DEBUG_ERR("rda_rules", "failed mutex %p unlock", &(gVDB.rules.lock));
          return NULL;
        }

        /* Run any rules that are due and forget them if they are done. */
        if(rda_process_rules(nowtime) != AMP_OK)
        {
                AMP_DEBUG_ERR("rda_rules","Problem processing rules.", NULL);
        }
    } // end while

    AMP_DEBUG_ALWAYS("rda_rules","Shutting Down Remote Data Aggregator Thread.",NULL);
    return NULL;
}
