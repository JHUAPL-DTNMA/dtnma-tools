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
/** @file
 * This file is only included in the build when either ::HAVE_POSTGRESQL or
 * ::HAVE_MYSQL are defined.
 */
#include "nm_sql.h"

#include <cace/ari/cbor.h>
#include <cace/amm/typing.h>
#include <cace/amm/semtype.h>
#include <cace/util/logging.h>
#include <cace/ari/text.h>
#include <cace/ari/text_util.h>

#include <string.h>
#include <arpa/inet.h>
#include <m-bstring.h>

// Constants: Database table names
const char *TBL_NAME_RPTSET = "ari_rptset";

// Constants: Database column names for the RPTSET table
const char *COL_NAME_REFERENCE_TIME   = "reference_time";
const char *COL_NAME_AGENT_ID         = "agent_id";
const char *COL_NAME_ARI_RPTSET_ID    = "ari_rptset_id";
const char *COL_NAME_REPORT_LIST      = "report_list";
const char *COL_NAME_REPORT_LIST_CBOR = "report_list_cbor";
const char *COL_NAME_NONCE_INT        = "nonce_int";
const char *COL_NAME_NONCE_BYTES      = "nonce_bytes";

/* Number of threads interacting with the database.
 - DB Polling Thread - Check for reports pending transmission
 - Mgr Report Rx Thread - Log received reports
 - UI - If we add UI functions to query the DB, a separate connection will be needed.
*/
typedef enum db_con_e
{
    /// Connection for receiving outgoing controls from database
    DB_CTRL_CON,
    /// Connection associated with DM RX thread.
    /// All activities in this thread will execute within transactions.
    DB_RPT_CON,
    /// Connection for REST API access
    DB_REST_CON,
    /// Total number of connections
    MGR_NUM_SQL_CONNECTIONS
} db_con_t;

/* Global connection to the MYSQL Server. */
#ifdef HAVE_MYSQL
static MYSQL *gConn[MGR_NUM_SQL_CONNECTIONS];
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static PGconn *gConn[MGR_NUM_SQL_CONNECTIONS];
#endif // HAVE_POSTGRESQL
static refdm_db_t *gParms;

static pthread_mutex_t db_rest_con_use;

// Private functions
#ifdef HAVE_MYSQL
static MYSQL_STMT *db_mgr_sql_prepare(size_t idx, const char *query);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static char *db_mgr_sql_prepare(size_t idx, const char *query, char *stmtName, int nParams, const Oid *paramTypes);
#endif // HAVE_POSTGRESQL

cace_ari_ac_t *db_query_ac(size_t dbidx, int ac_id);

/* Prepared query definitions
 *
 * This enumeration provides a consistent interface for selecting a
 * given query.  For simplicity, the same list of queries are used
 * across all database connections/threads, though most queries are
 * only utilized by a particular thread at this time.  This listing
 * may include some queries not being used at this time.
 */
enum queries
{
    ARI_RPTSET_INSERT,

    ARI_AGENT_INSERT,

    ARI_EXECSET_INSERT,

    REFDM_DB_LOG_MSG,
    MGR_NUM_QUERIES
};

#ifdef HAVE_MYSQL
static MYSQL_STMT *queries[MGR_NUM_SQL_CONNECTIONS][MGR_NUM_QUERIES];
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static char *queries[MGR_NUM_SQL_CONNECTIONS][MGR_NUM_QUERIES];
#endif // HAVE_POSTGRESQL

/******** SQL Utility Macros ******************/
#ifdef HAVE_MYSQL
#define dbprep_bind_res_cmn(idx, var, type)    \
    bind_res[idx].buffer_type = type;          \
    bind_res[idx].buffer      = (char *)var;   \
    bind_res[idx].is_null     = &is_null[idx]; \
    bind_res[idx].error       = &is_err[idx];
#endif // HAVE_MYSQL

#ifdef HAVE_MYSQL
#define dbprep_bind_param_cmn(idx, var, type)   \
    bind_param[idx].buffer_type = type;         \
    bind_param[idx].buffer      = (char *)&var; \
    bind_param[idx].is_null     = 0;            \
    bind_param[idx].error       = 0;
#endif // HAVE_MYSQL

#ifdef HAVE_MYSQL
#define dbprep_bind_param_bool(idx, var)   dbprep_bind_param_cmn(idx, var, MYSQL_TYPE_LONG);
#define dbprep_bind_param_int(idx, var)    dbprep_bind_param_cmn(idx, var, MYSQL_TYPE_LONG);
#define dbprep_bind_param_short(idx, var)  dbprep_bind_param_cmn(idx, var, MYSQL_TYPE_SHORT);
#define dbprep_bind_param_float(idx, var)  dbprep_bind_param_cmn(idx, var, MYSQL_TYPE_FLOAT);
#define dbprep_bind_param_double(idx, var) dbprep_bind_param_cmn(idx, var, MYSQL_TYPE_DOUBLE);
#define dbprep_bind_param_bigint(idx, var) dbprep_bind_param_cmn(idx, var, MYSQL_TYPE_LONGLONG);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define dbprep_bind_param_bool(idx, var)        \
    net8Vals[idx]     = (uint8_t)var;           \
    paramValues[idx]  = (char *)&net8Vals[idx]; \
    paramLengths[idx] = sizeof(net8Vals[idx]);  \
    paramFormats[idx] = 1; /* binary */
#define dbprep_bind_param_int(idx, var)          \
    net32Vals[idx]    = htonl((uint32_t)var);    \
    paramValues[idx]  = (char *)&net32Vals[idx]; \
    paramLengths[idx] = sizeof(net32Vals[idx]);  \
    paramFormats[idx] = 1; /* binary */
#define dbprep_bind_param_short(idx, var)        \
    net16Vals[idx]    = htons((uint16_t)var);    \
    paramValues[idx]  = (char *)&net16Vals[idx]; \
    paramLengths[idx] = sizeof(net16Vals[idx]);  \
    paramFormats[idx] = 1; /* binary */
#define dbprep_bind_param_float(idx, var)           \
    net32Vals[idx]    = htonl(*((uint32_t *)&var)); \
    paramValues[idx]  = (char *)&net32Vals[idx];    \
    paramLengths[idx] = sizeof(net32Vals[idx]);     \
    paramFormats[idx] = 1; /* binary */
#define dbprep_bind_param_double(idx, var)         \
    double_to_nbo(var, (double *)&net64Vals[idx]); \
    paramValues[idx]  = (char *)&net64Vals[idx];   \
    paramLengths[idx] = sizeof(net64Vals[idx]);    \
    paramFormats[idx] = 1; /* binary */
#define dbprep_bind_param_bigint(idx, var)         \
    vast_to_nbo(var, (uint64_t *)&net64Vals[idx]); \
    paramValues[idx]  = (char *)&net64Vals[idx];   \
    paramLengths[idx] = sizeof(net64Vals[idx]);    \
    paramFormats[idx] = 1; /* binary */
#endif                     // HAVE_POSTGRESQL

#ifdef HAVE_MYSQL
#define dbprep_bind_param_str(idx, var)                              \
    size_t len_##var              = (var == NULL) ? 0 : strlen(var); \
    bind_param[idx].buffer_length = len_##var;                       \
    bind_param[idx].length        = &len_##var;                      \
    bind_param[idx].buffer_type   = MYSQL_TYPE_STRING;               \
    bind_param[idx].buffer        = (m_string_t *)var;               \
    bind_param[idx].is_null       = 0;                               \
    bind_param[idx].error         = 0;

#define dbprep_bind_param_null(idx)                \
    bind_param[idx].buffer_type = MYSQL_TYPE_NULL; \
    bind_param[idx].buffer      = 0;               \
    bind_param[idx].is_null     = 0;               \
    bind_param[idx].error       = 0;
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define dbprep_bind_param_str(idx, var)                  \
    paramValues[idx]  = var;                             \
    paramLengths[idx] = 0; /* ignored for text format */ \
    paramFormats[idx] = 0; /* text */
#define dbprep_bind_param_null(idx) \
    paramValues[idx]  = NULL;       \
    paramLengths[idx] = 0;          \
    paramFormats[idx] = 1;
#define dbprep_bind_param_byte(idx, var, length) \
    paramValues[idx]  = (const char *)var;       \
    paramLengths[idx] = length;                  \
    paramFormats[idx] = 1;
#endif // HAVE_POSTGRESQL

#ifdef HAVE_MYSQL
#define dbprep_bind_res_int(idx, var)   dbprep_bind_res_cmn(idx, &var, MYSQL_TYPE_LONG);
#define dbprep_bind_res_short(idx, var) dbprep_bind_res_cmn(idx, &var, MYSQL_TYPE_SHORT);
#define dbprep_bind_res_str(idx, var, len)             \
    dbprep_bind_res_cmn(idx, &var, MYSQL_TYPE_STRING); \
    bind_res[idx].buffer_length = len;
#define dbprep_bind_res_int_ptr(idx, var) dbprep_bind_res_cmn(idx, var, MYSQL_TYPE_LONG);

#define dbprep_dec_res_int(idx, var) \
    int var;                         \
    dbprep_bind_res_int(idx, var);
#endif // HAVE_MYSQL

#ifdef HAVE_MYSQL
/* NOTE: my_bool is replaceed with 'bool' for MySQL 8.0.1+, but is still used for MariaDB
 *  A build flag may be needed to switch between them based on My/Maria-SQL version to support both.
 */
#define dbprep_declare(dbidx, idx, params, cols) \
    MYSQL_STMT   *stmt = queries[dbidx][idx];    \
    MYSQL_BIND    bind_res[cols];                \
    MYSQL_BIND    bind_param[params];            \
    bool          is_null[cols];                 \
    bool          is_err[cols];                  \
    unsigned long lengths[params];               \
    memset(bind_res, 0, sizeof(bind_res));       \
    memset(bind_param, 0, sizeof(bind_param));   \
    int return_status;
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define dbprep_declare(dbidx, idx, params, cols)       \
    PGconn     *conn     = gConn[dbidx];               \
    char       *stmtName = queries[dbidx][idx];        \
    int         nParams  = params;                     \
    const char *paramValues[nParams];                  \
    int         paramLengths[nParams];                 \
    int         paramFormats[nParams];                 \
    int         resultFormat = 1; /* binary results */ \
    PGresult   *res;                                   \
    uint8_t     net8Vals[nParams];                     \
    uint16_t    net16Vals[nParams];                    \
    uint32_t    net32Vals[nParams];                    \
    uint64_t    net64Vals[nParams];
#endif // HAVE_POSTGRESQL

#ifdef HAVE_POSTGRESQL
#define dbexec_prepared                                                                                   \
    res = PQexecPrepared(conn, stmtName, nParams, paramValues, paramLengths, paramFormats, resultFormat); \
    CACE_LOG_DEBUG("dbexec_prepared result: %s", PQresStatus(PQresultStatus(res)));
#define dbtest_result(expected) ((PQresultStatus(res) == expected) ? 0 : 1)
#endif // HAVE_POSTGRESQL

#ifdef HAVE_MYSQL
#define DB_CHKVOID(status)     \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        return;                \
    }
#define DB_CHKINT(status)      \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        return 0;              \
    }
#define DB_CHKNULL(status)     \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        return NULL;           \
    }
#define DB_CHKUSR(status, usr) \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        usr;                   \
    }
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define DB_CHKVOID(status)     \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        PQclear(res);          \
        return;                \
    }
#define DB_CHKINT(status)      \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        PQclear(res);          \
        return 0;              \
    }
#define DB_CHKNULL(status)     \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        PQclear(res);          \
        return NULL;           \
    }
#define DB_CHKUSR(status, usr) \
    if (status != 0)           \
    {                          \
        query_log_err(status); \
        PQclear(res);          \
        usr;                   \
    }
#endif // HAVE_POSTGRESQL

#ifdef HAVE_MYSQL
#define query_log_err(status) \
    CACE_LOG_ERR("ERROR at %s %i: %s (errno: %d)", __FILE__, __LINE__, mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
#define query_log_err(status) \
    CACE_LOG_ERR("ERROR at %s %i: %s (errno: %d)", __FILE__, __LINE__, PQresultErrorMessage(res), status);
#endif // HAVE_POSTGRESQL

void refdm_db_log_msg(const char *file, int line, const char *fun, int level, size_t dbidx, const char *format, ...)
{
    if (dbidx >= MGR_NUM_SQL_CONNECTIONS || gConn[dbidx] == NULL)
    {
        // DB Not connected or invalid idx
        return;
    }

    va_list    val;
    m_string_t msg;
    m_string_init(msg);
    va_start(val, format);
    m_string_vprintf(msg, format, val);
    va_end(val);
    dbprep_declare(dbidx, REFDM_DB_LOG_MSG, 5, 0);
    dbprep_bind_param_str(0, m_string_get_cstr(msg));
    dbprep_bind_param_int(1, level);
    dbprep_bind_param_str(2, fun);
    dbprep_bind_param_str(3, file);
    dbprep_bind_param_int(4, line);

#ifdef HAVE_MYSQL
    DB_CHKVOID(mysql_stmt_bind_param(stmt, bind_param));
    DB_CHKVOID(mysql_stmt_execute(stmt));
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    dbexec_prepared;
    DB_CHKVOID(dbtest_result(PGRES_COMMAND_OK))
    PQclear(res);
#endif // HAVE_POSTGRESQL

    m_string_clear(msg);
}

/******************************************************************************
 *
 * \par Function Name: refdm_db_mgt_init
 *
 * \par Initializes the gConnection to the database.
 *
 * \retval 0 Failure
 *        !0 Success
 *
 * \param[in]  server The machine hosting the SQL database.
 * \param[in]  user The username for the SQL database.
 * \param[in]  pwd The password for this user.
 * \param[in]  database The database housing the DTNMP tables.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  01/26/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
uint32_t refdm_db_mgt_init(refdm_db_t *parms, uint32_t clear, uint32_t log)
{
    pthread_mutex_init(&db_rest_con_use, NULL);

    CACE_LOG_INFO("setting up db connect for DB_CTRL_CON");
    refdm_db_mgt_init_con(DB_CTRL_CON, parms);

    CACE_LOG_INFO("setting up db connect for DB_RPT_CON");
    refdm_db_mgt_init_con(DB_RPT_CON, parms);

    CACE_LOG_INFO("setting up db connect for DB_REST_CON");
    refdm_db_mgt_init_con(DB_REST_CON, parms);

    // A mysql_commit or mysql_rollback will automatically start a new transaction as the old one is closed
    if (gConn[DB_RPT_CON] != NULL)
    {
#ifdef HAVE_MYSQL
        mysql_autocommit(gConn[DB_RPT_CON], 0);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
// TODO postgresql : turn off autocommit
#endif // HAVE_POSTGRESQL
       // DB_LOG_INFO(DB_CTRL_CON, "NM Manager Connections Initialized");
    }

    CACE_LOG_INFO("-->0");
    return 0;
}

/** Initialize specified (thread-specific) SQL connection and prepared queries
 *  Prepared queries are connection specific.  While we may not use all prepared statements for all connections,
 *initializing the same sets everywhere simplifies management.
 **/
uint32_t refdm_db_mgt_init_con(size_t idx, refdm_db_t *parms)
{

    if (gConn[idx] == NULL)
    {
#ifdef HAVE_MYSQL
        gConn[idx] = mysql_init(NULL);
#endif // HAVE_MYSQL
        gParms = parms;

#ifdef HAVE_MYSQL
        if (!mysql_real_connect(gConn[idx], parms->server, parms->username, parms->password, parms->database, 0, NULL,
                                0))
        {
            if (gConn[idx] != NULL)
            {
                mysql_close(gConn[idx]);
            }
        }

#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
        gConn[idx] = PQsetdbLogin(parms->server, NULL, NULL, NULL, parms->database, parms->username, parms->password);
        if (gConn[idx] == NULL)
        {
            CACE_LOG_WARNING("SQL Error: Null connection object returned");
        }
        else if (PQstatus(gConn[idx]) != CONNECTION_OK)
        {
            CACE_LOG_WARNING("SQL Error: %s", PQerrorMessage(gConn[idx]));
            PQfinish(gConn[idx]);
#endif                         // HAVE_POSTGRESQL
            gConn[idx] = NULL; // This was previously before the log entry which is likely a mistake
            CACE_LOG_INFO("--> 0");
            return 0;
        }

// Initialize prepared queries
#ifdef HAVE_MYSQL

#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL

        // RPTSET values
        // signature IN p_nonce_cbor BYTEA, p_reference_time TIMESTAMP, p_report_list TEXT, p_report_list_cbor BYTEA,
        // p_agent_endpoint_uri TEXT
        queries[idx][ARI_RPTSET_INSERT] =
            db_mgr_sql_prepare(idx, "call sp__insert_rptset($1::bytea, $2::timestamp, $3::text, $4::bytea, $5::text)",
                               "ARI_RPTSET_INSERT", 5, NULL);

        queries[idx][REFDM_DB_LOG_MSG] =
            db_mgr_sql_prepare(idx,
                               "INSERT INTO DB_LOG_INFO (msg,level,source,file,line) "
                               "VALUES($1::varchar,$2::int4,$3::varchar,$4::varchar,$5::int4)",
                               "REFDM_DB_LOG_MSG", 5, NULL);

        queries[idx][ARI_AGENT_INSERT] =
            db_mgr_sql_prepare(idx, "call SP__insert_agent($1::varchar,null)", "SP__insert_agent", 2, NULL);

        // EXECSET values
        queries[idx][ARI_EXECSET_INSERT] =
            db_mgr_sql_prepare(idx, "call SP__insert_execset($1::bytea, $2::varchar, $3::varchar, $4::bytea, $5::int4)",
                               "SP__insert_execset", 5, NULL);

#endif // HAVE_POSTGRESQL
    }

    CACE_LOG_INFO("refdm_db_mgt_init -->1");
    return 1;
}

/******************************************************************************
 *
 * \par Function Name: refdm_db_mgt_close
 *
 * \par Close the database gConnection.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *****************************************************************************/

void refdm_db_mgt_close(void)
{
    CACE_LOG_INFO("refdm_db_mgt_close", "()");

    for (int i = 0; i < MGR_NUM_SQL_CONNECTIONS; i++)
    {
        refdm__db_mgt_close_conn(i);
    }
    CACE_LOG_INFO("refdm_db_mgt_close", "-->.");

    pthread_mutex_destroy(&db_rest_con_use);
}
void refdm__db_mgt_close_conn(size_t idx)
{
    if (gConn[idx] != NULL)
    {
// Free prepared queries (mysql_stmt_close())
#ifdef HAVE_MYSQL
        for (int i = 0; i < MGR_NUM_QUERIES; i++)
        {
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
 * \par Function Name: refdm_db_mgt_connected
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

int refdm_db_mgt_connected(size_t idx)
{
    int     result    = -1;
    uint8_t num_tries = 0;

    if (gConn[idx] == NULL)
    {
        return -1;
    }

#ifdef HAVE_MYSQL
    result = mysql_ping(gConn[idx]);
#endif // HAVE_MYSQL
#if defined(HAVE_POSTGRESQL)
    result = (PQstatus(gConn[idx]) == CONNECTION_OK) ? 0 : 1;
#endif // HAVE_POSTGRESQL
    if (result != 0)
    {
        while (num_tries < SQL_CONN_TRIES)
        {
            // FIXME: Passing in gParms to a fn that assigns gParms
            /* NOTES/FIXME: Does this relate to gMbrDB.sql_info? If not, we have a disconnect in parameters
             * nm_mgr.c HAVE_MYSQL passes gMgrDB.sql_info to refdm_db_mgt_init which does the connection
             */
            refdm_db_mgt_init_con(idx, gParms);
#ifdef HAVE_MYSQL
            if ((result = mysql_ping(gConn[idx])) == 0)
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
                if ((result = (PQstatus(gConn[idx]) == CONNECTION_OK) ? 0 : 1) == 0)
                {
#endif // HAVE_POSTGRESQL

                    if (idx == DB_RPT_CON)
                    {
// Disable autocommit to ensure all queries are executed within a transaction to ensure consistency
// A mysql_commit or mysql_rollback will automatically start a new transaction as the old one is closed
#ifdef HAVE_MYSQL
                        mysql_autocommit(gConn[DB_RPT_CON], 0);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
// TODO postgresql : turn off autocommit
#endif // HAVE_POSTGRESQL
                    }
                    DB_LOG_INFO(idx, "NM DB Connection Restored", NULL, NULL);
                    return 0;
                }

            num_tries++;
        }
    }

    return result;
}

#ifdef HAVE_MYSQL
static MYSQL_STMT *db_mgr_sql_prepare(size_t idx, const char *query)
{
    MYSQL_STMT *rtv = mysql_stmt_init(gConn[idx]);
    if (rtv == NULL)
    {
        CACE_LOG_ERR("Failed to allocate statement", NULL);
        return rtv;
    }

    if (mysql_stmt_prepare(rtv, query, strlen(query)) != 0)
    {
        CACE_LOG_ERR("Failed to prepare %s: errno %d, error= %s", query, mysql_stmt_errno(rtv), mysql_stmt_error(rtv));
        mysql_stmt_close(rtv);
        return NULL;
    }
    return rtv;
}
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
static char *db_mgr_sql_prepare(size_t idx, const char *query, char *stmtName, int nParams, const Oid *paramTypes)
{
    PGresult *pgresult = PQprepare(gConn[idx], stmtName, query, nParams, paramTypes);

    if (pgresult == NULL)
    { // out of memory or failure to send the conmmand at all
        CACE_LOG_ERR("Failed to allocate statement %s", query);
    }

    if (PQresultStatus(pgresult) != PGRES_COMMAND_OK)
    {
        CACE_LOG_ERR("Failed to prepare %s: errno %d, error= %s", query, PQresultStatus(pgresult),
                     PQresultErrorMessage(pgresult));
        /* there is no libpq function for deleting a prepared statement, the SQL DEALLOCATE statement can be used
           for that purpose but if you do not explicitly deallocate a prepared statement, it is deallocated when the
           session ends. So no actuaion should be needed*/
    }

    PQclear(pgresult);
    return stmtName;
}
#endif // HAVE_POSTGRESQL

/******************************************************************************
 *
 * \par Function Name: refdm_db_mgt_query_fetch
 *
 * \par Runs a fetch in the database given a query and returns the result.
 *
 * \return Returns ::RET_PASS on success otherwise @c RET_FAIL_* on failure.
 *
 * \param[out] res    The result.
 * \param[in]  format Format to build query
 * \param[in]  ...    Var args to build query given format string.
 *
 * \note
 * The @c res structure should be a pointer but without being allocated. This
 *     function will create the storage.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/
#ifdef HAVE_MYSQL
int32_t refdm_db_mgt_query_fetch(int db_idx, MYSQL_RES **res, char *format, ...)
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    int32_t refdm_db_mgt_query_fetch(int db_idx, PGresult **res, char *format, ...)
#endif // HAVE_POSTGRESQL
{
    /* Step 0: Sanity check. */
    if (format == NULL)
    {
        CACE_LOG_ERR("Bad Args.");
        CACE_LOG_INFO("-->%d", RET_FAIL_BAD_ARGS);
        return RET_FAIL_BAD_ARGS;
    }

    /*
     * Step 1: Assert the DB connection. This should not only check
     *         the connection as well as try and re-establish it.
     */
    if (refdm_db_mgt_connected(db_idx) != 0)
    {
        CACE_LOG_ERR("DB not connected.");
        CACE_LOG_INFO("-->%d", RET_FAIL_DATABASE_CONNECTION);
        return RET_FAIL_DATABASE_CONNECTION;
    }

    va_list args;
    va_start(args, format); // format is last parameter before "..."
    m_string_t query;
    m_string_init_vprintf(query, format, args);
    va_end(args);

#ifdef HAVE_MYSQL
    int status = mysql_query(gConn[db_idx], m_string_get_cstr(query));
    m_string_clear(query);
    if (status)
    {
        const char *errm = mysql_error(gConn[db_idx]);
        CACE_LOG_ERR("Database Error: %s", errm);
        CACE_LOG_INFO("-->%d", RET_FAIL_DATABASE);
        return RET_FAIL_DATABASE;
    }

    if ((*res = mysql_store_result(gConn[db_idx])) == NULL)
    {
        CACE_LOG_ERR("Can't get result.");
        CACE_LOG_INFO("-->%d", RET_FAIL_DATABASE);
        return RET_FAIL_DATABASE;
    }
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    *res = PQexec(gConn[db_idx], m_string_get_cstr(query));
    m_string_clear(query);
    if ((PQresultStatus(*res) != PGRES_TUPLES_OK) && (PQresultStatus(*res) != PGRES_COMMAND_OK))
    {
        PQclear(*res);
        const char *errm = PQerrorMessage(gConn[db_idx]);
        CACE_LOG_ERR("Database Error: %s", errm);
        CACE_LOG_INFO("-->%d", RET_FAIL_DATABASE);
        return RET_FAIL_DATABASE;
    }
#endif // HAVE_POSTGRESQL

    CACE_LOG_INFO("-->%d", RET_PASS);
    return RET_PASS;
}

/******************************************************************************
 *
 * \par Function Name: refdm_db_mgt_query_insert
 *
 * \par Runs an insert in the database given a query and returns the
 *      index of the inserted item.
 *
 * \return -1 - System Error
 *         0   - Non-fatal issue.
 *         >0         - The index of the inserted item.
 *
 * \param[out] idx    The index of the inserted row.
 * \param[in]  format Format to build query
 * \param[in]  ...    Var args to build query given format string.
 *
 * \note
 *  The @c idx may be NULL if the insert index is not needed.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  01/26/17  E. Birrane     Initial implementation (JHU/APL).
 *****************************************************************************/
int32_t refdm_db_mgt_query_insert(int db_idx, uint32_t *idx, char *format, ...)
{
    if (refdm_db_mgt_connected(db_idx) == 0)
    {
        CACE_LOG_ERR("DB not connected.", NULL);
        CACE_LOG_INFO("-->%d", -1);
        return -1;
    }

    va_list args;
    va_start(args, format); // format is last parameter before "..."
    m_string_t query;
    m_string_init_vprintf(query, format, args);
    va_end(args);

#ifdef HAVE_MYSQL
    int status = mysql_query(gConn[db_idx], m_string_get_cstr(query));
    m_string_clear(query);
    if (status)
    {
        const char *errm = mysql_error(gConn[db_idx]);
        CACE_LOG_ERR("Database Error: %s", errm);
        CACE_LOG_INFO("-->%d", 0);
        return 0;
    }
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    PGresult *res = PQexec(gConn[db_idx], m_string_get_cstr(query));
    m_string_clear(query);
    if (dbtest_result(PGRES_COMMAND_OK) != 0 && dbtest_result(PGRES_TUPLES_OK) != 0)
    {
        PQclear(res);
        const char *errm = PQerrorMessage(gConn[db_idx]);
        CACE_LOG_ERR("Database Error: %s", errm);
        CACE_LOG_INFO("-->%d", 0);
        return 0;
    }
#endif // HAVE_POSTGRESQL

    if (idx != NULL)
    {
#ifdef HAVE_MYSQL
        if ((*idx = (uint32_t)mysql_insert_id(gConn[db_idx])) == 0)
        {
            CACE_LOG_ERR("Unknown last inserted row.", NULL);
            CACE_LOG_INFO("-->%d", 0);
            return 0;
        }
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
        // requires query string to include "RETURNING id"
        char *iptr = PQgetvalue(res, 0, 0);
        *idx       = ntohl(*((uint32_t *)iptr));
        if (*idx == 0)
        {
            CACE_LOG_ERR("Unknown last inserted row.", NULL);
            CACE_LOG_INFO("-->%d", 0);
            return 0;
        }
#endif // HAVE_POSTGRESQL
    }
#ifdef HAVE_POSTGRESQL
    PQclear(res);
#endif // HAVE_POSTGRESQL

    CACE_LOG_INFO("-->%d", 1);
    return 1;
}

/**
 * Takes a C string and returns the corresponding ::cace_ari_t
 *
 *  \return Returns @c RET_PASS on success otherwise @c RET_FAIL_* on failure.
 *
 * * @param[out] ari_item The ARI to decode into.
 *
 * * @param[in] cbor_str The string to decode from.
 *
 *  @param[out] errm If non-null, this will be set to a specific error message
 * associated with any failure. When the return code is non-zero, if the pointed-to pointer is non-null
 * it must be freed using CACE_FREE().
 */
static int transform_cbor_str_to_cace_data(cace_ari_t *ari_item, char *cbor_str, char **errm)
{
    size_t   bytea_len = 0;
    uint8_t *bytea_ptr = PQunescapeBytea((const uint8_t *)cbor_str, &bytea_len);

    cace_data_t inbin;
    cace_data_init_view(&inbin, bytea_len, bytea_ptr);

    // Transform from CBOR to ARI
    int ecode = cace_ari_cbor_decode(ari_item, &inbin, NULL, errm);
    cace_data_deinit(&inbin);
    free(bytea_ptr);
    if (ecode != 0)
    {
        return RET_FAIL_UNEXPECTED;
    }

    return RET_PASS;
}

#if FALSE && defined(HAVE_POSTGRESQL)

/**
 * Diagnostic method to aide with debugging the contents of the PostgreSQL result.
 * <p>
 * This should not be used in production code.
 */
static void debugPostgresSqlResult(PGresult *res, int max_row_cnt)
{
    fprintf(stderr, "\n\n >>>>>>>>>>>>>>----------------- DEBUG PGresult: -----------------<<<<<<<<<<<<<< \n");
    fprintf(stderr, "...PQstatus: %s \n\n", PQresStatus(PQresultStatus(res)));

    // Log the number of rows and number of cols
    int num_rows = PQntuples(res);
    int num_cols = PQnfields(res);
    fprintf(stderr, "...Number of rows: %d   |   Number of cols: %d\n", num_rows, num_cols);
    if (max_row_cnt > 0 && max_row_cnt < num_rows)
    {
        fprintf(stderr, "...Max rows to log: %d\n", max_row_cnt);
    }

    // Iterate through each row and send the table column and log contents
    for (int row = 0; row < num_rows; row++)
    {
        if (row == max_row_cnt)
        {
            break;
        }

        fprintf(stderr, "......Row %d:\n", row);
        for (int col = 0; col < num_cols; col++)
        {
            // Get the column name and value
            char *col_name = PQfname(res, col);
            char *value    = PQgetvalue(res, row, col);
            fprintf(stderr, ".........%s: %s\n", col_name, value);
        }
    }
}

#endif
#if defined(HAVE_POSTGRESQL)

//-------------------------------------------------------------------------------------
int refdm_db_clear_rptset(int32_t agent_idx)
{
    int ecode;
    if ((ecode = pthread_mutex_lock(&db_rest_con_use)))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return RET_FAIL_DATABASE_CONNECTION;
    }

    PGresult *res = NULL;
    ecode = refdm_db_mgt_query_fetch(DB_REST_CON, &res, "DELETE FROM %s WHERE agent_id=%d", TBL_NAME_RPTSET, agent_idx);
    if (ecode != RET_PASS)
    {
        CACE_LOG_ERR("Failed to clear table '%s' items. ecode: %d", TBL_NAME_RPTSET, ecode);
        pthread_mutex_unlock(&db_rest_con_use);
        return RET_FAIL_DATABASE;
    }

    PQclear(res);
    pthread_mutex_unlock(&db_rest_con_use);
    return RET_PASS;
}

//-------------------------------------------------------------------------------------
int refdm_db_fetch_rptset_count(int32_t agent_idx, size_t *count)
{
    int ecode;
    if ((ecode = pthread_mutex_lock(&db_rest_con_use)))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return RET_FAIL_DATABASE_CONNECTION;
    }

    PGresult *res = NULL;
    ecode = refdm_db_mgt_query_fetch(DB_REST_CON, &res, "SELECT COUNT(*) FROM %s WHERE agent_id=%d", TBL_NAME_RPTSET,
                                     agent_idx);
    if (ecode != RET_PASS)
    {
        CACE_LOG_ERR("Failed to retrieve the count of table '%s' items. ecode: %d", TBL_NAME_RPTSET, ecode);
        pthread_mutex_unlock(&db_rest_con_use);
        return RET_FAIL_DATABASE;
    }

    long count_val = 0;
    if ((PQntuples(res) > 0) && (PQnfields(res) > 0))
    {
        char *count_str = PQgetvalue(res, 0, 0);
        count_val       = strtol(count_str, NULL, 10);
    }
    *count = count_val;

    PQclear(res);
    pthread_mutex_unlock(&db_rest_con_use);
    return RET_PASS;
}

//-------------------------------------------------------------------------------------
int refdm_db_fetch_rptset_list(int32_t agent_idx, cace_ari_list_t *rptsets)
{
    int ecode;
    if ((ecode = pthread_mutex_lock(&db_rest_con_use)))
    {
        CACE_LOG_CRIT("failed to lock mutex");
        return RET_FAIL_DATABASE_CONNECTION;
    }

    // Get the rptset rows from the database
    PGresult *res = NULL;
    ecode         = refdm_db_mgt_query_fetch(DB_REST_CON, &res, "SELECT %s FROM %s WHERE agent_id=%d",
                                             COL_NAME_REPORT_LIST_CBOR, TBL_NAME_RPTSET, agent_idx);
    // debugPostgresSqlResult(res, 9);
    if (ecode != RET_PASS)
    {
        CACE_LOG_ERR("Failed to retrieve the RPTSET items.");
        pthread_mutex_unlock(&db_rest_con_use);
        return RET_FAIL_DATABASE;
    }

    // Extract the column indexes relevant for this request
    int idx_report_list_cbor = PQfnumber(res, COL_NAME_REPORT_LIST_CBOR);
    // Bail if we failed to locate any relevant column names
    if (idx_report_list_cbor == -1)
    {
        CACE_LOG_ERR("Failed to locate table column for %s", COL_NAME_REPORT_LIST_CBOR);
        PQclear(res);
        pthread_mutex_unlock(&db_rest_con_use);
        return RET_FAIL_DATABASE;
    }

    // Iterate through each row and transform to the equivalent rptset
    int num_rows = PQntuples(res);
    for (int row = 0; row < num_rows; row++)
    {
        // Extract the report_list_cbor from the database row
        char *cbor_str = PQgetvalue(res, row, idx_report_list_cbor);

        // Transform from database BYTEA to a RPTSET value
        cace_ari_t ari_item;
        cace_ari_init(&ari_item);
        char *errm = NULL;
        ecode      = transform_cbor_str_to_cace_data(&ari_item, cbor_str, &errm);
        if (ecode != RET_PASS)
        {
            // Skip to next on failure
            CACE_LOG_ERR("Database has invalid report.   row: %d   |   ecode: %d   |   errm: %s", row, ecode, errm);
            CACE_FREE(errm);
            pthread_mutex_unlock(&db_rest_con_use);
            continue;
        }
        CACE_FREE(errm);

        // Add the report to the list
        cace_ari_list_push_back_move(*rptsets, &ari_item);
    }

    CACE_LOG_INFO("Success with retrieval of rptset items. Num items: %d", num_rows);

    PQclear(res);
    pthread_mutex_unlock(&db_rest_con_use);
    return RET_PASS;
}

#endif // HAVE_POSTGRESQL

#ifdef HAVE_MYSQL

//-------------------------------------------------------------------------------------
int refdm_db_clear_rptset(int32_t agent_idx)
{
    return RET_FAIL_UNDEFINED;
}

//-------------------------------------------------------------------------------------
int refdm_db_fetch_rptset_count(int32_t agent_idx, size_t *count)
{
    return RET_FAIL_UNDEFINED;
}

//-------------------------------------------------------------------------------------
int refdm_db_fetch_rptset_list(int32_t agent_idx, cace_ari_list_t *rptsets)
{
    return RET_FAIL_UNDEFINED;
}

#endif // HAVE_MYSQL

/******************************************************************************
 *
 * \par Function Name: db_fetch_reg_agent
 *
 * \par Creates an adm_reg_agent_t structure from the database.
 *
 * \retval NULL Failure
 *        !NULL The built adm_reg_agent_t  structure.
 *
 * \param[in] id The Primary Key of the desired registered agent.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  07/12/13  S. Jacobs      Initial implementation,
 *  01/25/17  E. Birrane     Update to AMP 3.5.0 (JHU/APL)
 *****************************************************************************/
refdm_agent_t *refdm_db_fetch_agent(int32_t id)
{
    refdm_agent_t *result = NULL;
#ifdef HAVE_MYSQL
    MYSQL_RES *res = NULL;
    MYSQL_ROW  row;
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    PGresult *res = NULL;
#endif // HAVE_POSTGRESQL

    CACE_LOG_INFO("(%d)", id);

    /* Step 1: Grab the OID row. */
    if (refdm_db_mgt_query_fetch(DB_REST_CON, &res, "SELECT * FROM registered_agents WHERE registered_agents_id=%d", id)
        != RET_PASS)
    {
        CACE_LOG_ERR("Cant fetch agent %d", id);
        return NULL;
    }

#ifdef HAVE_MYSQL
    if ((row = mysql_fetch_row(res)) != NULL)
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
        int name_fnum = PQfnumber(res, "agent_endpoint_uri");
    if (PQntuples(res) != 0)
#endif // HAVE_POSTGRESQL
    {
        m_string_t eid;
        m_string_init(eid);
#ifdef HAVE_MYSQL
        m_string_set_cstr(eid, row[1]);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
        m_string_set_cstr(eid, PQgetvalue(res, 0, name_fnum));
#endif // HAVE POSTGRESQL

        /* Step 3: Create structure for agent */
        refdm_agent_init(result);
        m_string_set(result->eid, eid);
    }

#ifdef HAVE_MYSQL
    mysql_free_result(res);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    PQclear(res);
#endif // HAVE_POSTGRESQL

    CACE_LOG_INFO("-->%p", result);
    return result;
}

int32_t refdm_db_fetch_agent_idx(const char *eid)
{
    int32_t result = 0;
#ifdef HAVE_MYSQL
    MYSQL_RES *res = NULL;
    MYSQL_ROW  row;
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    PGresult *res = NULL;
#endif // HAVE_POSTGRESQL

    /* Step 0: Sanity Check.*/
    if (eid == NULL)
    {
        CACE_LOG_ERR("Bad Args.");
        CACE_LOG_INFO("-->%d", 0);
        return 0;
    }

    const size_t eid_len      = strlen(eid);
    char        *eid_buf      = CACE_MALLOC(2 * eid_len + 1);
    size_t       eid_buf_used = PQescapeStringConn(gConn[DB_RPT_CON], eid_buf, eid, eid_len, NULL);

    /* Step 1: Grab the OID row. */
    int status = refdm_db_mgt_query_fetch(DB_REST_CON, &res,
                                          "SELECT * FROM registered_agents WHERE agent_endpoint_uri='%s'", eid_buf);
    CACE_FREE(eid_buf);
    if (status != RET_PASS)
    {
        CACE_LOG_ERR("Can't fetch");
        CACE_LOG_INFO("-->%d", 0);
        return 0;
    }

/* Step 2: Parse information out of the returned row. */
#ifdef HAVE_MYSQL
    if ((row = mysql_fetch_row(res)) != NULL)
    {
        result = strtol(row[0], NULL, 10);
    }
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    int agent_id_fnum = PQfnumber(res, "registered_agents_id");
    if (PQntuples(res) != 0)
    {
        result = strtol(PQgetvalue(res, 0, agent_id_fnum), NULL, 10);
    }
#endif // HAVE_POSTGRESQL
    else
    {
        CACE_LOG_ERR("Did not find Agent with EID of %s", eid);
    }

/* Step 3: Free database resources. */
#ifdef HAVE_MYSQL
    mysql_free_result(res);
#endif // HAVE_MYSQL
#ifdef HAVE_POSTGRESQL
    PQclear(res);
#endif // HAVE_POSTGRESQL

    CACE_LOG_INFO("-->%d", result);
    return result;
}

/**
 * @param val Report
 * @param agent agent table set being inserted in
 * @param status Set to 0 if
 * parsing fails, but not modified on
 * success
 * @returns  Set ID, or 0 on error
 */
uint32_t refdm_db_insert_rptset(const cace_ari_t *val, const refdm_agent_t *agent)
{
    CACE_LOG_INFO("logging report set in db started");

    uint32_t rtv = 0;

    const cace_ari_rptset_t *rpt_set = cace_ari_cget_rptset(val);
    if (!rpt_set)
    {
        return 1;
    }

    // correlator_nonce: either NULL, UVAST, or BYTES
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &rpt_set->nonce, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("inserting RPTSET with nonce %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    cace_data_t nonce_cbor = CACE_DATA_INIT_NULL;
    cace_ari_cbor_encode(&nonce_cbor, &rpt_set->nonce);

    // reference_time INT not null,
    struct timespec ref_time;
    if (cace_ari_get_tp(&rpt_set->reftime, &ref_time))
    {
        CACE_LOG_ERR("unhandled ref_time value");
        return 1;
    }
    m_string_t tp;
    m_string_init(tp);
    cace_utctime_encode(tp, &ref_time, true);

    // report_list varchar as string
    m_string_t rpt;
    m_string_init(rpt);
    cace_ari_text_encode(rpt, val, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

    // report_list varchar as cbor,
    cace_data_t cbordata;
    cace_data_init(&cbordata);
    cace_ari_cbor_encode(&cbordata, val);

    dbprep_declare(DB_RPT_CON, ARI_RPTSET_INSERT, 5, 1);

    dbprep_bind_param_byte(0, nonce_cbor.ptr, nonce_cbor.len);
    dbprep_bind_param_str(1, m_string_get_cstr(tp));
    dbprep_bind_param_str(2, m_string_get_cstr(rpt));
    dbprep_bind_param_byte(3, cbordata.ptr, cbordata.len);
    dbprep_bind_param_str(4, m_string_get_cstr(agent->eid));

#ifdef HAVE_MYSQL
    mysql_stmt_bind_param(stmt, bind_param);
    dbprep_bind_res_int(0, rtv);
    mysql_stmt_execute(stmt);
    mysql_stmt_bind_result(stmt, bind_res);
#endif // HAVE_MYSQL

#ifdef HAVE_POSTGRESQL
    dbexec_prepared;
    PQclear(res);
#endif // HAVE_POSTGRESQL

    // cleaning up vars
    m_string_clear(tp);
    m_string_clear(rpt);
    cace_data_deinit(&cbordata);
    cace_data_deinit(&nonce_cbor);
    return rtv;
}

/**

 * @param eid agent eid being added
 * @param status Set to 0 if
 * parsing fails, but not modified on
 * success
 * @returns Report Set ID, or 0 on error
 */
uint32_t refdm_db_insert_agent(const m_string_t eid)
{
    CACE_LOG_INFO("logging agent in db started");
    uint32_t rtv = 0;
    int64_t  id;

    dbprep_declare(DB_RPT_CON, ARI_AGENT_INSERT, 1, 1);
    dbprep_bind_param_str(0, m_string_get_cstr(eid));

#ifdef HAVE_MYSQL
    mysql_stmt_bind_param(stmt, bind_param);
    dbprep_bind_res_int(0, rtv);
    mysql_stmt_execute(stmt);
    mysql_stmt_bind_result(stmt, bind_res);
#endif // HAVE_MYSQL

#ifdef HAVE_POSTGRESQL
    dbexec_prepared;
    PQclear(res);
#endif // HAVE_POSTGRESQL
       // cleaning up vars
    return rtv;
}
// cace_ari_execset_t
uint32_t refdm_db_insert_execset(const cace_ari_t *val, const refdm_agent_t *agent)
{
    uint32_t rtv = 0;

    const cace_ari_execset_t *execset = cace_ari_cget_execset(val);
    if (!execset)
    {
        return 1;
    }

    // correlator_nonce: either NULL, UVAST, or BYTES
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &execset->nonce, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("inserting EXECSET with nonce %s", m_string_get_cstr(buf));
        m_string_clear(buf);
    }

    cace_data_t nonce_cbor = CACE_DATA_INIT_NULL;
    cace_ari_cbor_encode(&nonce_cbor, &execset->nonce);

    // report_list varchar as cbor,xw
    cace_data_t cbordata;
    cace_data_init(&cbordata);
    cace_ari_cbor_encode(&cbordata, val);

    dbprep_declare(DB_RPT_CON, ARI_EXECSET_INSERT, 5, 1);

    // p_nonce_cbor BYTEA, p_user_desc varchar, p_agent_id varchar, p_exec_set BYTEA, p_num_entries INT
    dbprep_bind_param_byte(0, nonce_cbor.ptr, nonce_cbor.len);
    dbprep_bind_param_str(1, "");
    dbprep_bind_param_str(2, m_string_get_cstr(agent->eid));
    dbprep_bind_param_byte(3, cbordata.ptr, cbordata.len);
    dbprep_bind_param_int(4, cace_ari_list_size(execset->targets));

#ifdef HAVE_MYSQL
    mysql_stmt_bind_param(stmt, bind_param);
    dbprep_bind_res_int(0, rtv);
    mysql_stmt_execute(stmt);
    mysql_stmt_bind_result(stmt, bind_res);
#endif // HAVE_MYSQL

#ifdef HAVE_POSTGRESQL
    dbexec_prepared;
    PQclear(res);
#endif // HAVE_POSTGRESQL

    // cleaning up vars
    cace_data_deinit(&cbordata);
    cace_data_deinit(&nonce_cbor);

    return rtv;
}
