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
 * This file contains the definitions, prototypes, constants, and
 * other information necessary for the identification and
 * processing of AMM Resource Identifiers (ARIs). Every object in
 * the AMM can be uniquely identified using an ARI.
 */
#ifndef CACE_ARI_CONTAINERS_H_
#define CACE_ARI_CONTAINERS_H_

#include "base.h"
#include "algo.h"
#include <m-array.h>
#include <m-bptree.h>
#include <m-deque.h>
#include <m-dict.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct cace_ari_list_t
 * Linked list of cace_ari_t items.
 * This is used for AC literal type.
 */
/** @struct cace_ari_dict_t
 * Dictionary from (literal only) cace_ari_t to cace_ari_t items.
 */
/** @struct named_ari_dict_t
 * Dictionary from ::string_t text name to cace_ari_t items.
 */
/** @struct cace_ari_tree_t
 * Tree map from (literal only) cace_ari_t to cace_ari_t items.
 * This is used for AM literal type.
 */
/** @struct cace_ari_array_t
 * Flat array of cace_ari_t.
 * This is used for TBL literal type.
 */
/** @struct cace_ari_ptr_array_t
 * Flat array of ::cace_ari_t pointers.
 * This is used for referencing values without copying.
 */
/** @struct cace_ari_cptr_array_t
 * Flat array of ::cace_ari_t pointers.
 * This is used for referencing values without copying.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_DEQUE_DEF(cace_ari_list, cace_ari_t)
M_DICT_DEF2(cace_ari_dict, cace_ari_t, M_OPL_cace_ari_t(), cace_ari_t, M_OPL_cace_ari_t())
M_DICT_DEF2(cace_named_ari_dict, m_string_t, M_STRING_OPLIST, cace_ari_t, M_OPL_cace_ari_t())
M_BPTREE_DEF2(cace_ari_tree, 4, cace_ari_t, M_OPL_cace_ari_t(), cace_ari_t, M_OPL_cace_ari_t())
M_ARRAY_DEF(cace_ari_array, cace_ari_t)
M_ARRAY_DEF(cace_ari_ptr_array, cace_ari_t *, M_PTR_OPLIST)
M_ARRAY_DEF(cace_ari_cptr_array, const cace_ari_t *, M_PTR_OPLIST)
// GCOV_EXCL_STOP
/// @endcond

/*
 * An ARI Collection (AC) value is an ordered collection of ARI values.
 */
typedef struct cace_ari_ac_s
{
    /// Collection items
    cace_ari_list_t items;
} cace_ari_ac_t;

void cace_ari_ac_init(cace_ari_ac_t *obj);
void cace_ari_ac_deinit(cace_ari_ac_t *obj);
int  cace_ari_ac_cmp(const cace_ari_ac_t *left, const cace_ari_ac_t *right);
bool cace_ari_ac_equal(const cace_ari_ac_t *left, const cace_ari_ac_t *right);

/*
 * An ARI Map (AM) value is a sorted dictionary from ARI primitives to ARI values.
 */
typedef struct cace_ari_am_s
{
    /// Map items
    cace_ari_tree_t items;
} cace_ari_am_t;

void cace_ari_am_init(cace_ari_am_t *obj);
void cace_ari_am_deinit(cace_ari_am_t *obj);
int  cace_ari_am_cmp(const cace_ari_am_t *left, const cace_ari_am_t *right);
bool cace_ari_am_equal(const cace_ari_am_t *left, const cace_ari_am_t *right);

/*
 * A Table (TBL) value is a two-dimensional array of ARI values with
 * a fixed column count and arbitrary row count.
 */
typedef struct cace_ari_tbl_s
{
    /// Number of columns in the table
    size_t ncols;
    /// Row-major array of all values in the table
    cace_ari_array_t items;
} cace_ari_tbl_t;

void cace_ari_tbl_init(cace_ari_tbl_t *obj);
void cace_ari_tbl_deinit(cace_ari_tbl_t *obj);
bool cace_ari_tbl_cmp(const cace_ari_tbl_t *left, const cace_ari_tbl_t *right);
bool cace_ari_tbl_equal(const cace_ari_tbl_t *left, const cace_ari_tbl_t *right);

/** Reset a table to a specific size.
 *
 * @param[in,out] obj The table to append to.
 * @param ncols The number of columns.
 * @param nrows The number of rows, which can start zero and be appended to.
 * @sa cace_ari_tbl_move_row_ac(), cace_ari_tbl_move_row_array()
 */
void cace_ari_tbl_reset(cace_ari_tbl_t *obj, size_t ncols, size_t nrows);

/** Compute the number of rows in this table.
 *
 * @param[in] obj The table to inspect.
 * @return The row count.
 */
size_t cace_ari_tbl_num_rows(const cace_ari_tbl_t *obj);

/** Append a row to a table, moving data from the source.
 *
 * @param[in,out] obj The table to append to.
 * @param[in,out] row The row to move items from.
 * @return Zero if successful.
 * 2 if the row size is mismatched.
 */
int cace_ari_tbl_move_row_ac(cace_ari_tbl_t *obj, cace_ari_ac_t *row);
/// @overload
int cace_ari_tbl_move_row_array(cace_ari_tbl_t *obj, cace_ari_array_t row);

/*
 * An Execution Set (EXECSET) value is an ordered collection of
 * execution target ARIs (literal macro or reference to CTRL or
 * macro-producing object).
 */
typedef struct cace_ari_execset_s
{
    /// Nonce stored as an ARI
    cace_ari_t nonce;
    /// Targets to execute
    cace_ari_list_t targets;
} cace_ari_execset_t;

void cace_ari_execset_init(cace_ari_execset_t *obj);
void cace_ari_execset_deinit(cace_ari_execset_t *obj);
int  cace_ari_execset_cmp(const cace_ari_execset_t *left, const cace_ari_execset_t *right);
bool cace_ari_execset_equal(const cace_ari_execset_t *left, const cace_ari_execset_t *right);

/** A report is always a member of a Reporting Set (see ::cace_ari_rptset_s).
 */
typedef struct cace_ari_report_s
{
    /// Relative time within the reporting-set
    cace_ari_t reltime;
    /// Source of this report data
    cace_ari_t source;
    /// Items of the report itself
    cace_ari_list_t items;
} cace_ari_report_t;

void   cace_ari_report_init(cace_ari_report_t *obj);
void   cace_ari_report_init_copy(cace_ari_report_t *obj, const cace_ari_report_t *src);
void   cace_ari_report_init_move(cace_ari_report_t *obj, cace_ari_report_t *src);
void   cace_ari_report_deinit(cace_ari_report_t *obj);
void   cace_ari_report_set_copy(cace_ari_report_t *obj, const cace_ari_report_t *src);
void   cace_ari_report_set_move(cace_ari_report_t *obj, cace_ari_report_t *src);
size_t cace_ari_report_hash(const cace_ari_report_t *obj);
int    cace_ari_report_cmp(const cace_ari_report_t *left, const cace_ari_report_t *right);
bool   cace_ari_report_equal(const cace_ari_report_t *left, const cace_ari_report_t *right);

#define M_OPL_cace_ari_report_t()                                                                                    \
    (INIT(API_2(cace_ari_report_init)), INIT_SET(API_6(cace_ari_report_init_copy)),                                  \
     INIT_MOVE(API_6(cace_ari_report_init_move)), CLEAR(API_2(cace_ari_report_deinit)),                              \
     SET(API_6(cace_ari_report_set_copy)), MOVE(API_6(cace_ari_report_set_move)), HASH(API_2(cace_ari_report_hash)), \
     CMP(API_6(cace_ari_report_cmp)), EQUAL(API_6(cace_ari_report_equal)))

/// @cond Doxygen_Suppress
/// Linked list of reports
M_DEQUE_DEF(cace_ari_report_list, cace_ari_report_t)
/// @endcond

/*
 * An Reporting Set (RPTSET) value is an ordered collection of
 * reports, each of which is a timestamped list of ARI items with a specific
 * source ARI.
 */
typedef struct cace_ari_rptset_s
{
    /// Nonce stored as an ARI
    cace_ari_t nonce;
    /// Reference absolute time
    cace_ari_t reftime;

    /// Reports in this set
    cace_ari_report_list_t reports;
} cace_ari_rptset_t;

void cace_ari_rptset_init(cace_ari_rptset_t *obj);
void cace_ari_rptset_deinit(cace_ari_rptset_t *obj);
int  cace_ari_rptset_cmp(const cace_ari_rptset_t *left, const cace_ari_rptset_t *right);
bool cace_ari_rptset_equal(const cace_ari_rptset_t *left, const cace_ari_rptset_t *right);

/** Combine multiple RPTSET into a single value.
 * Each of the source values must have identical nonce parameters.
 *
 * @param[in,out] out The result to combine into.
 * @param[in,out] src The source list to move values from.
 * @return Zero if successful.
 */
int cace_ari_rptset_join_move(cace_ari_rptset_t *out, cace_ari_list_t *src);

/** Split a single RPTSET into a list where each result contains exactly
 * one report.
 *
 * @return Zero if successful.
 */
int cace_ari_rptset_split_move(cace_ari_list_t *out, cace_ari_rptset_t *src);

/** Helper to assign a new container to a literal ARI.
 *
 * @param[in,out] The literal value to modify.
 * @param ctype The container type to create.
 */

void cace_ari_lit_init_container(cace_ari_lit_t *lit, cace_ari_type_t ctype);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_CONTAINERS_H_ */
