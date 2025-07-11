/*
 * Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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
#include "ion_if/ion_if.h"
#include "shared/utils/threadset.h"
#include "shared/utils/utils.h"
#include <m-bstring.h>
#include <m-shared-ptr.h>
#include <m-buffer.h>
#include <signal.h>
#include <getopt.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <osapi-common.h>
#include <osapi-bsp.h>
#include <osapi-error.h>
#include <bp.h>
#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-daemon.h>
#endif


static int mgr_parse_args(int argc, char *const argv[]);
static void mgr_print_usage(void);

typedef struct {
  /// Peer EID
  eid_t peer;
  /// Application data
  m_bstring_t data;
} prox_item_t;

static void prox_item_init(prox_item_t *obj)
{
  memset(obj->peer.name, 0, sizeof(obj->peer.name));
  m_bstring_init(obj->data);
}

static void prox_item_deinit(prox_item_t *obj)
{
  m_bstring_clear(obj->data);
}

/// M*LIB oplist for prox_item_t
#define M_OPL_prox_item_t() (INIT(API_2(prox_item_init)), CLEAR(API_2(prox_item_deinit)), INIT_SET(0), INIT_MOVE(0), SET(0), MOVE(0))

// Thread-unsafe shared pointer for move semantics
SHARED_WEAK_PTR_DEF(prox_item_ptr, prox_item_t)

/// thread safe running state
static daemon_run_t running;
/// Local listening socket path
static char * arg_path_listen = NULL;
/// Socket FD for listening
static int sock_listen = -1;
/// Socket FD for traffic
static int sock_conn = -1;
/// Local BP endpoint ID
static char * arg_eid = NULL;
/// ION interface struct
static iif_t ion_iif;

#define PROX_ITEM_QUEUE_DEPTH 10
// Thread safe queue of shared pointers
M_BUFFER_DEF(prox_item_queue, prox_item_ptr_t *, PROX_ITEM_QUEUE_DEPTH, BUFFER_QUEUE | BUFFER_THREAD_SAFE | BUFFER_PUSH_INIT_POP_MOVE, SHARED_PTR_OPLIST(prox_item_ptr, M_OPL_prox_item_t()))

/// Items outgoing to the BPA
static prox_item_queue_t outgoing;
/// Items incoming from the BPA
static prox_item_queue_t incoming;
/// Thread states
static list_thread_t threads;

/** Thread work function for the socket side.
 */
static void *
sock_worker(void *)
{
  int res;
  AMP_DEBUG_INFO(__func__, "Starting worker");

  m_bstring_t msgbuf;
  m_bstring_init(msgbuf);

  while (daemon_run_get(&running))
  {
    const bool can_send = !prox_item_queue_empty_p(incoming);
    if ((sock_conn >= 0) && can_send)
    {
      AMP_DEBUG_INFO(__func__, "Polling with sendable items");
    }

    struct pollfd pfds[] = {
        { .fd = sock_listen, .events = POLLIN | POLLERR | POLLHUP },
        { .fd = sock_conn, .events = (can_send ? POLLOUT : 0) | POLLIN | POLLERR | POLLHUP },
    };

    // Wait up to 1 second
    res = poll(pfds, sizeof(pfds) / sizeof(struct pollfd), 1000);
    if (res < 0)
    {
      AMP_DEBUG_ERR(__func__, "Failed to poll with errno %d", errno);
      break;
    }
    else if (res == 0)
    {
        // nothing ready, but maybe daemon is shutting down
        continue;
    }

    // Handle listening socket
    if (pfds[0].revents & (POLLERR | POLLHUP))
    {
      // input has closed
      AMP_DEBUG_INFO(__func__, "Stopping due to hangup");
      daemon_run_stop(&running);
    }
    if (pfds[0].revents & POLLIN)
    {
      if (sock_conn < 0)
      {
        AMP_DEBUG_INFO(__func__, "Accepting incoming connection");
        sock_conn = accept(sock_listen, NULL, NULL);
        continue;
      }
      else
      {
        AMP_DEBUG_INFO(__func__, "Ignoring incoming connection");
      }
    }

    // Handle accepted user socket
    if ((sock_conn >= 0) && (pfds[1].revents & (POLLERR | POLLHUP)))
    {
      // input has closed
      AMP_DEBUG_INFO(__func__, "Disconnecting due to hangup");
      shutdown(sock_conn, SHUT_RDWR);
      close(sock_conn);
      sock_conn = -1;
    }
    if ((sock_conn >= 0) && (pfds[1].revents & POLLOUT))
    {
      prox_item_ptr_t *item_ptr;
      // if there is actually an item present at this point
      if (prox_item_queue_pop(&item_ptr, incoming))
      {
        prox_item_t *item = prox_item_ptr_ref(item_ptr);

        const size_t   data_size  = m_bstring_size(item->data);
        const uint8_t *data_begin = m_bstring_view(item->data, 0, data_size);

        res = proxy_if_send(sock_conn, &(item->peer), data_begin, data_size);
        if (res != AMP_OK)
        {
          shutdown(sock_conn, SHUT_RDWR);
        }

        // done with the item
        prox_item_ptr_release(item_ptr);
      }
    }
    if ((sock_conn >= 0) && (pfds[1].revents & POLLIN))
    {
      prox_item_ptr_t *item_ptr = prox_item_ptr_new();
      prox_item_t *item = prox_item_ptr_ref(item_ptr);

      res = proxy_if_recv(sock_conn, &(item->peer), item->data);
      if (res != AMP_OK)
      {
        AMP_DEBUG_ERR(__func__, "Failed proxy_if_recv()");
        if (prox_item_queue_empty_p(incoming))
        {
          // shutdown only when nothing to send
          shutdown(sock_conn, SHUT_RDWR);
        }
        prox_item_ptr_release(item_ptr);
      }
      else
      {
        prox_item_queue_push(outgoing, item_ptr);
      }
    }
  }

  m_bstring_clear(msgbuf);

  AMP_DEBUG_INFO(__func__, "Stopping worker");
  return NULL;
}

/** Thread work function for the BP sending (outgoing).
 */
static void *
bp_send_worker(void *)
{
  AMP_DEBUG_INFO(__func__, "Starting worker");

  while (daemon_run_get(&running))
  {
    prox_item_ptr_t *item_ptr;
    // if there is actually an item present at this point
    if (prox_item_queue_pop(&item_ptr, outgoing))
    {
      prox_item_t *item = prox_item_ptr_ref(item_ptr);

      // Create ADU as ZCO
      int result = AMP_OK;

      //FIXME guard SDR by mutex?
      Sdr sdr = bp_get_sdr();

      const size_t msg_size  = m_bstring_size(item->data);
      Object bundleData = 0;
      {
        if (!sdr_begin_xn(sdr))
        {
          AMP_DEBUG_ERR(__func__, "Failed sdr_begin_xn()");
        }
        else
        {
          uint8_t     *msg_begin = m_bstring_acquire_access(item->data, 0, msg_size);

          bundleData = sdr_malloc(sdr, msg_size);
          if (bundleData) {
            //FIXME: the signature should be const
            sdr_write(sdr, bundleData, (char *)msg_begin, msg_size);
          }
          m_bstring_release_access(item->data);

          if (sdr_end_xn(sdr))
          {
            AMP_DEBUG_ERR(__func__, "Failed sdr_end_xn()");
          }

          if (!bundleData) {
            AMP_DEBUG_ERR(__func__, "Error allocating bundle SDR for data size %zd", msg_size);
            result = AMP_FAIL;
          }
        }
      }

      Object bundleZco = 0;
      if (result == AMP_OK)
      {
        const int coarsePriority = 0;
        bundleZco = ionCreateZco(ZcoSdrSource, bundleData, 0, msg_size,
                                 coarsePriority, 0, ZcoOutbound, NULL);
        if ((bundleZco == 0) || (bundleZco == (Object)ERROR)) {
          AMP_DEBUG_ERR(__func__, "Failed ionCreateZco() for data size %zd", msg_size);
          result = AMP_FAIL;

          if (!sdr_begin_xn(sdr))
          {
            AMP_DEBUG_ERR(__func__, "Failed sdr_begin_xn()");
          }
          else
          {
            sdr_free(sdr, bundleData);

            if (sdr_end_xn(sdr))
            {
              AMP_DEBUG_ERR(__func__, "Failed sdr_end_xn()");
            }
          }
        }
      }

      if (result == AMP_OK)
      {
        const int lifetime_s = 500;
        const int priority = BP_STD_PRIORITY;
        const BpCustodySwitch custodySwitch = NoCustodyRequested;
        const int rrFlags = 0;
        BpAncillaryData ancData = {0};
        ancData.flags = BP_RELIABLE;

        AMP_DEBUG_INFO(__func__, "Sending bundle ADU to %s with %zd octets", item->peer.name, msg_size);
        int result = bp_send(
            ion_iif.sap,
            item->peer.name, /* destEid */
            NULL, /* reportToEid */
            lifetime_s, /* lifespan */
            priority, /* classOfService */
            custodySwitch, /* custodySwitch */
            rrFlags,
            0, /* ackRequested */
            &ancData, /* ancillaryData */
            bundleZco, /* adu */
            NULL /* bundleObj */
        );
        if (result <= 0) {
          AMP_DEBUG_ERR(__func__, "Failed bp_send() to %s", item->peer.name);

          if (!sdr_begin_xn(sdr))
          {
            AMP_DEBUG_ERR(__func__, "Failed sdr_begin_xn()");
          }
          zco_destroy(sdr, bundleZco);
          if (sdr_end_xn(sdr))
          {
            AMP_DEBUG_ERR(__func__, "Failed sdr_end_xn()");
          }
        }
      }

      // done with the item
      prox_item_ptr_release(item_ptr);
    }
  }

  AMP_DEBUG_INFO(__func__, "Stopping worker");
  return NULL;
}

/** Thread work function for the BP receiving (incoming).
 */
static void *
bp_recv_worker(void *)
{
  AMP_DEBUG_INFO(__func__, "Starting worker");

  m_bstring_t msgbuf;
  m_bstring_init(msgbuf);

  while (daemon_run_get(&running))
  {
    BpDelivery dlv;
    if (bp_receive(ion_iif.sap, &dlv, BP_BLOCKING) < 0) {
      continue;
    }
    AMP_DEBUG_INFO(__func__, "Result of bp_receive() is %d", dlv.result);
    switch (dlv.result) {
      case BpEndpointStopped:
        AMP_DEBUG_INFO(__func__, "Endpoint stopped");
        // need to restart
        daemon_run_stop(&running);
        continue;
      case BpReceptionTimedOut:
      case BpReceptionInterrupted:
        // fall through to ending logic above
        continue;

      case BpPayloadPresent:
        // fall through to payload handling below
        break;
    }

    {
      //FIXME guard SDR by mutex?
      Sdr sdr = bp_get_sdr();

      ZcoReader reader;
      const vast pyldSize = zco_source_data_length(sdr, dlv.adu);
      zco_start_receiving(dlv.adu, &reader);

      vast result = -1;
      if (!sdr_begin_xn(sdr))
      {
        AMP_DEBUG_ERR(__func__, "Failed sdr_begin_xn()");
      }
      else
      {
        m_bstring_resize(msgbuf, pyldSize);
        const size_t msg_size  = m_bstring_size(msgbuf);
        uint8_t     *msg_begin = m_bstring_acquire_access(msgbuf, 0, msg_size);

        result = zco_receive_source(sdr, &reader, msg_size, (char *)msg_begin);
        m_bstring_release_access(msgbuf);
        if (sdr_end_xn(sdr))
        {
          AMP_DEBUG_ERR(__func__, "Failed sdr_end_xn()");
        }
      }
      if (result < 0) {
        AMP_DEBUG_ERR(__func__, "Cannot receive payload");
        m_bstring_reset(msgbuf);
        continue;
      }
    }

    prox_item_ptr_t *item_ptr = prox_item_ptr_new();
    {
      prox_item_t *item = prox_item_ptr_ref(item_ptr);
      strncpy(item->peer.name, dlv.bundleSourceEid, sizeof(item->peer.name));
      m_bstring_move(item->data, msgbuf);
      m_bstring_init(msgbuf);
      AMP_DEBUG_INFO(__func__, "Received bundle ADU from %s with %zd octets",
                     item->peer.name, m_bstring_size(item->data));
    }
    bp_release_delivery(&dlv, 1);

    prox_item_queue_push(incoming, item_ptr);
  }

  m_bstring_clear(msgbuf);

  AMP_DEBUG_INFO(__func__, "Stopping worker");
  return NULL;
}

static void
daemon_signal_handler(int signum)
{
  AMP_DEBUG_INFO("daemon_signal_handler", "Received signal %d", signum);
  daemon_run_stop(&running);
}

void
OS_Application_Startup()
{
  if (OS_API_Init() != OS_SUCCESS)
  {
    fprintf(stderr, "Failed OS_API_Init\n");
    OS_ApplicationExit(-1);
  }

  prox_item_queue_init(outgoing, PROX_ITEM_QUEUE_DEPTH);
  prox_item_queue_init(incoming, PROX_ITEM_QUEUE_DEPTH);

  /* Step 1: Process Command Line Arguments. */
  const int argc = OS_BSP_GetArgC();
  char *const *argv = OS_BSP_GetArgV();
  if (mgr_parse_args(argc, argv))
  {
      OS_ApplicationExit(EXIT_FAILURE);
  }

  if (arg_path_listen == NULL)
  {
      mgr_print_usage();
      OS_ApplicationExit(EXIT_FAILURE);
  }

  eid_t mgr_eid;
  strncpy(mgr_eid.name, arg_eid, AMP_MAX_EID_LEN);
  AMP_DEBUG_INFO(__func__,"Manager EID: %s", mgr_eid.name);

  if (ionAttach() < 0)
  {
    AMP_DEBUG_ERR(__func__, "Proxy can't attach to ION.", NULL);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  /* Step 2:  Attach to ION. */
  if (iif_register_node(&ion_iif, mgr_eid) != 1)
  {
    AMP_DEBUG_ERR(__func__, "Unable to register BP Node. Exiting.", NULL);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  if (iif_is_registered(&ion_iif))
  {
      AMP_DEBUG_INFO(__func__, "Mgr registered with ION, EID: %s",
                               iif_get_local_eid(&ion_iif).name);
  }
  else
  {
      AMP_DEBUG_ERR(__func__,"Failed to register mgr with ION, EID %s",
                                       iif_get_local_eid(&ion_iif).name);
      AMP_DEBUG_EXIT("mgr_init","->-1.",NULL);
      OS_ApplicationExit(EXIT_FAILURE);
  }

  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  act.sa_handler = daemon_signal_handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);

  // Configure listening socket
  sock_listen = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (sock_listen == -1)
  {
    AMP_DEBUG_ERR(__func__,"Failed to create socket %d", errno);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  struct sockaddr_un laddr;
  laddr.sun_family = AF_UNIX;
  char *sun_end    = stpncpy(laddr.sun_path, arg_path_listen, sizeof(laddr.sun_path));
  if (sun_end - laddr.sun_path >= (ssize_t)sizeof(laddr.sun_path))
  {
    AMP_DEBUG_ERR(__func__, "given path that is too long to fit in sockaddr_un");
    OS_ApplicationExit(EXIT_FAILURE);
  }

  // preemptive unlink
  unlink(laddr.sun_path);

  AMP_DEBUG_INFO(__func__, "Binding to socket %s", laddr.sun_path);
  int res = bind(sock_listen, (struct sockaddr *)&laddr, sizeof(laddr));
  if (res)
  {
    AMP_DEBUG_ERR(__func__, "Failed to bind socket %s with errno %d", laddr.sun_path, errno);
    OS_ApplicationExit(EXIT_FAILURE);
  }

  res = listen(sock_listen, 1);
  if (res)
  {
    AMP_DEBUG_ERR(__func__, "Failed to listen on socket with errno %d", errno);
    OS_ApplicationExit(EXIT_FAILURE);
  }
}

void OS_Application_Run()
{
  threadinfo_t threadinfo[] = {
      {&sock_worker, "sock_worker"},
      {&bp_send_worker, "bp_send_worker"},
      {&bp_recv_worker, "bp_recv_worker"},
  };
  if (threadset_start(&threads, threadinfo, sizeof(threadinfo)/sizeof(threadinfo_t), NULL) != AMP_OK)
  {
    AMP_DEBUG_ERR(__func__, "Failed to start work threads");
    OS_ApplicationExit(EXIT_FAILURE);
  }
#if defined(HAVE_LIBSYSTEMD)
  sd_notify(0, "READY=1");
#endif

  // block until stopped
  daemon_run_wait(&running);
#if defined(HAVE_LIBSYSTEMD)
  sd_notify(0, "STOPPING=1");
#endif
  OS_ApplicationShutdown(true);
  bp_close(ion_iif.sap);
  // interrupt any reading
  if (sock_conn >= 0)
  {
    shutdown(sock_conn, SHUT_RDWR);
    close(sock_conn);
  }

  AMP_DEBUG_ALWAYS(__func__,"Shutting down proxy");
  if (threadset_join(&threads) != AMP_OK)
  {
    AMP_DEBUG_ERR(__func__, "Failed to join work threads");
  }
  iif_deregister_node(&ion_iif);

  prox_item_queue_clear(outgoing);
  prox_item_queue_clear(incoming);

  AMP_DEBUG_INFO(__func__,"Exiting after cleanup");
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
            {"listen", required_argument, 0,'l'},
            {"eid", required_argument, 0,'e'},
            {"help", no_argument, 0,'h'},
        };
    while ((c = getopt_long(argc, argv, ":l:e:h", long_options, &option_index)) != -1)
    {
        switch(c)
        {
        case 'e':
            arg_eid = optarg;
            break;
        case 'l':
            arg_path_listen = optarg;
            break;
        case 'h':
            return 3;
        default:
            fprintf(stderr, "Error parsing arguments\n");
            return 2;
        }
    }

    // Check for any remaining unrecognized arguments
    if ((argc-optind) != 0)
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
    printf("Usage: ion_app_proxy [options]\n");
    printf("Supported Options:\n");
    printf("-l <path> The path to open a listening socket on.\n");
    printf("-e <EID> The EID to register the manager endpoint on.\n");
}
