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
 ** File Name: nmagent.h
 **
 ** Description: This implements NM Agent main processing.
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
 **  10/03/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#ifndef _NM_AGENT_H
#define _NM_AGENT_H

#define DEBUG 1

// Standard includes
#include <stdint.h>

// Application includes
#include "shared/platform.h"
#include "shared/utils/daemon_run.h"
#include "shared/utils/nm_types.h"
#include "shared/utils/daemon_run.h"
#include "shared/utils/threadset.h"
#include "shared/primitives/ari.h"
#include "shared/primitives/rules.h"
#include "shared/msg/msg.h"
#include "shared/msg/msg_if.h"

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
  /// Running state
  daemon_run_t running;
  /// Messaging configuration
  mif_cfg_t mif;
  /// Threads associated with the agent
  list_thread_t threads;

} nmagent_t;

/*
 * +--------------------------------------------------------------------------+
 * |						  FUNCTION PROTOTYPES  							  +
 * +--------------------------------------------------------------------------+
 */

bool nmagent_init(nmagent_t *agent);

bool nmagent_destroy(nmagent_t *agent);

bool nmagent_start(nmagent_t *agent);

bool nmagent_stop(nmagent_t *agent);

/** Once the agent is running, send a registration message
 * @sa nmagent_start
 */
bool nmagent_register(nmagent_t *agent, const eid_t *agent_eid, const eid_t *mgr_eid);


/*
 * +--------------------------------------------------------------------------+
 * |						 GLOBAL DATA DEFINITIONS  		         		  +
 * +--------------------------------------------------------------------------+
 */



#ifdef __cplusplus
}
#endif

#endif  /* _NM_AGENT_H_ */
