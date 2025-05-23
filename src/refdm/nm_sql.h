/*
 * Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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

#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)

#ifndef NM_MGR_SQL_H
#define NM_MGR_SQL_H

/* System Headers */
#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_MYSQL
#include <mysql.h>
#endif // HAVE_MYSQL

#ifdef HAVE_POSTGRESQL
#include <libpq-fe.h>
#endif // HAVE_POSTGRESQL

#include <cace/util/defs.h>
#include <cace/ari.h>
#include <cace/amm/msg_if.h>
#include "agents.h"
#include "mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * +--------------------------------------------------------------------------+
 * |							  CONSTANTS +
 * +--------------------------------------------------------------------------+
 */

/*
 * Constants relating to how long to try and reconnect to the DB when
 * a connection has failed.
 */

#define SQL_RECONN_TIME_MSEC 500
#define SQL_CONN_TRIES       10
#define SQL_MAX_QUERY        8192

/*
 * +--------------------------------------------------------------------------+
 * |							  	MACROS
 * +
 * +--------------------------------------------------------------------------+
 */

#define CHKCONN db_mgt_connected();

/*
 * +--------------------------------------------------------------------------+
 * |							  DATA TYPES +
 * +--------------------------------------------------------------------------+
 */

/*
 * +--------------------------------------------------------------------------+
 * |						  FUNCTION PROTOTYPES +
 * +--------------------------------------------------------------------------+
 */

/* Functions to write primitives to associated database tables. */
int32_t db_add_agent(const cace_data_t *agent_eid);

/* Database Management Functions. */
uint32_t db_mgt_init(refdm_db_t *parms, uint32_t clear, uint32_t log);
uint32_t db_mgt_init_con(size_t idx, refdm_db_t *parms);

void db_mgt_close();
void db_mgt_close_conn(size_t i);
int  db_mgt_connected(size_t i);
#ifdef HAVE_MYSQL
int32_t db_mgt_query_fetch(MYSQL_RES **res, char *format, ...);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
int32_t db_mgt_query_fetch(PGresult **res, char *format, ...);
#endif // HAVE_POSTGRESQL
int32_t db_mgt_query_insert(uint32_t *idx, char *format, ...);

/* Functions to process outgoing  tables and reprots. */
uint32_t db_insert_msg_rpt_set(cace_ari_t *val, refdm_agent_t *agent, int *status);
uint32_t db_insert_msg_tbl(cace_ari_t *val, refdm_agent_t *agent, int *status);

refdm_agent_t *db_fetch_agent(int32_t id);
int32_t        db_fetch_agent_idx(string_t *sender);
string_t      *db_fetch_ari_col(int idx);

// If set, always log CBOR-encoded inputs and outputs to DB for debug purposes.  Received reports shall always be logged
// in the event of an error.
#define DB_LOG_RAW

/** Utility function to insert debug or error informational messages into the database.
 * NOTE: If operating within a transaction, caller is responsible for committing transaction.
 **/
void db_log_msg(const char *filename, int lineno, const char *funcname, int level, size_t dbidx, const char *format,
                ...);

#define DB_LOG_INFO(...) db_log_msg(__FILE__, __LINE__, __func__, LOG_INFO, __VA_ARGS__)
#define DB_LOG_WARN(...) db_log_msg(__FILE__, __LINE__, __func__, LOG_WARNING, __VA_ARGS__)
#define DB_LOG_ERR(...)  db_log_msg(__FILE__, __LINE__, __func__, LOG_ERR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* NM_MGR_SQL_H */

#endif // HAVE_MYSQL
