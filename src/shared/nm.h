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
#ifndef __NM__
#define __NM__

#ifdef __cplusplus
extern "C" {
#endif

// Preprocessor magic helper
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
/** AMP Protocol Version
 * STATUS: Currently supports version 6 and version 7/8 controllable via this define.
 * NOTICE: Backwards compatibility will be removed in the near future.
 */
#ifndef AMP_VERSION
#define AMP_VERSION 8
#endif

// IETF URL Must be 0-padded
#if AMP_VERSION < 10
#define AMP_VERSION_PAD "0" STR(AMP_VERSION)
#else
#define AMP_VERSION_PAD STR(AMP_VERSION)
#endif

#define AMP_PROTOCOL_URL_BASE "https://datatracker.ietf.org/doc/draft-birrane-dtn-amp"
#define AMP_PROTOCOL_URL      AMP_PROTOCOL_URL_BASE "/" AMP_VERSION_PAD
#define AMP_VERSION_STR       STR(AMP_VERSION) " - " AMP_PROTOCOL_URL


#ifdef __cplusplus
}
#endif

#endif  /* _NM_H_ */
