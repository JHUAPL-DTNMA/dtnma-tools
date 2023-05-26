#include <signal.h>
#include <getopt.h>
#include <osapi-common.h>
#include <osapi-bsp.h>
#include <osapi-error.h>
#include <bp.h>
#include "shared/nm.h"
#include "shared/adm/adm.h"
#include "shared/primitives/blob.h"
#include "mgr/agents.h"
#include "mgr/nmmgr.h"
#include "ion_if.h"


static char* mgr_parse_args(int argc, char *const argv[]);
static void mgr_print_usage(void);

static iif_t ion_ptr;
static nmmgr_t mgr;

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

  /* Step 1: Process Command Line Arguments. */
  const int argc = OS_BSP_GetArgC();
  char *const *argv = OS_BSP_GetArgV();

  const char *arg_eid = NULL;
  if (argc > 2)
  {
      // Assume argv[1] is required manager_eid
      arg_eid = mgr_parse_args(argc, argv);
  }
  else if(argc != 2)
  {
      fprintf(stderr,"Invalid number of arguments for nm_mgr\n");
      mgr_print_usage();
      OS_ApplicationExit(EXIT_FAILURE);
  }
  else
  {
      arg_eid = argv[1];
  }

  if (arg_eid == NULL)
  {
      mgr_print_usage();
      OS_ApplicationExit(EXIT_FAILURE);
  }
  eid_t mgr_eid;
  strncpy(mgr_eid.name, arg_eid, AMP_MAX_EID_LEN);

  if (ionAttach() < 0)
  {
    AMP_DEBUG_ERR("mgr_init", "Manager can't attach to ION.", NULL);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  /* Step 2:  Attach to ION. */
  if(iif_register_node(&ion_ptr, mgr_eid) == 0)
  {
      AMP_DEBUG_ERR("mgr_init","Unable to register BP Node. Exiting.", NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }

  if (iif_is_registered(&ion_ptr))
  {
      AMP_DEBUG_INFO("mgr_init", "Mgr registered with ION, EID: %s",
                               iif_get_local_eid(&ion_ptr).name);
  }
  else
  {
      AMP_DEBUG_ERR("mgr_init","Failed to register mgr with ION, EID %s",
                                       iif_get_local_eid(&ion_ptr).name);
      AMP_DEBUG_EXIT("mgr_init","->-1.",NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }

  /* Initialize the AMP Manager. */
  if(nmmgr_init(&mgr) != AMP_OK)
  {
      AMP_DEBUG_ERR("main","Can't init Manager.", NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }

  AMP_DEBUG_INFO("main","Manager EID: %s", argv[1]);

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
  OS_IdleLoop();

  nmmgr_stop(&mgr);

#ifdef USE_CIVETWEB
  nm_rest_stop();
#endif

  AMP_DEBUG_ALWAYS("main","Shutting down manager.", NULL);
  nmmgr_destroy(&mgr);

  AMP_DEBUG_INFO("main","Exiting Manager after cleanup.", NULL);

  AMP_DEBUG_ALWAYS("agent_main", "Stopping Agent.", NULL);
  ionDetach();
}

/**
 * Parse optional command line arguments
 */
char* mgr_parse_args(int argc, char *const argv[])
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
            {"help", required_argument, 0,'h'},
        };
    while ((c = getopt_long(argc, argv, "ldL:D:rtTRaAjJs:u:p:S:", long_options, &option_index)) != -1)
    {
        switch(c)
        {
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
#ifdef HAVE_MYSQL
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
            return NULL;
        default:
            fprintf(stderr, "Error parsing arguments\n");
            return NULL;
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
        return NULL;
    }
    else
    {
        return argv[optind];
    }
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
#ifdef HAVE_MYSQL
    printf("--sql-user MySQL Username\n");
    printf("--sql-pass MySQL Password\n");
    printf("--sql-db MySQL Datbase Name\n");
    printf("--sql-host MySQL Host\n");
#endif
}
