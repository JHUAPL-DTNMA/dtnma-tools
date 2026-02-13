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
/** @file
 * @ingroup ari
 * This file contains definitions for ARI text file functions.
 */
#ifndef CACE_ARI_MACROFILE_H_
#define CACE_ARI_MACROFILE_H_

#include "base.h"
#include "containers.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARI_TEXT_PARSE)

/** Read lines from a file as text ARIs into a list.
 * The file must be encoded as text/uri-list media type according to RFC 2483.
 *
 * @param[in] file The file to read lines from.
 * @param[out] list The list to push into.
 * @return Zero upon success.
 */
int cace_ari_macrofile_read(FILE *file, cace_ari_list_t items);

#endif /* ARI_TEXT_PARSE */

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_MACROFILE_H_ */
