/*
 * Copyright (c) 2023 The Johns Hopkins University Applied Physics
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
#include <signal.h>
#include <osapi-common.h>
#include <osapi-bsp.h>
#include <osapi-error.h>
#include <bp.h>
#include "shared/nm.h"
#include "shared/adm/adm.h"
#include "shared/primitives/blob.h"
#include "agent/instr.h"
#include "agent/nmagent.h"
#include "ion_if.h"

// ADMs
#include "shared/adm/adm_amp_agent.h"
#include "shared/adm/adm_ion_admin.h"
#include "shared/adm/adm_ion_ipn_admin.h"
#include "shared/adm/adm_ion_ltp_admin.h"
#include "shared/adm/adm_ionsec_admin.h"
#include "shared/adm/adm_ltp_agent.h"
#ifdef BUILD_BPv6
#else
#include "bpv7/adm/adm_bp_agent.h"
#include "bpv7/adm/adm_bpsec.h"
#include "bpv7/adm/adm_ion_bp_admin.h"
#endif

static nmagent_t agent;
static iif_t ion_ptr;
static eid_t manager_eid;
static eid_t agent_eid;

static void
daemon_signal_handler(int signum)
{
  AMP_DEBUG_INFO("daemon_signal_handler", "Received signal %d", signum);
  daemon_run_stop(&agent.running);
}

void
OS_Application_Startup()
{
  if (OS_API_Init() != OS_SUCCESS)
  {
    fprintf(stderr, "Failed OS_API_Init\n");
    OS_ApplicationExit(-1);
  }

  /* Step 1: Process Command Line Arguments. */
  const int argc = OS_BSP_GetArgC();
  char *const *argv = OS_BSP_GetArgV();
  if (argc != 3)
  {
    printf("Usage: nmagent <agent eid> <manager eid>\n");
    printf("AMP Protocol Version %d - %s, built on %s %s\n", AMP_VERSION,
           AMP_PROTOCOL_URL, __DATE__, __TIME__);
    OS_ApplicationExit(0);
  }

  if (((argv[0] == NULL) || (strlen (argv[0]) <= 0))
      || ((argv[1] == NULL) || (strlen (argv[1]) <= 0)
          || (strlen (argv[1]) >= AMP_MAX_EID_LEN))
      || ((argv[2] == NULL) || (strlen (argv[2]) <= 0)
          || (strlen (argv[2]) >= AMP_MAX_EID_LEN)))
  {
    AMP_DEBUG_ERR("agent_main", "Invalid Parameters (NULL or 0).", NULL);
    OS_ApplicationExit(-1);
  }

  strncpy(agent_eid.name, argv[1], AMP_MAX_EID_LEN);
  strncpy(manager_eid.name, argv[2], AMP_MAX_EID_LEN);
  AMP_DEBUG_INFO("main", "Agent EID: %s, Mgr EID: %s", argv[1], argv[2]);

  /* Step 2: Make sure that ION is running and we can attach. */
  if (ionAttach() < 0)
  {
    AMP_DEBUG_ERR("main", "Agent can't attach to ION.", NULL);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  if (iif_register_node(&ion_ptr, agent_eid) != 1)
  {
    AMP_DEBUG_ERR("main", "Unable to register BP Node. Exiting.", NULL);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  if (iif_is_registered(&ion_ptr))
  {
    AMP_DEBUG_INFO("main", "Agent registered with ION, EID: %s",
                   iif_get_local_eid (&ion_ptr).name);
  }
  else
  {
    AMP_DEBUG_ERR("main", "Failed to register agent with ION, EID %s",
                  iif_get_local_eid(&ion_ptr).name);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  if (nmagent_init(&agent) != AMP_OK)
  {
      AMP_DEBUG_ERR("main","Can't init Agent.", NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }
  agent.mif.send = msg_bp_send;
  agent.mif.receive = msg_bp_recv;
  agent.mif.ctx = &ion_ptr;

  /* Step 3: Initialize objects and instrumentation. */
  agent_instr_init();

  // ADM initialization
  amp_agent_init();
  dtn_bp_agent_init();
  dtn_ion_ionadmin_init();
  dtn_ion_ipnadmin_init();
  dtn_ion_ionsecadmin_init();
  dtn_ion_ltpadmin_init();
  dtn_ltp_agent_init();
  dtn_ion_bpadmin_init();
#ifdef BUILD_BPv6
  dtn_sbsp_init();
#else
//  dtn_bpsec_init();
#endif

  /* Step 4: Register signal handlers. */
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  act.sa_handler = daemon_signal_handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);

  /* Step 5: Start agent threads. */
  if (!nmagent_start(&agent))
  {
    OS_ApplicationExit(2);
  }

  if (!nmagent_register(&agent, &agent_eid, &manager_eid))
  {
    OS_ApplicationExit(2);
  }
}

void OS_Application_Run()
{
  // Block until stopped
  daemon_run_wait(&agent.running);
  OS_ApplicationShutdown(true);
  bp_close(ion_ptr.sap);

  /* Step 7: Join threads and wait for them to complete. */
  if (!nmagent_stop(&agent))
  {
    OS_ApplicationExit(2);
  }

  /* Step 8: Cleanup. */
  AMP_DEBUG_ALWAYS("agent_main", "Cleaning Agent Resources.", NULL);

  adm_common_destroy();
  db_destroy();
  utils_mem_teardown();

  AMP_DEBUG_ALWAYS("agent_main", "Stopping Agent.", NULL);
  iif_deregister_node(&ion_ptr);
}
