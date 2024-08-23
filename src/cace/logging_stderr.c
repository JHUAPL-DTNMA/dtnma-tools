/*
 * Copyright (c) 2024 The Johns Hopkins University Applied Physics
 * Laboratory LLC.
 *
 * This file is part of the BPSec Library (BSL).
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
 *
 * This work was performed for the Jet Propulsion Laboratory, California
 * Institute of Technology, sponsored by the United States Government under
 * the prime contract 80NM0018D0004 between the Caltech and NASA under
 * subcontract 1700763.
 */
#include "cace/config.h"
#include "logging.h"
#include "util.h"
#include <m-buffer.h>
#include <m-string.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

/// Number of events to buffer to I/O thread
#define BSL_LOG_QUEUE_SIZE 100

static const char *prionames[] = {
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
    string_t context;
    /// Fully formatted message
    string_t message;
} cace_log_event_t;

void cace_log_event_init(cace_log_event_t *obj)
{
    obj->thread = pthread_self();
    gettimeofday(&(obj->timestamp), NULL);
    obj->severity = LOG_DEBUG;
    string_init(obj->context);
    string_init(obj->message);
}

void cace_log_event_deinit(cace_log_event_t *obj)
{
    string_clear(obj->message);
    string_clear(obj->context);
}

#define M_OPL_cace_log_event_t() (INIT(API_2(cace_log_event_init)), CLEAR(API_2(cace_log_event_deinit)))

/// @cond Doxygen_Suppress
M_BUFFER_DEF(cace_log_queue, cace_log_event_t, BSL_LOG_QUEUE_SIZE, M_BUFFER_THREAD_SAFE | M_BUFFER_BLOCKING)
/// @endcond

/// Shared safe queue
static cace_log_queue_t event_queue;
/// Sink thread ID
static pthread_t thr_sink;
/// True if #thr_sink is valid
static atomic_bool thr_valid = ATOMIC_VAR_INIT(false);

static void write_log(const cace_log_event_t *event)
{
    CHKVOID(event);
    // already domain validated
    const char *prioname = prionames[event->severity];

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
    fprintf(stderr, "%s T:%s %s <%s> %s\n", tmbuf, thrbuf, string_get_cstr(event->context), prioname,
            string_get_cstr(event->message));
    fflush(stderr);
}

static void *work_sink(void *arg _U_)
{
    while (true)
    {
        cace_log_event_t event;
        cace_log_queue_pop(&event, event_queue);
        if (string_empty_p(event.message))
        {
            cace_log_event_deinit(&event);
            break;
        }

        write_log(&event);
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
        string_set_str(manual.message, "cace_openlog() failed");
        write_log(&manual);
        cace_log_event_deinit(&manual);
    }
    else
    {
        atomic_store(&thr_valid, true);
    }
}

void cace_log(int severity, const char *filename, int lineno, const char *funcname, const char *format, ...)
{
    if ((severity < 0) || (severity > LOG_DEBUG))
    {
        return;
    }

    cace_log_event_t event;
    cace_log_event_init(&event);
    event.severity = severity;

    if (filename)
    {
        static const char dirsep = '/';
        const char       *pos    = strrchr(filename, dirsep);
        if (pos)
        {
            pos += 1;
        }
        else
        {
            pos = filename;
        }
        string_printf(event.context, "%s:%d:%s", pos, lineno, funcname);
    }

    {
        va_list val;
        va_start(val, format);
        string_vprintf(event.message, format, val);
        va_end(val);
    }
    if (string_empty_p(event.message))
    {
        // ignore empty messages
        cace_log_event_deinit(&event);
        return;
    }

    if (atomic_load(&thr_valid))
    {
        cace_log_queue_push(event_queue, event);
    }
    else
    {
        cace_log_event_t manual;
        cace_log_event_init(&manual);
        manual.severity = LOG_CRIT;
        string_set_str(manual.message, "cace_log() called before cace_openlog()");
        write_log(&manual);
        cace_log_event_deinit(&manual);

        write_log(&event);
        cace_log_event_deinit(&event);
    }
}

void cace_closelog(void)
{
    // sentinel empty message
    cace_log_event_t event;
    cace_log_event_init(&event);
    cace_log_queue_push(event_queue, event);

    int res = pthread_join(thr_sink, NULL);
    if (res)
    {
        // unsynchronized write
        cace_log_event_t manual;
        cace_log_event_init(&manual);
        manual.severity = LOG_CRIT;
        string_set_str(manual.message, "cace_closelog() failed");
        write_log(&manual);
        cace_log_event_deinit(&manual);
    }
    else
    {
        atomic_store(&thr_valid, false);
    }
}
