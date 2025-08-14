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
#include "cace/config.h"
#include "logging.h"
#include "defs.h"
#include <m-buffer.h>
#include <m-string.h>
#include <m-atomic.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>

/// Number of events to buffer to I/O thread
#define BSL_LOG_QUEUE_SIZE 100

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

/// A single event for the log
typedef struct
{
    /// Source thread ID
    pthread_t thread;
    /// Source event timestamp
    struct timeval timestamp;
    /// Event severity enumeration
    int severity;
    /// File and function context
    m_string_t context;
    /// Fully formatted message
    m_string_t message;
} cace_log_event_t;

void cace_log_event_init(cace_log_event_t *obj)
{
    obj->thread = pthread_self();
    gettimeofday(&(obj->timestamp), NULL);
    obj->severity = LOG_DEBUG;
    m_string_init(obj->context);
    m_string_init(obj->message);
}

static void cace_log_event_init_set(cace_log_event_t *obj, const cace_log_event_t *src)
{
    obj->thread    = src->thread;
    obj->timestamp = src->timestamp;
    obj->severity  = src->severity;
    string_init_set(obj->context, src->context);
    string_init_set(obj->message, src->message);
}

static void cace_log_event_init_move(cace_log_event_t *obj, cace_log_event_t *src)
{
    obj->thread    = src->thread;
    obj->timestamp = src->timestamp;
    obj->severity  = src->severity;
    string_init_move(obj->context, src->context);
    string_init_move(obj->message, src->message);
}

static void cace_log_event_set(cace_log_event_t *obj, const cace_log_event_t *src)
{
    obj->thread    = src->thread;
    obj->timestamp = src->timestamp;
    obj->severity  = src->severity;
    string_set(obj->context, src->context);
    string_set(obj->message, src->message);
}

void cace_log_event_deinit(cace_log_event_t *obj)
{
    m_string_clear(obj->message);
    m_string_clear(obj->context);
}

/// OPLIST for cace_log_event_t
#define M_OPL_cace_log_event_t()                                                 \
    (INIT(API_2(cace_log_event_init)), INIT_SET(API_6(cace_log_event_init_set)), \
     INIT_MOVE(API_6(cace_log_event_init_move)), SET(API_6(cace_log_event_set)), CLEAR(API_2(cace_log_event_deinit)))

/// @cond Doxygen_Suppress
M_BUFFER_DEF(cace_log_queue, cace_log_event_t, BSL_LOG_QUEUE_SIZE,
             M_BUFFER_THREAD_SAFE | M_BUFFER_BLOCKING | M_BUFFER_PUSH_INIT_POP_MOVE)
/// @endcond

/// Shared least severity
static atomic_int least_severity = LOG_DEBUG;

/// Shared safe queue
static cace_log_queue_t event_queue;
/// Sink thread ID
static pthread_t thr_sink;
/// True if ::thr_sink is valid
static atomic_bool thr_valid = ATOMIC_VAR_INIT(false);

static void write_log(const cace_log_event_t *event)
{
    CHKVOID(event);
    // already domain validated
    const char *prioname = sev_names[event->severity];

    char tmbuf[32];
    {
        time_t    nowtime = event->timestamp.tv_sec;
        struct tm nowtm;
        gmtime_r(&nowtime, &nowtm);

        char  *curs   = tmbuf;
        size_t remain = sizeof(tmbuf) - 1;
        size_t len    = strftime(curs, remain, "%Y-%m-%dT%H:%M:%S", &nowtm);
        curs += len;
        remain -= len;
        snprintf(curs, remain, ".%06ld", event->timestamp.tv_usec);
    }
    char thrbuf[2 * sizeof(pthread_t) + 1];
    {
        const uint8_t *data = (const void *)&(event->thread);
        char          *out  = thrbuf;
        for (size_t ix = 0; ix < sizeof(pthread_t); ++ix)
        {
            sprintf(out, "%02X", *data);
            data++;
            out += 2;
        }
        *out = '\0';
    }
    fprintf(stderr, "%s T:%s <%s> [%s] %s\n", tmbuf, thrbuf, prioname, m_string_get_cstr(event->context),
            m_string_get_cstr(event->message));
    fflush(stderr);
}

static void *work_sink(void *arg _U_)
{
    bool running = true;
    while (running)
    {
        cace_log_event_t event;
        cace_log_queue_pop(&event, event_queue);
        if (m_string_empty_p(event.message))
        {
            running = false;
        }
        else
        {
            write_log(&event);
        }
        cace_log_event_deinit(&event);
    }
    return NULL;
}

void cace_openlog(void)
{
    cace_log_queue_init(event_queue, BSL_LOG_QUEUE_SIZE);

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
    cace_log_event_t event;
    cace_log_event_init(&event);
    cace_log_queue_push(event_queue, event);
    cace_log_event_deinit(&event);

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
    BSL_LogEvent_queue_clear(event_queue);
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
    const bool enabled = (least_severity >= severity);
    return enabled;
}

void cace_log(int severity, const char *filename, int lineno, const char *funcname, const char *format, ...)
{
    if (!cace_log_is_enabled_for(severity))
    {
        return;
    }

    cace_log_event_t event;
    cace_log_event_init(&event);
    event.severity = severity;

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
        m_string_printf(event.context, "%s:%d:%s", pos, lineno, funcname);
    }

    {
        va_list val;
        va_start(val, format);
        m_string_vprintf(event.message, format, val);
        va_end(val);
    }
    // ignore empty messages
    if (!m_string_empty_p(event.message))
    {
        if (atomic_load(&thr_valid))
        {
            cace_log_queue_push(event_queue, event);
        }
        else
        {
            cace_log_event_t manual;
            cace_log_event_init(&manual);
            manual.severity = LOG_CRIT;
            m_string_set_cstr(manual.message, "cace_log() called before cace_openlog()");
            write_log(&manual);
            cace_log_event_deinit(&manual);

            write_log(&event);
        }
    }
    cace_log_event_deinit(&event);
}
