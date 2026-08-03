/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "defs.h"
#include "logging.h"

#include "cace/config.h"

#include <m-atomic.h>
#include <m-buffer.h>
#include <m-shared-ptr.h>
#include <m-string.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>

/// Number of events to buffer to I/O thread
#define CACE_LOG_QUEUE_SIZE 100

// NOLINTBEGIN
static const char *sev_names[] = {
    NULL,      // LOG_EMERG
    NULL,      // LOG_ALERT
    "CRIT",    // LOG_CRIT
    "ERROR",   // LOG_ERR
    "WARNING", // LOG_WARNING
    NULL,      // LOG_NOTICE
    "INFO",    // LOG_INFO
    "DEBUG",   // LOG_DEBUG
};
// NOLINTEND

/// A single event for the log
typedef struct
{
    /// Source thread ID
    pthread_t thread;
    /// Source event timestamp
    struct timespec timestamp;
    /// Event severity enumeration
    int severity;
    /// File and function context
    m_string_t context;
    /// Fully formatted message
    m_string_t message;
} cace_log_event_t;

void cace_log_event_init(cace_log_event_t *obj)
{
    obj->thread    = pthread_self();
    obj->timestamp = (struct timespec) { 0 };
    obj->severity  = LOG_DEBUG;
    m_string_init(obj->context);
    m_string_init(obj->message);
}

void cace_log_event_deinit(cace_log_event_t *obj)
{
    m_string_clear(obj->message);
    m_string_clear(obj->context);
}

/// OPLIST for cace_log_event_t
#define M_OPL_cace_log_event_t() \
    (INIT(API_2(cace_log_event_init)), INIT_SET(0), SET(0), CLEAR(API_2(cace_log_event_deinit)))

/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_SHARED_WEAK_PTR_DEF(cace_log_event_ptr, cace_log_event_t)
#define M_OPL_cace_log_event_ptr_t() M_SHARED_PTR_OPLIST(cace_log_event_ptr, M_OPL_cace_log_event_t())
M_BUFFER_DEF(cace_log_queue, cace_log_event_ptr_t *, CACE_LOG_QUEUE_SIZE, M_BUFFER_QUEUE, M_OPL_cace_log_event_ptr_t())
// GCOV_EXCL_STOP
/// @endcond

/// Shared least severity
static atomic_int least_severity = LOG_DEBUG;

/// Shared safe queue
static cace_log_queue_t event_queue;
/// Sink thread ID
static pthread_t thr_sink;
/// True if ::thr_sink is valid
static atomic_bool thr_valid = ATOMIC_VAR_INIT(false);
/// Log internal error once
static atomic_bool did_crit = ATOMIC_VAR_INIT(false);

static void write_log(const cace_log_event_t *event)
{
    CHKVOID(event);
    // already domain validated
    const char *severity_name = sev_names[event->severity];

    char tmbuf[32]; // NOLINT
    {
        time_t    nowtime = event->timestamp.tv_sec;
        struct tm nowtm;
        gmtime_r(&nowtime, &nowtm);

        char  *curs   = tmbuf;
        size_t remain = sizeof(tmbuf) - 1;
        size_t len    = strftime(curs, remain, "%Y-%m-%dT%H:%M:%S", &nowtm);
        curs += len;
        remain -= len;
        snprintf(curs, remain, ".%06ldZ", event->timestamp.tv_nsec / 1000);
    }
    char thrbuf[2 * sizeof(pthread_t) + 1];
    {
        const uint8_t *data   = (const void *)&(event->thread);
        char          *out    = thrbuf;
        size_t         remain = sizeof(thrbuf);
        for (size_t ix = 0; ix < sizeof(pthread_t); ++ix)
        {
            snprintf(out, remain, "%02X", *data);
            data++;
            out += 2;
        }
        *out = '\0';
    }
    fprintf(stderr, "%s T:%s <%s> [%s] %s\n", tmbuf, thrbuf, severity_name, m_string_get_cstr(event->context),
            m_string_get_cstr(event->message));
    fflush(stderr);
}

static void *work_sink(void *arg _U_)
{
    bool running = true;
    while (running)
    {
        cace_log_event_ptr_t *event_ptr;
        cace_log_queue_pop_move(&event_ptr, event_queue);
        const cace_log_event_t *event = cace_log_event_ptr_cref(event_ptr);
        if (m_string_empty_p(event->message))
        {
            running = false;
        }
        else
        {
            write_log(event);
        }
        cace_log_event_ptr_release(event_ptr);
    }
    return NULL;
}

void cace_openlog(void)
{
    cace_log_queue_init(event_queue, CACE_LOG_QUEUE_SIZE);

    if (pthread_create(&thr_sink, NULL, work_sink, NULL))
    {
        // unsynchronized write
        cace_log_event_t manual;
        cace_log_event_init(&manual);
        manual.severity = LOG_CRIT;
        m_string_set_cstr(manual.message, "cace_openlog() failed");
        write_log(&manual);
        cace_log_event_deinit(&manual);
    }
    else
    {
        atomic_store(&thr_valid, true);
    }
}

void cace_closelog(void)
{
    // sentinel empty message
    cace_log_event_ptr_t *event_ptr = cace_log_event_ptr_new();
    cace_log_queue_push_move(event_queue, &event_ptr);

    int res = pthread_join(thr_sink, NULL);
    if (res)
    {
        // unsynchronized write
        cace_log_event_t manual;
        cace_log_event_init(&manual);
        manual.severity = LOG_CRIT;
        m_string_set_cstr(manual.message, "cace_closelog() failed");
        write_log(&manual);
        cace_log_event_deinit(&manual);
    }
    else
    {
        atomic_store(&thr_valid, false);
    }

    // no consumer after join above
    cace_log_queue_clear(event_queue);
}

int cace_log_get_severity(int *severity, const char *name)
{
    CHKERR1(severity)
    CHKERR1(name)

    for (size_t ix = 0; ix < sizeof(sev_names) / sizeof(const char *); ++ix)
    {
        if (!sev_names[ix])
        {
            continue;
        }
        if (strcasecmp(sev_names[ix], name) == 0)
        {
            *severity = (int)ix;
            return 0;
        }
    }
    return 2;
}

void cace_log_set_least_severity(int severity)
{
    if ((severity < 0) || (severity > LOG_DEBUG))
    {
        return;
    }

    atomic_store(&least_severity, severity);
}

bool cace_log_is_enabled_for(int severity)
{
    if ((severity < 0) || (severity > LOG_DEBUG))
    {
        return false;
    }

    const int limit = atomic_load(&least_severity);
    // lower severity has higher define value
    const bool enabled = (limit >= severity);
    return enabled;
}

void cace_log(int severity, const char *filename, int lineno, const char *funcname, const char *format, ...)
{
    if (!cace_log_is_enabled_for(severity))
    {
        return;
    }

    cace_log_event_ptr_t *event_ptr = cace_log_event_ptr_new();
    // set the full state
    cace_log_event_t *event = cace_log_event_ptr_ref(event_ptr);

    (void)clock_gettime(CLOCK_REALTIME, &event->timestamp);
    event->severity = severity;

    if (filename)
    {
        static const char dirsep = '/';

        const char *pos = strrchr(filename, dirsep);
        if (pos)
        {
            pos += 1;
        }
        else
        {
            pos = filename;
        }
        m_string_printf(event->context, "%s:%d:%s", pos, lineno, funcname);
    }

    {
        va_list val;
        va_start(val, format);
        m_string_vprintf(event->message, format, val);
        va_end(val);
    }
    // ignore empty messages
    if (m_string_empty_p(event->message))
    {
        cace_log_event_ptr_release(event_ptr);
    }
    else
    {
        if (atomic_load(&thr_valid))
        {
            cace_log_queue_push_move(event_queue, &event_ptr);
        }
        else
        {
            if (!atomic_load(&did_crit))
            {
                cace_log_event_t manual;
                cace_log_event_init(&manual);
                manual.severity = LOG_CRIT;
                m_string_set_cstr(manual.message, "cace_log() called before cace_openlog()");
                write_log(&manual);
                cace_log_event_deinit(&manual);

                atomic_store(&did_crit, true);
            }
            write_log(event);
            cace_log_event_ptr_release(event_ptr);
        }
    }
}
