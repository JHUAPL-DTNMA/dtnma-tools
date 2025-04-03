/*
 * Copyright (c) 2025 The Johns Hopkins University Applied Physics
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
#include "proxy_if.h"
#include "shared/utils/threadset.h"
#include "shared/utils/utils.h"
#include "shared/nm.h"
#include "shared/adm/adm.h"
#include "mgr/agents.h"
#include "mgr/nmmgr.h"
#include <signal.h>
#include <getopt.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <osapi-common.h>
#include <osapi-bsp.h>
#include <osapi-error.h>
#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-daemon.h>
#endif

static int mgr_parse_args(int argc, char *const argv[]);
static void mgr_print_usage(void);

/// Local listening socket path
static char * arg_path_sock = NULL;
/// Socket FD for connection
static int sock_conn = -1;
/// Mutex for value of #sock_conn, but not its state
static pthread_mutex_t sock_mutex;
/// Manager state
static nmmgr_t mgr;

static void
daemon_signal_handler(int signum)
{
  AMP_DEBUG_INFO("daemon_signal_handler", "Received signal %d", signum);
  daemon_run_stop(&mgr.running);
}


/** Connect to the proxy via the path supplied in
 * #arg_path_sock.
 * @return The connected file descriptor, or -1 if failed.
 */
static int proxy_connect(void)
{
  pthread_mutex_lock(&sock_mutex);
  if (sock_conn < 0)
  {
    struct sockaddr_un laddr;
    laddr.sun_family = AF_UNIX;
    char *sun_end    = stpncpy(laddr.sun_path, arg_path_sock, sizeof(laddr.sun_path));
    if (sun_end - laddr.sun_path >= (ssize_t)sizeof(laddr.sun_path))
    {
      AMP_DEBUG_ERR(__func__, "given path that is too long to fit in sockaddr_un");
      OS_ApplicationExit(EXIT_FAILURE);
    }

    for (int try_ix = 0; try_ix < 600; ++try_ix)
    {
      sock_conn = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (sock_conn == -1)
      {
        AMP_DEBUG_ERR(__func__,"Failed to create socket %d", errno);
        OS_ApplicationExit(EXIT_FAILURE);
      }

      AMP_DEBUG_INFO(__func__, "Connecting to socket %s", laddr.sun_path);
      int res = connect(sock_conn, (struct sockaddr *)&laddr, sizeof(laddr));
      if (res)
      {
        AMP_DEBUG_ERR(__func__, "Failed to connect socket %s with errno %d", laddr.sun_path, errno);
        close(sock_conn);
        sock_conn = -1;
      }
      else
      {
        break;
      }

      // linear backoff
      sleep(try_ix);
    }
  }

  const int ret = sock_conn;
  pthread_mutex_unlock(&sock_mutex);
  return ret;
}

static int msg_proxy_send(const blob_t *data, const eid_t *dest, void *)
{
  // necessary condition
  int sock_fd = proxy_connect();

  int res = proxy_if_send(sock_fd, dest, data->value, data->length);
  return res;
}

static blob_t * msg_proxy_recv(msg_metadata_t *meta, daemon_run_t *running, int *success, void *)
{
  // necessary condition
  int sock_fd = proxy_connect();

  m_bstring_t databuf;
  m_bstring_init(databuf);
  *success = proxy_if_recv(sock_fd, &(meta->source), databuf);
  blob_t *data = NULL;
  if (*success != AMP_OK)
  {
    shutdown(sock_conn, SHUT_RDWR);
  }
  else
  {
    // copy by value into result
    const size_t data_size = m_bstring_size(databuf);
    const uint8_t *data_begin = m_bstring_view(databuf, 0, data_size);
    AMP_DEBUG_ERR(__func__, "Creating blob size %zd", data_size);
    data = blob_create(data_begin, data_size, data_size);
    AMP_DEBUG_ERR(__func__, "Created blob");
  }
  m_bstring_clear(databuf);
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

  /* Step 1: Process Command Line Arguments. */
  const int argc = OS_BSP_GetArgC();
  char *const *argv = OS_BSP_GetArgV();
  if (mgr_parse_args(argc, argv))
  {
      OS_ApplicationExit(EXIT_FAILURE);
  }

  if(arg_path_sock == NULL)
  {
      mgr_print_usage();
      OS_ApplicationExit(EXIT_FAILURE);
  }

#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
	db_mgr_sql_init();
	 db_mgt_init(gMgrDB.sql_info, 0, 1);
#endif
  /* Initialize the AMP Manager. */
  if(nmmgr_init(&mgr) != AMP_OK)
  {
      AMP_DEBUG_ERR("main","Can't init Manager.", NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }

  // Wait to connect to socket for one round of attempts, no work threads yet
  proxy_connect();
  if (sock_conn < 0)
  {
    AMP_DEBUG_ERR(__func__, "Failed to connect socket after all retries");
    OS_ApplicationExit(EXIT_FAILURE);
  }
  else
  {
    AMP_DEBUG_INFO(__func__, "Connected to socket");
  }

  mgr.mif.send = msg_proxy_send;
  mgr.mif.receive = msg_proxy_recv;
  mgr.mif.ctx = NULL;

  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  act.sa_handler = daemon_signal_handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);

  if (nmmgr_start(&mgr) != AMP_OK)
  {
    OS_ApplicationExit(EXIT_FAILURE);
  }
}

void OS_Application_Run()
{
#if defined(HAVE_LIBSYSTEMD)
  sd_notify(0, "READY=1");
#endif

  // Block until stopped
  daemon_run_wait(&mgr.running);
#if defined(HAVE_LIBSYSTEMD)
  sd_notify(0, "STOPPING=1");
#endif
  OS_ApplicationShutdown(true);
  // interrupt any reading
  if (sock_conn >= 0)
  {
    shutdown(sock_conn, SHUT_RDWR);
  }

  nmmgr_stop(&mgr);

  AMP_DEBUG_ALWAYS("main","Shutting down manager.", NULL);
  nmmgr_destroy(&mgr);

  AMP_DEBUG_INFO("main","Exiting Manager after cleanup.", NULL);

  AMP_DEBUG_ALWAYS("agent_main", "Stopping Agent.", NULL);
}

/**
 * Parse optional command line arguments
 */
int mgr_parse_args(int argc, char *const argv[])
{
    int i;
    int c;
    int option_index = 0;
    static struct option long_options[] =
        {
            {"log", no_argument, 0,'l'},
            {"log-to-dirs", no_argument, 0,'d'},
            {"log-rx-rpt", no_argument, 0,'r'},
            {"log-rx-tbl", no_argument, 0,'t'},
            {"log-tx-cbor", no_argument, 0,'T'},
            {"log-rx-cbor", no_argument, 0,'R'},
            {"log-tx-cbor", no_argument, 0,'j'},
            {"log-rx-cbor", no_argument, 0,'J'},

            {"sql-user", required_argument, 0,'u'},
            {"sql-pass", required_argument, 0,'p'},
            {"sql-db", required_argument,0, 'S'},
            {"sql-host", required_argument,0, 's'},

            {"log-dir", required_argument, 0,'D'},
            {"log-limit", required_argument, 0,'L'},
            {"automator", required_argument, 0,'a'},
            {"connect", required_argument, 0,'c'},
            {"help", required_argument, 0,'h'},
        };
    while ((c = getopt_long(argc, argv, ":c:ldL:D:rtTRaAjJs:u:p:S:", long_options, &option_index)) != -1)
    {
        switch(c)
        {
          case 'c':
            arg_path_sock = optarg;
            break;
        case 'l':
            agent_log_cfg.enabled = 1;
            break;
        case 'd':
            agent_log_cfg.agent_dirs = 1;
            break;
        case 'r':
            agent_log_cfg.rx_rpt = 1;
            break;
        case 't':
            agent_log_cfg.rx_tbl = 1;
            break;
        case 'T':
            agent_log_cfg.tx_cbor = 1;
            break;
        case 'R':
            agent_log_cfg.rx_cbor = 1;
            break;
#ifdef USE_JSON
        case 'j':
            agent_log_cfg.rx_json_rpt = 1;
            break;
        case 'J':
            agent_log_cfg.rx_json_tbl = 1;
            break;
#endif
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
        case 's': // MySQL Server
            strncpy(gMgrDB.sql_info.server, optarg, UI_SQL_SERVERLEN-1);
            break;
        case 'u': // MySQL Username
            strncpy(gMgrDB.sql_info.username, optarg, UI_SQL_ACCTLEN-1);
            break;
        case 'p': // MySQL Password
            strncpy(gMgrDB.sql_info.password, optarg, UI_SQL_ACCTLEN-1);
            break;
        case 'S': // MySQL Database Name
            strncpy(gMgrDB.sql_info.database, optarg, UI_SQL_DBLEN-1);
            break;

#endif
        case 'D':
            strncpy(agent_log_cfg.dir, optarg, sizeof(agent_log_cfg.dir)-1);
            break;
        case 'L':
            agent_log_cfg.limit = atoi(optarg);
            break;
        case 'a':
        case 'A':
            mgr.mgr_ui_mode = MGR_UI_AUTOMATOR;
            break;
        case 'h':
            return 3;
        default:
            fprintf(stderr, "Error parsing arguments\n");
            return 2;
        }
    }

    // Check for any remaining unrecognized arguments
    if ((argc-optind) != 1)
    {
        fprintf(stderr,"%d unrecognized arguments:\n", (argc-optind));
        for(i = optind; i < argc; i++)
        {
            printf("\t%s\n", argv[i]);
        }
        return 4;
    }
    return 0;
}

void mgr_print_usage(void)
{

    printf("AMP Protocol Version %d - %s, built on %s %s\n",
           AMP_VERSION,
           AMP_PROTOCOL_URL,
           __DATE__, __TIME__);

    printf("Usage: nm_mgr [options] <manager eid>\n");
    printf("Supported Options:\n");
    printf("-A       Startup directly in the alternative Automator UI mode\n");
    printf("-l       If specified, enable file-based logging of Manager Activity.\n");
    printf("           If logging is not enabled, the following have no affect until enabled in UI\n");
    printf("-d       Log each agent to a different directory\n");
    printf("-L #      Specify maximum number of entries (reports+tables) per file before rotating\n");
    printf("-D DIR   NM logs will be placed in this directory\n");
    printf("-r       Log all received reports to file in text format (as shown in UI)\n");
    printf("-t       Log all received tables to file in text format (as shown in UI)\n");
    printf("-T       Log all transmitted message as ASCII-encoded CBOR HEX strings\n");
    printf("-R       Log all received messages as ASCII-encoded CBOR HEX strings\n");
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    printf("--sql-user MySQL Username\n");
    printf("--sql-pass MySQL Password\n");
    printf("--sql-db MySQL Datbase Name\n");
    printf("--sql-host MySQL Host\n");
#endif
}
