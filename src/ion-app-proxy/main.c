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
#include "cace/amp/proxy_msg.h"
#include "cace/amp/ion_bp.h"
#include "cace/ari/text.h"
#include "cace/util/threadset.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include <m-bstring.h>
#include <m-shared-ptr.h>
#include <m-buffer.h>
#include <signal.h>
#include <getopt.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <bp.h>
#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-daemon.h>
#endif

static int  mgr_parse_args(int argc, char *const argv[]);
static void show_usage(const char *argv0);

typedef struct
{
    /// Peer EID value
    cace_ari_t peer;
    /// Application data
    m_bstring_t data;
} prox_item_t;

static void prox_item_init(prox_item_t *obj)
{
    cace_ari_init(&obj->peer);
    m_bstring_init(obj->data);
}

static void prox_item_deinit(prox_item_t *obj)
{
    m_bstring_clear(obj->data);
    cace_ari_deinit(&obj->peer);
}

/// M*LIB oplist for prox_item_t
#define M_OPL_prox_item_t() \
    (INIT(API_2(prox_item_init)), CLEAR(API_2(prox_item_deinit)), INIT_SET(0), INIT_MOVE(0), SET(0), MOVE(0))

// Thread-unsafe shared pointer for move semantics
SHARED_WEAK_PTR_DEF(prox_item_ptr, prox_item_t)

/// thread safe running state
static cace_daemon_run_t running;
/// logging filter
static int log_limit = LOG_WARNING;
/// Local listening socket path
static char *arg_path_listen = NULL;
/// Socket FD for listening
static int sock_listen = -1;
/// Socket FD for traffic
static int sock_conn = -1;
/// Local BP endpoint ID
static char *arg_eid = NULL;
/// ION interface struct
static BpSAP ion_sap;

#define PROX_ITEM_QUEUE_DEPTH 10
// Thread safe queue of shared pointers
M_BUFFER_DEF(prox_item_queue, prox_item_ptr_t *, PROX_ITEM_QUEUE_DEPTH,
             BUFFER_QUEUE | BUFFER_THREAD_SAFE | BUFFER_PUSH_INIT_POP_MOVE,
             SHARED_PTR_OPLIST(prox_item_ptr, M_OPL_prox_item_t()))

/// Items outgoing to the BPA
static prox_item_queue_t outgoing;
/// Items incoming from the BPA
static prox_item_queue_t incoming;
/// Thread states
static cace_threadset_t threads;

/** Thread work function for the socket side.
 */
static void *sock_worker(void *arg _U_)
{
    int res;
    CACE_LOG_INFO("Starting worker");

    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);

    while (cace_daemon_run_get(&running))
    {
        const bool can_send = !prox_item_queue_empty_p(incoming);
        if ((sock_conn >= 0) && can_send)
        {
            CACE_LOG_INFO("Polling with sendable items");
        }

        struct pollfd pfds[] = {
            { .fd = sock_listen, .events = POLLIN | POLLERR | POLLHUP },
            { .fd = sock_conn, .events = (can_send ? POLLOUT : 0) | POLLIN | POLLERR | POLLHUP },
        };

        // Wait up to 1 second
        res = poll(pfds, sizeof(pfds) / sizeof(struct pollfd), 1000);
        if (res < 0)
        {
            CACE_LOG_ERR("Failed to poll with errno %d", errno);
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
            CACE_LOG_INFO("Stopping due to hangup");
            cace_daemon_run_stop(&running);
        }
        if (pfds[0].revents & POLLIN)
        {
            if (sock_conn < 0)
            {
                CACE_LOG_INFO("Accepting incoming connection");
                sock_conn = accept(sock_listen, NULL, NULL);
                continue;
            }
            else
            {
                CACE_LOG_INFO("Ignoring incoming connection");
            }
        }

        // Handle accepted user socket
        if ((sock_conn >= 0) && (pfds[1].revents & (POLLERR | POLLHUP)))
        {
            // input has closed
            CACE_LOG_INFO("Disconnecting due to hangup");
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

                res = cace_amp_proxy_msg_send(sock_conn, &(item->peer), data_begin, data_size);
                if (res)
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
            prox_item_t     *item     = prox_item_ptr_ref(item_ptr);

            res = cace_amp_proxy_msg_recv(sock_conn, &(item->peer), item->data);
            if (res)
            {
                CACE_LOG_ERR("Failed proxy_if_recv()");
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

    CACE_LOG_INFO("Stopping worker");
    return NULL;
}

/** Thread work function for the BP sending (outgoing).
 */
static void *bp_send_worker(void *ctx _U_)
{
    CACE_LOG_INFO("Starting worker");

    while (cace_daemon_run_get(&running))
    {
        prox_item_ptr_t *item_ptr;
        // if there is actually an item present at this point
        if (prox_item_queue_pop(&item_ptr, outgoing))
        {
            prox_item_t *item = prox_item_ptr_ref(item_ptr);

            // Create ADU as ZCO
            int result = 0;

            // FIXME guard SDR by mutex?
            Sdr sdr = bp_get_sdr();

            const size_t msg_size   = m_bstring_size(item->data);
            Object       bundleData = 0;
            {
                if (!sdr_begin_xn(sdr))
                {
                    CACE_LOG_ERR("Failed sdr_begin_xn()");
                }
                else
                {
                    uint8_t *msg_begin = m_bstring_acquire_access(item->data, 0, msg_size);

                    bundleData = sdr_malloc(sdr, msg_size);
                    if (bundleData)
                    {
                        // FIXME: the signature should be const
                        sdr_write(sdr, bundleData, (char *)msg_begin, msg_size);
                    }
                    m_bstring_release_access(item->data);

                    if (sdr_end_xn(sdr))
                    {
                        CACE_LOG_ERR("Failed sdr_end_xn()");
                    }

                    if (!bundleData)
                    {
                        CACE_LOG_ERR("Error allocating bundle SDR for data size %zd", msg_size);
                        result = 2;
                    }
                }
            }

            Object bundleZco = 0;
            if (result == 0)
            {
                const int coarsePriority = 0;
                bundleZco = ionCreateZco(ZcoSdrSource, bundleData, 0, msg_size, coarsePriority, 0, ZcoOutbound, NULL);
                if ((bundleZco == 0) || (bundleZco == (Object)ERROR))
                {
                    CACE_LOG_ERR("Failed ionCreateZco() for data size %zd", msg_size);
                    result = 3;

                    if (!sdr_begin_xn(sdr))
                    {
                        CACE_LOG_ERR("Failed sdr_begin_xn()");
                    }
                    else
                    {
                        sdr_free(sdr, bundleData);

                        if (sdr_end_xn(sdr))
                        {
                            CACE_LOG_ERR("Failed sdr_end_xn()");
                        }
                    }
                }
            }

            // This is pointer-to-non-const because of ION API issue
            char *dest_eid = NULL;
            if (result == 0)
            {
                const cace_data_t *dest_data = cace_ari_cget_tstr(&item->peer);
                if (dest_data)
                {
                    dest_eid = (char *)(dest_data->ptr);
                }
                else
                {
                    string_t buf;
                    string_init(buf);
                    cace_ari_text_encode(buf, &item->peer, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                    CACE_LOG_ERR("This transport can only send to text URI destinations, not %s",
                                 m_string_get_cstr(buf));
                    string_clear(buf);

                    result = 4;
                }
            }

            if (result == 0)
            {
                CACE_LOG_INFO("Sending bundle ADU to %s with %zd octets", dest_eid, msg_size);

                const int             lifetime_s    = 500;
                const int             priority      = BP_STD_PRIORITY;
                const BpCustodySwitch custodySwitch = NoCustodyRequested;
                const int             rrFlags       = 0;
                const int             ackRequested  = 0;
                BpAncillaryData       ancData       = { 0 };
                ancData.flags                       = BP_RELIABLE | BP_BEST_EFFORT;

                int result = bp_send(ion_sap, dest_eid, // destination
                                     NULL,              // report-to
                                     lifetime_s,        // lifetime in seconds
                                     priority,          // Class-of-Service / Priority
                                     custodySwitch,     // Custody Switch
                                     rrFlags,           // SRR Flags
                                     ackRequested,      // ACK Requested
                                     &ancData,          // ancillary data
                                     bundleZco,         // ADU
                                     NULL               // bundleObj
                );
                if (result <= 0)
                {
                    CACE_LOG_ERR("Failed bp_send() to %s", dest_eid);

                    if (!sdr_begin_xn(sdr))
                    {
                        CACE_LOG_ERR("Failed sdr_begin_xn()");
                    }
                    zco_destroy(sdr, bundleZco);
                    if (sdr_end_xn(sdr))
                    {
                        CACE_LOG_ERR("Failed sdr_end_xn()");
                    }
                }
            }

            // done with the item
            prox_item_ptr_release(item_ptr);
        }
    }

    CACE_LOG_INFO("Stopping worker");
    return NULL;
}

/** Thread work function for the BP receiving (incoming).
 */
static void *bp_recv_worker(void *ctx _U_)
{
    CACE_LOG_INFO("Starting worker");

    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);

    while (cace_daemon_run_get(&running))
    {
        BpDelivery dlv;
        if (bp_receive(ion_sap, &dlv, BP_BLOCKING) < 0)
        {
            continue;
        }
        CACE_LOG_INFO("Result of bp_receive() is %d", dlv.result);
        switch (dlv.result)
        {
            case BpEndpointStopped:
                CACE_LOG_INFO("Endpoint stopped");
                // need to restart
                cace_daemon_run_stop(&running);
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
            // FIXME guard SDR by mutex?
            Sdr sdr = bp_get_sdr();

            ZcoReader  reader;
            const vast pyldSize = zco_source_data_length(sdr, dlv.adu);
            zco_start_receiving(dlv.adu, &reader);

            vast result = -1;
            if (!sdr_begin_xn(sdr))
            {
                CACE_LOG_ERR("Failed sdr_begin_xn()");
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
                    CACE_LOG_ERR("Failed sdr_end_xn()");
                }
            }
            if (result < 0)
            {
                CACE_LOG_ERR("Cannot receive payload");
                m_bstring_reset(msgbuf);
                continue;
            }
        }

        prox_item_ptr_t *item_ptr = prox_item_ptr_new();
        {
            prox_item_t *item = prox_item_ptr_ref(item_ptr);
            cace_ari_set_tstr(&item->peer, dlv.bundleSourceEid, true);
            m_bstring_move(item->data, msgbuf);
            m_bstring_init(msgbuf);
            CACE_LOG_INFO("Received bundle ADU from %s with %zd octets", dlv.bundleSourceEid,
                          m_bstring_size(item->data));
        }
        bp_release_delivery(&dlv, 1);

        prox_item_queue_push(incoming, item_ptr);
    }

    m_bstring_clear(msgbuf);

    CACE_LOG_INFO("Stopping worker");
    return NULL;
}

static void daemon_signal_handler(int signum)
{
    CACE_LOG_INFO("Received signal %d", signum);
    cace_daemon_run_stop(&running);
}

int main(int argc, char *argv[])
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    prox_item_queue_init(outgoing, PROX_ITEM_QUEUE_DEPTH);
    prox_item_queue_init(incoming, PROX_ITEM_QUEUE_DEPTH);

    /* Step 1: Process Command Line Arguments. */
    if (mgr_parse_args(argc, argv))
    {
        retval = 1;
    }
    cace_log_set_least_severity(log_limit);
    CACE_LOG_DEBUG("Proxy starting up with log limit %d", log_limit);

    if (retval || (arg_path_listen == NULL))
    {
        show_usage(argv[0]);
    }

    if (!retval)
    {
        /* Step 2:  Attach to ION. */
        if (ionAttach() < 0)
        {
            CACE_LOG_ERR("Proxy can't attach to ION");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Attached to ION");
        }
    }

    if (!retval)
    {
        if (bp_attach() < 0)
        {
            CACE_LOG_ERR("Failed to attach to BP");
            retval = 3;
        }
    }

    if (!retval)
    {
        if (bp_open(arg_eid, &ion_sap) < 0)
        {
            CACE_LOG_ERR("Failed to open EID %s", arg_eid);
            retval = 4;
        }
        else
        {
            CACE_LOG_INFO("Opened endpoint with EID: %s", arg_eid);
        }
    }

    if (!retval)
    {
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        act.sa_handler = daemon_signal_handler;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
    }

    if (!retval)
    {
        // Configure listening socket
        sock_listen = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (sock_listen == -1)
        {
            CACE_LOG_ERR("Failed to create socket %d", errno);
            retval = 5;
        }
    }

    struct sockaddr_un laddr;
    laddr.sun_family = AF_UNIX;
    if (!retval)
    {
        char *sun_end = stpncpy(laddr.sun_path, arg_path_listen, sizeof(laddr.sun_path));
        if (sun_end - laddr.sun_path >= (ssize_t)sizeof(laddr.sun_path))
        {
            CACE_LOG_ERR("given path that is too long to fit in sockaddr_un");
            retval = 6;
        }
    }

    if (!retval)
    {
        // preemptive unlink
        unlink(laddr.sun_path);

        CACE_LOG_INFO("Binding to socket %s", laddr.sun_path);
        int res = bind(sock_listen, (struct sockaddr *)&laddr, sizeof(laddr));
        if (res)
        {
            CACE_LOG_ERR("Failed to bind socket %s with errno %d", laddr.sun_path, errno);
            retval = 7;
        }
    }

    if (!retval)
    {
        int res = listen(sock_listen, 1);
        if (res)
        {
            CACE_LOG_ERR("Failed to listen on socket with errno %d", errno);
            retval = 8;
        }
    }

    if (!retval)
    {
        cace_threadinfo_t threadinfo[] = {
            { &sock_worker, "sock_worker" },
            { &bp_send_worker, "bp_send_worker" },
            { &bp_recv_worker, "bp_recv_worker" },
        };
        if (cace_threadset_start(threads, threadinfo, sizeof(threadinfo) / sizeof(cace_threadinfo_t), NULL))
        {
            CACE_LOG_ERR("Failed to start work threads");
            retval = 9;
        }
    }

    if (!retval)
    {
#if defined(HAVE_LIBSYSTEMD)
        sd_notify(0, "READY=1");
#endif

        // block until stopped
        cace_daemon_run_wait(&running);
#if defined(HAVE_LIBSYSTEMD)
        sd_notify(0, "STOPPING=1");
#endif
    }

    bp_close(ion_sap);
    // interrupt any reading
    if (sock_conn >= 0)
    {
        shutdown(sock_conn, SHUT_RDWR);
        close(sock_conn);
    }

    CACE_LOG_INFO("Shutting down proxy");
    if (cace_threadset_join(threads))
    {
        CACE_LOG_ERR("Failed to join work threads");
    }
    bp_detach();
    ionDetach();

    prox_item_queue_clear(outgoing);
    prox_item_queue_clear(incoming);
    cace_closelog();

    CACE_LOG_INFO("Exiting after cleanup");
    return retval;
}

/**
 * Parse optional command line arguments
 */
int mgr_parse_args(int argc, char *const argv[])
{
    int                  i;
    int                  c;
    int                  option_index   = 0;
    static struct option long_options[] = {
        { "log-level", required_argument, 0, 'l' },
        { "bind", required_argument, 0, 'b' },
        { "eid", required_argument, 0, 'e' },
        { "help", no_argument, 0, 'h' },
    };
    while ((c = getopt_long(argc, argv, ":hl:b:e:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 'l':
                if (cace_log_get_severity(&log_limit, optarg))
                {
                    show_usage(argv[0]);
                    return 1;
                }
                break;
            case 'e':
                arg_eid = optarg;
                break;
            case 'b':
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
    if ((argc - optind) != 0)
    {
        fprintf(stderr, "%d unrecognized arguments:\n", (argc - optind));
        for (i = optind; i < argc; i++)
        {
            printf("\t%s\n", argv[i]);
        }
        return 4;
    }
    return 0;
}

static void show_usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s [options]\n", argv0);
    fprintf(stderr, "Supported Options:\n");
    fprintf(stderr, " -b <path> The path to open a listening socket on.\n");
    fprintf(stderr, " -e <EID> The EID to register the manager endpoint on.\n");
}
