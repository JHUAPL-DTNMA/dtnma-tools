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
 ** \file nm_mgr.h
 **
 ** File Name: nm_mgr.h
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
 **  10/06/18   E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#ifndef NMMGR_H
#define NMMGR_H

// Standard includes
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>


// Application includes
#include "shared/platform.h"
#include "shared/nm.h"
#include "shared/msg/msg_if.h"
#include "shared/utils/daemon_run.h"
#include "shared/utils/nm_types.h"
#include "shared/utils/threadset.h"

#include "shared/adm/adm.h"

#include "shared/primitives/report.h"

#include "shared/msg/msg.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
#include "nm_mgr_sql.h"
#endif


/* Constants */
#define NM_MGR_MAX_META (1024)


typedef enum mgr_ui_mode_enum {
   MGR_UI_STANDARD, // Standard Shell-Based UI
   MGR_UI_NCURSES, // NCURSES-Based UI (currently a compile-time flag mutually exclusive with MGR_UI_STANDARD)
   MGR_UI_AUTOMATOR, // Special Altenrative UI Optimized for Automation
} mgr_ui_mode_enum;

#ifdef USE_NCURSES
#define MGR_UI_DEFAULT MGR_UI_NCURSES
#else
#define MGR_UI_DEFAULT MGR_UI_STANDARD
#endif


typedef struct {
  //FIXME: this is not thread safe storage
  mgr_ui_mode_enum mgr_ui_mode;
  /// Running state
  daemon_run_t running;
  /// Messaging configuration
  mif_cfg_t mif;
  /// Threads associated with the mgr
  list_thread_t threads;

} nmmgr_t;


typedef struct
{
	vector_t agents;  /* (agent_t *) */
	rhht_t metadata; /* (metadata_t*) */
	amp_uvast tot_rpts;
	amp_uvast tot_tbls;

#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
	sql_db_t sql_info;
#endif
} mgr_db_t;

extern mgr_db_t gMgrDB;

// ============================= Global Data ===============================

/* Function Prototypes */

int nmmgr_init(nmmgr_t *mgr);

int nmmgr_destroy(nmmgr_t *mgr);

int nmmgr_start(nmmgr_t *mgr);

int nmmgr_stop(nmmgr_t *mgr);

#ifdef __cplusplus
}
#endif

#endif  /* NMMGR_H */
