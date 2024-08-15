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
/** @file
 * @ingroup ari
 * This file contains the definitions, prototypes, constants, and
 * other information necessary for the identification and
 * processing of AMM Resource Identifiers (ARIs). Every object in
 * the AMM can be uniquely identified using an ARI.
 */
#ifndef ARI_ARI_CONTAINERS_H_
#define ARI_ARI_CONTAINERS_H_

#include "ari_base.h"
#include "ari_access.h"
#include "ari_algo.h"
#include <m-array.h>
#include <m-bptree.h>
#include <m-deque.h>
#include <m-dict.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond Doxygen_Suppress
/// Linked list of ARI as ari_list_t
DEQUE_DEF(ari_list, ari_t)
/// Dictionary from (literal) ARI to ARI
M_DICT_DEF2(ari_dict, ari_t, ari_t)
/// Dictionary from text name to ARI
M_DICT_DEF2(named_ari_dict, string_t, M_STRING_OPLIST, ari_t, M_OPL_ari_t())
/// Tree map from (literal) ARI to ARI
BPTREE_DEF2(ari_tree, 4, ari_t, M_OPL_ari_t(), ari_t, M_OPL_ari_t())
/// Flat array of ARI for TBL
ARRAY_DEF(ari_array, ari_t)

/// @endcond

/*
 * An ARI Collection (AC) value is an ordered collection of ARI values.
 */
typedef struct ari_ac_s
{
    ari_list_t items;
} ari_ac_t;

void ari_ac_init(ari_ac_t *obj);
void ari_ac_deinit(ari_ac_t *obj);
bool ari_ac_equal(const ari_ac_t *left, const ari_ac_t *right);

/*
 * An ARI Map (AM) value is a sorted dictionary from ARI primitives to ARI values.
 */
typedef struct ari_am_s
{
    ari_dict_t items;
} ari_am_t;

void ari_am_init(ari_am_t *obj);
void ari_am_deinit(ari_am_t *obj);
bool ari_am_equal(const ari_am_t *left, const ari_am_t *right);

/*
 * A Table (TBL) value is a two-dimensional array of ARI values with
 * a fixed column count and arbitrary row count.
 */
typedef struct ari_tbl_s
{
    /// Number of columns in the table
    size_t ncols;
    /// Row-major array of values
    ari_array_t items;
} ari_tbl_t;

void ari_tbl_init(ari_tbl_t *obj, size_t ncols, size_t nrows);
void ari_tbl_deinit(ari_tbl_t *obj);
bool ari_tbl_equal(const ari_tbl_t *left, const ari_tbl_t *right);

int ari_tbl_move_row(ari_tbl_t *obj, ari_ac_t *row);

/*
 * An Execution Set (EXECSET) value is an ordered collection of
 * execution target ARIs (literal macro or reference to CTRL or
 * macro-producing object).
 */
typedef struct ari_execset_s
{
    /// Nonce stored as an ARI
    ari_t nonce;
    /// Targets to execute
    ari_list_t targets;
} ari_execset_t;

void ari_execset_init(ari_execset_t *obj);
void ari_execset_deinit(ari_execset_t *obj);
bool ari_execset_equal(const ari_execset_t *left, const ari_execset_t *right);

/** A report is always a member of a Reporting Set (see ari_rptset_s).
 */
typedef struct ari_report_s
{
    /// Relative time within the reporting-set
    ari_t reltime;
    /// Source of this report data
    ari_t source;
    /// Items of the report itself
    ari_list_t items;
} ari_report_t[1];

void   ari_report_init(ari_report_t obj);
void   ari_report_init_copy(ari_report_t obj, const ari_report_t src);
void   ari_report_init_move(ari_report_t obj, ari_report_t src);
void   ari_report_deinit(ari_report_t obj);
void   ari_report_set_copy(ari_report_t obj, const ari_report_t src);
void   ari_report_set_move(ari_report_t obj, ari_report_t src);
size_t ari_report_hash(const ari_report_t obj);
bool   ari_report_equal(const ari_report_t left, const ari_report_t right);

#define ARI_REPORT_OPLIST                                                                                            \
    M_OPEXTEND(M_A1_OPLIST, INIT(ari_report_init), INIT_SET(ari_report_init_copy), INIT_MOVE(ari_report_init_move),  \
               CLEAR(ari_report_deinit), SET(ari_report_set_copy), MOVE(ari_report_set_move), HASH(ari_report_hash), \
               EQUAL(ari_report_equal))

/// @cond Doxygen_Suppress
/// Linked list of reports
DEQUE_DEF(ari_report_list, ari_report_t, ARI_REPORT_OPLIST)
/// @endcond

/*
 * An Reporting Set (RPTSET) value is an ordered collection of
 * reports, each of which is a timestamped list of ARI items with a specific
 * source ARI.
 */
typedef struct ari_rptset_s
{
    /// Nonce stored as an ARI
    ari_t nonce;
    /// Reference absolute time
    ari_t reftime;

    /// Reports in this set
    ari_report_list_t reports;
} ari_rptset_t;

void ari_rptset_init(ari_rptset_t *obj);
void ari_rptset_deinit(ari_rptset_t *obj);
bool ari_rptset_equal(const ari_rptset_t *left, const ari_rptset_t *right);

/** Helper to assign a new container to a literal ARI.
 *
 * @param[in,out] The literal value to modify.
 * @param ctype The container type to create.
 */

void ari_lit_init_container(ari_lit_t *lit, ari_type_t ctype);

#ifdef __cplusplus
}
#endif

#endif /* ARI_ARI_CONTAINERS_H_ */
