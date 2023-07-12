/*
 * Copyright (c) 2023 The Johns Hopkins University Applied Physics
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
#ifndef NM_SHARED_PLATFORM_H_
#define NM_SHARED_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void * amp_uaddr;
typedef uint32_t amp_uint;
typedef int32_t amp_int;
typedef uint64_t amp_uvast;
typedef int64_t amp_vast;

#ifndef _PLATFORM_H_
#define CHKERR(expr) if (!(expr)) { return 1; }
#define CHKVOID(expr) if (!(expr)) { return; }
#define CHKZERO(expr) if (!(expr)) { return 0; }
#define CHKNULL(expr) if (!(expr)) { return NULL; }
#endif
#define CHKUSR(expr,usr) if (!(expr)) { return usr; }

#ifdef __cplusplus
}
#endif

#endif  /* NM_SHARED_PLATFORM_H_ */
