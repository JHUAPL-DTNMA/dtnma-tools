/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2011 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 ******************************************************************************/

/*****************************************************************************
 ** \file nm_mgr.c
 **
 ** File Name: nm_mgr.c
 **
 ** Subsystem:
 **          Network Manager Application
 **
 ** Description: This file implements the DTNMP Manager user interface
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR          DESCRIPTION
 **  --------  ------------    ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation (JHU/APL)
 **  08/19/13  E. Birrane      Documentation clean up and code review comments. (JHU/APL)
 **  08/21/16  E. Birrane      Update to AMP v02 (Secure DTN - NASA: NNX14CS58P)
 **  10/06/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#include <inttypes.h>

// Application headers.
#include "../shared/primitives/rules.h"
#include "nm_mgr_rx.h"
#include "nm_mgr_ui.h"
#include "metadata.h"

#ifdef USE_CIVETWEB
#include "nm_rest.h"
#endif
#include "agents.h"
#include "nmmgr.h"


mgr_db_t gMgrDB;



/******************************************************************************
 *
 * \par Function Name: mgr_cleanup
 *
 * \par Cleans resources before exiting the manager.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation
 **  08/20/13  E. Birrane      Code Review Updates
 **  10/06/18  E. Birrane      Updated to AMP v0.5 (JHU/APL)
 *****************************************************************************/

int nmmgr_destroy(nmmgr_t *mgr)
{

#ifdef HAVE_MYSQL
	db_mgt_close();
#endif

	vec_release(&(gMgrDB.agents), 0);
	rhht_release(&(gMgrDB.metadata), 0);

	db_destroy();

	utils_mem_teardown();

	return AMP_OK;
}



/******************************************************************************
 *
 * \par Function Name: mgr_init
 *
 * \par Initialize the manager...
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation
 **  08/20/13  E. Birrane      Code Review Updates
 **  10/06/18  E. Birrane      Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/
int nmmgr_init(nmmgr_t *mgr)
{
	int success;
	AMP_DEBUG_ENTRY("mgr_init","mgr(%p)", mgr);

	memset(mgr, 0, sizeof(nmmgr_t));
	mgr->mgr_ui_mode = MGR_UI_DEFAULT;

	/* Initialize the non-volatile database.
	 *   Note: Initializing the structure here allows some attributes to be pre-defined by
	 *   command line parsing if not re-initialized later.
	 */
	memset(&gMgrDB, 0, sizeof(gMgrDB));

	/* Step 1: Initialize MGR-specific data.*/
	gMgrDB.agents = vec_create(AGENT_DEF_NUM_AGTS, agent_cb_del,agent_cb_comp, NULL, 0, &success);
	if(success != VEC_OK)
	{
		AMP_DEBUG_ERR("mgr_init", "Can't make agents vec.", NULL);
		return AMP_FAIL;
	}

	gMgrDB.metadata = rhht_create(NM_MGR_MAX_META, ari_cb_comp_no_parm_fn, ari_cb_hash, meta_cb_del, &success);
	if(success != RH_OK)
	{
		AMP_DEBUG_ERR("mgr_init", "Can't make parmspec ht.", NULL);
		return AMP_FAIL;
	}


	gMgrDB.tot_rpts = 0;
    gMgrDB.tot_tbls = 0;

    if((utils_mem_int()       != AMP_OK) ||
       (db_init("nmmgr_db", &adm_init) != AMP_OK))
    {
    	db_destroy();
    	AMP_DEBUG_ERR("mgr_init","Unable to initialize DB.", NULL);
    	return AMP_FAIL;
    }

#ifdef HAVE_MYSQL
	db_mgr_sql_init();
	success = db_mgt_init(gMgrDB.sql_info, 0, 1);
#endif

    success = AMP_OK;

    return success;
}

int nmmgr_start(nmmgr_t *mgr)
{
  AMP_DEBUG_ENTRY("nmmgr_start","("PRIdPTR")", mgr);

  threadinfo_t threadinfo[] = {
      {&mgr_rx_thread, "nm_mgr_rx"},
      {&ui_thread, "nm_mgr_ui"},
      {NULL, NULL},
  };
#ifdef HAVE_MYSQL
  threadinfo[2] = (threadinfo_t){&db_mgt_daemon, "nm_mgr_db"};
#endif
  if (threadset_start(&mgr->threads, threadinfo, sizeof(threadinfo)/sizeof(threadinfo_t), mgr) != AMP_OK)
  {
    return AMP_SYSERR;
  }

  #ifdef USE_CIVETWEB
  nm_rest_start(mgr);
  #endif

  return AMP_OK;
}

int nmmgr_stop(nmmgr_t *mgr)
{
  /* Notify threads */
  daemon_run_stop(&mgr->running);
  threadset_join(&mgr->threads);

#ifdef USE_CIVETWEB
  nm_rest_stop();
#endif

  return AMP_OK;
}
