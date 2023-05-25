/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2011 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 **
 ******************************************************************************/
/*****************************************************************************
 **
 ** File Name: nmagent.c
 **
 ** Description: This implements NM Agent main processing.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR          DESCRIPTION
 **  --------  ------------    ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation (JHU/APL)
 **  01/10/13  E. Birrane      Update to lasted DTNMP Spec. (JHU/APL)
 **  06/10/13  E. Birrane      Added SDR data persistence. (JHU/APL)
 **  10/04/18  E. Birrane      Updated to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include "shared/platform.h"

#include "../shared/nm.h"
#include "../shared/adm/adm.h"
#include "../shared/utils/db.h"

#include "nmagent.h"
#include "ingest.h"
#include "rda.h"

#include "instr.h"

bool nmagent_init(nmagent_t *agent)
{
  memset(agent, 0, sizeof(nmagent_t));
  daemon_run_init(&agent->running);
  return true;
}

bool nmagent_destroy(nmagent_t *agent)
{
  daemon_run_cleanup(&agent->running);
  return true;
}

/******************************************************************************
 *
 * \par Function Name: main
 *
 * \par Main agent processing function.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation
 **  01/10/13  E. Birrane      Update to lasted DTNMP Spec.
 **  06/10/13  E. Birrane      Added SDR data persistence.
 **  02/23/15  E. Birrane      Updated to support ION_LWT targets
 **  10/04/18  E. Birrane      Updated to AMP v0.5 (JHU/APL)
 *****************************************************************************/
bool agent_start(nmagent_t *agent)
{
    int rc;

    AMP_DEBUG_ENTRY("agent_main","("PRIdPTR")", agent);

    rda_init();

    /* Step 5: Start agent threads. */
    struct threadinfo {
      void* (*func)(void*);
      const char *name;
    } threads[] = {
        {&rx_thread, "rx_thread"},
    };

    for (struct threadinfo *it = threads;
	 it < threads + sizeof(threads)/sizeof(struct threadinfo); ++it)
    {
      pthread_t thr;
      rc = pthread_create(&thr, NULL, it->func, agent);
      if (rc)
      {
        AMP_DEBUG_ERR("agent_main","Unable to create pthread %s, errno = %s",
                      it->name, strerror(errno));

        db_destroy();

        AMP_DEBUG_EXIT("agent_main","->-1",NULL);
        return false;
      }
      list_thread_push_back(agent->threads, thr);
      pthread_setname_np(thr, it->name);
    }
    AMP_DEBUG_ALWAYS("agent_main","Threads started...", NULL);


    /* Step 6: Send out agent broadcast message. */
    {
      	msg_agent_t *msg = NULL;
	if((msg = msg_agent_create()) == NULL)
	{
		AMP_DEBUG_ERR("agent_register","Unable to create agent registration.",NULL);
		return false;
	}

//	msg_agent_set_agent(msg, agent_eid);

	if(mif_send_msg(&agent->mif, MSG_TYPE_REG_AGENT, msg, "dtn:none", AMP_TV_ZERO) != AMP_OK)
	{
		AMP_DEBUG_ERR("agent_register","Couldn't send agent reg.", NULL);
	}

	msg_agent_release(msg, 1);
    }

    return true;
}



/******************************************************************************
 *
 * \par Function Name: agent_signal_handler
 *
 * \par Catches kill signals and gracefully shuts down the agent.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 **  08/18/13  E. Birrane    Initial Implementation
 *****************************************************************************/

bool agent_stop(nmagent_t *agent)
{
  /* Notify threads */
  daemon_run_stop(&agent->running);

  list_thread_it_t it;
  for (list_thread_it(it, agent->threads);
      !list_thread_end_p(it);
      list_thread_next(it))
  {
    pthread_t *thr = list_thread_ref(it);
    if (pthread_join(*thr, NULL))
    {
        AMP_DEBUG_ERR("agent_main","Unable to join pthread %s, errno = %s",
                      "name", strerror(errno));
    }
  }

  /* Step 8: Cleanup. */
  AMP_DEBUG_ALWAYS("agent_main","Cleaning Agent Resources.",NULL);
  
  rda_cleanup();
  
  AMP_DEBUG_ALWAYS("agent_main","Stopping Agent.",NULL);
  
  return true;
}
