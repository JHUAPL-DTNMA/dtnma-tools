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
agent_signal_handler(int signum)
{
  AMP_DEBUG_INFO("agent_signal_handler", "Received signal %d", signum);
  OS_ApplicationShutdown(true);
  bp_close(ion_ptr.sap);
}

static int
msg_bp_send(const blob_t *data, void *ctx)
{
  Sdr sdr = bp_get_sdr();
  if (sdr_begin_xn(sdr) < 0)
  {
    AMP_DEBUG_ERR("iif_send","Unable to start transaction.", NULL);
    return AMP_FAIL;
  }

  Object extent = sdr_malloc(sdr, data->length);
  if(extent)
  {
    sdr_write(sdr, extent, (char *) data->value, data->length);
  }
  else
  {
    AMP_DEBUG_ERR("iif_send","Can't write to NULL extent.", NULL);
    sdr_cancel_xn(sdr);
    return AMP_FAIL;
  }
  if (sdr_end_xn(sdr) < 0)
  {
      AMP_DEBUG_ERR("iif_send","Can't close transaction?", NULL);
  }

  Object content = ionCreateZco(ZcoSdrSource, extent, 0, data->length, BP_STD_PRIORITY, 0, ZcoOutbound, NULL);
  if(content == 0 || content == (Object) ERROR)
  {
      AMP_DEBUG_ERR("iif_send","Zero-Copy Object creation failed.", NULL);
      AMP_DEBUG_EXIT("iif_send", "->0.", NULL);
      return 0;
  }

  Object newBundle = 0;
  int res = bp_send(
    ion_ptr.sap,
    manager_eid.name,       // recipient
    NULL,                   // report-to
    300,                    // lifespan (?)
    BP_STD_PRIORITY,        // Class-of-Service / Priority
    NoCustodyRequested,     // Custody Switch
    0,                      // SRR Flags
    0,                      // ACK Requested
    NULL,                   // Extended COS
    content,                // ADU
    &newBundle              // New Bundle
  );
  if (res != 1)
  {
    AMP_DEBUG_ERR("iif_send","Send failed (%d) to %s", res, manager_eid.name);
    AMP_DEBUG_EXIT("iif_send", "->0.", NULL);
    return AMP_FAIL;
  }
  return AMP_OK;
}

static blob_t *
msg_bp_recv(msg_metadata_t *meta, int *success, void *ctx)
{
  Sdr sdr = bp_get_sdr();
  int res;

  *success = AMP_FAIL;

  BpDelivery dlv;
  memset(&dlv, 0, sizeof(BpDelivery));

  while (true)
  {
    // Timeout is required to check agent running status
    static const int timeout = 5;
    if((res = bp_receive(ion_ptr.sap, &dlv, timeout)) < 0)
    {
      AMP_DEBUG_INFO("iif_receive","bp_receive failed. Result: %d.", res);
      *success = AMP_SYSERR;
      return NULL;
    }
    switch(dlv.result)
    {
      case BpEndpointStopped:
        /* The endpoint stopped? Panic.*/
        AMP_DEBUG_INFO("iif_receive","Endpoint stopped.");
        return NULL;

      case BpPayloadPresent:
        /* Clear to process the payload. */
        AMP_DEBUG_INFO("iif_receive", "Payload present.");
        break;

      default:
        /* No message yet. */
        continue;
    }
  }
  int content_len = zco_source_data_length(sdr, dlv.adu);

  blob_t *data;
  if((data = blob_create(NULL, 0, content_len)) == NULL)
  {
      *success = AMP_SYSERR;

      AMP_DEBUG_ERR("iif_receive","Can't alloc %d of msg.", content_len);
      bp_release_delivery(&dlv, 1);
      return NULL;
  }

  /* Step 3: Read the bundle in from the ZCO. */
  if (sdr_begin_xn(sdr) < 0)
  {
      *success = AMP_SYSERR;

      blob_release(data, 1);
      AMP_DEBUG_ERR("iif_receive","Can't start transaction.", NULL);
      bp_release_delivery(&dlv, 1);
      return NULL;
  }

  ZcoReader reader;
  zco_start_receiving(dlv.adu, &reader);
  data->length = zco_receive_source(sdr, &reader, data->alloc, (char*)data->value);

  if((sdr_end_xn(sdr) < 0) || (data->length == 0))
  {
      *success = AMP_SYSERR;

      AMP_DEBUG_ERR("iif_receive", "Unable to process received bundle.", NULL);
      blob_release(data, 1);
      bp_release_delivery(&dlv, 1);
      return NULL;
  }

  istrcpy(meta->senderEid.name, dlv.bundleSourceEid,
                  sizeof meta->senderEid.name);
  istrcpy(meta->originatorEid.name, dlv.bundleSourceEid,
                  sizeof meta->originatorEid.name);
  istrcpy(meta->recipientEid.name, ion_ptr.local_eid.name,
                  sizeof meta->recipientEid.name);
  bp_release_delivery(&dlv, 1);

  *success = AMP_OK;
  return data;
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
  act.sa_handler = agent_signal_handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);

  /* Step 5: Start agent threads. */
  if (!agent_start(&agent))
  {
    OS_ApplicationExit(2);
  }
}

void OS_Application_Run()
{
  OS_IdleLoop();

  /* Step 7: Join threads and wait for them to complete. */
  if (!agent_stop(&agent))
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
