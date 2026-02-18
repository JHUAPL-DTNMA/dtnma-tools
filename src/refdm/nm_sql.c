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
 * This file is only included in the build when ::HAVE_POSTGRESQL is defined.
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

typedef struct refdm_db_pool_t {
    PGconn *conn;
    pthread_mutex_t lock;
} refdm_db_pool_t;

/* Global connections to the MYSQL Server. */
refdm_db_pool_t dbpool[MGR_NUM_SQL_CONNECTIONS];

#define checkConn(idx) (idx < MGR_NUM_SQL_CONNECTIONS && dbpool[idx].conn != NULL)
#define getConn(idx) pthread_mutex_lock(&dbpool[idx].lock)
#define giveConn(idx) pthread_mutex_unlock(&dbpool[idx].lock)

static refdm_db_t *gParms;

// Private functions
static char *db_mgr_sql_prepare(size_t idx, const char *query, char *stmtName, int nParams, const Oid *paramTypes);

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

static char *queries[MGR_NUM_SQL_CONNECTIONS][MGR_NUM_QUERIES];

/******** SQL Utility Macros ******************/
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

#define dbprep_declare(dbidx, idx, params, cols)       \
    PGconn     *conn     = dbpool[dbidx].conn;        \
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

#define dbexec_prepared                                                                                   \
    res = PQexecPrepared(conn, stmtName, nParams, paramValues, paramLengths, paramFormats, resultFormat); \
    CACE_LOG_DEBUG("dbexec_prepared result: %s", PQresStatus(PQresultStatus(res)));
#define dbtest_result(expected) ((PQresultStatus(res) == expected) ? 0 : 1)

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

#define query_log_err(status) \
    CACE_LOG_ERR("ERROR at %s %i: %s (errno: %d)", __FILE__, __LINE__, PQresultErrorMessage(res), status);

void refdm_db_log_msg(const char *file, int line, const char *fun, int level, size_t dbidx, const char *format, ...)
{
    if (!checkConn(dbidx))
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

    getConn(dbidx);
    dbprep_declare(dbidx, REFDM_DB_LOG_MSG, 5, 0);
    dbprep_bind_param_str(0, m_string_get_cstr(msg));
    dbprep_bind_param_int(1, level);
    dbprep_bind_param_str(2, fun);
    dbprep_bind_param_str(3, file);
    dbprep_bind_param_int(4, line);

    dbexec_prepared;
    giveConn(dbidx);
        
    DB_CHKVOID(dbtest_result(PGRES_COMMAND_OK))
    PQclear(res);

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

    for(size_t i = 0; i < MGR_NUM_SQL_CONNECTIONS; i++) {
        CACE_LOG_INFO("Initiating DB Pool idx %i", i);
        refdm_db_mgt_init_con(i, parms);        
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
    refdm_db_pool_t *conn = &dbpool[idx];

    if (idx >= MGR_NUM_SQL_CONNECTIONS) {
        CACE_LOG_ERR("Invalid DB pool index %d", idx);
        return 0;
    }
    if (conn->conn == NULL)
    {
        // Initialize mutex
        pthread_mutex_init(&conn->lock, NULL);
        
        // Initialize connection        
        gParms = parms;

        conn->conn = PQsetdbLogin(parms->server, NULL, NULL, NULL, parms->database, parms->username, parms->password);
        if (conn->conn == NULL)
        {
            CACE_LOG_WARNING("SQL Error: Null connection object returned");
        }
        else if (PQstatus(conn->conn) != CONNECTION_OK)
        {
            CACE_LOG_WARNING("SQL Error: %s", PQerrorMessage(conn->conn));
            PQfinish(conn->conn);
            conn->conn = NULL; // This was previously before the log entry which is likely a mistake
            CACE_LOG_INFO("--> 0");
            return 0;
        }

        // Initialize prepared queries

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
}
void refdm__db_mgt_close_conn(size_t idx)
{
    refdm_db_pool_t *conn = &dbpool[idx];
    
    if (conn->conn != NULL)
    {
        if (pthread_mutex_trylock(&conn->lock) ) {
            CACE_LOG_WARNING("refdm_db_mgt_close_conn; Unable to lock connection. Attempting to force close anyway");
        }
        
        /* close the connection to the database and cleanup */
        PQfinish(conn->conn);

        conn->conn = NULL;
        pthread_mutex_unlock(&conn->lock); // Mutex must be unlocked to destroy
        pthread_mutex_destroy(&conn->lock);
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
    refdm_db_pool_t *conn = &dbpool[idx];

    if (!checkConn(idx))
    {
        return -1;
    }

    result = (PQstatus(conn->conn) == CONNECTION_OK) ? 0 : 1;

    if (result != 0)
    {
        while (num_tries < SQL_CONN_TRIES)
        {
            // FIXME: Passing in gParms to a fn that assigns gParms
            /* NOTES/FIXME: Does this relate to gMbrDB.sql_info? If not, we have a disconnect in parameters
             * nm_mgr.c HAVE_MYSQL passes gMgrDB.sql_info to refdm_db_mgt_init which does the connection
             */
            refdm_db_mgt_init_con(idx, gParms);
            if ((result = (PQstatus(conn->conn) == CONNECTION_OK) ? 0 : 1) == 0)
            {

                DB_LOG_INFO(idx, "NM DB Connection Restored", NULL, NULL);
                return 0;
            }

            num_tries++;
        }
    }

    return result;
}

static char *db_mgr_sql_prepare(size_t idx, const char *query, char *stmtName, int nParams, const Oid *paramTypes)
{
    PGresult *pgresult = PQprepare(dbpool[idx].conn, stmtName, query, nParams, paramTypes);

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
int32_t refdm_db_mgt_query_fetch(int db_idx, PGresult **res, char *format, ...)
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

    getConn(db_idx);
    *res = PQexec(dbpool[db_idx].conn, m_string_get_cstr(query));
    m_string_clear(query);
    giveConn(db_idx);
    
    if ((PQresultStatus(*res) != PGRES_TUPLES_OK) && (PQresultStatus(*res) != PGRES_COMMAND_OK))
    {
        PQclear(*res);
        const char *errm = PQerrorMessage(dbpool[db_idx].conn);
        CACE_LOG_ERR("Database Error: %s", errm);
        CACE_LOG_INFO("-->%d", RET_FAIL_DATABASE);
        return RET_FAIL_DATABASE;
    }


    //CACE_LOG_INFO("-->%d", RET_PASS);
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

    getConn(db_idx);
    PGresult *res = PQexec(dbpool[db_idx].conn, m_string_get_cstr(query));
    m_string_clear(query);
    giveConn(db_idx);
    
    if (dbtest_result(PGRES_COMMAND_OK) != 0 && dbtest_result(PGRES_TUPLES_OK) != 0)
    {
        PQclear(res);
        const char *errm = PQerrorMessage(dbpool[db_idx].conn);
        CACE_LOG_ERR("Database Error: %s", errm);
        CACE_LOG_INFO("-->%d", 0);
        return 0;
    }

    if (idx != NULL)
    {
        // requires query string to include "RETURNING id"
        char *iptr = PQgetvalue(res, 0, 0);
        *idx       = ntohl(*((uint32_t *)iptr));
        if (*idx == 0)
        {
            CACE_LOG_ERR("Unknown last inserted row.", NULL);
            CACE_LOG_INFO("-->%d", 0);
            return 0;
        }
    }
    PQclear(res);
    
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

//-------------------------------------------------------------------------------------
int refdm_db_clear_rptset(int32_t agent_idx)
{
    PGresult *res = NULL;
    int ecode = refdm_db_mgt_query_fetch(DB_REST_CON, &res, "DELETE FROM %s WHERE agent_id=%d", TBL_NAME_RPTSET, agent_idx);
    if (ecode != RET_PASS)
    {
        CACE_LOG_ERR("Failed to clear table '%s' items. ecode: %d", TBL_NAME_RPTSET, ecode);
        return RET_FAIL_DATABASE;
    }

    PQclear(res);
    return RET_PASS;
}

//-------------------------------------------------------------------------------------
int refdm_db_fetch_rptset_count(int32_t agent_idx, size_t *count)
{
    PGresult *res = NULL;
    int ecode = refdm_db_mgt_query_fetch(DB_REST_CON, &res, "SELECT COUNT(*) FROM %s WHERE agent_id=%d", TBL_NAME_RPTSET,
                                     agent_idx);
    if (ecode != RET_PASS)
    {
        CACE_LOG_ERR("Failed to retrieve the count of table '%s' items. ecode: %d", TBL_NAME_RPTSET, ecode);
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
    return RET_PASS;
}

//-------------------------------------------------------------------------------------
int refdm_db_fetch_rptset_list(int32_t agent_idx, cace_ari_list_t *rptsets)
{
    // Get the rptset rows from the database
    PGresult *res = NULL;
    int ecode         = refdm_db_mgt_query_fetch(DB_REST_CON, &res, "SELECT %s FROM %s WHERE agent_id=%d",
                                             COL_NAME_REPORT_LIST_CBOR, TBL_NAME_RPTSET, agent_idx);
    
    // debugPostgresSqlResult(res, 9);
    if (ecode != RET_PASS)
    {
        CACE_LOG_ERR("Failed to retrieve the RPTSET items.");
        return RET_FAIL_DATABASE;
    }

    // Extract the column indexes relevant for this request
    int idx_report_list_cbor = PQfnumber(res, COL_NAME_REPORT_LIST_CBOR);
    // Bail if we failed to locate any relevant column names
    if (idx_report_list_cbor == -1)
    {
        CACE_LOG_ERR("Failed to locate table column for %s", COL_NAME_REPORT_LIST_CBOR);
        PQclear(res);
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
            continue;
        }
        CACE_FREE(errm);

        // Add the report to the list
        cace_ari_list_push_back_move(*rptsets, &ari_item);
    }

    CACE_LOG_INFO("Success with retrieval of rptset items. Num items: %d", num_rows);

    PQclear(res);
    return RET_PASS;
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
    PGresult *res = NULL;

    CACE_LOG_INFO("(%d)", id);

    /* Step 1: Grab the OID row. */
    if (refdm_db_mgt_query_fetch(DB_REST_CON, &res, "SELECT * FROM registered_agents WHERE registered_agents_id=%d", id)
        != RET_PASS)
    {
        CACE_LOG_ERR("Cant fetch agent %d", id);
        return NULL;
    }

    int name_fnum = PQfnumber(res, "agent_endpoint_uri");
    if (PQntuples(res) != 0)
    {
        m_string_t eid;
        m_string_init(eid);
        m_string_set_cstr(eid, PQgetvalue(res, 0, name_fnum));

        /* Step 3: Create structure for agent */
        refdm_agent_init(result);
        m_string_set(result->eid, eid);
    }

    PQclear(res);

    CACE_LOG_INFO("-->%p", result);
    return result;
}

int32_t refdm_db_fetch_agent_idx(const char *eid)
{
    int32_t result = 0;
    PGresult *res = NULL;

    /* Step 0: Sanity Check.*/
    if (eid == NULL)
    {
        CACE_LOG_ERR("Bad Args.");
        CACE_LOG_INFO("-->%d", 0);
        return 0;
    }

    const size_t eid_len      = strlen(eid);
    char        *eid_buf      = CACE_MALLOC(2 * eid_len + 1);
    size_t       eid_buf_used = PQescapeStringConn(dbpool[DB_RPT_CON].conn, eid_buf, eid, eid_len, NULL);

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
    int agent_id_fnum = PQfnumber(res, "registered_agents_id");
    if (PQntuples(res) != 0)
    {
        result = strtol(PQgetvalue(res, 0, agent_id_fnum), NULL, 10);
    }
    else
    {
        CACE_LOG_ERR("Did not find Agent with EID of %s", eid);
    }

/* Step 3: Free database resources. */
    PQclear(res);

    //CACE_LOG_INFO("-->%d", result);
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

    getConn(DB_RPT_CON);
    dbprep_declare(DB_RPT_CON, ARI_RPTSET_INSERT, 5, 1);

    dbprep_bind_param_byte(0, nonce_cbor.ptr, nonce_cbor.len);
    dbprep_bind_param_str(1, m_string_get_cstr(tp));
    dbprep_bind_param_str(2, m_string_get_cstr(rpt));
    dbprep_bind_param_byte(3, cbordata.ptr, cbordata.len);
    dbprep_bind_param_str(4, m_string_get_cstr(agent->eid));

    dbexec_prepared;
    giveConn(DB_RPT_CON);
    PQclear(res);

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

    getConn(DB_RPT_CON);
    dbprep_declare(DB_RPT_CON, ARI_AGENT_INSERT, 1, 1);
    dbprep_bind_param_str(0, m_string_get_cstr(eid));

    dbexec_prepared;
    giveConn(DB_RPT_CON);
    PQclear(res);
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

    getConn(DB_RPT_CON);
    dbprep_declare(DB_RPT_CON, ARI_EXECSET_INSERT, 5, 1);

    // p_nonce_cbor BYTEA, p_user_desc varchar, p_agent_id varchar, p_exec_set BYTEA, p_num_entries INT
    dbprep_bind_param_byte(0, nonce_cbor.ptr, nonce_cbor.len);
    dbprep_bind_param_str(1, "");
    dbprep_bind_param_str(2, m_string_get_cstr(agent->eid));
    dbprep_bind_param_byte(3, cbordata.ptr, cbordata.len);
    dbprep_bind_param_int(4, cace_ari_list_size(execset->targets));

    dbexec_prepared;
    giveConn(DB_RPT_CON);
    PQclear(res);
    

    // cleaning up vars
    cace_data_deinit(&cbordata);
    cace_data_deinit(&nonce_cbor);

    return rtv;
}
