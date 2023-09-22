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
#include <sys/select.h>
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

// ADMs
#include "shared/adm/adm_amp_agent.h"

#define MAX_HEXMSG_SIZE 10240

static nmagent_t agent;
static eid_t manager_eid;
static eid_t agent_eid;

static void
daemon_signal_handler(int signum)
{
  AMP_DEBUG_INFO("daemon_signal_handler", "Received signal %d", signum);
  daemon_run_stop(&agent.running);
}


static int stdout_send(const blob_t *data, const eid_t *dest, void *ctx)
{
  char *buf = utils_hex_to_string(data->value, data->length);
  if (fputs(buf, stdout) <= 0)
  {
    SRELEASE(buf);
    return AMP_SYSERR;
  }
  SRELEASE(buf);

  if (fputs("\n", stdout) <= 0)
  {
    return AMP_SYSERR;
  }
  fflush(stdout);

  return AMP_OK;
}

static blob_t * stdin_recv(msg_metadata_t *meta, daemon_run_t *running, int *success, void *ctx)
{
  blob_t *res = NULL;
  char buf[MAX_HEXMSG_SIZE];
  fd_set rfds;
  struct timeval timeout;
  int ret;

  while (true)
  {
    // Watch stdin (fd 0) for input, assuming whole-lines are given
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    // Wait up to 1 second
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    ret = select(1, &rfds, NULL, NULL, &timeout);
    if (ret == -1)
    {
      *success = AMP_SYSERR;
      return res;
    }
    else if (ret == 0)
    {
      // nothing ready, but maybe daemon is shutting down
      if (!daemon_run_get(running))
      {
        *success = AMP_FAIL;
        return res;
      }
      continue;
    }

    // assume that if something is ready to read that a whole line will come
    if (!fgets(buf, MAX_HEXMSG_SIZE, stdin))
    {
      *success = AMP_SYSERR;
      return res;
    }

    // strip the text
    size_t len = strnlen(buf, MAX_HEXMSG_SIZE);
    while ((len > 0) && isspace(buf[len - 1]))
    {
      fprintf(stderr, "strip 1\n");
      buf[--len] = '\0';
    }
    if (len == 0){
      AMP_DEBUG_WARN("stdin_recv", "Received empty line");
      continue;
    }

    break;
  }

  res = utils_string_to_hex(buf);
  AMP_DEBUG_INFO("stdin_recv", "Received message: %s, dehex %d", buf, !!res);
  *success = (res ? AMP_OK : AMP_FAIL);
  return res;
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
  strncpy(agent_eid.name, "", AMP_MAX_EID_LEN);
  strncpy(manager_eid.name, "", AMP_MAX_EID_LEN);

  if (nmagent_init(&agent) != AMP_OK)
  {
      AMP_DEBUG_ERR("main","Can't init Agent.", NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }
  agent.mif.send = stdout_send;
  agent.mif.receive = stdin_recv;

  /* Step 3: Initialize objects and instrumentation. */
  agent_instr_init();

  // ADM initialization
  amp_agent_init();
#if 0
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

  fprintf(stdout, "READY\n");
  fflush(stdout);

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

  fprintf(stdout, "SHUTDOWN\n");
  fflush(stdout);

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
}
