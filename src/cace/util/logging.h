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
/** @file
 * Abstract interface for event logging.
 */
/** @page Logging
 * The CACE library provides a general purpose thread-safe logging facility
 * for the library itself and users of the library.
 *
 * Logging must be initialized once per process using cace_openlog() and
 * should be de-initialized before exiting the process using cace_closelog().
 * Log events themselves are queued by using one of the severity-specific
 * macros listed below.
 *
 * The supported log severity values are a subset of the POSIX syslog values
 * with enumerations and descriptions repeated below.
 *
 *  * @c LOG_CRIT critical conditions, logged by ::CACE_LOG_CRIT
 *  * @c LOG_ERR error conditions, logged by ::CACE_LOG_ERR
 *  * @c LOG_WARNING warning conditions, logged by ::CACE_LOG_WARNING
 *  * @c LOG_INFO informational message, logged by ::CACE_LOG_INFO
 *  * @c LOG_DEBUG debug-level message, logged by ::CACE_LOG_DEBUG
 */
#ifndef CACE_LOGGING_H_
#define CACE_LOGGING_H_

#include <syslog.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Opens the event log.
 * @note This should be called once per process, not thread or library instance.
 * At the end of the process there should be a call to cace_closelog()
 *
 * This is a mimic to POSIX openlog()
 */
void cace_openlog(void);

/** Closes the event log.
 * This is a mimic to POSIX closelog()
 * @sa cace_openlog
 */
void cace_closelog(void);

/** Interpret a text name as a severity level.
 *
 * @param[out] severity The associated severity level.
 * @param[in] name The text name, which is case insensitive.
 * @return Zero if successful.
 */
int cace_log_get_severity(int *severity, const char *name);

/** Set the least severity enabled for logging.
 * Other events will be dropped by the logging facility.
 * This function is multi-thread safe.
 *
 * @param severity The severity from a subset of the POSIX syslog values.
 * @sa cace_log_is_enabled_for
 */
void cace_log_set_least_severity(int severity);

/** Determine if a particular severity is being logged.
 * This function is multi-thread safe.
 *
 * @param severity The severity from a subset of the POSIX syslog values.
 * @return True if the severity level will be logged.
 * @sa cace_log_set_least_severity
 */
bool cace_log_is_enabled_for(int severity);

/** Log an event.
 *
 * @param severity The severity from a subset of the POSIX syslog values.
 * @param[in] filename The originating file name, which may include directory parts.
 * @param[in] lineno The originating file line number.
 * @param[in] funcname The originating function name.
 * @param[in] format The log message format string.
 * @param ... Values for the format string.
 */
void cace_log(int severity, const char *filename, int lineno, const char *funcname, const char *format, ...);

/** Perform LOG_CRIT level logging with auto-filled parameters.
 * The arguments to this macro are passed to cace_log() as the @c format and
 * its parameter values.
 */
#define CACE_LOG_CRIT(...) cace_log(LOG_CRIT, __FILE__, __LINE__, __func__, __VA_ARGS__)
/// @overload
#define CACE_LOG_ERR(...) cace_log(LOG_ERR, __FILE__, __LINE__, __func__, __VA_ARGS__)
/// @overload
#define CACE_LOG_WARNING(...) cace_log(LOG_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)
/// @overload
#define CACE_LOG_INFO(...) cace_log(LOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
/// @overload
#define CACE_LOG_DEBUG(...) cace_log(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_LOGGING_H_ */
