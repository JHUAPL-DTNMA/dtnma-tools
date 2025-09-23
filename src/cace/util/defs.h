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
 * @ingroup ari
 * This file contains preprocessor utility definitions shared among the
 * ARI library.
 */
#ifndef CACE_UTIL_H_
#define CACE_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Mark an unused parameter Within a function definition.
 * This avoids compiler warnings when parameters need to be present to satisfy
 * an interface but are otherwise unused.
 */
#if defined(__GNUC__) || defined(__clang__)
#define _U_ __attribute__((unused))
#elif defined(_MSC_VER)
#define _U_ __pragma(warning(suppress : 4100 4189))
#else
#define _U_
#endif

#ifndef UNLIKELY
#if defined(__GNUC__)
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#define LIKELY(expr)   __builtin_expect(!!(expr), 1)
#else
#define UNLIKELY(expr) (expr)
#define LIKELY(expr)   (expr)
#endif
#endif /* UNLIKELY */

/** Check a condition and if not met return a specific value.
 *
 * @param cond The condition to check.
 * @param val The return value if the check fails.
 */
#define CHKRET(cond, val) \
    if (!LIKELY(cond))    \
    {                     \
        return val;       \
    }
#ifndef CHKVOID
/// Return from void functions if condition fails.
#define CHKVOID(cond) CHKRET(cond, )
#endif /* CHKVOID */
#ifndef CHKNULL
/// Return a null pointer if condition fails.
#define CHKNULL(cond) CHKRET(cond, NULL)
#endif /* CHKNULL */
/// Return false if condition fails.
#define CHKFALSE(cond) CHKRET(cond, false)
/// Return the error value 1 if condition fails.
#define CHKERR1(cond) CHKRET(cond, 1)
/** Check a value for non-zero and return that value.
 * @warning The parameter is evaluated twice so should be a simple variable.
 *
 * @param value The value to check and conditionally return.
 */
#define CHKERRVAL(value) CHKRET(!value, value)

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_UTIL_H_ */
