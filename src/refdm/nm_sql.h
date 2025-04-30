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
 * Transmit enumerations govern the state associated with messages stored in
 * the database to be sent to an agent. Given that the database may have
 * multiple writers, these states serve as a synchronization mechanism.
 */
#define TX_INIT  (0) /* An outgoing message group is being written to the db. */
#define TX_READY (1) /* An outgoing message group is ready to be processed. */
#define TX_PROC  (2) /* The message group is being processed. */
#define TX_SENT  (3) /* The message group has been processed and sent. */

/*
 * Receive enumerations govern the state associated with messages stored in
 * the database that have been received by an agent.
 */
#define RX_INIT  (0) /* An incoming message group is being received. */
#define RX_READY (1) /* An incoming message group is done being received. */
#define RX_PROC  (2) /* An incoming message group has been processed. */

/*
 * The DB schema uses a table of tables to identify types of information
 * stored in outgoing messages.  These enumerations capture supported
 * table identifiers.
 */

#define UNKNOWN_MSG   (0)
#define EXEC_CTRL_MSG (3)

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
void    *db_mgt_daemon(void *arg);
uint32_t db_mgt_init(refdm_db_s *parms, uint32_t clear, uint32_t log);
uint32_t db_mgt_init_con(size_t idx, refdm_db_s *parms);

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
void    db_mgt_txn_start();
// void     db_mgt_txn_commit();
void db_mgt_txn_rollback();

void    db_mgr_sql_info_deserialize(string_t *data);
string_t *db_mgr_sql_info_serialize();
void  db_process_outgoing(refdm_mgr_t *mgr);

/* Functions to process outgoing message tables. */
// int32_t db_tx_build_group(int32_t grp_idx, msg_grp_t *msg_group);
// int     db_tx_collect_agents(int32_t grp_idx, vector_t *vec);

/* Functions to process incoming messages. */
uint32_t db_incoming_initialize(refdm_mgr_t* mgr);
int32_t  db_incoming_finalize(uint32_t id, uint32_t grp_status, m_string_t src_eid, const char *raw_input);

uint32_t db_insert_msg_rpt_set(cace_ari_t *val, refdm_agent_t *agent, int *status);
uint32_t db_insert_msg_tbl(cace_ari_t *val, refdm_agent_t *agent, int *status);

refdm_agent_t *db_fetch_agent(int32_t id);
int32_t  db_fetch_agent_idx(string_t *sender);
string_t    *db_fetch_ari_col(int idx);

#if 0


/* Functions to write primitives to associated database tables. */
int32_t db_add_adm(char *name, char *version, char *oid_root);
int32_t db_add_tdc(tdc_t tdc);
int32_t db_add_mid(mid_t *mid);
int32_t db_add_mc(Lyst mc);
int32_t db_add_nn(oid_nn_t *nn);
int32_t db_add_oid(oid_t oid);
int32_t db_add_oid_str(char *oid_str);
int32_t db_add_parms(oid_t oid);
int32_t db_add_protomid(mid_t *mid, ui_parm_spec_t *spec, amp_type_e type);
int32_t db_add_protoparms(ui_parm_spec_t *spec);


/* Functions to fetch primitives from associated database tables. */
int32_t           db_fetch_adm_idx(char *name, char *version);
tdc_t             db_fetch_tdc(int32_t tdc_idx);
string_t*           db_fetch_tdc_entry_from_row(MYSQL_ROW row, amp_type_e *type);
mid_t*            db_fetch_mid(int32_t idx);
Lyst              db_fetch_mid_col(int idx);
mid_t*            db_fetch_mid_from_row(MYSQL_ROW row);
int32_t           db_fetch_mid_idx(mid_t *mid);
int32_t           db_fetch_nn(uint32_t idx);
int32_t           db_fetch_nn_idx(uint32_t nn);
uint8_t*          db_fetch_oid_val(uint32_t idx, uint32_t *size);
oid_t             db_fetch_oid(uint32_t nn_idx, uint32_t parm_idx, uint32_t oid_idx);
int32_t           db_fetch_oid_idx(oid_t oid);
Lyst			  db_fetch_parms(uint32_t idx);
int32_t           db_fetch_protomid_idx(mid_t *mid);

#endif // 0

// If set, always log CBOR-encoded inputs and outputs to DB for debug purposes.  Received reports shall always be logged
// in the event of an error.
#define DB_LOG_RAW

/** Utility function to insert debug or error informational messages into the database.
 * NOTE: If operating within a transaction, caller is responsible for committing transaction.
 **/
void db_log_msg(const char *filename, int lineno, const char *funcname, int level, size_t dbidx, const char *format, ...);


#define DB_LOG_INFO( ...) db_log_msg(__FILE__, __LINE__, __func__, LOG_INFO, __VA_ARGS__)
#define DB_LOG_WARN( ...) db_log_msg(__FILE__, __LINE__, __func__, LOG_WARNING, __VA_ARGS__)
#define DB_LOG_ERR( ...)  db_log_msg(__FILE__, __LINE__, __func__,  LOG_ERR ,__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* NM_MGR_SQL_H */


