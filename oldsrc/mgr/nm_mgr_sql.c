/*
 * Copyright (c) 2012-2023 The Johns Hopkins University Applied Physics
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
/*****************************************************************************
 ** \file nm_mgr_sql.c
 **
 ** File Name: nm_mgr_sql.c
 **
 **
 ** Subsystem:
 **          Network Manager Daemon: Database Utilities
 **
 ** Description: This file implements a SQL interface to the ION AMP manager.
 **
 ** Notes:
 ** 	This software assumes that there are no other applications modifying
 ** 	the AMP database tables.
 **
 ** 	These functions do not, generally, rollback DB writes on error.
 ** 	\todo: Add transactions.
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  07/10/13  S. Jacobs      Initial Implementation (JHU/APL)
 **  08/19/13  E. Birrane     Documentation clean up and code review comments. (JHU/APL)
 **  08/22/15  E. Birrane     Updates for new schema and dynamic user permissions. (Secure DTN - NASA: NNX14CS58P)
 **  01/24/17  E. Birrane     Updates to latest AMP IOS 3.5.0 (JHU/APL)
 **  10/20/18  E. Birrane     Updates for AMPv0.5 (JHU/APL)
 *****************************************************************************/

#if defined(HAVE_MYSQL) || 	defined(HAVE_POSTGRESQL)

#include <string.h>
#include <osapi-task.h>
#include <arpa/inet.h>

#include "nmmgr.h"
#include "nm_mgr_sql.h"

/* Number of threads interacting with the database.
 - DB Polling Thread - Check for reports pending transmission
 - Mgr Report Rx Thread - Log received reports
 - UI - If we add UI functions to query the DB, a separate connection will be needed.
*/
typedef enum db_con_t {
	DB_CTRL_CON, // Primary connection for receiving outgoing controls from database
	DB_RPT_CON, // Primary connection associated with Mgr rx thread. All activities in this thread will execute within transactions.
	MGR_NUM_SQL_CONNECTIONS
} db_con_t;

/* Global connection to the MYSQL Server. */
#ifdef HAVE_MYSQL
static MYSQL *gConn[MGR_NUM_SQL_CONNECTIONS];
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static PGconn *gConn[MGR_NUM_SQL_CONNECTIONS];
#endif // HAVE_POSTGRESQL
 static sql_db_t gParms;
 static uint8_t gInTxn;
 int db_log_always = 1; // If set, always log raw CBOR of incoming messages for debug purposes, otherwise log errors only. TODO: Add UI or command-line option to change setting at runtime
 
 // Private functions
#ifdef HAVE_MYSQL
 static MYSQL_STMT* db_mgr_sql_prepare(size_t idx, const char* query);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static char* db_mgr_sql_prepare(size_t idx, const char* query, char *stmtName, int nParams, const Oid *paramTypes);
#endif // HAVE_POSTGRESQL
void db_process_outgoing(nmmgr_t *mgr);
ac_t* db_query_ac(size_t dbidx, int ac_id);
int db_query_tnvc(size_t dbidx, int tnvc_id, tnvc_t *parms);
uint32_t db_insert_tnvc(db_con_t dbidx, tnvc_t *tnvc, int *status);
uint32_t db_insert_tnvc_params(db_con_t dbidx, uint32_t fp_spec_id, tnvc_t *tnvc, int *status);

/* Prepared query definitions
 *
 * This enumeration provides a consistent interface for selecting a
 * given query.  For simplicity, the same list of queries are used
 * across all database connections/threads, though most queries are
 * only utilized by a particular thread at this time.  This listing
 * may include some queries not being used at this time.
 */
enum queries {
	AC_CREATE = 0,
	AC_INSERT,
	AC_GET,
	ARI_GET,
	ARI_GET_META,
	ARI_INSERT_CTRL, // TODO: Additional variants may be needed for other ARI types
	TNVC_CREATE,
	TNVC_VIEW,
	TNVC_INSERT_ARI,
	TNVC_INSERT_AC,
	TNVC_INSERT_TNVC,

	TNVC_INSERT_STR,
	TNVC_INSERT_BOOL,
	TNVC_INSERT_BYTE,
	TNVC_INSERT_INT,
	TNVC_INSERT_UINT,
	TNVC_INSERT_VAST,
	TNVC_INSERT_TV,
	TNVC_INSERT_TS,
	TNVC_INSERT_UVAST,
	TNVC_INSERT_REAL32,
	TNVC_INSERT_REAL64,
	
	TNVC_PARMSPEC_INSERT_AC, // TODO: Is this deprecated?
	TNVC_PARMSPEC_INSERT_TNVC,
	TNVC_PARMSPEC_CREATE,
	TNVC_ENTRIES,
	MSGS_GET,
	MSGS_GET_AGENTS,
	MSG_GET_AGENTS,
	MSGS_UPDATE_GROUP_STATE,
	MSGS_ENTRIES_GET,
	MSGS_ENTRIES_GET_AGENTS,
	MSGS_OUTGOING_GET,
	MSGS_OUTGOING_CREATE,
	MSGS_INCOMING_GET,
	MSGS_INCOMING_CREATE,
	MSGS_AGENT_GROUP_ADD_NAME,
//	MSGS_AGENT_GROUP_ADD_ID,
	MSGS_AGENT_MSG_ADD,
	MSGS_ADD_REPORT_SET_ENTRY,
	MSGS_REGISTER_AGENT_INSERT,
	MSGS_REGISTER_AGENT_GET,
	MSGS_PERF_CTRL_INSERT,
	MSGS_PERF_CTRL_GET,
	MSGS_REPORT_SET_INSERT,
	MSGS_REPORT_SET_GET,
//	RPT_CREATE,
//	RPT_GET,

	DB_LOG_MSG,
	MGR_NUM_QUERIES
};

#ifdef HAVE_MYSQL
 static MYSQL_STMT* queries[MGR_NUM_SQL_CONNECTIONS][MGR_NUM_QUERIES];
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static char* queries[MGR_NUM_SQL_CONNECTIONS][MGR_NUM_QUERIES];
#endif // HAVE_POSTGRESQL
 
 /******** SQL Utility Macros ******************/
#ifdef HAVE_MYSQL
 #define dbprep_bind_res_cmn(idx,var,type) \
 	bind_res[idx].buffer_type = type; \
 	bind_res[idx].buffer = (char*)var; \
    bind_res[idx].is_null = &is_null[idx];        \
    bind_res[idx].error = &is_err[idx];
#endif // HAVE_MYSQL

#ifdef HAVE_MYSQL
 #define dbprep_bind_param_cmn(idx,var,type) \
 	bind_param[idx].buffer_type = type; \
 	bind_param[idx].buffer = (char*)&var; \
     bind_param[idx].is_null = 0;          \
     bind_param[idx].error = 0;
#endif // HAVE_MYSQL
 
#ifdef HAVE_POSTGRESQL
static void double_to_nbo(double in, double *out) {
    uint64_t *i = (uint64_t *)&in;
    uint32_t *r = (uint32_t *)out;
 
    /* convert input to network byte order */
    r[0] = htonl((uint32_t)((*i) >> 32));
    r[1] = htonl((uint32_t)*i);
}
static void vast_to_nbo(amp_vast in, amp_vast *out) {
    uint64_t *i = (uint64_t *)&in;
    uint32_t *r = (uint32_t *)out;

    /* convert input to network byte order */
    r[0] = htonl((uint32_t)((*i) >> 32));
    r[1] = htonl((uint32_t)*i);
}
#endif // HAVE_POSTGRESQL


#ifdef HAVE_MYSQL
#define dbprep_bind_param_bool(idx,var) dbprep_bind_param_cmn(idx,var,MYSQL_TYPE_LONG);
 #define dbprep_bind_param_int(idx,var) dbprep_bind_param_cmn(idx,var,MYSQL_TYPE_LONG);
 #define dbprep_bind_param_short(idx,var) dbprep_bind_param_cmn(idx,var,MYSQL_TYPE_SHORT);
 #define dbprep_bind_param_float(idx,var) dbprep_bind_param_cmn(idx,var,MYSQL_TYPE_FLOAT);
 #define dbprep_bind_param_double(idx,var) dbprep_bind_param_cmn(idx,var,MYSQL_TYPE_DOUBLE);
#define dbprep_bind_param_bigint(idx,var) dbprep_bind_param_cmn(idx,var,MYSQL_TYPE_LONGLONG);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define dbprep_bind_param_bool(idx,var) 									\
	net8Vals[idx] = (uint8_t)var;										\
	paramValues[idx] = (char *) &net8Vals[idx];								\
	paramLengths[idx] = sizeof(net8Vals[idx]);								\
	paramFormats[idx] = 1; /* binary */							
#define dbprep_bind_param_int(idx,var) 										\
	net32Vals[idx] = htonl((uint32_t) var); 								\
	paramValues[idx] = (char *) &net32Vals[idx];								\
	paramLengths[idx] = sizeof(net32Vals[idx]);								\
	paramFormats[idx] = 1; /* binary */							
#define dbprep_bind_param_short(idx,var)  									\
	net16Vals[idx] = htons((uint16_t) var); 								\
	paramValues[idx] = (char *) &net16Vals[idx];								\
	paramLengths[idx] = sizeof(net16Vals[idx]);								\
	paramFormats[idx] = 1; /* binary */							
#define dbprep_bind_param_float(idx,var) 									\
	net32Vals[idx] = htonl(* ( (uint32_t*) &var ));								\
	paramValues[idx] = (char *) &net32Vals[idx];								\
	paramLengths[idx] = sizeof(net32Vals[idx]);								\
	paramFormats[idx] = 1; /* binary */			
#define dbprep_bind_param_double(idx,var) 									\
	double_to_nbo(var, (double *) &net64Vals[idx]); 							\
	paramValues[idx] = (char *) &net64Vals[idx];								\
	paramLengths[idx] = sizeof(net64Vals[idx]);								\
	paramFormats[idx] = 1; /* binary */				
#define dbprep_bind_param_bigint(idx,var) 									\
	vast_to_nbo(var, (amp_vast *) &net64Vals[idx]); 								\
	paramValues[idx] = (char *) &net64Vals[idx];								\
	paramLengths[idx] = sizeof(net64Vals[idx]);								\
	paramFormats[idx] = 1; /* binary */				
#endif // HAVE_POSTGRESQL
 
#ifdef HAVE_MYSQL
#define dbprep_bind_param_str(idx,var) \
	size_t len_##var = (var==NULL) ? 0 : strlen(var);					\
	bind_param[idx].buffer_length = len_##var;							\
	bind_param[idx].length = &len_##var;								\
	bind_param[idx].buffer_type = MYSQL_TYPE_STRING;					\
	bind_param[idx].buffer = (char*)var;								\
    bind_param[idx].is_null = 0;										\
    bind_param[idx].error = 0;

#define dbprep_bind_param_null(idx)					  \
	bind_param[idx].buffer_type = MYSQL_TYPE_NULL;	  \
	bind_param[idx].buffer = 0;						  \
    bind_param[idx].is_null = 0;					  \
    bind_param[idx].error = 0;
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define dbprep_bind_param_str(idx,var)					\
	paramValues[idx] = var;								\
	paramLengths[idx] = 0; /* ignored for text format */\
	paramFormats[idx] = 0; /* text */
#define dbprep_bind_param_null(idx)					  	\
	paramValues[idx] = NULL;							\
	paramLengths[idx] = 0;								\
	paramFormats[idx] = 1;
#endif // HAVE_POSTGRESQL
 

#ifdef HAVE_MYSQL
#define dbprep_bind_res_int(idx,var) dbprep_bind_res_cmn(idx,&var,MYSQL_TYPE_LONG);
#define dbprep_bind_res_short(idx,var) dbprep_bind_res_cmn(idx,&var,MYSQL_TYPE_SHORT);
#define dbprep_bind_res_str(idx,var, len) dbprep_bind_res_cmn(idx,&var,MYSQL_TYPE_STRING); \
	bind_res[idx].buffer_length = len;
#define dbprep_bind_res_int_ptr(idx,var) dbprep_bind_res_cmn(idx,var,MYSQL_TYPE_LONG);

#define dbprep_dec_res_int(idx,var) int var; dbprep_bind_res_int(idx,var);
#endif // HAVE_MYSQL

#ifdef HAVE_MYSQL
/* NOTE: my_bool is replaceed with 'bool' for MySQL 8.0.1+, but is still used for MariaDB
 *  A build flag may be needed to switch between them based on My/Maria-SQL version to support both.
 */
#define dbprep_declare(dbidx,idx, params, cols)				\
	MYSQL_STMT* stmt = queries[dbidx][idx];					\
	MYSQL_BIND bind_res[cols];								\
	MYSQL_BIND bind_param[params];							\
	bool is_null[cols];										\
	bool is_err[cols];										\
	unsigned long lengths[params];							\
	memset(bind_res,0,sizeof(bind_res));					\
	memset(bind_param,0,sizeof(bind_param));       \
	int return_status;
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define dbprep_declare(dbidx,idx, params, cols)								\
	PGconn *conn = gConn[dbidx];									\
	char* stmtName = queries[dbidx][idx];								\
	int nParams = params;										\
	const char *paramValues[nParams];								\
	int paramLengths[nParams];									\
	int paramFormats[nParams];									\
    	int resultFormat = 1; /* binary results */							\
	PGresult   *res;										\
	uint8_t  net8Vals[nParams];									\
	uint16_t net16Vals[nParams];									\
	uint32_t net32Vals[nParams];									\
	uint64_t net64Vals[nParams];
#endif // HAVE_POSTGRESQL

#ifdef HAVE_POSTGRESQL
#define dbexec_prepared	res = PQexecPrepared(conn, stmtName, nParams, paramValues, paramLengths, paramFormats, resultFormat); 
#define dbtest_result(expected) ((PQresultStatus(res) == expected) ? 0 : 1)
#endif // HAVE_POSTGRESQL
 
#ifdef HAVE_MYSQL
#define DB_CHKVOID(status) if(status!=0) { query_log_err(status); return; }
#define DB_CHKINT(status) if (status!=0) { query_log_err(status); return AMP_FAIL; }
#define DB_CHKNULL(status) if(status!=0) { query_log_err(status); return NULL; }
#define DB_CHKUSR(status,usr) if(status!=0) { query_log_err(status); usr; }
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define DB_CHKVOID(status) if(status!=0) { query_log_err(status); PQclear(res); return; }
#define DB_CHKINT(status) if (status!=0) { query_log_err(status); PQclear(res); return AMP_FAIL; }
#define DB_CHKNULL(status) if(status!=0) { query_log_err(status); PQclear(res); return NULL; }
#define DB_CHKUSR(status,usr) if(status!=0) { query_log_err(status); PQclear(res); usr; }
#endif // HAVE_POSTGRESQL
 
#ifdef HAVE_MYSQL
#define query_log_err(status) AMP_DEBUG_ERR("ERROR at %s %i: %s (errno: %d)\n", __FILE__,__LINE__, mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define query_log_err(status) AMP_DEBUG_ERR("ERROR at %s %i: %s (errno: %d)\n", __FILE__,__LINE__, PQresultErrorMessage(res), status);
#endif // HAVE_POSTGRESQL

/** Utility function to insert debug or error informational messages into the database.
 * NOTE: If operating within a transaction, caller is responsible for committing transaction.
 **/
void db_logf_msg(size_t dbidx, const char* msg, const char* details, int level, const char *fun, const char* file, size_t line, ...)
{
	va_list args;
	va_start(args,line);

	char buf[1024];
	vsnprintf(buf, 1024, details, args);
	va_end(args);
	
	db_log_msg(dbidx, msg, buf, level, fun, file, line);
}
void db_log_msg(size_t dbidx, const char* msg, const char* details, int level, const char *fun, const char* file, size_t line)
{
  amp_log(level, 'd', file, line, fun, "%s \t %s",
          msg,
          ( (details == NULL) ? "" : details )
  );
	if (dbidx >= MGR_NUM_SQL_CONNECTIONS || gConn[dbidx] == NULL) {
		// DB Not connected or invalid idx
		return;
	}
	dbprep_declare(dbidx, DB_LOG_MSG, 6, 0);
	dbprep_bind_param_str(0,msg);
	dbprep_bind_param_str(1,details);
	dbprep_bind_param_int(2,level);
	dbprep_bind_param_str(3,fun);
	dbprep_bind_param_str(4,file);
	dbprep_bind_param_int(5,line);
	#ifdef HAVE_MYSQL
 	DB_CHKVOID(mysql_stmt_bind_param(stmt, bind_param));
 	DB_CHKVOID(mysql_stmt_execute(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKVOID(dbtest_result(PGRES_COMMAND_OK))
	PQclear(res);
	#endif // HAVE_POSTGRESQL
}

/******************************************************************************
 *
 * \par Function Name: db_mgt_txn_commit
 *
 * \par Commits a transaction in the database, if we are in a txn.
 *
 * \par Notes:
 *   - This function is not multi-threaded. We assume that we are the only
 *     input into the database and that there is only one "active" transaction
 *     at a time.
 *   - This function does not support nested transactions.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/

static inline void db_mgt_txn_commit(int dbidx)
{
	 	if (dbidx < MGR_NUM_SQL_CONNECTIONS && gConn[dbidx] != NULL) {
		#ifdef HAVE_MYSQL
 		mysql_commit(gConn[dbidx]);
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		PGresult *res = PQexec(gConn[dbidx], "END");
    	PQclear(res);
		#endif // HAVE_POSTGRESQL]);
	}
}



/******************************************************************************
 *
 * \par Function Name: db_incoming_initialize
 *
 * \par Returns the id of the last insert into dbtIncoming.
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] timestamp  - the generated (UNIX) timestamp
 * \param[in] sender_eid - Who sent the messages.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/07/13  S. Jacobs      Initial implementation,
 *  08/29/15  E. Birrane     Added sender EID.
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
uint32_t db_incoming_initialize(amp_tv_t timestamp, eid_t sender_eid)
{
	uint32_t rtv = 0; // Note: An ID of 0 is reserved as an error condition. MySQL will never create a new entry for this table with a value of 0. // TODO postgresql is that true for postgresql too?
	char *name = sender_eid.name;
	int64 time_stamp_seconds = OS_TimeGetTotalSeconds(timestamp.secs); 

	CHKZERO(!db_mgt_connected(DB_RPT_CON));

	dbprep_declare(DB_RPT_CON, MSGS_INCOMING_CREATE, 2, 1);
	dbprep_bind_param_int(0,time_stamp_seconds);
	dbprep_bind_param_str(1,name);
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);

	dbprep_bind_res_int(0, rtv);
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);

	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	mysql_stmt_fetch(stmt);

	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
	char *iptr = PQgetvalue(res, 0, 0);
	rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	return rtv;
}


/******************************************************************************
 *
 * \par Function Name: db_incoming_finalize
 *
 * \par Finalize processing of the incoming messages.
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] id - The incoming message group ID.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/07/13  S. Jacobs      Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_incoming_finalize(uint32_t id, uint32_t grp_status, char* src_eid, char* raw_input)
{
	// If logging is set, or status is not successful
	if (grp_status != AMP_OK || db_log_always) {
		db_log_msg(DB_RPT_CON, "Received Message Set", raw_input, grp_status,
				   src_eid, // Source is Agent EID instead of file for this record
				   NULL, // File is n/a
				   id // Override line as a debug record of associated group_id
			);
	}
	db_mgt_txn_commit(DB_RPT_CON);
	AMP_DEBUG_EXIT("db_incoming_finalize","-->%d", AMP_OK);
	return AMP_OK;
}



/******************************************************************************
 *
 * \par Function Name: db_mgt_daemon
 *
 * \par Thread to poll database for controls pending transmission.
 *
 * \par Note: In the future, alternate DB engines, such as PostgreSQL, may allow
 *   this thread to be replaced with a LISTEN/NOTIFY type push approach.
 *
 *
 * \param[in] running - Pointer to system flag to allow for clean exit.
 *
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/13/13  S. Jacobs      Initial implementation,
 *  08/29/15  E. Birrane     Only query DB if we have an active connection.
 *  04/24/16  E. Birrane     Accept global "running" flag.
 *  01/26/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

void *db_mgt_daemon(void *arg)
{
  nmmgr_t *mgr = arg;
	OS_time_t start_time, now_time;
	OS_time_t delta, remain;


	AMP_DEBUG_ALWAYS("db_mgt_daemon","Starting Manager Database Daemon",NULL);

	while (daemon_run_get(&mgr->running))
	{
	  OS_GetLocalTime(&start_time);

	  if(db_mgt_connected(DB_CTRL_CON) == 0)
	  {
	    db_process_outgoing(mgr);
	  }

	  OS_GetLocalTime(&now_time);

	  delta = OS_TimeSubtract(now_time, start_time);
	  remain = OS_TimeSubtract(OS_TimeFromTotalSeconds(2), remain);

	  // Sleep for 1 second (10^6 microsec) subtracting the processing time.
	  if((TimeCompare(remain, OS_TimeFromTotalSeconds(0)) > 0)
	      && (TimeCompare(delta, OS_TimeFromTotalSeconds(0)) > 0))
	  {
	    OS_TaskDelay(OS_TimeGetTotalMilliseconds(remain));
	  }
	}

	AMP_DEBUG_ALWAYS("db_mgt_daemon","Cleaning up Manager Database Daemon", NULL);

	db_mgt_close();

	AMP_DEBUG_ALWAYS("db_mgt_daemon","Manager Database Daemon Finished.",NULL);
	pthread_exit(NULL);
}



/******************************************************************************
 *
 * \par Function Name: db_mgt_init
 *
 * \par Initializes the gConnection to the database.
 *
 * \retval 0 Failure
 *        !0 Success
 *
 * \param[in]  server - The machine hosting the SQL database.
 * \param[in]  user - The username for the SQL database.
 * \param[in]  pwd - The password for this user.
 * \param[in]  database - The database housing the DTNMP tables.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  01/26/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
uint32_t db_mgt_init(sql_db_t parms, uint32_t clear, uint32_t log)
{
	AMP_DEBUG_ENTRY("db_mgt_init","(parms, %d)", clear);

	db_mgt_init_con(DB_CTRL_CON, parms);

	db_mgt_init_con(DB_RPT_CON, parms);

	// A mysql_commit or mysql_rollback will automatically start a new transaction as the old one is closed
	if (gConn[DB_RPT_CON] != NULL)
	{
		#ifdef HAVE_MYSQL
 		mysql_autocommit(gConn[DB_RPT_CON], 0);
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		//TODO postgresql : turn off autocommit
		#endif // HAVE_POSTGRESQL
		DB_LOG_INFO(DB_CTRL_CON, "NM Manager Connections Initialized"); 
	}
	

	AMP_DEBUG_EXIT("db_mgt_init", "-->1", NULL);
	return 1;
}

/** Initialize specified (thread-specific) SQL connection and prepared queries
 *  Prepared queries are connection specific.  While we may not use all prepared statements for all connections, initializing the same sets everywhere simplifies management.
**/
uint32_t db_mgt_init_con(size_t idx, sql_db_t parms)
{
	
	if(gConn[idx] == NULL)
	{
		#ifdef HAVE_MYSQL
		gConn[idx] = mysql_init(NULL);
		#endif // HAVE_MYSQL
		gParms = parms;
		gInTxn = 0;

		AMP_DEBUG_INFO("db_mgt_init", "(%s,%s,%s,%s)", parms.server, parms.username, parms.password, parms.database);
		#ifdef HAVE_MYSQL
		if (!mysql_real_connect(gConn[idx], parms.server, parms.username, parms.password, parms.database, 0, NULL, 0))
		{
			if (gConn[idx] != NULL)
			{
				mysql_close(gConn[idx]);
			}
			
			AMP_DEBUG_WARN("db_mgt_init", "SQL Error: %s", mysql_error(gConn[idx]));
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		gConn[idx] = PQsetdbLogin(parms.server, NULL, NULL, NULL, parms.database, parms.username, parms.password);
		if(gConn[idx] == NULL)
		{
			// if(log > 0)
      //       {
			// 	AMP_DEBUG_WARN("db_mgt_init", "SQL Error: Null connection object returned", NULL);
      //       }
		}
		else if(PQstatus(gConn[idx]) != CONNECTION_OK)
		{
			// if(log > 0)
      //       {
			// 	AMP_DEBUG_WARN("db_mgt_init", "SQL Error: %s", PQerrorMessage(gConn[idx]));
      //       }
			PQfinish(gConn[idx]);

		#endif // HAVE_POSTGRESQL
		
			gConn[idx] = NULL;  // This was previously before the log entry which is likely a mistake
			AMP_DEBUG_EXIT("db_mgt_init", "-->0", NULL);
			return 0;
		}

		AMP_DEBUG_INFO("db_mgt_init", "Connected to Database.", NULL);

		// Initialize prepared queries
		#ifdef HAVE_MYSQL
		queries[idx][AC_CREATE]         = db_mgr_sql_prepare(idx,"SELECT create_ac(?,?)"); // num_entries, use_desc
		queries[idx][AC_INSERT]         = db_mgr_sql_prepare(idx,"SELECT insert_ac_actual_entry(?,?, ?)"); // ac_id, obj_actual_definition_id, idx

		queries[idx][AC_GET] = db_mgr_sql_prepare(idx, "SELECT ace.obj_actual_definition_id "
												  "FROM ari_collection_entry ac "
												  "LEFT JOIN ari_collection_actual_entry ace ON ace.ac_entry_id=ac.ac_entry_id "
												  "WHERE ac.ac_id=? "
												  "ORDER BY ac.order_num ASC"
												  //"SELECT obj_actual_definition_id FROM vw_ac WHERE ac_id=?"
			);


		queries[idx][ARI_GET]           = db_mgr_sql_prepare(idx,"SELECT data_type_id, adm_type, adm_enum, obj_enum, tnvc_id, issuing_org FROM vw_ari WHERE obj_actual_definition_id=?");
		queries[idx][ARI_GET_META]      = db_mgr_sql_prepare(idx, "SELECT vof.obj_metadata_id, cfd.fp_spec_id "
															 "FROM vw_obj_formal_def vof "
															 "LEFT JOIN control_formal_definition cfd ON cfd.obj_formal_definition_id=vof.obj_formal_definition_id "
															 "WHERE vof.obj_enum=? AND vof.data_type_id=? AND vof.adm_enum=?"
			);
		
		queries[idx][ARI_INSERT_CTRL]   = db_mgr_sql_prepare(idx,"SELECT insert_ari_ctrl(?,?,NULL)"); // obj_metadata_id, actual_parmspec_id, description
		
		queries[idx][TNVC_CREATE]       = db_mgr_sql_prepare(idx,"SELECT create_tnvc(NULL)"); // use_desc
		queries[idx][TNVC_INSERT_AC]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_ac_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_ARI]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_obj_entry(?,NULL,NULL,?) "); // tnvc_id, obj_id		
		queries[idx][TNVC_INSERT_TNVC]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_tnvc_entry(?,NULL,NULL,?) "); // tnvc_id, tnvc_id

		queries[idx][TNVC_INSERT_STR]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_str_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_BOOL]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_bool_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_BYTE]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_byte_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_UINT]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_uint_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_VAST]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_vast_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_TV]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_tv_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_TS]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_ts_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_UVAST]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_uvast_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_REAL32]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_real32_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id
		queries[idx][TNVC_INSERT_REAL64]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_real64_entry(?,NULL,NULL,?) "); // tnvc_id, ac_id

		
		
		queries[idx][TNVC_VIEW]         = db_mgr_sql_prepare(idx,"SELECT * FROM type_name_value_collection WHERE tnvc_id = ?");
		queries[idx][TNVC_ENTRIES] = db_mgr_sql_prepare(idx,"SELECT data_type_id, int_value, uint_value, obj_value, str_value, ac_value, tnvc_value FROM vw_tnvc_entries WHERE tnvc_id = ? ORDER BY order_num ASC");
		
		queries[idx][TNVC_PARMSPEC_INSERT_AC]   = db_mgr_sql_prepare(idx,"CALL SP__insert_actual_parms_ac(?,?,?)"); // ap_spec_id, order_num, ac_id
		queries[idx][TNVC_PARMSPEC_INSERT_TNVC] = db_mgr_sql_prepare(idx,"CALL SP__insert_actual_parms_tnvc(?,?,?)"); // ap_spec_id, order_num, tnvc_id
		queries[idx][TNVC_PARMSPEC_CREATE]      = db_mgr_sql_prepare(idx,"SELECT create_actual_parmspec_tnvc(?,?,NULL)"); // formal_def_id, tnvc_id, description

		queries[idx][MSGS_GET]         = db_mgr_sql_prepare(idx,"SELECT * FROM message_group WHERE group_id=?");
		queries[idx][MSG_GET_AGENTS]  = db_mgr_sql_prepare(idx,"SELECT agent_id_string FROM vw_message_agents WHERE message_id=?");
		queries[idx][MSGS_GET_AGENTS]  = db_mgr_sql_prepare(idx,"SELECT agent_id_string FROM vw_message_group_agents WHERE group_id=?");
		queries[idx][MSGS_ENTRIES_GET] = db_mgr_sql_prepare(idx,"SELECT * FROM message_group_entry WHERE group_id=? ORDER BY order_num ASC");
		queries[idx][MSGS_ENTRIES_GET_AGENTS] = db_mgr_sql_prepare(idx,"SELECT * FROM vw_message_agents WHERE message_id=?");
		queries[idx][MSGS_UPDATE_GROUP_STATE] = db_mgr_sql_prepare(idx,"UPDATE message_group mg SET state_id=? WHERE group_id=?");
		queries[idx][MSGS_OUTGOING_GET]    = db_mgr_sql_prepare(idx,"SELECT group_id, ts FROM vw_ready_outgoing_message_groups");
		queries[idx][MSGS_OUTGOING_CREATE] = db_mgr_sql_prepare(idx,"INSERT INTO message_group (state_id, is_outgoing) VALUES(1, TRUE)");
		
		//skywalker uses anms patch to fix these reported issues
		//queries[idx][MSGS_INCOMING_GET]    = db_mgr_sql_prepare(idx,"SELECT * FROM vw_ready_INCOMING_message_groups");
		//queries[idx][MSGS_INCOMING_CREATE] = db_mgr_sql_prepare(idx,"SELECT create_incoming_message_group(FROM_UNIXTIME(?), ? )"); // Received timestamp, From Agent name (ie: ipn:2.1)
		queries[idx][MSGS_INCOMING_GET]    = db_mgr_sql_prepare(idx,"SELECT * FROM vw_ready_incoming_message_groups");
		queries[idx][MSGS_INCOMING_CREATE] = db_mgr_sql_prepare(idx,"SELECT create_incoming_message_group(?, ? )"); // Received timestamp, From Agent name (ie: ipn:2.1)



		queries[idx][MSGS_AGENT_GROUP_ADD_NAME] = db_mgr_sql_prepare(idx,"SELECT insert_message_group_agent_name(?, ?)"); // group_id, agent_name
	//		queries[idx][MSGS_AGENT_GROUP_ADD_ID] = db_mgr_sql_prepare(idx,"SELECT insert_message_group_agent_id(?, ?)"); // group_id, agent_id
		queries[idx][MSGS_AGENT_MSG_ADD] = db_mgr_sql_prepare(idx,"CALL SP__insert_message_entry_agent(?, ?)"); // message_id, agent_name

		//skywalker uses anms patch to fix these reported issues
		//queries[idx][MSGS_ADD_REPORT_SET_ENTRY] = db_mgr_sql_prepare(idx,"SELECT insert_message_report_entry(?, NULL, ?, ?, FROM_UNIXTIME(?))"); // message_id, order_num, ari_id, tnvc_id, ts
		queries[idx][MSGS_ADD_REPORT_SET_ENTRY] = db_mgr_sql_prepare(idx,"SELECT insert_message_report_entry(?, NULL, ?, ?, ?)"); // message_id, order_num, ari_id, tnvc_id, ts


		queries[idx][MSGS_REGISTER_AGENT_INSERT] = db_mgr_sql_prepare(idx,"SELECT add_message_register_entry(?,?,?,?,NULL,?)"); // group_id, ack, nak, acl, idx, agent_name
		queries[idx][MSGS_REGISTER_AGENT_GET] = db_mgr_sql_prepare(idx,"SELECT * FROM message_agents WHERE message_id = ?");
		queries[idx][MSGS_PERF_CTRL_INSERT] = db_mgr_sql_prepare(idx,"SELECT add_message_ctrl_entry(?, ?, ?, ?, ?, ?, ?)"); // group_id, ack, nak, acl, idx, timevalue or NULL, ac_id
		queries[idx][MSGS_PERF_CTRL_GET] = db_mgr_sql_prepare(idx,"SELECT tv, ac_id FROM message_perform_control WHERE message_id=?");
		queries[idx][MSGS_REPORT_SET_INSERT] = db_mgr_sql_prepare(idx,"SELECT add_message_report_set(?,?,?,?,NULL)"); // group_id, ack, nak, acl, idx
		queries[idx][MSGS_REPORT_SET_GET] = db_mgr_sql_prepare(idx,"SELECT * FROM report_template_actual_definition WHERE obj_actual_definition_id=?");
		
		// TODO MSGS_TABLE_SET_INSERT/GET

		queries[idx][DB_LOG_MSG] = db_mgr_sql_prepare(idx, "INSERT INTO nm_mgr_log (msg,details,level,source,file,line) VALUES(?,?,?,?,?,?)");
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL 
		queries[idx][AC_CREATE]         = db_mgr_sql_prepare(idx,"SELECT create_ac($1::int4,$2::varchar)", "AC_CREATE", 2, NULL); // num_entries, use_desc
		queries[idx][AC_INSERT]         = db_mgr_sql_prepare(idx,"SELECT insert_ac_actual_entry($1::int4, $2::int4, $3::int4)", "AC_INSERT", 3, NULL); // ac_id, obj_actual_definition_id, idx

		queries[idx][AC_GET] = db_mgr_sql_prepare(idx, "SELECT ace.obj_actual_definition_id "
												  "FROM ari_collection_entry ac "
												  "LEFT JOIN ari_collection_actual_entry ace ON ace.ac_entry_id=ac.ac_entry_id "
												  "WHERE ac.ac_id=$1::int4 "
												  "ORDER BY ac.order_num ASC", 
												  //"SELECT obj_actual_definition_id FROM vw_ac WHERE ac_id=$2"
												  "AC_GET", 1, NULL
			);


		queries[idx][ARI_GET]           = db_mgr_sql_prepare(idx,"SELECT data_type_id, adm_type, adm_enum, obj_enum, tnvc_id, issuing_org FROM vw_ari WHERE obj_actual_definition_id=$1::int4", "ARI_GET", 1, NULL);
		queries[idx][ARI_GET_META]      = db_mgr_sql_prepare(idx, "SELECT vof.obj_metadata_id, cfd.fp_spec_id "
															 "FROM vw_obj_formal_def vof "
															 "LEFT JOIN control_formal_definition cfd ON cfd.obj_formal_definition_id=vof.obj_formal_definition_id "
															 "WHERE vof.obj_enum=$1::int4 AND vof.data_type_id=$2::int4 AND vof.adm_enum=$3::int4",
															 "ARI_GET_META", 3, NULL
			);
		
		queries[idx][ARI_INSERT_CTRL]   = db_mgr_sql_prepare(idx,"SELECT insert_ari_ctrl($1::int4,$2::int4,NULL)", "ARI_INSERT_CTRL", 2, NULL); // obj_metadata_id, actual_parmspec_id, description
		
		queries[idx][TNVC_CREATE]       = db_mgr_sql_prepare(idx,"SELECT create_tnvc(NULL)", "TNVC_CREATE", 0, NULL); // use_desc
		queries[idx][TNVC_INSERT_AC]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_ac_entry($1::int4,NULL,NULL,$2::int4) ", "TNVC_INSERT_AC", 2, NULL); // tnvc_id, integer val
		queries[idx][TNVC_INSERT_ARI]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_obj_entry($1::int4,NULL,NULL,$2::int4) ", "TNVC_INSERT_ARI", 2, NULL); // tnvc_id, integer val	
		queries[idx][TNVC_INSERT_TNVC]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_tnvc_entry($1::int4,NULL,NULL,$2::int4) ", "TNVC_INSERT_TNVC", 2, NULL); // tnvc_id, integer val

		queries[idx][TNVC_INSERT_STR]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_str_entry($1::int4,NULL,NULL,$2::varchar) ", "TNVC_INSERT_STR", 2, NULL); // tnvc_id, varchar val
		queries[idx][TNVC_INSERT_BOOL]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_bool_entry($1::int4,NULL,NULL,$2::boolean) ", "TNVC_INSERT_BOOL", 2, NULL); // tnvc_id, boolean val
		queries[idx][TNVC_INSERT_BYTE]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_byte_entry($1::int4,NULL,NULL,$2::int2) ", "TNVC_INSERT_BYTE", 2, NULL); // tnvc_id, smallint val
		queries[idx][TNVC_INSERT_UINT]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_uint_entry($1::int4,NULL,NULL,$2::int4) ", "TNVC_INSERT_UINT", 2, NULL); // tnvc_id, integer val
		queries[idx][TNVC_INSERT_VAST]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_vast_entry($1::int4,NULL,NULL,$2::bigint) ", "TNVC_INSERT_VAST", 2, NULL); // tnvc_id, bigint val
		queries[idx][TNVC_INSERT_TV]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_tv_entry($1::int4,NULL,NULL,$2::int4) ", "TNVC_INSERT_TV", 2, NULL); // tnvc_id, integer val
		queries[idx][TNVC_INSERT_TS]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_ts_entry($1::int4,NULL,NULL,$2::int4) ", "TNVC_INSERT_TS", 2, NULL); // tnvc_id, integer val
		queries[idx][TNVC_INSERT_UVAST]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_uvast_entry($1::int4,NULL,NULL,$2::bigint) ", "TNVC_INSERT_UVAST", 2, NULL); // tnvc_id, bigint val
		queries[idx][TNVC_INSERT_REAL32]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_real32_entry($1::int4,NULL,NULL,$2::float8) ", "TNVC_INSERT_REAL32", 2, NULL); // tnvc_id, double val
		queries[idx][TNVC_INSERT_REAL64]    = db_mgr_sql_prepare(idx,"SELECT insert_tnvc_real64_entry($1::int4,NULL,NULL,$2::float8) ", "TNVC_INSERT_REAL64", 2, NULL); // tnvc_id, double val

		
 		
		queries[idx][TNVC_VIEW]         = db_mgr_sql_prepare(idx,"SELECT * FROM type_name_value_collection WHERE tnvc_id = $1::int4", "TNVC_VIEW", 1, NULL);
		queries[idx][TNVC_ENTRIES] = db_mgr_sql_prepare(idx,"SELECT data_type_id, int_value, uint_value, obj_value, str_value, ac_value, tnvc_value FROM vw_tnvc_entries WHERE tnvc_id = $1::int4 ORDER BY order_num ASC",
																"TNVC_ENTRIES", 1, NULL);
		
		queries[idx][TNVC_PARMSPEC_INSERT_AC]   = db_mgr_sql_prepare(idx,"CALL SP__insert_actual_parms_ac($1::int4,$2::int4,$3::int4)", "TNVC_PARMSPEC_INSERT_AC", 3, NULL); // ap_spec_id, order_num, ac_id
		queries[idx][TNVC_PARMSPEC_INSERT_TNVC] = db_mgr_sql_prepare(idx,"CALL SP__insert_actual_parms_tnvc($1::int4,$2::int4,$3::int4)", "TNVC_PARMSPEC_INSERT_TNVC", 3, NULL); // ap_spec_id, order_num, tnvc_id
		queries[idx][TNVC_PARMSPEC_CREATE]      = db_mgr_sql_prepare(idx,"SELECT create_actual_parmspec_tnvc($1::int4,$2::int4,NULL)", "TNVC_PARMSPEC_CREATE", 2, NULL); // formal_def_id, tnvc_id, description

		queries[idx][MSGS_GET]         = db_mgr_sql_prepare(idx,"SELECT * FROM message_group WHERE group_id=$1::int4", "MSGS_GET", 1, NULL);
		queries[idx][MSG_GET_AGENTS]  = db_mgr_sql_prepare(idx,"SELECT agent_id_string FROM vw_message_agents WHERE message_id=$1::int4", "MSG_GET_AGENTS", 1, NULL);
		queries[idx][MSGS_GET_AGENTS]  = db_mgr_sql_prepare(idx,"SELECT agent_id_string FROM vw_message_group_agents WHERE group_id=$1::int4", "MSGS_GET_AGENTS", 1, NULL);
		queries[idx][MSGS_ENTRIES_GET] = db_mgr_sql_prepare(idx,"SELECT * FROM message_group_entry WHERE group_id=$1::int4 ORDER BY order_num ASC", "MSGS_ENTRIES_GET", 1, NULL);
		queries[idx][MSGS_ENTRIES_GET_AGENTS] = db_mgr_sql_prepare(idx,"SELECT * FROM vw_message_agents WHERE message_id=$1::int4", "MSGS_ENTRIES_GET_AGENTS", 1, NULL);
		queries[idx][MSGS_UPDATE_GROUP_STATE] = db_mgr_sql_prepare(idx,"UPDATE message_group mg SET state_id=$1::int4 WHERE group_id=$2::int4", "MSGS_UPDATE_GROUP_STATE", 2, NULL);
		queries[idx][MSGS_OUTGOING_GET]    = db_mgr_sql_prepare(idx,"SELECT group_id, ts FROM vw_ready_outgoing_message_groups", "MSGS_OUTGOING_GET", 0, NULL);
		queries[idx][MSGS_OUTGOING_CREATE] = db_mgr_sql_prepare(idx,"INSERT INTO message_group (state_id, is_outgoing) VALUES(1, TRUE)", "MSGS_OUTGOING_CREATE", 0, NULL);
		queries[idx][MSGS_INCOMING_GET]    = db_mgr_sql_prepare(idx,"SELECT * FROM vw_ready_incoming_message_groups", "MSGS_INCOMING_GET", 0, NULL);
		queries[idx][MSGS_INCOMING_CREATE] = db_mgr_sql_prepare(idx,"SELECT create_incoming_message_group($1::int4, $2::varchar)", "MSGS_INCOMING_CREATE", 2, NULL); // Received timestamp, From Agent name (ie: ipn:2.1)
		
		queries[idx][MSGS_AGENT_GROUP_ADD_NAME] = db_mgr_sql_prepare(idx,"SELECT insert_message_group_agent_name($1::int4, $2::varchar)", "MSGS_AGENT_GROUP_ADD_NAME", 2, NULL); // group_id, agent_name
	//		queries[idx][MSGS_AGENT_GROUP_ADD_ID] = db_mgr_sql_prepare(idx,"SELECT insert_message_group_agent_id($1, $2)", "MSGS_AGENT_GROUP_ADD_ID", 2, NULL); // group_id, agent_id
		queries[idx][MSGS_AGENT_MSG_ADD] = db_mgr_sql_prepare(idx,"CALL SP__insert_message_entry_agent($1::int4, $2::varchar)", "MSGS_AGENT_MSG_ADD", 2, NULL); // message_id, agent_name
		queries[idx][MSGS_ADD_REPORT_SET_ENTRY] = db_mgr_sql_prepare(idx,"SELECT insert_message_report_entry($1::int4, NULL, $2::int4, $3::int4, $4::int4)", "MSGS_ADD_REPORT_SET_ENTRY", 4, NULL); // message_id, order_num, ari_id, tnvc_id, ts

		queries[idx][MSGS_REGISTER_AGENT_INSERT] = db_mgr_sql_prepare(idx,"SELECT add_message_register_entry($1::int4,$2::boolean,$3::boolean,$4::boolean,NULL,$5::varchar)", "MSGS_REGISTER_AGENT_INSERT", 5, NULL); // group_id, ack, nak, acl, idx, agent_name
		queries[idx][MSGS_REGISTER_AGENT_GET] = db_mgr_sql_prepare(idx,"SELECT * FROM message_agents WHERE message_id = $1::int4", "MSGS_REGISTER_AGENT_GET", 1, NULL);
		queries[idx][MSGS_PERF_CTRL_INSERT] = db_mgr_sql_prepare(idx,"SELECT add_message_ctrl_entry($1::int4, $2::boolean, $3::boolean, $4::boolean, $5::int4, $6::int4, $7::int4)", "MSGS_PERF_CTRL_INSERT", 7, NULL); // group_id, ack, nak, acl, idx, timevalue or NULL, ac_id
		queries[idx][MSGS_PERF_CTRL_GET] = db_mgr_sql_prepare(idx,"SELECT tv, ac_id FROM message_perform_control WHERE message_id=$1::int4", "MSGS_PERF_CTRL_GET", 1, NULL);
		queries[idx][MSGS_REPORT_SET_INSERT] = db_mgr_sql_prepare(idx,"SELECT add_message_report_set($1::int4,$2::boolean,$3::boolean,$4::boolean,NULL)", "MSGS_REPORT_SET_INSERT", 4, NULL); // group_id, ack, nak, acl, idx
		queries[idx][MSGS_REPORT_SET_GET] = db_mgr_sql_prepare(idx,"SELECT * FROM report_template_actual_definition WHERE obj_actual_definition_id=$1::int4", "MSGS_REPORT_SET_GET", 1, NULL);
		
		// TODO MSGS_TABLE_SET_INSERT/GET

		queries[idx][DB_LOG_MSG] = db_mgr_sql_prepare(idx, "INSERT INTO nm_mgr_log (msg,details,level,source,file,line) VALUES($1::varchar,$2::text,$3::int4,$4::varchar,$5::varchar,$6::int4)", "DB_LOG_MSG", 6, NULL);
	#endif // HAVE_POSTGRESQL
	
	}

	AMP_DEBUG_EXIT("db_mgt_init", "-->1", NULL);
	return 1;
	}



/******************************************************************************
 *
 * \par Function Name: db_mgt_clear
 *
 * \par Clears all of the database tables used by the DTNMP Management Daemon.
 *
 * \retval 0 Failure
 *        !0 Success
 *
 *
 * \todo Add support to clear all tables. Maybe add a parm to select a
 *        table to clear (perhaps a string?)
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  08/27/15  E. Birrane     Updated to latest schema
 *****************************************************************************/


int db_mgt_clear()
{

	AMP_DEBUG_ENTRY("db_mgt_clear - DISABLED", "()", NULL);
#if 0 // VERIFY: Do we need this? Dropping for now
	if( db_mgt_clear_table("dbtMIDs") ||
		db_mgt_clear_table("dbtIncomingMessages") ||
		db_mgt_clear_table("dbtOIDs") ||
		db_mgt_clear_table("dbtADMs") ||
		db_mgt_clear_table("dbtADMNicknames") ||
		db_mgt_clear_table("dbtIncomingMessageGroup") ||
		db_mgt_clear_table("dbtOutgoingMessageGroup") ||
		db_mgt_clear_table("dbtRegisteredAgents") ||
		db_mgt_clear_table("dbtDataCollections") ||
		db_mgt_clear_table("dbtDataCollection") ||
		db_mgt_clear_table("dbtMIDCollections") ||
		db_mgt_clear_table("dbtMIDCollection") ||
		db_mgt_clear_table("dbtMIDParameters") ||
		db_mgt_clear_table("dbtMIDParameter"))
	{
		AMP_DEBUG_ERR("db_mgt_clear", "SQL Error: %s", mysql_error(gConn));
		AMP_DEBUG_EXIT("db_mgt_clear", "--> 0", NULL);
		return 0;
	}

	AMP_DEBUG_EXIT("db_mgt_clear", "--> 1", NULL);
#endif
	return 1;
}


/******************************************************************************
 *
 * \par Function Name: db_mgt_clear_table
 *
 * \par Clears a database table used by the DTNMP Management Daemon.
 *
 * Note:
 *   We don't use truncate here because of foreign key constraints. Delete
 *   is able to remove items from a table, but does not reseed the
 *   auto-incrementing for the table, so an alter table command is also
 *   used.
 *
 * \retval !0 Failure
 *          0 Success
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/29/15  E. Birrane     Initial implementation,
 *****************************************************************************/

int db_mgt_clear_table(char *table)
{
	if(table == NULL)
	{
		return 1;
	}

	#ifdef HAVE_MYSQL
	char *disableChecksQ = "SET FOREIGN_KEY_CHECKS=0";
	char *enableChecksQ = "SET FOREIGN_KEY_CHECKS=1";
	char *checksArgs = NULL;
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *disableChecksQ = "ALTER TABLE %s DISABLE TRIGGER ALL";
	char *enableChecksQ = "ALTER TABLE %s ENABLE TRIGGER ALL";
	char *checksArgs = table;
	#endif // HAVE_POSTGRESQL

	if (db_mgt_query_insert(NULL,disableChecksQ,checksArgs) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_mgt_clear_table", "SQL Error: %s", 
			#ifdef HAVE_MYSQL
			mysql_error(gConn[0])
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			PQerrorMessage(gConn[0])
			#endif // HAVE_POSTGRESQL
		);
		AMP_DEBUG_EXIT("db_mgt_clear_table", "--> 0", NULL);
		return 1;
	}

	if (db_mgt_query_insert(NULL,"TRUNCATE %s", table) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_mgt_clear_table", "SQL Error: %s",  
			#ifdef HAVE_MYSQL
			mysql_error(gConn[0])
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			PQerrorMessage(gConn[0])
			#endif // HAVE_POSTGRESQL
		);
		AMP_DEBUG_EXIT("db_mgt_clear_table", "--> 0", NULL);
		return 1;
	}

	if (db_mgt_query_insert(NULL,enableChecksQ,checksArgs) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_mgt_clear_table", "SQL Error: %s",  
			#ifdef HAVE_MYSQL
			mysql_error(gConn[0])
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			PQerrorMessage(gConn[0])
			#endif // HAVE_POSTGRESQL
		);
		AMP_DEBUG_EXIT("db_mgt_clear_table", "--> 0", NULL);
		return 1;
	}

	AMP_DEBUG_EXIT("db_mgt_clear_table", "--> 0", NULL);
	return 0;
}


/******************************************************************************
 *
 * \par Function Name: db_mgt_close
 *
 * \par Close the database gConnection.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *****************************************************************************/

void db_mgt_close()
{
	AMP_DEBUG_ENTRY("db_mgt_close","()",NULL);

	for(int i = 0; i < MGR_NUM_SQL_CONNECTIONS; i++) {
		db_mgt_close_conn(i);
	}
	AMP_DEBUG_EXIT("db_mgt_close","-->.", NULL);
}
void db_mgt_close_conn(size_t idx) {
	if(gConn[idx] != NULL)
	{
		// Free prepared queries (mysql_stmt_close())
		#ifdef HAVE_MYSQL
		for(int i = 0; i < MGR_NUM_QUERIES; i++) {
			mysql_stmt_close(queries[idx][i]);
		}
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		/*  There is no libpq function for deleting a prepared statement, 
			the SQL DEALLOCATE statement can be used for that purpose but 
			if you do not explicitly deallocate a prepared statement, it is deallocated when the session ends. 
			So no actuaion should be needed*/
		#endif // HAVE_POSTGRESQL
		// Close the connection
		#ifdef HAVE_MYSQL
		mysql_close(gConn[idx]);
		mysql_library_end();
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		/* close the connection to the database and cleanup */
	    PQfinish(gConn[idx]);
		#endif // HAVE_POSTGRESQL

		gConn[idx] = NULL;
	}

}



/******************************************************************************
 *
 * \par Function Name: db_mgt_connected
 *
 * \par Checks to see if the database connection is still active and, if not,
 *      try to reconnect up to some configured number of times.
 *
 * \par Notes:
 *
 * \retval !0 Error
 *          0 Success
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/27/15  E. Birrane     Updated to try and reconnect to DB.
 *****************************************************************************/

int   db_mgt_connected(size_t idx)
{
	int result = -1;
	uint8_t num_tries = 0;

	if(gConn[idx] == NULL)
	{
		return -1;
	}

	#ifdef HAVE_MYSQL
 	result = mysql_ping(gConn[idx]);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	result = (PQstatus(gConn[idx]) == CONNECTION_OK) ? 0 : 1;
	#endif // HAVE_POSTGRESQL
	if(result != 0)
	{
		while(num_tries < SQL_CONN_TRIES)
		{
			// FIXME: Passing in gParms to a fn that assigns gParms
			/* NOTES/FIXME: Does this relate to gMbrDB.sql_info? If not, we have a disconnect in parameters
			 * nm_mgr.c HAVE_MYSQL passes gMgrDB.sql_info to db_mgt_init which does the connection
			 */
			db_mgt_init_con(idx, gParms);
			#ifdef HAVE_MYSQL
 			if((result = mysql_ping(gConn[idx])) == 0)
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			if((result = (PQstatus(gConn[idx]) == CONNECTION_OK) ? 0 : 1) == 0)
 			#endif // HAVE_POSTGRESQL
			{
				if (idx == DB_RPT_CON) {
					// Disable autocommit to ensure all queries are executed within a transaction to ensure consistency
					// A mysql_commit or mysql_rollback will automatically start a new transaction as the old one is closed
					#ifdef HAVE_MYSQL
 					mysql_autocommit(gConn[DB_RPT_CON], 0);
					#endif // HAVE_MYSQL
					#ifdef HAVE_POSTGRESQL
					//TODO postgresql : turn off autocommit
					#endif // HAVE_POSTGRESQL
				}
				DB_LOG_MSG(idx, "NM DB Connection Restored", NULL, AMP_OK); 
				return 0;
			}

			OS_TaskDelay(SQL_RECONN_TIME_MSEC);
			num_tries++;
		}
	}

	return result;
}

#ifdef HAVE_MYSQL
static MYSQL_STMT* db_mgr_sql_prepare(size_t idx, const char* query) {
	MYSQL_STMT* rtv = mysql_stmt_init(gConn[idx]);
	if (rtv == NULL) {
		AMP_DEBUG_ERR("Failed to allocate statement", NULL);
		return rtv;
	}

	if (mysql_stmt_prepare(rtv, query, strlen(query) ) != 0)
	{
		AMP_DEBUG_ERR("Failed to prepare %s: errno %d, error= %s", query, mysql_stmt_errno(rtv),mysql_stmt_error(rtv));
		mysql_stmt_close(rtv);
		return NULL;
	}
	return rtv;
}
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static char* db_mgr_sql_prepare(size_t idx, const char* query, char *stmtName, int nParams, const Oid *paramTypes) {
	PGresult *pgresult = PQprepare(gConn[idx], stmtName, query, nParams, paramTypes);

	if (pgresult == NULL) { // out of memory or failure to send the conmmand at all
		AMP_DEBUG_ERR("Failed to allocate statement %s", query);
	}

	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK)
	{
		AMP_DEBUG_ERR("Failed to prepare %s: errno %d, error= %s", query, PQresultStatus(pgresult),PQresultErrorMessage(pgresult));
		/* there is no libpq function for deleting a prepared statement, the SQL DEALLOCATE statement can be used for that purpose but 
			if you do not explicitly deallocate a prepared statement, it is deallocated when the session ends. 
			So no actuaion should be needed*/
	}

	PQclear(pgresult);
	return stmtName;
}
#endif // HAVE_POSTGRESQL


int  db_mgr_sql_persist()
{
	int success = AMP_OK;
#if 0
	//FIXME: persistance
	Sdr sdr = getIonsdr();

	if(gMgrDB.sql_info.desc.descObj == 0)
	{
		gMgrDB.sql_info.desc.descObj = sdr_malloc(sdr, sizeof(gMgrDB.sql_info.desc));
	}

	blob_t *data = db_mgr_sql_info_serialize(&(gMgrDB.sql_info));

	CHKERR(sdr_begin_xn(sdr));

	if(gMgrDB.sql_info.desc.itemObj != 0)
	{
		sdr_free(sdr, gMgrDB.sql_info.desc.itemObj);
	}

	gMgrDB.sql_info.desc.itemObj = sdr_malloc(sdr, data->length);
	gMgrDB.sql_info.desc.itemSize = data->length;

	sdr_write(sdr, gMgrDB.sql_info.desc.itemObj, (char *) data->value, data->length);
	sdr_write(sdr, gMgrDB.sql_info.desc.descObj, (char *) &(gMgrDB.sql_info.desc), sizeof(gMgrDB.sql_info.desc));

	sdr_end_xn(sdr);

	blob_release(data, 1);
#endif
	return success;
}


void db_mgr_sql_info_deserialize(blob_t *data)
{
	QCBORError err;
	QCBORItem item;
	QCBORDecodeContext it;
	size_t length;

	QCBORDecode_Init(&it,
					 (UsefulBufC){data->value,data->length},
					 QCBOR_DECODE_MODE_NORMAL);

	err = QCBORDecode_GetNext(&it, &item);
	if (err != QCBOR_SUCCESS || item.uDataType != QCBOR_TYPE_ARRAY)
	{
		AMP_DEBUG_ERR("mgr_sql_info_deserialize","Not a container. Error is %d Type %d", err, item.uDataType);
		return;
	}
	else if (item.val.uCount != 4)
	{
		AMP_DEBUG_ERR("mgr_sql_info_deserialize","Bad length. %d not 4", item.val.uCount);
		return;
	}

	cut_get_cbor_str_ptr(&it, gMgrDB.sql_info.server, UI_SQL_SERVERLEN);
	cut_get_cbor_str_ptr(&it, gMgrDB.sql_info.username, UI_SQL_ACCTLEN);
	cut_get_cbor_str_ptr(&it, gMgrDB.sql_info.password, UI_SQL_ACCTLEN);
	cut_get_cbor_str_ptr(&it, gMgrDB.sql_info.database, UI_SQL_DBLEN);

	// Verify Decoding Completed Successfully
	cut_decode_finish(&it);

	return;
}

blob_t*	  db_mgr_sql_info_serialize(sql_db_t *item)
{
	QCBOREncodeContext encoder;

	blob_t *result = blob_create(NULL, 0, 2 * sizeof(sql_db_t));

	QCBOREncode_Init(&encoder, (UsefulBuf){result->value, result->alloc});
	QCBOREncode_OpenArray(&encoder);

	QCBOREncode_AddSZString(&encoder, item->server);
	QCBOREncode_AddSZString(&encoder, item->username);
	QCBOREncode_AddSZString(&encoder, item->password);
	QCBOREncode_AddSZString(&encoder, item->database);

	QCBOREncode_CloseArray(&encoder);

	UsefulBufC Encoded;
	if(QCBOREncode_Finish(&encoder, &Encoded)) {
		AMP_DEBUG_ERR("db_mgr_sql_info_serialize", "Encoding failed", NULL);
		blob_release(result,1);
		return NULL;
	}
	
	result->length = Encoded.len;
	return result;
}


int  db_mgr_sql_init()
{
#if 0
  //FIXME persistance
	Sdr sdr = getIonsdr();
	char *name = "mgr_sql";
#endif

	// * Initialize the non-volatile database. * /
	// Note: Moved to main() to allow connection parameters to be specified on the command-line.
	//memset((char*) &(gMgrDB.sql_info), 0, sizeof(gMgrDB.sql_info));

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&(gMgrDB.sql_info.lock), &attr);

#if 0
  //FIXME persistance
	/* Recover the Agent database, creating it if necessary. */
	CHKERR(sdr_begin_xn(sdr));

	gMgrDB.sql_info.desc.descObj = sdr_find(sdr, name, NULL);
	switch(gMgrDB.sql_info.desc.descObj)
	{
		case -1:  // SDR error. * /
			sdr_cancel_xn(sdr);
			AMP_DEBUG_ERR("db_mgr_sql_init", "Can't search for DB in SDR.", NULL);
			return -1;

		case 0: // Not found; Must create new DB. * /

			if((gMgrDB.sql_info.desc.descObj = sdr_malloc(sdr, sizeof(gMgrDB.sql_info.desc))) == 0)
			{
				sdr_cancel_xn(sdr);
				AMP_DEBUG_ERR("db_mgr_sql_init", "No space for database.", NULL);
				return -1;
			}
			AMP_DEBUG_ALWAYS("db_mgr_sql_init", "Creating DB: %s", name);

			sdr_write(sdr, gMgrDB.sql_info.desc.descObj, (char *) &(gMgrDB.sql_info.desc), sizeof(gMgrDB.sql_info.desc));
			sdr_catlg(sdr, name, 0, gMgrDB.sql_info.desc.descObj);

			break;

		default:  /* Found DB in the SDR */
			/* Read in the Database. */
			sdr_read(sdr, (char *) &(gMgrDB.sql_info.desc), gMgrDB.sql_info.desc.descObj, sizeof(gMgrDB.sql_info.desc));
			AMP_DEBUG_ALWAYS("db_mgr_sql_init", "Found DB", NULL);

			if(gMgrDB.sql_info.desc.itemSize > 0)
			{
				blob_t *data = blob_create(NULL, 0, gMgrDB.sql_info.desc.itemSize);
				if(data != NULL)
				{
					sdr_read(sdr, (char *) data->value, gMgrDB.sql_info.desc.itemObj, gMgrDB.sql_info.desc.itemSize);
					data->length = gMgrDB.sql_info.desc.itemSize;
					db_mgr_sql_info_deserialize(data);
					blob_release(data, 1);
				}
			}


	}

	if(sdr_end_xn(sdr))
	{
		AMP_DEBUG_ERR("db_mgr_sql_init", "Can't create Agent database.", NULL);
		return -1;
	}
#endif

	return AMP_OK;
}




/******************************************************************************
 *
 * \par Function Name: db_mgt_query_fetch
 *
 * \par Runs a fetch in the database given a query and returns the result, if
 *      a result field is provided..
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[out] res    - The result.
 * \param[in]  format - Format to build query
 * \param[in]  ...    - Var args to build query given format string.
 *
 * \par Notes:
 *   - The res structure should be a pointer but without being allocated. This
 *     function will create the storage.
 *   - If res is NULL that's ok, but no result will be returned.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/

#ifdef HAVE_MYSQL
 int32_t db_mgt_query_fetch(MYSQL_RES **res, char *format, ...)
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
int32_t db_mgt_query_fetch(PGresult **res, char *format, ...)
#endif // HAVE_POSTGRESQL
{
	char query[1024];
	size_t idx = DB_RPT_CON; // TODO

	AMP_DEBUG_ENTRY("db_mgt_query_fetch","(%p,%p)",
			        res, format);

	/* Step 0: Sanity check. */
	if(format == NULL)
	{
		AMP_DEBUG_ERR("db_mgt_query_fetch", "Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_mgt_query_fetch", "-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/*
	 * Step 1: Assert the DB connection. This should not only check
	 *         the connection as well as try and re-establish it.
	 */
	if(db_mgt_connected(idx) == 0)
	{
		va_list args;

		va_start(args, format); // format is last parameter before "..."
		vsnprintf(query, 1024, format, args);
		va_end(args);

#ifdef HAVE_MYSQL
 		if (mysql_query(gConn[idx], query))
 		{
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		*res = PQexec(gConn[idx], query);
		if((PQresultStatus(*res) != PGRES_TUPLES_OK) && (PQresultStatus(*res) != PGRES_COMMAND_OK))
		{
			PQclear(*res);
		#endif // HAVE_POSTGRESQL
 			AMP_DEBUG_ERR("db_mgt_query_fetch", "Database Error: %s",
					#ifdef HAVE_MYSQL
					mysql_error(gConn[idx])
					#endif // HAVE_MYSQL
					#ifdef HAVE_POSTGRESQL
					PQerrorMessage(gConn[idx])
					#endif // HAVE_POSTGRESQL
			);
			AMP_DEBUG_EXIT("db_mgt_query_fetch", "-->%d", AMP_FAIL);
			return AMP_FAIL;
		}

		#ifdef HAVE_MYSQL
		if((*res = mysql_store_result(gConn[idx])) == NULL)
		{
			AMP_DEBUG_ERR("db_mgt_query_fetch", "Can't get result.", NULL);
			AMP_DEBUG_EXIT("db_mgt_query_fetch", "-->%d", AMP_FAIL);
			return AMP_FAIL;
		}
			#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		PQclear(*res);
		#endif // HAVE_POSTGRESQL
	}
	else
	{
		AMP_DEBUG_ERR("db_mgt_query_fetch", "DB not connected.", NULL);
		AMP_DEBUG_EXIT("db_mgt_query_fetch", "-->%d", AMP_SYSERR);
		return AMP_SYSERR;
	}

	AMP_DEBUG_EXIT("db_mgt_query_fetch", "-->%d", AMP_OK);
	return AMP_OK;
}



/******************************************************************************
 *
 * \par Function Name: db_mgt_query_insert
 *
 * \par Runs an insert in the database given a query and returns the
 *      index of the inserted item.
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[out] idx    - The index of the inserted row.
 * \param[in]  format - Format to build query
 * \param[in]  ...    - Var args to build query given format string.
 *
 * \par Notes:
 *   - The idx may be NULL if the insert index is not needed.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/

int32_t db_mgt_query_insert(uint32_t *idx, char *format, ...)
{
	char query[SQL_MAX_QUERY];
	size_t db_idx = DB_RPT_CON; // TODO

	AMP_DEBUG_ENTRY("db_mgt_query_insert","(%p,%p)",idx, format);
/*EJB
	if(idx == NULL)
	{
		AMP_DEBUG_ERR("db_mgt_query_insert", "Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_mgt_query_insert", "-->%d", AMP_FAIL);
		return AMP_FAIL;
	}
*/
	if(db_mgt_connected(db_idx) == 0)
	{
		va_list args;

		va_start(args, format); // format is last parameter before "..."
		if(vsnprintf(query, SQL_MAX_QUERY, format, args) == SQL_MAX_QUERY)
		{
			AMP_DEBUG_ERR("db_mgt_query_insert", "query is too long. Maximum length is %d", SQL_MAX_QUERY);
		}
		va_end(args);

	#ifdef HAVE_MYSQL
 		if (mysql_query(gConn[db_idx], query))
 		{
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		PGresult *res = PQexec(gConn[db_idx], query);
		if(dbtest_result(PGRES_COMMAND_OK) != 0 && dbtest_result(PGRES_TUPLES_OK) != 0)
		{
			PQclear(res);
		#endif // HAVE_POSTGRESQL
 			AMP_DEBUG_ERR("db_mgt_query_insert", "Database Error: %s",
					#ifdef HAVE_MYSQL
					mysql_error(gConn[db_idx])
					#endif // HAVE_MYSQL
					#ifdef HAVE_POSTGRESQL
					PQerrorMessage(gConn[db_idx])
					#endif // HAVE_POSTGRESQL
			);
			AMP_DEBUG_EXIT("db_mgt_query_insert", "-->%d", AMP_FAIL);
			return AMP_FAIL;
		}

		if(idx != NULL)
		{
			#ifdef HAVE_MYSQL
 			if((*idx = (uint32_t) mysql_insert_id(gConn[db_idx])) == 0)
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			//requires query string to include "RETURNING id"
			char *iptr = PQgetvalue(res, 0, 0);
			*idx = ntohl(*((uint32_t *) iptr));
			if(*idx == 0)
			#endif // HAVE_POSTGRESQL
			{
				AMP_DEBUG_ERR("db_mgt_query_insert", "Unknown last inserted row.", NULL);
				AMP_DEBUG_EXIT("db_mgt_query_insert", "-->%d", AMP_FAIL);
				return AMP_FAIL;
			}
		}
		#ifdef HAVE_POSTGRESQL
		PQclear(res);
		#endif // HAVE_POSTGRESQL
	}
	else
	{
		AMP_DEBUG_ERR("db_mgt_query_insert", "DB not connected.", NULL);
		AMP_DEBUG_EXIT("db_mgt_query_insert", "-->%d", AMP_SYSERR);
		return AMP_SYSERR;
	}

	AMP_DEBUG_EXIT("db_mgt_query_insert", "-->%d", AMP_OK);
	return AMP_OK;
}



/******************************************************************************
 *
 * \par Function Name: db_mgt_txn_start
 *
 * \par Starts a transaction in the database, if we are not already in a txn.
 *
 * \par Notes:
 *   - This function is not multi-threaded. We assume that we are the only
 *     input into the database and that there is only one "active" transaction
 *     at a time.
 *   - This function does not support nested transactions.
 *   - If a transaction is already open, this function assumes that is the
 *     transaction to use.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/

void db_mgt_txn_start() // DEPRECATED in favor of disabling autocommit for RPT_CON, while CTRL_CON does not need transactions. If a third connection is added in the future for the UI, that version may require explicitly starting transactions
{
	if(gInTxn == 0)
	{
		if(db_mgt_query_insert(NULL,"START TRANSACTION",NULL) == AMP_OK)
		{
			gInTxn = 1;
		}
	}
}



/******************************************************************************
 *
 * \par Function Name: db_mgt_txn_rollback
 *
 * \par Rolls back a transaction in the database, if we are in a txn.
 *
 * \par Notes:
 *   - This function is not multi-threaded. We assume that we are the only
 *     input into the database and that there is only one "active" transaction
 *     at a time.
 *   - This function does not support nested transactions.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/

void db_mgt_txn_rollback()
{
	if(gInTxn == 1)
	{
		if(db_mgt_query_insert(NULL,"ROLLBACK",NULL) == AMP_OK)
		{
			gInTxn = 0;
		}
	}
}



/******************************************************************************
 *
 * \par Function Name: db_tx_msg_groups
 *
 * \par Returns 1 if the message is ready to be sent
 *
 * \retval AMP_SYSERR on system error
 *         AMP_FAIL   if no message groups ready.
 *         AMP_OK     If there are message groups ready to be sent.
 *
 * \param[out] sql_res - The outgoing messages.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/13/13  E. Birrane      Initial implementation,
 *  07/18/13  S. Jacobs       Added outgoing agents
 *  09/27/13  E. Birrane      Configure each agent with custom rpt, if applicable.
 *  08/27/15  E. Birrane      Update to new data model, schema
 *  01/26/17  E. Birrane      Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
static int32_t db_tx_msg_group_agents(nmmgr_t *mgr, int group_id,  msg_grp_t *msg_group)
{
	int rtv = AMP_OK;
	dbprep_declare(DB_CTRL_CON, MSGS_GET_AGENTS, 1, 1);
	dbprep_bind_param_int(0,group_id);
	
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);

        eid_t destination;
	dbprep_bind_res_str(0, destination.name, AMP_MAX_EID_LEN);
	
	// Execute Get Number of results
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);
	mysql_stmt_store_result(stmt);

	while(!mysql_stmt_fetch(stmt) )
	{
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
  eid_t destination;
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
	for (int i = 0; i < PQntuples(res); i++)
	{
	#endif // HAVE_POSTGRESQL
		if (mif_send_grp(&mgr->mif, msg_group, &destination) != AMP_OK) {
			rtv = AMP_FAIL;
			DB_LOG_MSG(DB_CTRL_CON, "Failed to send group to agent", destination.name, AMP_FAIL);
		}
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	return rtv;

}
	  
int db_query_tnvc(size_t dbidx, int tnvc_id, tnvc_t *parms)
{
	int rtv = AMP_FAIL;
	CHKZERO(parms);
	memset(parms,0,sizeof(tnvc_t));

	// Tmp variables
	ac_t *ac_entry;
	tnvc_t *tnvc_entry;

	// Query TNVC_PARM_ENTRIES
	enum cols {
		C_DATA_TYPE_ID=0,
		C_INT_VALUE,
		C_UINT_VALUE,
		C_OBJ_VALUE,
		C_STR_VALUE,
		C_AC_VALUE,
		C_TNVC_VALUE,
		NUM_RES_COLS
	};

	dbprep_declare(dbidx, TNVC_ENTRIES, 1, NUM_RES_COLS);
	dbprep_bind_param_int(0,tnvc_id);

	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);

	// Bind results
	amp_uvast uvast_val;
	amp_vast vast_val;
	dbprep_dec_res_int(C_DATA_TYPE_ID, tnv_type);
	dbprep_bind_res_int(C_INT_VALUE, vast_val);
	dbprep_bind_res_int(C_UINT_VALUE, uvast_val);
	dbprep_dec_res_int(C_OBJ_VALUE, obj_val);

	char str_val[255];
	dbprep_bind_res_str(C_STR_VALUE, str_val,255);
	
	dbprep_dec_res_int(C_AC_VALUE, ac_val);
	dbprep_dec_res_int(C_TNVC_VALUE, tnvc_val);
	
	// Execute Get Number of results
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);
	mysql_stmt_store_result(stmt);
	int array_len = mysql_stmt_num_rows(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
	int array_len = PQntuples(res);
	#endif // HAVE_POSTGRESQL
	int *cache_ids = STAKE(array_len * sizeof(int) );
	
	// Create vector
	parms->values = vec_create(array_len, tnv_cb_del,tnv_cb_comp,tnv_cb_copy, VEC_FLAG_AS_STACK, &rtv);
	if (rtv != AMP_OK) {
		#ifdef HAVE_MYSQL
 		mysql_stmt_free_result(stmt);
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		PQclear(res);
		#endif // HAVE_POSTGRESQL
	
		return rtv;
	}

	#ifdef HAVE_MYSQL
 	for(int i = 0; !mysql_stmt_fetch(stmt) && rtv == AMP_OK; i++)
 	{
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	/* Use PQfnumber to avoid assumptions about field order in result */
	int tnv_type_fnum = PQfnumber(res, "data_type_id");
    int vast_val_fnum = PQfnumber(res, "int_value");
    int uvast_val_fnum = PQfnumber(res, "uint_value");
    int obj_val_fnum = PQfnumber(res, "obj_value");
    int str_val_fnum = PQfnumber(res, "str_value");
    int ac_val_fnum = PQfnumber(res, "ac_value");
    int tnvc_val_fnum = PQfnumber(res, "tnvc_value");
	for (int i = 0; i < array_len && rtv == AMP_OK; i++)
	{
		int tnv_type;
		char *iptr = PQgetvalue(res, i, tnv_type_fnum);
		tnv_type = ntohl(*((uint32_t *) iptr));
		//TODO postgresql : read result vast_val
		//TODO postgresql : read result uvast_val
		//TODO postgresql : read result obj_val
		//TODO postgresql : read result str_val
		int ac_val;
		iptr = PQgetvalue(res, i, ac_val_fnum);
		ac_val = ntohl(*((uint32_t *) iptr));
		
		int tnvc_val;
		iptr = PQgetvalue(res, i, tnvc_val_fnum);
		tnvc_val = ntohl(*((uint32_t *) iptr));

	#endif // HAVE_POSTGRESQL
	
		tnv_t *val = tnv_create();
		val->type = tnv_type;
		
		switch(tnv_type) {
		case AMP_TYPE_AC:
			cache_ids[i] = ac_val;
			break;
		case AMP_TYPE_TNVC:
			cache_ids[i] = tnvc_val;
			break;
		default:
			AMP_DEBUG_ERR(__FUNCTION__, "SQL Support for TNV type %d not implemented", tnv_type);
			rtv = AMP_FAIL;
		}
		vec_insert(&(parms->values), val, NULL);
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	for(int i = 0; i < array_len && rtv == AMP_OK; i++) {
		tnv_t *val = (tnv_t*)vec_at(&(parms->values), i);
		switch(val->type) {
		case AMP_TYPE_AC:
			ac_entry = db_query_ac(dbidx, cache_ids[i]);
			val->value.as_ptr = ac_entry;
			break;
		case AMP_TYPE_TNVC:
			tnvc_entry = tnvc_create(0);
			db_query_tnvc(dbidx, cache_ids[i], tnvc_entry);
			val->value.as_ptr = tnvc_entry;
			break;
		default:
			// Nothing to be done but appease the compiler
			break;
		}
	}
	SRELEASE(cache_ids);

	// Note: In case of an error, parent will free any partial values already in tnvc
	return rtv;
}

/** Build an ARI Object from given ID
 */
ari_t* db_query_ari(size_t dbidx, int ari_id)
{
	ari_t *ari;
	amp_uvast temp;

	// Query ARI
	enum cols { C_ARI_TYPE=0, // data_type_id
				C_ADM_TYPE, // adm_type
				C_ADM_ENUM, // adm_enum
				C_OBJ_ENUM,
				C_TNVC_ID, // Parameters ID (actual)
				C_OBJ_NAME,
				C_ISSUING_ORG,
				NUM_RES_COLS
	};

	dbprep_declare(dbidx, ARI_GET, 1, NUM_RES_COLS);
	dbprep_bind_param_int(0,ari_id);

	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);

	// Declare result fields
	amp_type_e ari_type;
	dbprep_bind_res_int(C_ARI_TYPE, ari_type);

	dbprep_dec_res_int(C_ADM_TYPE, adm_type);
	dbprep_dec_res_int(C_ADM_ENUM, adm_enum);
	dbprep_dec_res_int(C_OBJ_ENUM, obj_enum);
	dbprep_dec_res_int(C_TNVC_ID, tnvc_id);

	char obj_name[255];
	dbprep_bind_res_str(C_OBJ_NAME, obj_name,255);

	char issuing_org[255];
	dbprep_bind_res_str(C_ISSUING_ORG, issuing_org,255);
	

	// Bind results
	DB_CHKNULL(mysql_stmt_bind_result(stmt, bind_res));
	DB_CHKNULL(mysql_stmt_execute(stmt));
	DB_CHKNULL(mysql_stmt_store_result(stmt)); // Results must be buffered to allow execution of nested queries

	if (mysql_stmt_num_rows(stmt) != 1) {
		AMP_DEBUG_ERR(__FUNCTION__, "Unable to retrieve ARI ID %i", ari_id);
		return NULL;
	}

	// Retrieve single row, or abort with error
	if (mysql_stmt_fetch(stmt) != 0) {
		AMP_DEBUG_ERR(__FUNCTION__, "Unable to retrieve ARI row for %i", ari_id);
		return NULL;
	}

	// Free result (all data is already retrieveed)
	mysql_stmt_free_result(stmt);

	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKNULL(dbtest_result(PGRES_TUPLES_OK))
	int num_rows = PQntuples(res);
	if (num_rows != 1) {
		AMP_DEBUG_ERR("Unable to retrieve ARI ID %i", ari_id);
		PQclear(res);
		return NULL;
	}
	/* Use PQfnumber to avoid assumptions about field order in result */
	int ari_type_fnum = PQfnumber(res, "data_type_id");
        int adm_type_fnum = PQfnumber(res, "adm_type");
        int adm_enum_fnum = PQfnumber(res, "adm_enum");
        int obj_enum_fnum = PQfnumber(res, "obj_enum");
        int tnvc_id_fnum = PQfnumber(res, "tnvc_id");
        int obj_name_fnum = PQfnumber(res, "obj_name");
        int issuing_org_fnum = PQfnumber(res, "issuing_org");
	int ari_type;
	int adm_type = 0;
	int adm_enum = 0;
	int obj_enum;
	int tnvc_id;
	char obj_name[255];
	char issuing_org[255];
	int adm_type_null = 1;
	int adm_enum_null = 1;
	int issuing_org_null = 1;
	//int obj_name_null = 1;
	int obj_enum_null = 1;
	int tnvc_id_null = 1;


	char *iptr = PQgetvalue(res, 0, ari_type_fnum);
	ari_type = ntohl(*((uint32_t *) iptr));
	
	if(PQgetisnull(res, 0, adm_type_fnum)){
		iptr = PQgetvalue(res, 0, adm_type_fnum);
		adm_type = ntohl(*((uint32_t *) iptr));
		adm_type_null = 0;
	}
	
	if(PQgetisnull(res, 0, adm_enum_fnum)){
		iptr = PQgetvalue(res, 0, adm_enum_fnum);
		adm_enum = ntohl(*((uint32_t *) iptr));
		adm_enum_null = 0;
	}

	if(PQgetisnull(res, 0, obj_enum_fnum)){
		iptr = PQgetvalue(res, 0, obj_enum_fnum);
		obj_enum = ntohl(*((uint32_t *) iptr));
		obj_enum_null = 0;
	}

	if(PQgetisnull(res, 0, tnvc_id_fnum)){
		iptr = PQgetvalue(res, 0, tnvc_id_fnum);
		tnvc_id = ntohl(*((uint32_t *) iptr));
		tnvc_id_null = 0;
	}

	strncpy(obj_name, PQgetvalue(res, 0, obj_name_fnum), 254); // -1 to accomodate NULL-character

	if(PQgetisnull(res, 0, issuing_org_fnum)){
		strncpy(issuing_org, PQgetvalue(res, 0, issuing_org_fnum), 254); // -1 to accomodate NULL-character
		issuing_org_null = 0;
	}

	PQclear(res);
	#endif // HAVE_POSTGRESQL
		
	// Build ARI	
	
	if (ari_type == AMP_TYPE_LIT) // TODO
	{
		AMP_DEBUG_ERR(__FUNCTION__, "TODO: ARI LIT", NULL);
		return NULL;
	}

	ari = ari_create(ari_type);
	CHKNULL(ari);

	// ARI Type
	ari->type = ari_type;
	ARI_SET_FLAG_TYPE(ari->as_reg.flags, ari_type);

	// Nickname
	// namespace/20 + adm_type
	// adm_enum    adm_Type
	#ifdef HAVE_MYSQL
 	if (!is_null[C_ADM_TYPE] && !is_null[C_ADM_ENUM])
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (!adm_type_null && !adm_enum_null)
	#endif // HAVE_POSTGRESQL
	{
		temp = (adm_enum*20) + adm_type;

		VDB_ADD_NN(temp, &(ari->as_reg.nn_idx));
		ARI_SET_FLAG_NN(ari->as_reg.flags);
	#ifdef HAVE_MYSQL
 	} else if (!is_null[C_ISSUING_ORG] ) {
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	} else if (!issuing_org_null) {
	#endif // HAVE_POSTGRESQL
		// Issuer is only set if Nickname is excluded
		blob_t *issuer = utils_string_to_hex(issuing_org);
		ARI_SET_FLAG_ISS(ari->as_reg.flags);
		VDB_ADD_ISS(*issuer, &(ari->as_reg.iss_idx));
		blob_release(issuer,0);
	}

	// Name
	#ifdef HAVE_MYSQL
	if (!is_null[C_OBJ_ENUM]) {
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
if (!obj_enum_null) {
#endif // HAVE_POSTGRESQL

		cut_enc_uvast(obj_enum, &(ari->as_reg.name));
	} else {
		blob_t *name = utils_string_to_hex(obj_name);
		ari->as_reg.name = *name;
		blob_release(name,0);
	}

	
	// Tag
	// TODO

	// Parameters
	#ifdef HAVE_MYSQL
 	if (!is_null[C_TNVC_ID])
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (!tnvc_id_null)
	#endif // HAVE_POSTGRESQL
	{
		ARI_SET_FLAG_PARM(ari->as_reg.flags);
		if (db_query_tnvc(dbidx, tnvc_id, &(ari->as_reg.parms)) != AMP_OK) {
			ari_release(ari,1);
			return NULL;
		}
	}


	return ari;
}

ac_t* db_query_ac(size_t dbidx, int ac_id)
{
	ac_t *ac = ac_create();
	
	// Query AC
    // Note: While we can query ARI contents as part of a single
	// query, we split into discrete queries to simplify C API when a
	// single ARI may need to be retrieved
	dbprep_declare(dbidx, AC_GET, 1, 1);
	dbprep_bind_param_int(0,ac_id);

	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);

	dbprep_dec_res_int(0, ari_id);
	
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);
	mysql_stmt_store_result(stmt); // Results must be buffered to allow execution of nested queries
	size_t nrows = mysql_stmt_num_rows(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKNULL(dbtest_result(PGRES_TUPLES_OK))
	int nrows = PQntuples(res);
	#endif // HAVE_POSTGRESQL

	int *ari_ids = STAKE(nrows * sizeof(int) );

	// Buffer ARI IDs (to avoid recursion issues with prepared statements)
	#ifdef HAVE_MYSQL
 	for(int i = 0; !mysql_stmt_fetch(stmt); i++)
 	{
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	for(int i = 0; i < nrows; i++)
	{
		char *iptr = PQgetvalue(res, i, 0);
		int ari_id = ntohl(*((uint32_t *) iptr)); 
	#endif // HAVE_POSTGRESQL
		ari_ids[i] = ari_id;
	}
	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	// Query details
	for(int i = 0; i < nrows; i++)
	{
		ari_t *ari = db_query_ari(dbidx, ari_ids[i]);
		if (ari == NULL) {
			#ifdef HAVE_MYSQL
 			mysql_stmt_free_result(stmt);
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL		
			PQclear(res);  // TODO postgresql : unclear why this is here as the mysql_stmt_free_result and PQclear 
			#endif // HAVE_POSTGRESQL
			ac_release(ac,1);
			return NULL;
		}
		ac_insert(ac, ari);
	}

	SRELEASE(ari_ids);
	
	return ac;
}

msg_ctrl_t* db_tx_build_perf_ctrl(int msg_id, int ack, int nak, int acl)
{
	msg_ctrl_t *ctrl = NULL;

	// Query PerfCtrl Record
	dbprep_declare(DB_CTRL_CON, MSGS_PERF_CTRL_GET, 1, 2);
	dbprep_bind_param_int(0,msg_id);
	#ifdef HAVE_MYSQL
	DB_CHKINT(mysql_stmt_bind_param(stmt, bind_param));

	dbprep_dec_res_int(0,tv);
	dbprep_dec_res_int(1,ac_id);
	
	DB_CHKINT(mysql_stmt_execute(stmt));
	DB_CHKINT(mysql_stmt_bind_result(stmt, bind_res));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKNULL(dbtest_result(PGRES_TUPLES_OK))
	int nrows = PQntuples(res);
	#endif // HAVE_POSTGRESQL

	#ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0)
	{
		// Failed to retrieve resullt (single row expected here)
		AMP_DEBUG_ERR("db_tx_msg_groups","Failed to query PerfCtrl",NULL);
		return NULL;
	}
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (nrows == 0)
	{
		// Failed to retrieve resullt (single row expected here)
		AMP_DEBUG_ERR("db_tx_msg_groups","Failed to query PerfCtrl",NULL);
		PQclear(res);
		return NULL;
	}
	#endif // HAVE_POSTGRESQL
 
	// Free result now that we retrieved row
	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
	int ac_id = ntohl(*((uint32_t *) iptr));
	//TODO postgresql : read result tv
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	// Build Control
	ctrl = msg_ctrl_create();

	// AC
	ctrl->ac = db_query_ac(DB_CTRL_CON, ac_id);

	if (ctrl->ac == NULL) {
		msg_ctrl_release(ctrl,1);
		return NULL;
	}

	// Timestamp
	// TODO:  ctrl->start = conversion(tv) if not NULL

	// Set Flags
	if (ack) {
		ctrl->hdr.flags = MSG_HDR_SET_ACK(ctrl->hdr.flags);
	}
	if (nak) {
		ctrl->hdr.flags = MSG_HDR_SET_NACK(ctrl->hdr.flags);
	}
	if (acl) {
		ctrl->hdr.flags = MSG_HDR_SET_ACL(ctrl->hdr.flags);
	}
	

	return ctrl;
}


/******************************************************************************
 *
 * \par Function Name: db_tx_build_group
 *
 * \par This function populates an AMP message group with messages
 *      for this group from the database.
 *
 * \retval AMP_SYSERR on system error
 *         AMP_FAIL   if no message groups ready.
 *         AMP_OK     If there are message groups ready to be sent.
 *
 * \param[in]  grp_idx   - The DB identifier of the message group
 * \param[out] msg_group - The message group being populated
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/13/13  E. Birrane      Initial implementation,
 *  09/27/13  E. Birrane      Collect any rpt defs from this message.
 *  08/27/15  E. Birrane      Update to latest data model and schema.
 *  01/26/17  E. Birrane      Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_tx_build_group(int32_t grp_idx, msg_grp_t *msg_group)
{
	msg_ctrl_t* ctrl;
	int32_t result = AMP_OK;
	dbprep_declare(DB_CTRL_CON, MSGS_ENTRIES_GET, 1, 7);
	int r_grp_id, r_msg_id, r_ack, r_nak, r_acl, r_order_num, r_type_id;

	// Bind parameters
	dbprep_bind_param_int(0,grp_idx);
	#ifdef HAVE_MYSQL
	DB_CHKINT(mysql_stmt_bind_param(stmt, bind_param));

    // Declare result columns	
	dbprep_bind_res_int(0,r_grp_id);
	dbprep_bind_res_int(1,r_msg_id);
	dbprep_bind_res_int(2,r_ack);
	dbprep_bind_res_int(3,r_nak);
	dbprep_bind_res_int(4,r_acl);
	dbprep_bind_res_int(5,r_order_num);
	dbprep_bind_res_int(6,r_type_id);
	#endif // HAVE_MYSQL
	
	AMP_DEBUG_ENTRY("db_tx_build_group",
					  "(%d, %p)",
			          grp_idx, msg_group);

	/* Step 0: Sanity check. */
	if(msg_group == NULL)
	{
		AMP_DEBUG_ERR("db_tx_build_group","Bad args.", NULL);
		AMP_DEBUG_EXIT("db_tx_build_group","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 1: Find all messages for this outgoing group. */
	#ifdef HAVE_MYSQL
	// Execute
	DB_CHKINT(mysql_stmt_execute(stmt));

	// Bind results
	DB_CHKINT(mysql_stmt_bind_result(stmt, bind_res));
	DB_CHKINT(mysql_stmt_store_result(stmt)); // Results must be buffered to allow execution of nested queries
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
	int nrows = PQntuples(res);
	/* Use PQfnumber to avoid assumptions about field order in result */
	int grp_id_fnum = PQfnumber(res, "group_id");
    int msg_id_fnum = PQfnumber(res, "message_id");
    int ack_fnum = PQfnumber(res, "ack");
    int nak_fnum = PQfnumber(res, "nack");
    int acl_fnum = PQfnumber(res, "acl");
    int order_num_fnum = PQfnumber(res, "order_num");
    int type_id_fnum = PQfnumber(res, "type_id");
	#endif // HAVE_POSTGRESQL

	/* Step 2: For each message that belongs in this group....*/
		#ifdef HAVE_MYSQL
 	while(!mysql_stmt_fetch(stmt))
	{
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	for(int i = 0; i < nrows; i++)
	{
		//TODO postgresql : read result r_grp_id
		char *iptr = PQgetvalue(res, i, msg_id_fnum);
		r_msg_id = ntohl(*((uint32_t *) iptr));
		iptr = PQgetvalue(res, i, ack_fnum);
		r_ack = ntohl(*((uint32_t *) iptr)); 
		iptr = PQgetvalue(res, i, nak_fnum);
		r_nak = ntohl(*((uint32_t *) iptr)); 
		iptr = PQgetvalue(res, i, acl_fnum);
		r_acl = ntohl(*((uint32_t *) iptr)); 
		//TODO postgresql : read result r_order_num
		iptr = PQgetvalue(res, i, type_id_fnum);
		r_type_id = ntohl(*((uint32_t *) iptr)); 
	#endif // HAVE_POSTGRESQL

		// NOTE: Support for other types can be added here in the future if needed, for debugging or other uses.
		switch(r_type_id) {
		case MSG_TYPE_PERF_CTRL:
			ctrl = db_tx_build_perf_ctrl(r_msg_id, r_ack, r_nak, r_acl);
			if (ctrl == NULL) {
				// Set return code to AMP_FAIL.
				// Note: This will cause group to be marked error in DB, but we will still attempt to add other messages in the group (if any).
				result = AMP_FAIL;
			} else {
				msg_grp_add_msg_ctrl(msg_group, ctrl);
			}
			break;
		default:
			AMP_DEBUG_ERR("db_tx_build_group","Only Tx of Controls from the Manager is supported at this time", NULL);
			continue;
		}
	}

	if (vec_num_entries(msg_group->msgs) == 0) {
		// If this set is empty (or has no valid messages), it is a failure
		AMP_DEBUG_ERR("db_tx_build_group", "No valid entries in this group",NULL);
		result = AMP_FAIL;
	}
	
	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	AMP_DEBUG_EXIT("db_tx_build_group","-->%d", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_tx_collect_agents
 *
 * \par Returns a vector of the agents to send a message to
 *
 * \retval NULL no recipients.
 *        !NULL There are recipients to be sent to.
 *
 * \param[in] grp_idx - The index of the message group being sent.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/18/13  S. Jacobs       Initial Implementation
 *  01/26/17  E. Birrane      Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int db_tx_collect_agents(int32_t grp_idx, vector_t *vec)
{
  #ifdef HAVE_MYSQL
 	MYSQL_RES *res = NULL;
 	MYSQL_ROW row;
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PGresult * res = NULL;
	#endif // HAVE_POSTGRESQL
	agent_t *agent = NULL;
	int cur_row = 0;
	int max_row = 0;
	int success;

	AMP_DEBUG_ENTRY("db_tx_collect_agents","(%d)", grp_idx);

	/*
	 * Step 1: Grab the list of agents from the DB for this
	 *         message group.
	 */

	if(db_mgt_query_fetch(&res,
			              "SELECT AgentID FROM dbtOutgoingRecipients "
			              "WHERE OutgoingID=%d", grp_idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_tx_collect_agents",
				        "Can't get agents for grp: %d", grp_idx);
		AMP_DEBUG_EXIT("db_tx_collect_agents","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}


	/* Step 3: For each row returned.... */
	#ifdef HAVE_MYSQL
 	max_row = mysql_num_rows(res);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	max_row = PQntuples(res);
	#endif // HAVE_POSTGRESQL
	*vec = vec_create(1, NULL, NULL, NULL, 0, &success);
	for(cur_row = 0; cur_row < max_row; cur_row++)
	{
		#ifdef HAVE_MYSQL
		if ((row = mysql_fetch_row(res)) != NULL)
		{
			/* Step 3.1: Grab the agent information.. */
			if((agent = db_fetch_agent(atoi(row[0]))) != NULL)
		#endif // HAVE_MYSQL
 		#ifdef HAVE_POSTGRESQL
			/* Step 3.1: Grab the agent information.. */
			if((agent = db_fetch_agent(atoi(PQgetvalue(res, cur_row, 0)))) != NULL)
		#endif // HAVE POSTGRESQL
			{
				AMP_DEBUG_INFO("db_outgoing_process_recipients",
						         "Adding agent name %s.",
						         agent->eid.name);

				vec_push(vec, agent);
			}
			else
			{
				AMP_DEBUG_ERR("db_outgoing_process_recipients",
						        "Cannot fetch registered agent",NULL);
			}
			#ifdef HAVE_MYSQL
		}
		#endif // HAVE_MYSQL
	}

	#ifdef HAVE_MYSQL
 	mysql_free_result(res);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	AMP_DEBUG_EXIT("db_outgoing_process_recipients","-->0x%#llx",
			         vec_num_entries(*vec));

	return AMP_OK;
}



/******************************************************************************
 *
 * \par Function Name: db_fetch_reg_agent
 *
 * \par Creates an adm_reg_agent_t structure from the database.
 *
 * \retval NULL Failure
 *        !NULL The built adm_reg_agent_t  structure.
 *
 * \param[in] id - The Primary Key of the desired registered agent.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

agent_t *db_fetch_agent(int32_t id)
{
	agent_t *result = NULL;
	#ifdef HAVE_MYSQL
 	MYSQL_RES *res = NULL;
 	MYSQL_ROW row;
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PGresult * res = NULL;
	#endif // HAVE_POSTGRESQL

	AMP_DEBUG_ENTRY("db_fetch_agent","(%d)", id);

	/* Step 1: Grab the OID row. */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM registered_agents WHERE registered_agents_id=%d",
						  id) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_agent","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_agent","-->NULL", NULL);
		return NULL;
	}

	#ifdef HAVE_MYSQL
 	if ((row = mysql_fetch_row(res)) != NULL)
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
    int name_fnum = PQfnumber(res, "agent_id_string");
	if (PQntuples(res) != 0)
	#endif // HAVE_POSTGRESQL
 	{
 		eid_t eid;
		#ifdef HAVE_MYSQL
 		strncpy(eid.name, row[1], AMP_MAX_EID_LEN-1); // -1 to accomodate NULL-character
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		strncpy(eid.name, PQgetvalue(res, 0, name_fnum), AMP_MAX_EID_LEN-1); // -1 to accomodate NULL-character
		#endif // HAVE POSTGRESQL

		/* Step 3: Create structure for agent */
		if((result = agent_create(&eid)) == NULL)
		{
			AMP_DEBUG_ERR("db_fetch_agent","Cannot create a registered agent",NULL);
			#ifdef HAVE_MYSQL
 			mysql_free_result(res);
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			PQclear(res);
			#endif // HAVE_POSTGRESQL
			AMP_DEBUG_EXIT("db_fetch_agent","-->NULL", NULL);
			return NULL;
		}
	}

	#ifdef HAVE_MYSQL
 			mysql_free_result(res);
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			PQclear(res);
			#endif // HAVE_POSTGRESQL

	AMP_DEBUG_EXIT("db_fetch_agent", "-->%p", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_fetch_reg_agent_idx
 *
 * \par Retrieves the index associated with an agent's EID.
 *
 * \retval 0 Failure
 *        !0 The index of the agent.
 *
 * \param[in] eid - The EID of the agent being queried.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/29/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_fetch_agent_idx(eid_t *eid)
{
	int32_t result = AMP_FAIL;
	#ifdef HAVE_MYSQL
 	MYSQL_RES *res = NULL;
 	MYSQL_ROW row;
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PGresult * res = NULL;
	#endif // HAVE_POSTGRESQL

	AMP_DEBUG_ENTRY("db_fetch_agent_idx","(%p)", eid);

	/* Step 0: Sanity Check.*/
	if(eid == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_agent_idx","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_fetch_agent_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 1: Grab the OID row. */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM registered_agents WHERE agent_id_string='%s'",
						  eid->name) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_agent_idx","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_agent_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: Parse information out of the returned row. */
	#ifdef HAVE_MYSQL
 	if ((row = mysql_fetch_row(res)) != NULL)
 	{
 		result = atoi(row[0]);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
    int agent_id_fnum = PQfnumber(res, "registered_agents_id");
	if (PQntuples(res) != 0)
	{
		result = atoi(PQgetvalue(res, 0, agent_id_fnum));
	#endif // HAVE_POSTGRESQL
	}
	else
	{
		AMP_DEBUG_ERR("db_fetch_agent_idx", "Did not find EID with ID of %s\n", eid->name);
	}

	/* Step 3: Free database resources. */
	#ifdef HAVE_MYSQL
 			mysql_free_result(res);
			#endif // HAVE_MYSQL
			#ifdef HAVE_POSTGRESQL
			PQclear(res);
			#endif // HAVE_POSTGRESQL

	AMP_DEBUG_EXIT("db_fetch_agent_idx","-->%d", result);
	return result;
}



#if 0


//TODO - Add transactions
//TODO - Make -1 the system error return and 0 the non-system-error return.
//TODO - Update the comments.

/******************************************************************************
 *
 * \par Function Name: db_add_adm()
 *
 * \par Adds an ADM to the DB list of supported ADMs.
 *
 * Tables Effected:
 *    1. dbtADMs
 *
 * +---------+------------------+------+-----+---------+----------------+
 * | Field   | Type             | Null | Key | Default | Extra          |
 * +---------+------------------+------+-----+---------+----------------+
 * | ID      | int(10) unsigned | NO   | PRI | NULL    | auto_increment |
 * | Label   | varchar(255)     | NO   | UNI |         |                |
 * | Version | varchar(255)     | NO   |     |         |                |
 * | OID     | int(10) unsigned | NO   | MUL | NULL    |                |
 * +---------+------------------+------+-----+---------+----------------+
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in]  name     - The name of the ADM.
 * \param[in]  version  - Version of the ADM.
 * \param[in]  oid_root - ADM root OID.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/29/15  E. Birrane     Initial implementation,
 *  04/02/16  E. Birrane     Check connection
 *  01/24/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/

int32_t db_add_adm(char *name, char *version, char *oid_root)
{
	uint32_t oid_idx = 0;
	uint32_t row_idx = 0;

	AMP_DEBUG_ENTRY("db_add_adm,%p,%p,%p)",
			        name, version, oid_root);

	/* Step 0: Sanity check. */
	if((name == NULL) || (version == NULL) || (oid_root == NULL))
	{
		AMP_DEBUG_ERR("db_add_adm","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_add_adm","-->0",NULL);
		return AMP_FAIL;
	}

	/*
	 * Step 1: If the adm is already in the DB, return the index.
	 *         If there was a system error, return that.
	 */
	if((row_idx = db_fetch_adm_idx(name, version)) != 0)
	{
		return row_idx;
	}

	db_mgt_txn_start();

	/* Step 2 - Put the OID in the Database and save the index. */
	if((oid_idx = db_add_oid_str(oid_root)) <= 0)
	{
		AMP_DEBUG_ERR("db_add_adm","Can't add ADM OID to DB.",NULL);

		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_adm","-->%d",oid_idx);
		return oid_idx;
	}

	/* Step 3: Write the ADM entry into the DB. */
	if(db_mgt_query_insert(&row_idx, "INSERT INTO dbtADMs(Label, Version, OID) "
			"VALUES('%s','%s',%d)", name, version, oid_idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_adm","Can't add ADM to DB.",NULL);

		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_adm","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	db_mgt_txn_commit();
	AMP_DEBUG_EXIT("db_add_adm", "-->%d", row_idx);
	return row_idx;
}





/******************************************************************************
 *
 * \par Function Name: db_add_tdc
 *
 * \par Adds a TDC to the database and returns the index of the
 *      parameters table.
 *
 * Tables Effected:
 *    1. dbtDataCollections
 *
 *   +-------+------------------+------+-----+-------------------------+----------------+
 *   | Field | Type             | Null | Key | Default                 | Extra          |
 *   +-------+------------------+------+-----+-------------------------+----------------+
 *   | ID    | int(10) unsigned | NO   | PRI | NULL                    | auto_increment |
 *   | Label | varchar(255)     | NO   |     | Unnamed Data Collection |                |
 *   +-------+------------------+------+-----+-------------------------+----------------+
 *
 *
 *    2. dbtDataCollection
 *
 *   +--------------+------------------+------+-----+---------+-------+
 *   | Field        | Type             | Null | Key | Default | Extra |
 *   +--------------+------------------+------+-----+---------+-------+
 *   | CollectionID | int(10) unsigned | NO   | PRI | NULL    |       |
 *   | DataOrder    | int(10) unsigned | NO   | PRI | 0       |       |
 *   | DataType     | int(10) unsigned | NO   | MUL | NULL    |       |
 *   | DataBlob     | blob             | YES  |     | NULL    |       |
 *   +--------------+------------------+------+-----+---------+-------+
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in]  tdc  - The TDC being added to the DB.
 *
 * \par Notes:
 *		- Comments for the dc are not included.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/22/15  E. Birrane     Initial Implementation
 *  09/10/15  E. Birrane     Update to to db spec.
 *  11/12/16  E. Birrane     Update to new schema. Optimizations. (JHU/APL)
 *  01/24/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/

int32_t db_add_tdc(tdc_t tdc)
{
	uint32_t tdc_idx = 0;
	LystElt elt;

	AMP_DEBUG_ENTRY("db_add_tdc", "(%d)", tdc.hdr.length);

	/* Step 0: Sanity check arguments. */
	if(tdc.hdr.length == 0)
	{
		AMP_DEBUG_WARN("db_add_tdc","Not persisting empty TDC",NULL);
		AMP_DEBUG_EXIT("db_add_tdc","-->0",NULL);
		return AMP_FAIL;
	}

	db_mgt_txn_start();

	/*
	 * Step 1: Build and execute query to add row to dbtDataCollections. Also, store the
	 *         new DC index.
	 */

	if(db_mgt_query_insert(&tdc_idx, "INSERT INTO dbtDataCollections (Label) "
			"VALUE(NULL)") != AMP_OK)
	{
		AMP_DEBUG_WARN("db_add_tdc","Can't insert TDC",NULL);
		db_mgt_txn_rollback();
		AMP_DEBUG_EXIT("db_add_tdc","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	/*
	 *  Step 2: For each BLOB in the data collection, add it to the data collection
	 *          entries table.
	 */
	for(elt = lyst_first(tdc.datacol); elt; elt = lyst_next(elt))
	{
		blob_t *entry = (blob_t *) lyst_data(elt);
		int i = 0;
		char *content = NULL;
		uint32_t content_len = 0;
		uint32_t dc_idx = 0;

		/* Step 2.1: Built sting version of the BLOB data. */
		if((content = utils_hex_to_string(entry->value, entry->length)) == NULL)
		{
			AMP_DEBUG_ERR("db_add_tdc","Can't cvt %d bytes to hex str.", entry->length);

			db_mgt_txn_rollback();

			AMP_DEBUG_EXIT("db_add_tdc", "-->%d", AMP_FAIL);
			return AMP_FAIL;
		}

		/* Step 2.2: Write the data into the DC collections table and associated
		 *           the entry with the data collection idx.
		 *           NOTE: content+2 is used to "skip over" the leading "0x"
		 *           characters that preface the contents information.
		 */

		if(db_mgt_query_insert(&dc_idx,
				           "INSERT INTO dbtDataCollection(CollectionID, DataOrder, DataType,DataBlob)"
		 	                "VALUES(%d,1,%d,'%s')",
							tdc_idx,
							tdc.hdr.data[i],
							content+2) != AMP_OK)
		{
			AMP_DEBUG_ERR("db_add_tdc","Can't insert entry %d.", i);
			SRELEASE(content);
			db_mgt_txn_rollback();

			AMP_DEBUG_EXIT("db_add_tdc", "-->%d", AMP_FAIL);
			return AMP_FAIL;
		}

		i++;
		SRELEASE(content);
	}

	db_mgt_txn_commit();
	AMP_DEBUG_EXIT("db_add_tdc", "-->%d", tdc_idx);
	return tdc_idx;
}



/******************************************************************************
 *
 * \par Function Name: db_add_mid
 *
 * \par Creates a MID in the database.
 *
 * Tables Effected:
 *    1. dbtMIDs
 *
 *    +--------------+---------------------+------+-----+--------------------+----------------+
 *    | Field        | Type                | Null | Key | Default            | Extra          |
 *    +--------------+---------------------+------+-----+--------------------+----------------+
 *    | ID           | int(10) unsigned    | NO   | PRI | NULL               | auto_increment |
 *    | NicknameID   | int(10) unsigned    | YES  | MUL | NULL               |                |
 *    | OID          | int(10) unsigned    | NO   | MUL | NULL               |                |
 *    | ParametersID | int(10) unsigned    | YES  | MUL | NULL               |                |
 *    | Type         | int(10) unsigned    | NO   | MUL | NULL               |                |
 *    | Category     | int(10) unsigned    | NO   | MUL | NULL               |                |
 *    | IssuerFlag   | bit(1)              | NO   |     | b'0'               |                |
 *    | TagFlag      | bit(1)              | NO   |     | b'0'               |                |
 *    | OIDType      | int(10) unsigned    | YES  | MUL | NULL               |                |
 *    | IssuerID     | bigint(20) unsigned | NO   |     | 0                  |                |
 *    | TagValue     | bigint(20) unsigned | NO   |     | 0                  |                |
 *    | DataType     | int(10) unsigned    | NO   | MUL | NULL               |                |
 *    | Name         | varchar(50)         | NO   |     | Unnamed MID        |                |
 *    | Description  | varchar(255)        | NO   |     | No MID Description |                |
 *    +--------------+---------------------+------+-----+--------------------+----------------+
 *
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] mid     - The MID to be persisted in the DB.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  08/23/15  E. Birrane     Update to new DB Schema.
 *  01/24/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/
int32_t db_add_mid(mid_t *mid)
{
	int32_t nn_idx = 0;
	int32_t oid_idx = 0;
	int32_t parm_idx = 0;
	int32_t num_parms = 0;
	uint32_t mid_idx = 0;

	AMP_DEBUG_ENTRY("db_add_mid", "(%p)", mid);

	/* Step 0: Sanity check arguments. */
	if(mid == NULL)
	{
		AMP_DEBUG_ERR("db_add_mid","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_mid","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 1: Make sure the ID is not already in the DB (or not failure). */
	if ((mid_idx = db_fetch_mid_idx(mid)) != 0)
	{
		AMP_DEBUG_WARN("db_add_mid","MID already exists.",NULL);
		AMP_DEBUG_EXIT("db_add_mid", "-->%d", mid_idx);
		return mid_idx;
	}

	/* Step 2: If this MID has a nickname, grab the index. */
	if((MID_GET_FLAG_OID(mid->flags) == OID_TYPE_COMP_FULL) ||
	   (MID_GET_FLAG_OID(mid->flags) == OID_TYPE_COMP_PARAM))
	{
		if((nn_idx = db_fetch_nn_idx(mid->oid.nn_id)) <= 0)
		{
			AMP_DEBUG_ERR("db_add_mid","MID references unknown Nickname %d", mid->oid.nn_id);
			AMP_DEBUG_EXIT("db_add_mid", "-->%d", nn_idx);
			return nn_idx;
		}
	}

	db_mgt_txn_start();

	/* Step 3: Get the index for the OID. */
	if((oid_idx = db_add_oid(mid->oid)) <= 0)
	{
		AMP_DEBUG_ERR("db_add_mid", "Can't add OID.", NULL);
		AMP_DEBUG_EXIT("db_add_mid", "-->%d", oid_idx);
		return oid_idx;
	}

	/* Step 4: Get the index for parameters, if any. */
	if((num_parms = oid_get_num_parms(mid->oid)) > 0)
	{
		if((parm_idx = db_add_parms(mid->oid)) <= 0)
		{
			AMP_DEBUG_ERR("db_add_mid", "Can't add PARMS.", NULL);

			db_mgt_txn_rollback();

			AMP_DEBUG_EXIT("db_add_mid", "-->%d", parm_idx);
			return parm_idx;
		}
	}

	/*
	 * Step 5: Build and execute query to add row to dbtMIDs. Also, store the
	 *         row ID of the inserted row.
	 */

	if(db_mgt_query_insert(&mid_idx,
			            "INSERT INTO dbtMIDs(NicknameID,OID,ParametersID,Type,Category,IssuerFlag,TagFlag,"
			            "OIDType,IssuerID,TagValue,DataType,Name,Description)"
			            "VALUES (%s, %d, %s, %d, %d, %d, %d, %d, "UVAST_FIELDSPEC","UVAST_FIELDSPEC",%d,'%s','%s')",
			            (nn_idx == 0) ? "NULL" : itoa(nn_idx),
			            oid_idx,
			            (parm_idx == 0) ? "NULL" : itoa(parm_idx),
			            0,
			            MID_GET_FLAG_ID(mid->flags),
			            (MID_GET_FLAG_ISS(mid->flags)) ? 1 : 0,
			            (MID_GET_FLAG_TAG(mid->flags)) ? 1 : 0,
			            MID_GET_FLAG_OID(mid->flags),
			            mid->issuer,
			            mid->tag,
			            AMP_TYPE_MID,
			            "No Name",
			            "No Descr") != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_mid", "Can't add MID.", NULL);

		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_mid", "-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	db_mgt_txn_commit();

	AMP_DEBUG_EXIT("db_add_mid", "-->%d", mid_idx);
	return mid_idx;
}




/******************************************************************************
 *
 * \par Function Name: db_add_mc
 *
 * \par Creates a MID Collection in the database.
 *
 * Tables Effected:
 *    1. dbtMIDCollections
 *
 *   +---------+------------------+------+-----+---------+----------------+
 *   | Field   | Type             | Null | Key | Default | Extra          |
 *   +---------+------------------+------+-----+---------+----------------+
 *   | ID      | int(10) unsigned | NO   | PRI | NULL    | auto_increment |
 *   | Comment | varchar(255)     | YES  |     | NULL    |                |
 *   +---------+------------------+------+-----+---------+----------------+

 *       +---------------+------------+---------------+-----------------------+
 *       | Column Object |     Type   | Default Value | Comment               |
 *       +---------------+------------+---------------+-----------------------+
 *       |      ID*      | Int32      | auto-         | Used as primary key   |
 *       |               |(unsigned)  | incrementing  |                       |
 *       +---------------+------------+---------------+-----------------------+
 *       | Comment       |VARCHAR(255)| ''            | Optional Comment      |
 *       +---------------+------------+---------------+-----------------------+
 *
 *    2. dbtMIDCollection
 *       +---------------+------------+---------------+-----------------------+
 *       | Column Object |     Type   | Default Value | Comment               |
 *       +---------------+------------+---------------+-----------------------+
 *       | CollectionID  | Int32      | 0             | Foreign key into      |
 *       |               |(unsigned)  |               | dbtMIDCollection.ID   |
 *       +---------------+------------+---------------+-----------------------+
 *       | MIDID         | Int32      | 0             | Foreign key into      |
 *       |               | (unsigned) |               | dbtMIDs.ID            |
 *       +---------------+------------+---------------+-----------------------+
 *       | MIDOrder      | Int32      | 0             | Order of MID in the   |
 *       |               | (unsigned) |               | Collection.           |
 *       +---------------+------------+---------------+-----------------------+
 *
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] mc     - The MC being added to the DB.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/23/15  E. Birrane     Initial Implementation
 *  01/25/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/
int32_t db_add_mc(Lyst mc)
{
	uint32_t mc_idx = 0;
	LystElt elt = NULL;
	mid_t *mid = NULL;
	uint32_t i = 0;
	int32_t mid_idx = 0;

	AMP_DEBUG_ENTRY("db_add_mc", "(%p)", mc);

	/* Step 0 - Sanity check arguments. */
	if(mc == NULL)
	{
		AMP_DEBUG_ERR("db_add_mc","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_mc","-->%d", AMP_SYSERR);
		return AMP_SYSERR;
	}

	db_mgt_txn_start();

	/* Step 1 - Create a new entry in the dbtMIDCollections DB. */
	if(db_mgt_query_insert(&mc_idx,
			            "INSERT INTO dbtMIDCollections (Comment) VALUES ('No Comment')") != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_mc","Can't insert MC",NULL);

		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_mc","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2 - For each MID in the MC, add the MID into the dbtMIDCollection. */
	for(elt = lyst_first(mc); elt; elt = lyst_next(elt))
	{
		/* Step 2a: Extract nth MID from MC. */
		if((mid = (mid_t *) lyst_data(elt)) == NULL)
		{
			AMP_DEBUG_ERR("db_add_mc","Can't get MID.", NULL);

			db_mgt_txn_rollback();
			AMP_DEBUG_EXIT("db_add_mc", "-->%d", AMP_SYSERR);
			return AMP_SYSERR;
		}

		/* Step 2b: Make sure MID is in the DB. */
		if((mid_idx = db_add_mid(mid)) > 0)
		{
			AMP_DEBUG_ERR("db_add_mc","MID not there and can't insert.", NULL);

			db_mgt_txn_rollback();
			AMP_DEBUG_EXIT("db_add_mc", "-->%d", mid_idx);
			return mid_idx;
		}

		/* Step 2c - Insert entry into DB MC list from this MC. */
		if(db_mgt_query_insert(NULL,
				            "INSERT INTO dbtMIDCollection"
							"(CollectionID, MIDID, MIDOrder)"
							"VALUES (%d, %d, %d",
							mc_idx, mid_idx, i) != AMP_OK)
		{
			AMP_DEBUG_ERR("db_add_mc","Can't insert MID %d", i);

			db_mgt_txn_rollback();

			AMP_DEBUG_EXIT("db_add_mc","-->%d", AMP_FAIL);
			return AMP_FAIL;
		}

		i++;
	}

	db_mgt_txn_commit();

	AMP_DEBUG_EXIT("db_add_mc", "-->%d", mc_idx);
	return mc_idx;
}



/******************************************************************************
 *
 * \par Function Name: db_add_nn
 *
 * \par Creates a Nickname in the database.
 *
 * Tables Effected:
 *
 *    1. dbtMIDCollections
 *
 * +----------------+------------------+------+-----+--------------------------+----------------+
 * | Field          | Type             | Null | Key | Default                  | Extra          |
 * +----------------+------------------+------+-----+--------------------------+----------------+
 * | ID             | int(10) unsigned | NO   | PRI | NULL                     | auto_increment |
 * | ADM_ID         | int(10) unsigned | NO   | MUL | NULL                     |                |
 * | Nickname_UID   | int(10) unsigned | NO   |     | NULL                     |                |
 * | Nickname_Label | varchar(25)      | NO   |     | Undefined ADM Tree Value |                |
 * | OID            | int(10) unsigned | NO   | MUL | NULL                     |                |
 * +----------------+------------------+------+-----+--------------------------+----------------+
 *
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] nn     - The Nickname being added to the DB.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/29/15  E. Birrane     Initial Implementation
 *  01/25/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/

int32_t db_add_nn(oid_nn_t *nn)
{
	uint32_t nn_idx = 0;
	oid_t oid;
	int32_t oid_idx = 0;
	int32_t adm_idx = 0;

	AMP_DEBUG_ENTRY("db_add_nn", "(%p)", nn);

	/* Step 0 - Sanity check arguments. */
	if(nn == NULL)
	{
		AMP_DEBUG_ERR("db_add_nn","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_nn","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	/*
	 * Step 1 - Duplicate check.
	 */
	if((nn_idx = db_fetch_nn_idx(nn->id)) > 0)
	{
		AMP_DEBUG_EXIT("db_add_nn","-->%d", nn_idx);
		return nn_idx;
	}

	db_mgt_txn_start();

	/* Step 2 - Ensure OID. */
	oid = oid_construct(OID_TYPE_FULL, NULL, 0, nn->raw, nn->raw_size);
	if( (oid.type == OID_TYPE_UNK) ||
	    ((oid_idx = db_add_oid(oid)) <= 0))
	{
		AMP_DEBUG_ERR("db_add_nn","Can't create OID.",NULL);
		oid_release(&oid);
		db_mgt_txn_rollback();
		AMP_DEBUG_EXIT("db_add_nn","-->%d",oid_idx);
		return oid_idx;
	}

	oid_release(&oid);

	/* Step 3 - Add the ADM. */
	if((adm_idx = db_fetch_adm_idx(nn->adm_name, nn->adm_ver)) <= 0)
	{
		AMP_DEBUG_ERR("db_add_nn","Can't Find ADM.",NULL);
		db_mgt_txn_rollback();
		AMP_DEBUG_EXIT("db_add_nn","-->%d",adm_idx);
		return adm_idx;
	}


	/* Step 3 - Create a new entry in the dbtADMNicknames DB. */
	if(db_mgt_query_insert(&nn_idx,
			            "INSERT INTO dbtADMNicknames (ADM_ID, Nickname_UID, Nickname_Label, OID)"
						"VALUES (%d, "UVAST_FIELDSPEC", 'No Comment', %d)",
						adm_idx, nn->id, oid_idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_nn","Can't insert Nickname", NULL);

		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_nn","-->%d", AMP_SYSERR);
		return AMP_SYSERR;
	}

	db_mgt_txn_commit();
	AMP_DEBUG_EXIT("db_add_nn", "-->%d", nn_idx);
	return nn_idx;
}



/******************************************************************************
 *
 * \par Function Name: db_add_oid_str
 *
 * \par Adds an OID to the database given a serialized string rep of the OID.
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] oid_str - The string representation of the OID.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/29/15  E. Birrane     Initial Implementation
 *  01/25/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/

int32_t db_add_oid_str(char *oid_str)
{
	uint8_t *data = NULL;
	uint32_t datasize = 0;
	int32_t result = 0;
	oid_t oid;

	AMP_DEBUG_ENTRY("db_add_oid_str", "(%p)", oid_str);

	/* Step 1: Sanity checks. */
	if(oid_str == NULL)
	{
		AMP_DEBUG_ERR("db_add_oid_str","Bad args", NULL);
		AMP_DEBUG_EXIT("db_add_oid_str","-->%d", AMP_SYSERR);

		return AMP_FAIL;
	}

	/* Step 2: Assume input is not in hex and convert to hex string. */
	if((data = utils_string_to_hex(oid_str,&datasize)) == NULL)
	{
		AMP_DEBUG_ERR("db_add_oid_str","Can't convert OID of %s.", oid_str);
		return AMP_FAIL;
	}

	/* Step 3: Build an OID. */
	oid = oid_construct(OID_TYPE_FULL, NULL, 0, data, datasize);
	SRELEASE(data);

	if(oid.type == OID_TYPE_UNK)
	{
		AMP_DEBUG_ERR("db_add_oid_str","Can't create OID.",NULL);
		SRELEASE(data);
		AMP_DEBUG_EXIT("db_add_oid_str","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	result = db_add_oid(oid);

	oid_release(&oid);

	AMP_DEBUG_EXIT("db_add_oid_str", "-->%d", result);

	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_add_oid()
 *
 * \par Adds an Object Identifier to the database, or returns the index of
 *      a matching object identifier.
 *
 * Tables Effected:
 *    1. dbtOIDs
 *  +-------------+------------------+------+-----+---------------------+----------------+
 *  | Field       | Type             | Null | Key | Default             | Extra          |
 *  +-------------+------------------+------+-----+---------------------+----------------+
 *  | ID          | int(10) unsigned | NO   | PRI | NULL                | auto_increment |
 *  | IRI_Label   | varchar(255)     | NO   | MUL | Undefined IRI value |                |
 *  | Dot_Label   | varchar(255)     | NO   |     | 190.239.254.237     |                |
 *  | Encoded     | varchar(255)     | NO   |     | BEEFFEED            |                |
 *  | Description | varchar(255)     | NO   |     |                     |                |
 *  +-------------+------------------+------+-----+---------------------+----------------+
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in]  oid  - The OID being added to the DB.
 * \param[in]  spec - Listing of types of oid parms, if they exist.
 * \par Notes:
 *		- Only the encoded OID is persisted in the database.
 *		  No other OID information is persisted at this time.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/22/15  E. Birrane     Initial Implementation
 *  01/25/17  E. Birrane     Update to AMP IOS 3.5.0. (JHU/APL)
 *****************************************************************************/

int32_t db_add_oid(oid_t oid)
{
	uint32_t oid_idx = 0;
	int32_t num_parms = 0;
	char *oid_str = NULL;

	AMP_DEBUG_ENTRY("db_add_oid", "(%d)", oid.type);

	/* Step 0: Sanity check arguments. */
	if(oid.type == OID_TYPE_UNK)
	{
		AMP_DEBUG_ERR("db_add_oid","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_oid","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	/*
	 * Step 1: Return existing ID, or failure code.
	 */
	if ((oid_idx = db_fetch_oid_idx(oid)) != 0)
	{
		AMP_DEBUG_EXIT("db_add_oid","-->%d", oid_idx);
		return oid_idx;
	}

	/* Step 2: Convert OID to string for storage. */
	if((oid_str = oid_to_string(oid)) == NULL)
	{
		AMP_DEBUG_ERR("db_add_oid","Can't get string rep of OID.", NULL);
		AMP_DEBUG_EXIT("db_add_oid","-->%d", AMP_SYSERR);
		return AMP_SYSERR;
	}

	db_mgt_txn_start();

	/* Step 3: Build and execute query to add row to dbtOIDs. */

	if(db_mgt_query_insert(&oid_idx,
			            "INSERT INTO dbtOIDs"
						"(IRI_Label, Dot_Label, Encoded, Description)"
						"VALUES ('empty','empty','%s','empty')",
					    oid_str) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_oid","Can't insert Nickname", NULL);

		SRELEASE(oid_str);
		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_oid","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	SRELEASE(oid_str);

	db_mgt_txn_commit();

	AMP_DEBUG_EXIT("db_add_oid", "-->%d", oid_idx);
	return oid_idx;
}



/******************************************************************************
 *
 * \par Function Name: db_add_parms
 *
 * \par Adds OID parameters to the database and returns the index of the
 *      parameters table.
 *
 * Tables Effected:
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in]  oid  - The OID whose parameters are being added to the DB.
 *
 * \par Notes:
 *		- Comments for the parameters are not included.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/22/15  E. Birrane     Initial Implementation
 *  09/10/15  E. Birrane     Removed dbtMIDParameters
 *  01/24/17  E. Birrane     Updated to new AMP implementation for ION 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_add_parms(oid_t oid)
{
	int32_t result = 0;

	AMP_DEBUG_ENTRY("db_add_parms", "(%d)", oid.type);

	/* Step 0: Sanity check arguments. */
	if(oid.type == OID_TYPE_UNK)
	{
		AMP_DEBUG_ERR("db_add_parms","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_parms","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	result = db_add_tdc(oid.params);

	AMP_DEBUG_EXIT("db_add_parms", "-->%d", result);

	return result;
}




/******************************************************************************
 *
 * \par Function Name: db_add_protomid
 *
 * \par Creates a MID template in the database.
 *
 * Tables Effected:
 *    1. dbtProtoMIDs
 *
 * +--------------+------------------+------+-----+--------------------+----------------+
 * | Field        | Type             | Null | Key | Default            | Extra          |
 * +--------------+------------------+------+-----+--------------------+----------------+
 * | ID           | int(10) unsigned | NO   | PRI | NULL               | auto_increment |
 * | NicknameID   | int(10) unsigned | YES  | MUL | NULL               |                |
 * | OID          | int(10) unsigned | NO   | MUL | NULL               |                |
 * | ParametersID | int(10) unsigned | YES  | MUL | NULL               |                |
 * | DataType     | int(10) unsigned | NO   | MUL | 0                  |                |
 * | OIDType      | int(10) unsigned | NO   | MUL | 0                  |                |
 * | Type         | int(10) unsigned | NO   | MUL | 0                  |                |
 * | Category     | int(10) unsigned | NO   | MUL | 0                  |                |
 * | Name         | varchar(50)      | NO   |     | Unnamed MID        |                |
 * | Description  | varchar(255)     | NO   |     | No MID Description |                |
 * +--------------+------------------+------+-----+--------------------+----------------+
 *
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in] mid     - The MID to be persisted in the DB.
 * \param[in] spec    - The parameter spec for this protomid.
 * \param[in] type    - The type of the MID.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/28/15  E. Birrane     Initial implementation,
 *  01/24/17  E. Birrane     Update to latest version of AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
int32_t db_add_protomid(mid_t *mid, ui_parm_spec_t *spec, amp_type_e type)
{
	int32_t result = 0;
	uint32_t nn_idx = 0;
	uint32_t oid_idx = 0;
	uint32_t parm_idx = 0;
	uint32_t num_parms = 0;

	AMP_DEBUG_ENTRY("db_add_protomid", "(%p,%p,%d)",
			       mid, spec, type);

	/* Step 0: Sanity check arguments. */
	if((mid == NULL) || (spec==NULL))
	{
		AMP_DEBUG_ERR("db_add_protomid","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_protomid","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 1: Make sure the ID is not already in the DB. */
	if ((result = db_fetch_protomid_idx(mid)) != 0)
	{
		AMP_DEBUG_EXIT("db_add_protomid", "-->%d", result);
		return result;
	}

	/* Step 2: If this MID has a nickname, grab the index. */
	if((MID_GET_FLAG_OID(mid->flags) == OID_TYPE_COMP_FULL) ||
	   (MID_GET_FLAG_OID(mid->flags) == OID_TYPE_COMP_PARAM))
	{
		if((nn_idx = db_fetch_nn_idx(mid->oid.nn_id)) <= 0)
		{
			AMP_DEBUG_ERR("db_add_protomid","MID references unknown Nickname %d", mid->oid.nn_id);
			AMP_DEBUG_EXIT("db_add_protomid", "-->%d", nn_idx);
			return nn_idx;
		}
	}

	db_mgt_txn_start();

	/* Step 3: Get the index for the OID. */
	if((oid_idx = db_add_oid(mid->oid)) <= 0)
	{
		AMP_DEBUG_ERR("db_add_protomid", "Can't add OID.", NULL);
		db_mgt_txn_rollback();
		AMP_DEBUG_EXIT("db_add_protomid", "-->%d", oid_idx);
		return oid_idx;
	}

	/* Step 4: Get the index for parameters, if any. */
	if((parm_idx = db_add_protoparms(spec)) <= 0)
	{
		AMP_DEBUG_ERR("db_add_protomid", "Can't add protoparms.", NULL);
		db_mgt_txn_rollback();
		AMP_DEBUG_EXIT("db_add_protomid", "-->%d", parm_idx);
		return parm_idx;
	}

	if(db_mgt_query_insert((uint32_t*)&result,
			"INSERT INTO dbtProtoMIDs"
						"(NicknameID,OID,ParametersID,Type,Category,"
						"OIDType,DataType,Name,Description)"
						"VALUES (%s, %d, %d, %d, %d, %d, %d, '%s','%s')",
						(nn_idx == 0) ? "NULL" : itoa(nn_idx),
						oid_idx,
						parm_idx,
						0,
						MID_GET_FLAG_ID(mid->flags),
						MID_GET_FLAG_OID(mid->flags),
						type,
						"No Name",
						"No Descr") != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_protomid", "Can't add protomid.", NULL);
		db_mgt_txn_rollback();
		AMP_DEBUG_EXIT("db_add_protomid", "-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	db_mgt_txn_commit();

	AMP_DEBUG_EXIT("db_add_protomid", "-->%d", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_add_protoparms
 *
 * \par Adds parameter specs to the database and returns the index of the
 *      parameters table.
 *
 * Tables Effected:
 *    1. dbtProtoMIDParameters
 *
 * +---------+------------------+------+-----+------------+----------------+
 * | Field   | Type             | Null | Key | Default    | Extra          |
 * +---------+------------------+------+-----+------------+----------------+
 * | ID      | int(10) unsigned | NO   | PRI | NULL       | auto_increment |
 * | Comment | varchar(255)     | NO   |     | No Comment |                |
 * +---------+------------------+------+-----+------------+----------------+
 *
 *
 *    2. dbtProtoMIDParameter
 *
 * +-----------------+------------------+------+-----+---------+----------------+
 * | Field           | Type             | Null | Key | Default | Extra          |
 * +-----------------+------------------+------+-----+---------+----------------+
 * | ID              | int(10) unsigned | NO   | PRI | NULL    | auto_increment |
 * | CollectionID    | int(10) unsigned | YES  | MUL | NULL    |                |
 * | ParameterOrder  | int(10) unsigned | NO   |     | 0       |                |
 * | ParameterTypeID | int(10) unsigned | YES  | MUL | NULL    |                |
 * +-----------------+------------------+------+-----+---------+----------------+
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[in]  spec - The parm spec that gives the types of OID parms.
 *
 * \par Notes:
 *		- Comments for the parameters are not included.
 *		- A return of AMP_FAIL is only an error if the oid has parameters.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/28/15  E. Birrane     Initial Implementation
 *  01/25/17  E. Birrane     Update to latest version of AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_add_protoparms(ui_parm_spec_t *spec)
{
	int32_t i = 0;
	int32_t num_parms = 0;
	uint32_t result = 0;
	uint32_t parm_idx = 0;

	AMP_DEBUG_ENTRY("db_add_protoparms", "(%p)",
					  spec);

	/* Step 0: Sanity check arguments. */
	if(spec == NULL)
	{
		AMP_DEBUG_ERR("db_add_protoparms","Bad args",NULL);
		AMP_DEBUG_EXIT("db_add_protoparms","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	if((spec->num_parms == 0) || (spec->num_parms >= MAX_PARMS))
	{
		AMP_DEBUG_ERR("db_add_protoparms","Bad # parms.",NULL);
		AMP_DEBUG_EXIT("db_add_protoparms","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	db_mgt_txn_start();

	/* Step 1: Add an entry in the parameters table. */
	if(db_mgt_query_insert(&result,
			"INSERT INTO dbtProtoMIDParameters (Comment) "
						"VALUES ('No comment')") != AMP_OK)
	{
		AMP_DEBUG_ERR("db_add_protoparms","Can't insert Protoparm", NULL);

		db_mgt_txn_rollback();

		AMP_DEBUG_EXIT("db_add_protoparms","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: For each parameter, get the DC, add the DC into the DB,
	 * and then add an entry into dbtMIDParameter
	 */

	for(i = 0; i < spec->num_parms; i++)
	{

		if(db_mgt_query_insert(&parm_idx,
				"INSERT INTO dbtProtoMIDParameter "
								"(CollectionID, ParameterOrder, ParameterTypeID) "
								"VALUES (%d, %d, %d)",
								result, i, spec->parm_type[i]) != AMP_OK)
		{
			AMP_DEBUG_ERR("db_add_protoparms","Can't insert Parm", NULL);

			db_mgt_txn_rollback();

			AMP_DEBUG_EXIT("db_add_protoparms","-->%d", AMP_FAIL);
			return AMP_FAIL;
		}
	}

	db_mgt_txn_commit();
	AMP_DEBUG_EXIT("db_add_protoparms", "-->%d", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_adm_idx
 *
 * \par Gets the ADM index given an ADM description
 *
 * \return AMP_SYSERR - System Error
 *         AMP_FAIL   - Non-fatal issue.
 *         >0         - The fetched idx.
 *
 * \param[in] name    - The ADM name.
 * \param[in] version - The ADM version.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/29/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to latest version of AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_fetch_adm_idx(char *name, char *version)
{
	int32_t result = 0;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_adm_idx","(%s,%s)",
					  name, version);

	if((name == NULL) || (version == NULL))
	{
		AMP_DEBUG_ERR("db_fetch_adm_idx","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_fetch_adm_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	if(db_mgt_query_fetch(&res,
			"SELECT * FROM dbtADMs WHERE Label='%s' AND Version='%s'",
			name, version) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_adm_idx","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_adm_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: Parse information out of the returned row. */
	if ((row = mysql_fetch_row(res)) != NULL)
	{
		result = atoi(row[0]);
	}

	/* Step 3: Free database resources. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_adm_idx","-->%d", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_fetch_tdc
 *
 * \par Creates a typed data collection from dbtDataCollections in the database
 *
 * \retval a TDC object (with type UNKNOWN on error).
 *
 * \param[in] id - The Primary Key in the dbtDataCollections table.
 *
 * \par Notes:
 *  - A TDC with a length of 0 indicates an error retrieving the TDC.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/23/13  S. Jacobs      Initial implementation,
 *  08/23/15  E. Birrane     Update to new schema.
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
tdc_t db_fetch_tdc(int32_t tdc_idx)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	tdc_t result;
	blob_t *entry;

	AMP_DEBUG_ENTRY("db_fetch_tdc", "(%d)", tdc_idx);

	tdc_init(&result);

	/* Step 1: Construct/run the Query and capture results. */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM dbtDataCollection "
			              "WHERE CollectionID=%d "
						  "ORDER BY DataOrder",
						  tdc_idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_tdc","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_tdc","-->NULL", NULL);
		return result;
	}

	/* Step 2: For each entry returned as part of the collection. */
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		amp_type_e type;

		if((entry = db_fetch_tdc_entry_from_row(row, &type)) == NULL)
		{
			AMP_DEBUG_ERR("db_fetch_tdc", "Can't get entry.", NULL);
			tdc_clear(&result);
			mysql_free_result(res);

			tdc_init(&result);

			AMP_DEBUG_EXIT("db_fetch_tdc","-->NULL", NULL);
			return result;
		}

		tdc_insert(&result, type, entry->value, entry->length);
	}

	/* Step 4: Free results. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_dc", "-->%d", result.hdr.length);
	return result;
}



/*******************************************************************************
 *
 * \par Function Name: db_fetch_data_col_entry_from_row
 *
 * \par Parses a data collection entry from a database row from the
 *      dbtataCollection table.
 *
 * \retval NULL The entry could not be retrieved
 *         !NULL The created data collection entry.
 *
 * \param[in] row  - The row containing the data col entry information.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/19/13  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 ******************************************************************************/

blob_t* db_fetch_tdc_entry_from_row(MYSQL_ROW row, amp_type_e *type)
{
	blob_t *result = NULL;
	uint8_t *value = NULL;
	uint32_t length = 0;

	AMP_DEBUG_ENTRY("db_fetch_tdc_entry_from_row","(%p,%p)",
					  row, type);

	/* Step 1: grab data from the row. */
	value = utils_string_to_hex(row[3], &length);
	if((value == NULL) || (length == 0))
	{
		AMP_DEBUG_ERR("db_fetch_tdc_entry_from_row", "Can't grab value for %s", row[3]);
		AMP_DEBUG_EXIT("db_fetch_tdc_entry_from_row", "-->NULL", NULL);
		return NULL;
	}

	/* Step 2: Create the blob representing the entry. */
	if((result = blob_create(value,length)) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_tdc_entry_from_row", "Can't make blob", NULL);
		SRELEASE(value);
		AMP_DEBUG_EXIT("db_fetch_tdc_entry_from_row", "-->NULL", NULL);
		return NULL;
	}

	/* Step 3: Store the type. */
	*type = atoi(row[2]);

	AMP_DEBUG_EXIT("db_fetch_tdc_entry_from_row", "-->%p", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_fetch_mid
 *
 * \par Creates a MID structure from a row in the dbtMIDs database.
 *
 * \retval NULL Failure
 *        !NULL The built MID structure.
 *
 * \param[in] idx - The Primary Key of the desired MID in the dbtMIDs table.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  08/23/15  E. Birrane     Update to new schema.
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

mid_t *db_fetch_mid(int32_t idx)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	mid_t *result = NULL;

	AMP_DEBUG_ENTRY("db_fetch_mid", "(%d)", idx);

	/* Step 1: Construct and run the query to get the MID information. */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM dbtMIDs WHERE ID=%d", idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_mid","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_mid","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: Parse information out of the returned row. */
	if ((row = mysql_fetch_row(res)) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_mid","Can't grab row", NULL);
		mysql_free_result(res);
		AMP_DEBUG_EXIT("db_fetch_mid","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 3: Build MID from the row. */
	if((result = db_fetch_mid_from_row(row)) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_mid","Can't build MID from row", NULL);
		mysql_free_result(res);
		AMP_DEBUG_EXIT("db_fetch_mid","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_mid", "-->%p", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_fetch_mid_col
 *
 * \par Creates a MID collection from a row in the dbtMIDCollection database.
 *
 * \retval NULL Failure
 *        !NULL The built MID collection.
 *
 * \param[in] idx - The Primary Key in the dbtMIDCollection table.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation
 *  08/23/15  E. Birrane     Update to new database schema
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
Lyst db_fetch_mid_col(int idx)
{
	Lyst result = lyst_create();
	mid_t *new_mid = NULL;
	char query[1024];
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_mid_col","(%d)", idx);

	/* Step 1: Construct and run the query to get the MID information. */
	if(db_mgt_query_fetch(&res,
			              "SELECT MIDID FROM dbtMIDCollection WHERE CollectionID=%d ORDER BY MIDOrder",
						  idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_mid_col","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_mid_col","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: For each MID in the collection... */
	while ((row = mysql_fetch_row(res)) != NULL)
	{

		/* Step 2.1: For each row, build a MID and add it to the collection. */
		if((new_mid = db_fetch_mid(atoi(row[0]))) == NULL)
		{
			AMP_DEBUG_ERR("db_fetch_mid_col", "Can't grab MID with ID %d.",
					        atoi(row[0]));

			midcol_destroy(&result);
			mysql_free_result(res);

			AMP_DEBUG_EXIT("db_fetch_mid_col", "-->NULL", NULL);
			return NULL;
		}

		lyst_insert_last(result, new_mid);
	}

	/* Step 3: Free database resources. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_mid_col", "-->%p", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_mid_from_row
 *
 * \par Gets a MID from the MID table.
 *
 * \retval  NULL Failure
 *         !NULL The fetched MID
 *
 * \param[in] row  - The row containing the data col entry information.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/25/17  E. Birrane     Initial implementation, (JHU/APL)
 *****************************************************************************/

mid_t* db_fetch_mid_from_row(MYSQL_ROW row)
{
	oid_t oid;
	mid_t *result = NULL;

	AMP_DEBUG_ENTRY("db_fetch_mid_from_row", "(%p)", row);

	/* Step 0: Sanity check. */
	if(row == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_mid_from_row","Bad args", NULL);
		AMP_DEBUG_EXIT("db_fetch_mid_from_row","-->NULL", NULL);
		return NULL;
	}

	/* Step 1: Build parametrs from row. */
	uint32_t nn_idx   = (row[1] == NULL) ? 0 : atoi(row[1]);
	uint32_t oid_idx  = (row[2] == NULL) ? 0 : atoi(row[2]);
	uint32_t parm_idx = (row[3] == NULL) ? 0 : atoi(row[3]);
	uint8_t type      = (row[4] == NULL) ? 0 : atoi(row[4]);
	uint8_t cat       = (row[5] == NULL) ? 0 : atoi(row[5]);
	uint8_t issFlag   = (row[6] == NULL) ? 0 : atoi(row[6]);
	uint8_t tagFlag   = (row[7] == NULL) ? 0 : atoi(row[7]);
	uint8_t oidType   = (row[8] == NULL) ? 0 : atoi(row[8]);
	amp_uvast issuer      = (amp_uvast) (row[9] == NULL) ? 0 : atoll(row[9]);
	amp_uvast tag         = (amp_uvast) (row[10] == NULL) ? 0 : atoll(row[10]);
	uint32_t dtype    = (row[11] == NULL) ? 0 : atoi(row[11]);
 	uint32_t mid_type = 0;

	/* Step 2: Create the OID. */
	oid = db_fetch_oid(nn_idx, parm_idx, oid_idx);
	if(oid.type == OID_TYPE_UNK)
	{
		AMP_DEBUG_ERR("db_fetch_mid_from_row","Cannot fetch the oid: %d", oid_idx);
		oid_release(&oid);
		AMP_DEBUG_EXIT("db_fetch_mid_from_row","-->NULL", NULL);
		return NULL;
	}

	oid.type = oidType;

        switch(cat)
        {
          case 0: mid_type = MID_ATOMIC; break;
          case 1: mid_type = MID_COMPUTED; break;
          case 2: mid_type = MID_REPORT; break;
          case 3: mid_type = MID_CONTROL; break;
	  case 4: mid_type = MID_SRL; break;
          case 5: mid_type = MID_TRL; break;
          case 6: mid_type = MID_MACRO; break;
          case 7: mid_type = MID_LITERAL; break;
          case 8: mid_type = MID_OPERATOR; break;
          default: mid_type = MID_ANY;
        }
          
	if ((result = mid_construct(mid_type,
			                    issFlag ? &issuer : NULL,
					            tagFlag ? &tag : NULL,
				                oid)) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_mid_from_row", "Cannot construct MID", NULL);
		oid_release(&oid);
		AMP_DEBUG_EXIT("db_fetch_mid_from_row","-->NULL", NULL);
		return NULL;
	}

	oid_release(&oid);

	if (mid_sanity_check(result) == 0)
	{
		char *data = mid_pretty_print(result);
		AMP_DEBUG_ERR("db_fetch_mid_from_row", "Failed MID sanity check. %s", data);
		SRELEASE(data);
		mid_release(result);
		result = NULL;
	}

	AMP_DEBUG_EXIT("db_fetch_mid_from_row","-->%p", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_mid_idx
 *
 * \par Gets a MID and returns the index of the MID
 *
 * \retval 0 Failure
 *        !0 The index of the MID
 *
 * \param[in] mid    - the MID whose index is being queried
 *
 * Note: There is probably a much better way to do this.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/23/13  S. Jacobs      Initial implementation,
 *  08/24/15  E. Birrane     Update to latest schema
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_fetch_mid_idx(mid_t *mid)
{
	int32_t result = 0;
	int32_t cur_idx = 0;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_mid_idx","(%p)", mid);

	/* Step 0: Sanity check arguments. */
	if(mid == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_mid_idx","Bad args",NULL);
		AMP_DEBUG_EXIT("db_fetch_mid_idx","-->NULL", NULL);
		return AMP_FAIL;
	}

	if(db_mgt_query_fetch(&res,
			            "SELECT * FROM dbtMIDs WHERE "
						"Type=%d AND Category=%d AND IssuerFlag=%d AND TagFlag=%d "
						"AND OIDType=%d AND IssuerID="UVAST_FIELDSPEC" "
						"AND TagValue="UVAST_FIELDSPEC,
						0,
						MID_GET_FLAG_ID(mid->flags),
						(MID_GET_FLAG_ISS(mid->flags)) ? 1 : 0,
						(MID_GET_FLAG_TAG(mid->flags)) ? 1 : 0,
						MID_GET_FLAG_OID(mid->flags),
						mid->issuer,
						mid->tag) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_mid_col","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_mid_col","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: For each matching MID, check other items... */
	result = AMP_FAIL;

	while ((row = mysql_fetch_row(res)) != NULL)
	{
		cur_idx = (row[0] == NULL) ? 0 : atoi(row[0]);

		int32_t nn_idx   = (row[1] == NULL) ? 0 : atoi(row[1]);
		int32_t oid_idx  = (row[2] == NULL) ? 0 : atoi(row[2]);
		int32_t parm_idx = (row[3] == NULL) ? 0 : atoi(row[3]);
		oid_t   oid      = db_fetch_oid(nn_idx, parm_idx, oid_idx);

		if((oid.type != OID_TYPE_UNK) &&
	       (oid_compare(oid, mid->oid, 1) == 0))
		{
			oid_release(&oid);
			result = cur_idx;
			break;
		}

		oid_release(&oid);
	}

	/* Step 3: Free database resources. */
	mysql_free_result(res);

	/* Step 4: Return the IDX. */
	AMP_DEBUG_EXIT("db_fetch_mid_idx", "-->%d", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_nn
 *
 * \par Gets the nickname UID given a primary key index into the Nickname table.
 *
 * \retval -1 system error
 *          0 non-fatal error
 *         >0 The nickname UID.
 *
 * \param[in] idx  - Index of the nickname UID being queried.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/24/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_fetch_nn(uint32_t idx)
{
	int32_t result = AMP_FAIL;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_nn","(%d)", idx);

	/* Step 0: Sanity checks. */
	if(idx == 0)
	{
		AMP_DEBUG_ERR("db_fetch_nn","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_fetch_nn","-->AMP_FAIL", NULL);
		return AMP_FAIL;
	}

	/* Step 1: Grab the NN row */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM dbtADMNicknames WHERE ID=%d",
						   idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_nn","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_nn","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: Parse information out of the returned row. */
	if ((row = mysql_fetch_row(res)) != NULL)
	{
		result = atoi(row[2]);
	}
	else
	{
		AMP_DEBUG_ERR("db_fetch_nn", "Did not find NN with ID of %d\n", idx);
	}

	/* Step 3: Free database resources. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_nn","-->%d", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_nn_idx
 *
 * \par Gets the index of a nickname UID.
 *
 * \retval -1 system error
 *          0 non-fatal error
 *         >0 The nickname index.
 *
 * \param[in] nn  - The nickname UID whose index is being queried.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/24/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_fetch_nn_idx(uint32_t nn)
{
	int32_t result = AMP_FAIL;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_nn_idx","(%d)", nn);


	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM dbtADMNicknames WHERE Nickname_UID=%d",
						  nn) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_nn_idx","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_nn_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: Parse information out of the returned row. */
	if ((row = mysql_fetch_row(res)) != NULL)
	{
		result = atoi(row[0]);
	}

	/* Step 3: Free database resources. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_nn_idx","-->%d", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_oid_val
 *
 * \par Gets OID encoded value of an OID from the database.
 *
 * \retval NULL Failure
 *        !NULL The encoded value as a series of bytes.
 *
 * \param[in]  idx  - Index of the OID being queried.
 * \param[out] size - Size of the returned encoded value.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/24/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

uint8_t* db_fetch_oid_val(uint32_t idx, uint32_t *size)
{
	uint8_t *result = NULL;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_oid_val","(%d,%p)",
			          idx, size);

	/* Step 0: Sanity check. */
	if((idx == 0) || (size == NULL))
	{
		AMP_DEBUG_ERR("db_fetch_oid_val","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_fetch_oid_val","-->NULL", NULL);
		return NULL;
	}


	if(db_mgt_query_fetch(&res,
			              "SELECT Encoded FROM dbtOIDs WHERE ID=%d",
						  idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_oid_val","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_oid_val","-->NULL", NULL);
		return NULL;
	}

	/* Step 2: Parse information out of the returned row. */
	if ((row = mysql_fetch_row(res)) != NULL)
	{
		result = utils_string_to_hex(row[0],size);
	}
	else
	{
		AMP_DEBUG_ERR("db_fetch_oid_val", "Did not find OID with ID of %d\n", idx);
	}

	/* Step 3: Free database resources. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_oid_val","-->%d", result);
	return result;
}



/*****************************************************************************
 *
 * \par Function Name: db_fetch_oid
 *
 * \par Grabs an OID from the database, querying from the nickname and
 *      parameter tables as well to create a full OID structure.
 *
 * \retval NULL OID could not be fetched
 *        !NULL The OID
 *
 * \param[in]  nn_idx   - The index for the OID nickname, or 0.
 * \param[in]  parm_idx - The index for the OIDParms, or 0.
 * \param[out] oid_idx  - The index for the OID value.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/25/13  S. Jacobs      Initial implementation,
 *  08/24/15  E. Birrane     Updated to new schema.
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 ******************************************************************************/

oid_t db_fetch_oid(uint32_t nn_idx, uint32_t parm_idx, uint32_t oid_idx)
{
	oid_t result;
	tdc_t parms;
	uint32_t nn_id = 0;
	uint32_t val_size = 0;
	uint8_t *val = NULL;
	uint32_t oid_type = OID_TYPE_FULL;

	AMP_DEBUG_ENTRY("db_fetch_oid","(%d, %d, %d)",
					  nn_idx, parm_idx, oid_idx);

	oid_init(&result);
	tdc_init(&parms);

	/* Step 0: Sanity Check. */
	if(oid_idx == 0)
	{
		AMP_DEBUG_ERR("db_fetch_oid","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_fetch_oid","-->OID_TYPE_UNK", NULL);
		return result;
	}

	/* Step 1: Grab the OID value string. */
	if((val = db_fetch_oid_val(oid_idx, &val_size)) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_oid","Can't get OID for idx %d.", oid_idx);
		AMP_DEBUG_EXIT("db_fetch_oid","-->OID_TYPE_UNK", NULL);
		return result;
	}

	/* Step 2: Grab parameters, if the OID has them. */
	if(parm_idx > 0)
	{
		parms = db_fetch_tdc(parm_idx);

		if(nn_idx == 0)
		{
			oid_type = OID_TYPE_PARAM;
		}
	}

	/* Step 3: Grab the nickname, if the OID has one. */
	if(nn_idx > 0)
	{
		nn_id = db_fetch_nn(nn_idx);

		oid_type = (parm_idx == 0) ? OID_TYPE_COMP_FULL : OID_TYPE_COMP_PARAM;
	}

	/*
	 * Step 4: Construct the OID. This deep-copies parameters so we can
	 *          release the parms and value afterwards.
	 */
	result = oid_construct(oid_type, &parms, nn_id, val, val_size);

	SRELEASE(val);

	tdc_clear(&parms);

	AMP_DEBUG_EXIT("db_fetch_oid","-->%d", result.type);
	return result;
}



/*****************************************************************************
 *
 * \par Function Name: db_fetch_oid_idx
 *
 * \par Retrieves the index of an OID value in the OID table.
 *
 * \retval 0 The OID is not found in the DB.
 *        !0 The index of the OID value.
 *
 * \param[in]  oid   - The OID whose index is being queried.
 *
 * Note: This function only matches on the OID value and ignores
 *       nicknames and parameters.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/24/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to newest AMP implementation
 ******************************************************************************/

int32_t db_fetch_oid_idx(oid_t oid)
{
	int32_t result = AMP_FAIL;
	char *oid_str = NULL;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_oid_idx","(%d)", oid.type);

	/* Step 0: Sanity checks. */
	if(oid.type == OID_TYPE_UNK)
	{
		AMP_DEBUG_ERR("db_fetch_oid_idx","Bad Args.", NULL);
		AMP_DEBUG_EXIT("db_fetch_oid_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 1: Build string version of OID for searching. */
	if((oid_str = oid_to_string(oid)) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_oid_idx","Can't get string rep of OID.", NULL);
		AMP_DEBUG_EXIT("db_fetch_oid_idx","-->%d",AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: Grab the OID row. */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM dbtOIDs WHERE Encoded='%s'",
						  oid_str) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_oid_idx","Can't fetch", NULL);
		SRELEASE(oid_str);
		AMP_DEBUG_EXIT("db_fetch_oid_idx","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 3: Grab the row idx. */
	if ((row = mysql_fetch_row(res)) != NULL)
	{
		result = atoi(row[0]);
	}

	mysql_free_result(res);
	SRELEASE(oid_str);

	AMP_DEBUG_EXIT("db_fetch_oid_idx","-->%d", result);
	return result;
}



/******************************************************************************
 *
 * \par Function Name: db_fetch_parms
 *
 * \par Retrieves the set of parameters associated with a parameter index.
 *
 * Tables Effected:
 *
 * \return NULL Failure
 *        !NULL The parameters.
 *
 * \param[in]  idx  - The index of the parameters
 *
 * \par Notes:
 *		- If there are no parameters, but no error, then a Lyst with no entries
 *		  is returned.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/24/15  E. Birrane     Initial Implementation
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

Lyst db_fetch_parms(uint32_t idx)
{
	Lyst result = 0;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	uint32_t dc_idx = 0;
	blob_t* entry = NULL;

	AMP_DEBUG_ENTRY("db_fetch_parms", "(%d)", idx);

	/* Step 0: Sanity check arguments. */
	if(idx == 0)
	{
		AMP_DEBUG_ERR("db_fetch_parms","Bad args",NULL);
		AMP_DEBUG_EXIT("db_fetch_parms","-->NULL",NULL);
		return NULL;
	}

	/* Step 1: Grab the OID row. */
	if(db_mgt_query_fetch(&res,
			              "SELECT DataCollectionID FROM dbtMIDParameter "
			              "WHERE CollectionID=%d ORDER BY ItemOrder",
			              idx) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_parms","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_parms","-->NULL", NULL);
		return NULL;
	}

	/* Step 2: Allocate the return lyst. */
	if((result = lyst_create()) == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_parms","Can't allocate lyst",NULL);
		mysql_free_result(res);
		AMP_DEBUG_EXIT("db_fetch_parms","-->NULL",NULL);
		return NULL;
	}

	/* Step 3: For each matching parameter... */
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		amp_type_e type;

		if((entry = db_fetch_tdc_entry_from_row(row, &type)) == NULL)
		{
			AMP_DEBUG_ERR("db_fetch_parms", "Can't get entry.", NULL);
			dc_destroy(&result);
			mysql_free_result(res);

			AMP_DEBUG_EXIT("db_fetch_parms","-->NULL",NULL);
			return NULL;
		}

		lyst_insert_last(result, entry);
	}

	/* Step 4: Free results. */
	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_parms", "-->%p", result);
	return result;
}



/******************************************************************************
 * \par Function Name: db_fetch_protomid_idx
 *
 * \par Gets a MID and returns the index of the matching proto mid.
 *
 * \retval 0 Failure finding index.
 *        !0 The index of the MID
 *
 * \param[in] mid    - the MID whose proto index is being queried
 *
 * Note: There is probably a much better way to do this.
 *
 *  Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/28/15  E. Birrane     Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/

int32_t db_fetch_protomid_idx(mid_t *mid)
{
	int32_t result = AMP_FAIL;
	int32_t cur_idx = 0;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	AMP_DEBUG_ENTRY("db_fetch_protomid_idx","(%p)", mid);

	/* Step 0: Sanity check arguments. */
	if(mid == NULL)
	{
		AMP_DEBUG_ERR("db_fetch_protomid_idx","Bad args",NULL);
		AMP_DEBUG_EXIT("db_fetch_parms", "-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 1: Grab the OID row. */
	if(db_mgt_query_fetch(&res,
			              "SELECT * FROM dbtProtoMIDs WHERE "
						  "Type=%d AND Category=%d AND OIDType=%d",
						  0,
						  MID_GET_FLAG_ID(mid->flags),
						  MID_GET_FLAG_OID(mid->flags)) != AMP_OK)
	{
		AMP_DEBUG_ERR("db_fetch_parms","Can't fetch", NULL);
		AMP_DEBUG_EXIT("db_fetch_parms","-->%d", AMP_FAIL);
		return AMP_FAIL;
	}

	/* Step 2: For each matching MID, check other items... */
	while ((row = mysql_fetch_row(res)) != NULL)
	{

		cur_idx = atoi(row[0]);

		int32_t nn_idx = atoi(row[1]);
		int32_t oid_idx = atoi(row[2]);
		oid_t oid = db_fetch_oid(nn_idx, 0, oid_idx);

		if(oid_compare(oid, mid->oid, 0) == 0)
		{
			oid_release(&oid);
			result = cur_idx;
			break;
		}

		oid_release(&oid);
	}

	mysql_free_result(res);

	AMP_DEBUG_EXIT("db_fetch_protomid_idx", "-->%d", result);
	return result;
}


#endif // End deprecated section

void query_update_msg_group_state(size_t dbidx, int group_id, int is_error) {
   int status;
   int state = (is_error) ? 3 : 2;
   dbprep_declare(dbidx,MSGS_UPDATE_GROUP_STATE, 2, 0);
   
   dbprep_bind_param_int(0,state);
   dbprep_bind_param_int(1,group_id);
		#ifdef HAVE_MYSQL
    DB_CHKVOID(mysql_stmt_bind_param(stmt, bind_param));
 
    DB_CHKVOID(mysql_stmt_execute(stmt));
   #endif // HAVE_MYSQL
   #ifdef HAVE_POSTGRESQL
   dbexec_prepared;
   DB_CHKVOID(dbtest_result(PGRES_TUPLES_OK))
   PQclear(res);
   #endif // HAVE_POSTGRESQL

   return;
}

void db_process_outgoing(nmmgr_t *mgr) {
	msg_grp_t *msg_group = NULL;
	int group_id;
	int ts; // TODO: UVAST? TODO: change this to an output (update record) instead of input from record
	dbprep_declare(DB_CTRL_CON, MSGS_OUTGOING_GET, 0, 2); // FUTURE: May add MgrEid parameter to allow a single DB to serve multiple managers
	
	#ifdef HAVE_MYSQL
	dbprep_bind_res_int(0,group_id);
	dbprep_bind_res_int(1,ts);
	
	DB_CHKVOID(mysql_stmt_execute(stmt));
	DB_CHKVOID(mysql_stmt_bind_result(stmt, bind_res));
	DB_CHKVOID(mysql_stmt_store_result(stmt)); // Results must be buffered to allow execution of nested queries
		#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKVOID(dbtest_result(PGRES_TUPLES_OK))
    int grp_id_fnum = PQfnumber(res, "group_id");
    int ts_fnum = PQfnumber(res, "ts");
	int nrows = PQntuples(res);
	#endif // HAVE_POSTGRESQL

	// Fetch all rows
	#ifdef HAVE_MYSQL
 	while(!mysql_stmt_fetch(stmt)) {
 
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	for(int i = 0; i < nrows; i++){
		char *iptr = PQgetvalue(res, i, grp_id_fnum);
		group_id = ntohl(*((uint32_t *) iptr));
		iptr = PQgetvalue(res, i, ts_fnum);
		ts = ntohl(*((uint32_t *) iptr)); 
	#endif // HAVE_POSTGRESQL

		/* Create an AMP PDU for this outgoing message. */
		if((msg_group = msg_grp_create(1)) == NULL)
		{
			AMP_DEBUG_ERR("db_tx_msg_groups","Cannot create group.", NULL);
			AMP_DEBUG_EXIT("db_tx_msg_groups","-->%d",AMP_SYSERR);
			continue; // to next group
		}

		// Set timestamp
		msg_group->timestamp = amp_tv_from_ctime(OS_TimeFromTotalSeconds(ts), NULL);
				
		// Query Group Contents & Build
		if((db_tx_build_group(group_id, msg_group)) != AMP_OK)
		{
			msg_grp_release(msg_group, 1);

			// Set status to error to avoid re-parsing the same set
			query_update_msg_group_state(DB_CTRL_CON, group_id, AMP_FAIL); 
			continue; // to next group
		}

		// Send Group
		int status = db_tx_msg_group_agents(mgr, group_id, msg_group);
		
		// Update Status (note: outgoing thread does not use transactions)
		query_update_msg_group_state(DB_CTRL_CON, group_id, status);


		// Release the group
		msg_grp_release(msg_group, 1);

	}
	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL
}

uint32_t db_insert_ari_lit(db_con_t dbidx, ari_t *ari, int *status)
{
	// TODO
	DB_LOG_ERR(dbidx,"db_insert ARI LIT TODO");
	*status = AMP_FAIL;
	return 0;
}

int db_query_ari_metadata(db_con_t dbidx, ari_t *ari, uint32_t *metadata_id, uint32_t *fp_spec_id)
{
	CHKZERO(ari);

	/** Decode values **/

	// Convert name from blob_t to amp_uvast
	amp_uvast name_idx;

	// VERIFY: Is this correct? optimal? Do we need to handle cases where name is not numeric?
	cut_get_cbor_numeric_raw(&(ari->as_reg.name), AMP_TYPE_UVAST, &name_idx);

	//vec_idx_t nn = ari->as_reg.nn_idx;
	amp_uvast *nn = (amp_uvast *) VDB_FINDIDX_NN(ari->as_reg.nn_idx);
	CHKZERO(nn);
	int namespace = *nn/20;
	int adm_type = *nn % 20;

	
	dbprep_declare(dbidx, ARI_GET_META, 3, 2);
	
	dbprep_bind_param_int(0, name_idx);
	dbprep_bind_param_int(1, ari->type);
	dbprep_bind_param_int(2, namespace);	
	
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);
	
	dbprep_bind_res_int_ptr(0, metadata_id);
	dbprep_bind_res_int_ptr(1, fp_spec_id);
	mysql_stmt_bind_result(stmt, bind_res);
	
	mysql_stmt_execute(stmt);
  #endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
    int metadata_id_fnum = PQfnumber(res, "obj_metadata_id");
    int fp_spec_id_fnum = PQfnumber(res, "fp_spec_id");
	int nrows = PQntuples(res);
	#endif // HAVE_POSTGRESQL

	#ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0) 
	{
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (nrows == 0)
 	{
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		DB_LOGF_WARN(dbidx,"ARI Unrecognized Nickname",
					 "query(name_idx=%d, ari->type=%d, namespace=%d)",
					 name_idx, ari->type, namespace);

		// Note: Caller determines if failure here is cause for error
		return AMP_FAIL;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, metadata_id_fnum);
	*metadata_id = ntohl(*((uint32_t *) iptr));
	iptr = PQgetvalue(res, 0, fp_spec_id_fnum);
	*fp_spec_id = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL
	return AMP_OK;
}
/** @returns 0 on error, obj_actual_definition/ari id on success */
uint32_t db_insert_ari_reg(db_con_t dbidx, ari_t *ari, int *status)
{
	uint32_t metadata_id = 0;
	uint32_t fp_spec_id = 0;
	uint32_t params_id = 0;
	uint32_t rtv = 0;
	
	
		
	// If Nickname (including Namespace) is defined
	if(ARI_GET_FLAG_NN(ari->as_reg.flags))
	{
		int adm_enum; // ari->as_reg.nn_idx
		int adm_obj_type;
		// Query metadata
		if (db_query_ari_metadata(dbidx, ari, &metadata_id, &fp_spec_id) == AMP_FAIL)
		{
			*status = AMP_FAIL;
			DB_LOGF_ERR(dbidx,"db_insert ARI CTRL Unrecognized Nickname", "nn_idx=%d", ari->as_reg.nn_idx);
			return 0;
		}

	}
	// If Namespace is not defined, not supported at present
	else
	{
		DB_LOG_ERR(dbidx,"db_insert ARI CTRL without defined NN not currently supported");
		*status = AMP_FAIL;
		return 0;
	}

	// Insert Parameters (if any)
	params_id = db_insert_tnvc_params(dbidx, fp_spec_id, &(ari->as_reg.parms), status);

	
	// Insert Ctrl( metadata_id, parms_id )
	dbprep_declare(dbidx, ARI_INSERT_CTRL, 2, 1);
	
	dbprep_bind_param_int(0, metadata_id);
	if (params_id == 0)
	{
		dbprep_bind_param_null(1);
	}
	else
	{
		dbprep_bind_param_int(1, params_id);
	}
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);
	
	dbprep_bind_res_int(0, rtv);
	mysql_stmt_bind_result(stmt, bind_res);
	
	mysql_stmt_execute(stmt);
  #endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
	int nrows = PQntuples(res);
	#endif // HAVE_POSTGRESQL


  #ifdef HAVE_MYSQL
 	if (mysql_stmt_fetch(stmt) != 0)
 	{
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (nrows == 0)
	{
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		return 0;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
  rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL
	return rtv;
}

uint32_t db_insert_ari(db_con_t dbidx, ari_t *ari, int *status)
{
	switch(ari->type)
	{
	case AMP_TYPE_LIT: return db_insert_ari_lit(dbidx, ari, status); break;
		// NOTE: Custom handling (or validation) may be needed for some ARI Types, but reg should handle most cases
	default:
		return db_insert_ari_reg(dbidx, ari, status);
	}
}
void db_insert_ac_entry(db_con_t dbidx, uint32_t ac_id, size_t idx, uint32_t ari_id, int *status)
{
	dbprep_declare(dbidx, AC_INSERT, 3, 0);
	
	dbprep_bind_param_int(0, ac_id);
	dbprep_bind_param_int(1, ari_id);
	dbprep_bind_param_int(2, idx);	

	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);
	mysql_stmt_execute(stmt);
  #endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	int nrows = PQntuples(res);
	#endif // HAVE_POSTGRESQL
	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	#ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0)
	{
		AMP_DEBUG_ERR("Failed to Insert AC Entry: %s", mysql_stmt_error(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		AMP_DEBUG_ERR("Failed to Insert AC Entry: %s", PQresultErrorMessage(res));
		PQclear(res);

	#endif // HAVE_POSTGRESQL
		CHKVOID(status);
		*status = AMP_FAIL;
		return;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL
}
uint32_t db_insert_ac(db_con_t dbidx, ac_t *ac, int *status)
{
	int rtv = 0;
	CHKZERO(ac);
	
	int num = vec_num_entries(ac->values);
	if (num == 0) {
		// We won't create a tnvc if empty (0 will be converted to NULL by calller)
		return 0;
	}
	
	/* Create AC */
	dbprep_declare(dbidx, AC_CREATE, 0, 1);

	#ifdef HAVE_MYSQL
	dbprep_bind_res_int(0, rtv);
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKINT(dbtest_result(PGRES_TUPLES_OK))
	int nrows = PQntuples(res);
  #endif // HAVE_POSTGRESQL

	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	#ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create AC: %s", mysql_stmt_error(stmt));
			#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if(nrows == 0)
	{
		AMP_DEBUG_ERR("Failed to Create AC: %s", PQresultErrorMessage(res));
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		CHKZERO(status);
		*status = AMP_FAIL;
		return 0;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
	rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	/* Add entries */
	for(int i = 0; i < num; i++)
	{
		ari_t *ari = (ari_t*) vec_at(&(ac->values),i);
		uint32_t ari_id = db_insert_ari(dbidx, ari, status);
		if (ari_id == 0)
		{
			*status = AMP_FAIL;
		}
		else
		{
			db_insert_ac_entry(dbidx, rtv, i, ari_id, status);
		}
		
	}
	
	return rtv;

	
}

void db_insert_tnv(db_con_t dbidx, uint32_t tnvc_id, tnv_t *tnv, int *status)
{
	dbprep_declare(dbidx, TNVC_INSERT_AC, 2,0); // Ignore return value, all TNVC_INSERT_* primitive types have same params

	enum cols {
		C_TNVC_ID=0,
		C_VAL=1
	};
	size_t id;
	int do_bind_id = 0; // Flag, to consolidate NULL handling
	dbprep_bind_param_int(0, tnvc_id);
	
	switch(tnv->type)
	{
    // Primitives
	case AMP_TYPE_STR:
		#ifdef HAVE_MYSQL
		stmt = queries[dbidx][TNVC_INSERT_STR];

		id = strlen( (char*) tnv->value.as_ptr );
		bind_param[dbidx].buffer_length = id;
		bind_param[dbidx].length = &id;
		bind_param[dbidx].buffer_type = MYSQL_TYPE_STRING;
		bind_param[dbidx].buffer = (char*)tnv->value.as_ptr;
		bind_param[dbidx].is_null = 0;
		bind_param[dbidx].error = 0;
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_STR];
		paramValues[dbidx] = (char*)tnv->value.as_ptr;								\
		paramLengths[dbidx] = 0; /* ignored for text format */\
		paramFormats[dbidx] = 0; /* text */
		#endif // HAVE_POSTGRESQL

		break;
	case AMP_TYPE_BOOL:
		#ifdef HAVE_MYSQL
		stmt = queries[dbidx][TNVC_INSERT_BOOL];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_BOOL];
		#endif // HAVE_POSTGRESQL
		dbprep_bind_param_bool(C_VAL, tnv->value.as_byte);
 		break;
 	case AMP_TYPE_BYTE:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_BYTE];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_BYTE];
		#endif // HAVE_POSTGRESQL
 		dbprep_bind_param_int(C_VAL, tnv->value.as_byte);
 		break;
 	case AMP_TYPE_INT:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_INT];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_INT];
		#endif // HAVE_POSTGRESQL
 		dbprep_bind_param_int(C_VAL, tnv->value.as_int);
 		break;
 	case AMP_TYPE_UINT:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_UINT];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_UINT];
		#endif // HAVE_POSTGRESQL
 		dbprep_bind_param_int(C_VAL, tnv->value.as_uint);
 		break;
 	case AMP_TYPE_VAST:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_VAST];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_VAST];
		#endif // HAVE_POSTGRESQL
		dbprep_bind_param_bigint(C_VAL, tnv->value.as_vast);
 		break;
 	case AMP_TYPE_TV:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_TV];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_TV];
		#endif // HAVE_POSTGRESQL
		dbprep_bind_param_bigint(C_VAL, tnv->value.as_uvast);
 		break;
 	case AMP_TYPE_TS:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_TS];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_TS];
		#endif // HAVE_POSTGRESQL
		dbprep_bind_param_bigint(C_VAL, tnv->value.as_uvast);
 		break;
 	case AMP_TYPE_UVAST:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_UVAST];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_UVAST];
		#endif // HAVE_POSTGRESQL
		dbprep_bind_param_bigint(C_VAL, tnv->value.as_uvast);
 		break;
 	case AMP_TYPE_REAL32:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_REAL32];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_REAL32];
		#endif // HAVE_POSTGRESQL
 		dbprep_bind_param_float(C_VAL, tnv->value.as_real32);
 		break;
 	case AMP_TYPE_REAL64:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_REAL64];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_REAL64];
		#endif // HAVE_POSTGRESQL
 		dbprep_bind_param_double(C_VAL, tnv->value.as_real64);
 		break;

		// Object Types
	case AMP_TYPE_EDD:
	case AMP_TYPE_CNST:
	case AMP_TYPE_ARI:
	case AMP_TYPE_LIT:
		id = db_insert_ari(dbidx, (ari_t*)tnv->value.as_ptr, status);
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_ARI];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_ARI];
		#endif // HAVE_POSTGRESQL
		do_bind_id=1;
		break;

	case AMP_TYPE_AC:
		id = db_insert_ac(dbidx, (ac_t*)tnv->value.as_ptr, status);
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_AC];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_AC];
		#endif // HAVE_POSTGRESQL
		do_bind_id=1;
		break;
	case AMP_TYPE_TNVC:
		#ifdef HAVE_MYSQL
 		stmt = queries[dbidx][TNVC_INSERT_TNVC];
		#endif // HAVE_MYSQL
		#ifdef HAVE_POSTGRESQL
		stmtName = queries[dbidx][TNVC_INSERT_TNVC];
		#endif // HAVE_POSTGRESQL
		do_bind_id=1;
		break;
	default:
		DB_LOGF_ERR(dbidx,"SQL Support for TNV Type Not Implemented", "%d", tnv->type);
		*status = AMP_FAIL;
		return;
	}

	if (do_bind_id) {
		if (id == 0) {
			dbprep_bind_param_null(C_VAL);
		} else {
			dbprep_bind_param_int(C_VAL, id);
		}
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_bind_param(stmt, bind_param);
 	mysql_stmt_execute(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	#endif // HAVE_POSTGRESQL


	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	#ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create TNV: %s", mysql_stmt_error(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create TNV: %s", PQresultErrorMessage(res));
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		CHKVOID(status);
		*status = AMP_FAIL;
		return;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL
	
}
uint32_t db_insert_tnvc_params(db_con_t dbidx, uint32_t fp_spec_id, tnvc_t *tnvc, int *status)
{
	uint32_t rtv = 0;
	uint32_t tnvc_id = db_insert_tnvc(dbidx, tnvc, status);
	if (tnvc_id == 0)
	{
		return 0; // No need to insert an empty record
	}

	dbprep_declare(dbidx, TNVC_PARMSPEC_CREATE, 2, 1);
	
	dbprep_bind_param_int(0, fp_spec_id);
	dbprep_bind_param_int(1, tnvc_id);
	
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);
	
	dbprep_bind_res_int(0, rtv);
	mysql_stmt_bind_result(stmt, bind_res);
	
	mysql_stmt_execute(stmt);
  #endif //HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	#endif // HAVE_POSTGRESQL
 
  #ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0)
	{
		#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		// Note: Caller determines if failure here is cause for error
		return 0;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
	rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL
	return rtv;
}

uint32_t db_insert_tnvc(db_con_t dbidx, tnvc_t *tnvc, int *status)
{
	CHKZERO(tnvc);
	
	int num = vec_num_entries(tnvc->values);
	if (num == 0) {
		// We won't create a tnvc if empty (0 will be converted to NULL by calller)
		return 0;
	}
	
	/* Create TNVC */
	uint32_t rtv = 0;
	dbprep_declare(dbidx, TNVC_CREATE, 0, 1);

  #ifdef HAVE_MYSQL
	dbprep_bind_res_int(0, rtv);
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);
  #endif //HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	#endif // HAVE_POSTGRESQL

	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	#ifdef HAVE_MYSQL
 	if (mysql_stmt_fetch(stmt) != 0)
 	{
 		AMP_DEBUG_ERR("Failed to Create TNVC: %s", mysql_stmt_error(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create TNVC: %s", PQresultErrorMessage(res));
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		CHKZERO(status);
		*status = AMP_FAIL;
		return 0;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
	rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	/* Add entries */
	for(int i = 0; i < num; i++)
	{
		tnv_t *tnv = (tnv_t*) vec_at(&(tnvc->values),i);
		db_insert_tnv(dbidx, rtv, tnv, status);

	}
	
	return rtv;

}

void db_insert_msg_rpt_set_rpt(db_con_t dbidx, uint32_t entry_id, rpt_t* rpt, int *status)
{
	enum cols {
		C_ENTRY_ID=0,
		//C_IDX_ID, // Query set to NULL to auto-insert next idx
		C_ARI_ID,
		C_PARMS_ID,
		C_TS,
		C_NUM_COLS
	};
	dbprep_declare(DB_RPT_CON, MSGS_ADD_REPORT_SET_ENTRY, C_NUM_COLS, 1);
	dbprep_bind_param_int(C_ENTRY_ID,entry_id);
	// adding back the AVTIME and the offset to the unixposix for DB storage and display
	int64 real_time_stamp = rpt->time.ticks +EPOCH_ABSTIME_DTN + EPOCH_DTN_POSIX;
	dbprep_bind_param_int(C_TS, real_time_stamp);

	/** Prepare Dependent Fields **/
	uint32_t ari_id=db_insert_ari(dbidx, rpt->id, status);
	if (ari_id == 0) {
		// We can't proceed if ARI is missing
		*status = AMP_FAIL;
		return;
	}
	dbprep_bind_param_int(C_ARI_ID, ari_id);
	
	uint32_t parms_id = db_insert_tnvc(dbidx, rpt->entries, status);
	if (parms_id > 0) { // Parameters are defined and inserted
		dbprep_bind_param_int(C_PARMS_ID,parms_id);
	} else {
		// Either status==AMP_OK and this message has no parameters, or we failed to create tnvc record for some reason
		dbprep_bind_param_null(C_PARMS_ID);
	}
	/** Insert Report **/	
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);
#if 0 // We don't need ID at this time
	dbprep_bind_res_int(0, rtv);
	mysql_stmt_bind_result(stmt, bind_res);
#endif
	mysql_stmt_execute(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	#endif // HAVE_POSTGRESQL

	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
  #ifdef HAVE_MYSQL
 	if (mysql_stmt_fetch(stmt) != 0)
 	{
 		AMP_DEBUG_ERR("Failed to Create Entry: %s", mysql_stmt_error(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create Entry: %s", PQresultErrorMessage(res));
		PQclear(res);
	#endif // HAVE_POSTGRESQL

		CHKVOID(status);
		*status = AMP_FAIL;
		return;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	PQclear(res);
	#endif // HAVE_POSTGRESQL
	
	return;
}


void db_insert_msg_rpt_set_name(db_con_t dbidx, uint32_t entry_id, char* name, int *status)
{
	dbprep_declare(dbidx, MSGS_AGENT_MSG_ADD, 2, 0);
	dbprep_bind_param_int(0,entry_id);
	dbprep_bind_param_str(1,name);
	
	#ifdef HAVE_MYSQL
	DB_CHKUSR(mysql_stmt_bind_param(stmt, bind_param), {*status = AMP_FAIL; return;});

	DB_CHKUSR(mysql_stmt_execute(stmt), {*status = AMP_FAIL; return;});
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	DB_CHKUSR(dbtest_result(PGRES_COMMAND_OK), {*status = AMP_FAIL; return; });
	PQclear(res);
	#endif // HAVE_POSTGRESQL
}

/**
 * @param reg - Register agent message
 * @param status - Set to AMP_FAIL if parsing fails, but not modified on success
 * @returns Message ID, or 0 on error
 */
uint32_t db_insert_msg_reg_agent(uint32_t grp_id, msg_agent_t *msg, int *status)
{
	CHKZERO(gConn[DB_RPT_CON]);
	DB_LOGF_INFO(DB_RPT_CON, "Registering agent","%s", msg->agent_id.name);
	int rtv = 0;
	enum cols {
		C_GRP_ID=0,
		C_ACK,
		C_NAK,
		C_ACL,
		// order_num is auto-incremented
		C_EID,
		C_NUM_COLS
	};
	int is_ack = MSG_HDR_GET_ACK(msg->hdr.flags);
	int is_nak = MSG_HDR_GET_NACK(msg->hdr.flags);
	int is_acl = MSG_HDR_GET_ACL(msg->hdr.flags);
	dbprep_declare(DB_RPT_CON, MSGS_REGISTER_AGENT_INSERT, C_NUM_COLS, 1);
	dbprep_bind_param_int(C_GRP_ID,grp_id);
  dbprep_bind_param_bool(C_ACK,is_ack);
	dbprep_bind_param_bool(C_NAK,is_nak);
	dbprep_bind_param_bool(C_ACL,is_acl);

	char *name = msg->agent_id.name;
	dbprep_bind_param_str(C_EID, name);
	
	#ifdef HAVE_MYSQL
	mysql_stmt_bind_param(stmt, bind_param);

	dbprep_bind_res_int(0, rtv);
	mysql_stmt_execute(stmt);
	mysql_stmt_bind_result(stmt, bind_res);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	#endif // HAVE_POSTGRESQL

	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	#ifdef HAVE_MYSQL
	if (mysql_stmt_fetch(stmt) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create Entry: %s", mysql_stmt_error(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		AMP_DEBUG_ERR("Failed to Create Entry: %s", PQresultErrorMessage(res));
		PQclear(res);
	#endif // HAVE_POSTGRESQL
		CHKZERO(status);
		*status = AMP_FAIL;
		return 0;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
	rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	return rtv;
}
uint32_t db_insert_msg_tbl_set(uint32_t grp_id, msg_tbl_t *rpt, int *status)
{
	DB_LOG_ERR(DB_RPT_CON,"TBL Set SQL Support TODO");
	CHKZERO(status);
	*status = AMP_FAIL;
	return 0;
}
/**
 * @param rpt - Report
 * @param status - Set to AMP_FAIL if parsing fails, but not modified on success
 * @returns Report Set ID, or 0 on error
 */
uint32_t db_insert_msg_rpt_set(uint32_t grp_id, msg_rpt_t *rpt, int *status)
{
	uint32_t rtv = 0;
	vecit_t it;
	int dbstatus;
	CHKZERO(gConn[DB_RPT_CON]);
	enum cols {
		C_GRP_ID=0,
		C_ACK,
		C_NAK,
		C_ACL,
		// order_num is auto-incremented
		C_NUM_COLS
	};
	int is_ack = MSG_HDR_GET_ACK(rpt->hdr.flags);
	int is_nak = MSG_HDR_GET_NACK(rpt->hdr.flags);
	int is_acl = MSG_HDR_GET_ACL(rpt->hdr.flags);
	
	dbprep_declare(DB_RPT_CON, MSGS_REPORT_SET_INSERT, C_NUM_COLS, 1);
	dbprep_bind_param_int(C_GRP_ID,grp_id);
	dbprep_bind_param_bool(C_ACK,is_ack);
	dbprep_bind_param_bool(C_NAK,is_nak);
	dbprep_bind_param_bool(C_ACL,is_acl);
	
	#ifdef HAVE_MYSQL
	DB_CHKINT(mysql_stmt_bind_param(stmt, bind_param));

	dbprep_bind_res_int(0, rtv);
	DB_CHKINT(mysql_stmt_bind_result(stmt, bind_res));
	DB_CHKINT(mysql_stmt_execute(stmt));
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	dbexec_prepared;
	#endif // HAVE_POSTGRESQL

	// Fetch results (Note: Because we are using a stored procedure, we can't depend on LAST_INSERT_ID)
	// We fetch the (single) row, which will automatically populate our rtv.
	// In the case of an error, it will remain at the default error value of 0
	#ifdef HAVE_MYSQL
	dbstatus = mysql_stmt_fetch(stmt);
	if (dbstatus != 0)
	{
		DB_LOGF_ERR(DB_RPT_CON, "Failed to Create MSG_RPT_SET Entry", "status=%d, msg=%s, MSGS_REPORT_SET_INSERT(%i,%i,%i,%i)",
					dbstatus, mysql_stmt_error(stmt),
					grp_id, is_ack, is_nak, is_acl
			);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	if (dbtest_result(PGRES_TUPLES_OK) != 0)
	{
		DB_LOGF_ERR(DB_RPT_CON, "Failed to Create MSG_RPT_SET Entry", "status=%d, msg=%s, MSGS_REPORT_SET_INSERT(%i,%i,%i,%i)",
					PQresStatus(PQresultStatus(res)), PQresultErrorMessage(res),
					grp_id, is_ack, is_nak, is_acl
			);
		PQclear(res);
	#endif // HAVE_POSTGRESQL

		CHKZERO(status);
		*status = AMP_FAIL;
		return 0;
	}

	#ifdef HAVE_MYSQL
 	mysql_stmt_free_result(stmt);
	#endif // HAVE_MYSQL
	#ifdef HAVE_POSTGRESQL
	char *iptr = PQgetvalue(res, 0, 0);
	rtv = ntohl(*((uint32_t *) iptr));
	PQclear(res);
	#endif // HAVE_POSTGRESQL

	// Parse Recipients
	if (vec_num_entries(rpt->rx) > 0)
	{
		for(it = vecit_first(&(rpt->rx)); vecit_valid(it); it = vecit_next(it))
		{
			db_insert_msg_rpt_set_name(DB_RPT_CON,
									   rtv,
									   (char*)vecit_data(it),
									   status);
		}
	}

	// Parse Reports
	for(it = vecit_first(&(rpt->rpts)); vecit_valid(it); it = vecit_next(it))
	{
		db_insert_msg_rpt_set_rpt(DB_RPT_CON,
								  rtv,
								 (rpt_t*)vecit_data(it),
								 status);
	}

	return rtv;

		
}

#endif /* ifdef HAVE_MYSQL  or HAVE_POSTGRESQL*/

