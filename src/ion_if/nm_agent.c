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

static nmagent_t agent;
static iif_t ion_ptr;
static eid_t manager_eid;
static eid_t agent_eid;

static void
daemon_signal_handler(int signum)
{
  AMP_DEBUG_INFO("daemon_signal_handler", "Received signal %d", signum);
  OS_ApplicationShutdown(true);
  bp_close(ion_ptr.sap);
}

void
OS_Application_Startup()
{
  if (OS_API_Init() != OS_SUCCESS)
  {
    fprintf(stderr, "Failed OS_API_Init\n");
    OS_ApplicationExit(-1);
  }

  nmagent_init(&agent);
  agent.mif.send = msg_bp_send;
  agent.mif.receive = msg_bp_recv;

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

  AMP_DEBUG_INFO("agent main", "Agent EID: %s, Mgr EID: %s", argv[1], argv[2]);

  strcpy ((char*) manager_eid.name, argv[2]);
  strcpy ((char*) agent_eid.name, argv[1]);

  /* Step 2: Make sure that ION is running and we can attach. */
  if (ionAttach () < 0)
  {
    AMP_DEBUG_ERR("agent_main", "Agent can't attach to ION.", NULL);
    OS_ApplicationExit(-1);
  }

  if (iif_register_node (&ion_ptr, agent_eid) != 1)
  {
    AMP_DEBUG_ERR("agent_main", "Unable to register BP Node. Exiting.", NULL);
    OS_ApplicationExit(-1);
  }

  if (iif_is_registered (&ion_ptr))
  {
    AMP_DEBUG_INFO("agent_main", "Agent registered with ION, EID: %s",
                   iif_get_local_eid (&ion_ptr).name);
  }
  else
  {
    AMP_DEBUG_ERR("agent_main", "Failed to register agent with ION, EID %s",
                  iif_get_local_eid (&ion_ptr).name);
    OS_ApplicationExit(-1);
  }

  /* Step 3: Initialize objects and instrumentation. */

  if ((utils_mem_int () != AMP_OK)
      || (db_init ("nmagent_db", &adm_common_init) != AMP_OK))
  {
    db_destroy ();
    AMP_DEBUG_ERR("agent_main", "Unable to initialize DB.", NULL);
    OS_ApplicationExit(-1);
  }

  agent_instr_init ();

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
}

void OS_Application_Run()
{
  OS_IdleLoop();

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
  ionDetach();
}
