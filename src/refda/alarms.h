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
#ifndef REFDA_ALARMS_H_
#define REFDA_ALARMS_H_

#include "amm/ident.h"
#include "cace/ari/base.h"
#include <m-atomic.h>
#include <m-deque.h>
#include <m-rbtree.h>
#include <m-bptree.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Enumeration of alarm severity levels.
 * Descriptions are copied from the ADM.
 */
typedef enum
{
    REFDA_ALARMS_SEVERITY_CLEARED = 0,
} refda_alarms_severity_t;

/** A single entry of the alarm list.
 *
 */
typedef struct
{
    /// Resource reference
    refda_amm_ident_base_t resource;
    /// Optional category reference
    refda_amm_ident_base_t category;
    /// Current severity
    int severity;

    /// Added-at timestamp
    cace_ari_t added_at;
    /// Updated-at timestamp
    cace_ari_t updated_at;

    // TODO add history storage

    /// Manager state
    int mgr_state;
    /// Associated manager identity
    cace_ari_t mgr_ident;
    /// Manager state timestamp
    cace_ari_t mgr_state_at;
} refda_alarms_entry_t;

void refda_alarms_entry_init(refda_alarms_entry_t *obj);

void refda_alarms_entry_deinit(refda_alarms_entry_t *obj);

int refda_alarms_entry_cmp(const refda_alarms_entry_t *left, const refda_alarms_entry_t *right);

#define M_OPL_refda_alarms_entry_t() \
    (INIT(API_2(refda_alarms_entry_init)), CLEAR(API_2(refda_alarms_entry_deinit)), CMP(API_6(refda_alarms_entry_cmp)))

/** @struct refda_alarms_entry_list_t
 * An ordered list of ::refda_alarms_entry_t instances.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_RBTREE_DEF(refda_alarms_entry_list, refda_alarms_entry_t)
// GCOV_EXCL_STOP
/// @endcond

/** Storage of the agent alarms list and shelving config.
 */
typedef struct
{
    /** All groups configured in the Agent.
     */
    refda_alarms_entry_list_t alarms;

} refda_alarms_t;

void refda_alarms_init(refda_alarms_t *obj);

void refda_alarms_deinit(refda_alarms_t *obj);

// forward declared
typedef struct refda_agent_s refda_agent_t;

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_ALARMS_H_ */
