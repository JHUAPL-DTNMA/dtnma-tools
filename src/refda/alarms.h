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
#ifndef REFDA_ALARMS_H_
#define REFDA_ALARMS_H_

#include "runctx.h"
#include "amm/ident.h"
#include "cace/ari/base.h"
#include <m-array.h>
#include <m-dict.h>
#include <pthread.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Enumeration of alarm severity levels.
 * Descriptions are copied from the ADM.
 */
typedef enum
{
    /**
     * There is currently no specific alarm condition on
     * a resource.
     */
    REFDA_ALARMS_SEVERITY_CLEARED = 0,
    /**
     * This indicates that the severity level cannot be
     * determined.
     */
    REFDA_ALARMS_SEVERITY_INDETERMINATE = 1,
    /**
     * This indicates the detection of a potential or
     * impending service affecting fault, before any
     * significant effects have been felt.
     */
    REFDA_ALARMS_SEVERITY_WARNING = 2,
    /**
     * This indicates the existence of a non-service
     * affecting fault condition and that corrective action
     * should be taken in order to prevent a more serious
     * (for example, service affecting) fault.
     */
    REFDA_ALARMS_SEVERITY_MINOR = 3,
    /**
     * This indicates that a service affecting condition has
     * developed and an urgent corrective action is required.
     */
    REFDA_ALARMS_SEVERITY_MAJOR = 4,
    /**
     * This indicates that a service affecting condition has
     * occurred and an immediate corrective action is
     * required.
     */
    REFDA_ALARMS_SEVERITY_CRITICAL = 5,
} refda_alarms_severity_t;

/** Enumeration of alarm entry manager state.
 * Descriptions are copied from the ADM.
 */
typedef enum
{
    /**
     * The alarm is not being taken care of.
     */
    REFDA_ALARMS_MGR_STATE_NONE = 0,
    /**
     * The alarm is being taken care of.
     */
    REFDA_ALARMS_MGR_STATE_ACK = 1,
    /**
     * Corrective action taken successfully.
     */
    REFDA_ALARMS_MGR_STATE_CLOSED = 2,
    /**
     * The alarm is shelved and moved out of the table.
     */
    REFDA_ALARMS_MGR_STATE_SHELVED = 3,
} refda_alarms_mgr_state_t;

/** Each item of an alarm entry history.
 */
typedef struct
{
    /// Timestamp of this update
    cace_ari_t timestamp;
    /// Severity at this update
    refda_alarms_severity_t severity;
} refda_alarms_history_item_t;

void refda_alarms_history_item_init(refda_alarms_history_item_t *obj);

void refda_alarms_history_item_deinit(refda_alarms_history_item_t *obj);

/// M*LIB oplist for ::refda_alarms_history_item_t
#define M_OPL_refda_alarms_history_item_t() \
    (INIT(API_2(refda_alarms_history_item_init)), CLEAR(API_2(refda_alarms_history_item_deinit)))

/** @struct refda_alarms_history_list_t
 * An ordered list of ::refda_alarms_history_item_t instances.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_DEQUE_DEF(refda_alarms_history_list, refda_alarms_history_item_t)
// GCOV_EXCL_STOP
/// @endcond

/** A single entry of the alarm list.
 */
typedef struct
{
    /// Resource reference
    refda_amm_ident_base_t resource;
    /// Optional category reference
    refda_amm_ident_base_t category;

    /// Current severity
    refda_alarms_severity_t severity;

    /// Added-at timestamp
    cace_ari_t created_at;
    /// Updated-at timestamp
    cace_ari_t updated_at;

    // History storage, ordered with newest first
    refda_alarms_history_list_t history;

    /// Manager state
    refda_alarms_mgr_state_t mgr_state;
    /// Associated manager identity
    cace_ari_t mgr_ident;
    /// Manager state timestamp
    cace_ari_t mgr_time;
} refda_alarms_entry_t;

void refda_alarms_entry_init(refda_alarms_entry_t *obj);

void refda_alarms_entry_deinit(refda_alarms_entry_t *obj);

/// M*LIB oplist for ::refda_alarms_entry_t
#define M_OPL_refda_alarms_entry_t() (INIT(API_2(refda_alarms_entry_init)), CLEAR(API_2(refda_alarms_entry_deinit)))

/** Search key for an alarm entry.
 */
typedef struct
{
    /// Non-null pointer to an IDENT object
    const struct refda_amm_ident_desc_s *resource;
    /// Optional pointer to an IDENT object
    const struct refda_amm_ident_desc_s *category;
} refda_alarms_entry_key_t;

int refda_alarms_entry_key_cmp(const refda_alarms_entry_key_t *left, const refda_alarms_entry_key_t *right);

/// M*LIB oplist for ::refda_alarms_entry_key_t
#define M_OPL_refda_alarms_entry_key_t() M_OPEXTEND(M_POD_OPLIST, CMP(API_6(refda_alarms_entry_cmp)))

/** @struct refda_alarms_entry_ptr_t
 * A non-thread-safe shared pointer to a ::refda_alarms_entry_t instance.
 */
/** @struct refda_alarms_entry_list_t
 * An ordered list of pointers to ::refda_alarms_entry_t instances.
 */
/** @struct refda_alarms_entry_list_t
 * A lookup from unique key to ::refda_alarms_entry_t pointer.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_SHARED_WEAK_PTR_DEF(refda_alarms_entry_ptr, refda_alarms_entry_t)
M_ARRAY_DEF(refda_alarms_entry_list, refda_alarms_entry_ptr_t *,
            M_SHARED_PTR_OPLIST(refda_alarms_entry_ptr, M_OPL_refda_alarms_entry_t()))
M_DICT_DEF2(refda_alarms_entry_index, refda_alarms_entry_key_t, M_OPL_refda_alarms_entry_key_t(),
            refda_alarms_entry_t *, M_PTR_OPLIST)
// GCOV_EXCL_STOP
/// @endcond

/** Storage of the agent alarms list and shelving config.
 */
typedef struct
{
    /** All alarm entries.
     */
    refda_alarms_entry_list_t alarm_list;
    /// Index by key (resource,category)
    refda_alarms_entry_index_t alarm_index;
    /// Mutex for the state of #alarm_list and #alarm_index
    pthread_mutex_t alarm_mutex;

} refda_alarms_t;

void refda_alarms_init(refda_alarms_t *obj);

void refda_alarms_deinit(refda_alarms_t *obj);

// forward declared
typedef struct refda_agent_s refda_agent_t;

/** Record a new alarm state in the Agent based on object references.
 *
 * @param agent The agent state to modify.
 * @param[in] resource The resource to set state on.
 * @param[in] category The optional category to set state on.
 * @param[in] severity The new severity level.
 */
void refda_alarms_set_refs(refda_agent_t *agent, const cace_ari_t *resource, const cace_ari_t *category,
                           refda_alarms_severity_t severity);

/** Get a table representing the alarm list.
 *
 * @param[in] runctx The context to produce under.
 * @param[out] out The already-initialized value to store into.
 * @return Zero if successful.
 */
int refda_alarms_get_table(refda_runctx_t *runctx, cace_ari_t *out);

/** Purge the list of alarms based on filtered entries.
 *
 * @param[in] runctx The context to evaluate expressions and the agent state to modify.
 * @param[in] filter The expression to LABEL-substitute and evaluate for each alarm entry.
 * @return The number of affected entries.
 */
size_t refda_alarms_purge(refda_runctx_t *runctx, const cace_ari_t *filter);

/** Compress the history on on filtered entries.
 *
 * @param[in] runctx The context to evaluate expressions and the agent state to modify.
 * @param[in] filter The expression to LABEL-substitute and evaluate for each alarm entry.
 */
size_t refda_alarms_compress(refda_runctx_t *runctx, const cace_ari_t *filter);

/** Set the manager state on on filtered entries.
 *
 * @param[in] runctx The context to evaluate expressions and the agent state to modify.
 * @param[in] filter The expression to LABEL-substitute and evaluate for each alarm entry.
 * @param state The new state.
 */
size_t refda_alarms_mgr_state(refda_runctx_t *runctx, const cace_ari_t *filter, refda_alarms_mgr_state_t state);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_ALARMS_H_ */
