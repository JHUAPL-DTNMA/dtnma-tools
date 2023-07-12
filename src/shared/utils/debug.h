/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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

/*****************************************************************************
 **
 ** \file debug.h
 **
 ** Description: AMP debugging tools
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  10/21/11  E. Birrane     Initial Implementation. (JHU/APL)
 **  08/03/16  E. Birrane     Cleanup from DTNMP to AMP (Secure DTN - NASA: NNX14CS58P)
 *****************************************************************************/
#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 *                              DEBUG DEFINITIONS                            *
 *****************************************************************************/

#ifndef AMP_DEBUGGING
#define AMP_DEBUGGING	1  /** Whether to enable (1) or disable (0) debugging */
#endif

#define AMP_DEBUG_LVL_PROC 1 /** Function entry/exit and above debugging */
#define AMP_DEBUG_LVL_INFO 2 /** Info information and above debugging */
#define AMP_DEBUG_LVL_WARN 3 /** Warning and above debugging */
#define AMP_DEBUG_LVL_ERR  4 /** Error and above debugging */

#define AMP_DEBUG_LVL	AMP_DEBUG_LVL_PROC
#define	AMP_GMSG_BUFLEN	256

/**
 * \def AMP_DEBUG
 * Constructs an error string message and sends it to putErrmsg. There are
 * four levels of debugging specified:
 * 1: Function entry/exit logging.  This logs the entry and exit of all major
 *    functions in the AMP library and is useful for confirming control flow
 *    through the AMP module.
 * 2: Information logging.  Information statements are peppered through the
 *    code to provide insight into the state of the module at processing
 *    points considered useful by AMP module software engineers.
 * 3: Warning logging.  Warning statements are used to flag unexpected 
 *    values that, based on context, may not constitute errors.
 * 4: Error logging.  Errors are areas in the code where some sanity check
 *    or other required condition fails to be met by the software. 
 * 
 * Error logging within the AMP module is of the form:
 * <id> <function name>: <message>
 * Where id is one of:
 * + (function entry)
 * - (function exit)
 * i (information statement)
 * ? (warning statement)
 * x (error statement)
 * 
 * Debugging can be turned off at compile time by removing the
 * AMP_DEBUGGING #define.
 */

#if AMP_DEBUGGING == 1
#define AMP_DEBUG_ENTRY(func, format, ...)  amp_log(AMP_DEBUG_LVL_PROC,'+', __FILE__, __LINE__, func, format, ##__VA_ARGS__)
#define AMP_DEBUG_EXIT(func, format, ...)   amp_log(AMP_DEBUG_LVL_PROC,'-', __FILE__, __LINE__, func, format, ##__VA_ARGS__)
#define AMP_DEBUG_INFO(func, format, ...)   amp_log(AMP_DEBUG_LVL_INFO,'i', __FILE__, __LINE__, func, format, ##__VA_ARGS__)
#define AMP_DEBUG_WARN(func, format, ...)   amp_log(AMP_DEBUG_LVL_WARN,'w', __FILE__, __LINE__, func, format, ##__VA_ARGS__)
#define AMP_DEBUG_ERR(func, format, ...)    amp_log(AMP_DEBUG_LVL_ERR, 'x', __FILE__, __LINE__, func, format, ##__VA_ARGS__)
#define AMP_DEBUG_ALWAYS(func, format, ...) amp_log(AMP_DEBUG_LVL,     ':', __FILE__, __LINE__, func, format, ##__VA_ARGS__)
void amp_log(int level, char label, const char *file, int line, const char *func, const char *fmt, ...);

#else
#define AMP_DEBUG_ENTRY(func, format, ...)
#define AMP_DEBUG_EXIT(func, format, ...)
#define AMP_DEBUG_INFO(func, format, ...)
#define AMP_DEBUG_WARN(func, format, ...)
#define AMP_DEBUG_ERR(func, format, ...)
#define AMP_DEBUG_ALWAYS(func, format, ...)

#endif

#ifdef __cplusplus
}
#endif

#endif  /* _DEBUG_H_ */
