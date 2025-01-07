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

#include "nm_rest.h"
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

// CivetWeb includes
#include <cjson/cJSON.h>
#include <civetweb.h>

// #include "nm_mgr_ui.h"
// #include "nm_mgr_print.h"

#define EID_BUF_SIZE    1024
#define MAX_CMD_SIZE    32
#define MAX_INPUT_BYTES 4096

#define BASE_API_URI   "/nm/api"
#define VERSION_URI    BASE_API_URI "/version$"
#define AGENTS_URI     BASE_API_URI "/agents$"
#define AGENTS_IDX_URI BASE_API_URI "/agents/idx/"
#define AGENTS_EID_URI BASE_API_URI "/agents/eid/"

// REST API Handler Declarations
static int versionHandler(struct mg_connection *conn, void *cbdata);
static int agentsHandler(struct mg_connection *conn, void *cbdata);
// FIXME static int agentIdxHandler(struct mg_connection *conn, void *cbdata);
static int agentEidHandler(struct mg_connection *conn, void *cbdata);

/*** Start Common API Functions ***/
static size_t SendJSON(struct mg_connection *conn, const cJSON *json_obj)
{
    char  *json_str     = cJSON_PrintUnformatted(json_obj);
    size_t json_str_len = strlen(json_str);

    /* Send HTTP message header */
    mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len);

    /* Send HTTP message content */
    mg_write(conn, json_str, json_str_len);

    /* Free string allocated by cJSON_Print* */
    cJSON_free(json_str);

    return json_str_len;
}

static int log_message(const struct mg_connection *conn _U_, const char *message)
{
    CACE_LOG_INFO("%s", message);
    return 1;
}

int nm_rest_start(struct mg_context **ctx, refdm_mgr_t *mgr)
{
    const char *options[] = { "listening_ports",
                              PORT,
                              "request_timeout_ms",
                              "10000",
                              "error_log_file",
                              "error.log",
#ifndef NO_SSL
                              "ssl_certificate",
                              "../../resources/cert/server.pem",
                              "ssl_protocol_version",
                              "3",
                              "ssl_cipher_list",
                              "DES-CBC3-SHA:AES128-SHA:AES128-GCM-SHA256",
#endif
                              "decode_url",
                              "no",
                              "enable_auth_domain_check",
                              "no",
                              0 };

    struct mg_callbacks callbacks;
    int                 err = 0;

/* Check if libcivetweb has been built with all required features. */
#ifndef NO_SSL
    if (!mg_check_feature(2))
    {
        CACE_LOG_ERR("Error: Embedded example built with SSL support, "
                     "but civetweb library build without.\n");
        err = 1;
    }

    mg_init_library(MG_FEATURES_SSL);

#else
    mg_init_library(0);

#endif
    if (err)
    {
        CACE_LOG_ERR("Cannot start CivetWeb - inconsistent build.\n");
        return EXIT_FAILURE;
    }

    /* Callback will print error messages to console */
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.log_message = log_message;

    /* Start CivetWeb web server */
    *ctx = mg_start(&callbacks, (void *)mgr, options);

    /* Check return value: */
    if (*ctx == NULL)
    {
        CACE_LOG_ERR("Cannot start CivetWeb - mg_start failed.\n");
        return EXIT_FAILURE;
    }

    /* Add URL Handlers.   */
    mg_set_request_handler(*ctx, VERSION_URI, versionHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_URI, agentsHandler, 0);
    // FIXME mg_set_request_handler(*ctx, AGENTS_IDX_URI, agentIdxHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_EID_URI, agentEidHandler, 0);
    CACE_LOG_INFO("REST API Server Started on port " PORT "\n");
    return EXIT_SUCCESS;
}

void nm_rest_stop(struct mg_context *ctx)
{
    CHKVOID(ctx);
    mg_stop(ctx);
}

/*** Start REST API Handlers ***/

static int versionHandler(struct mg_connection *conn, void *cbdata)
{
    cJSON                        *obj;
    const struct mg_request_info *ri = mg_get_request_info(conn);
    (void)cbdata; /* currently unused */

    if (0 != strcmp(ri->request_method, "GET"))
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only GET method supported for this page");
        return HTTP_METHOD_NOT_ALLOWED;
    }

    obj = cJSON_CreateObject();

    if (!obj)
    {
        /* insufficient memory? */
        mg_send_http_error(conn, HTTP_INTERNAL_ERROR, "Server error");
        return HTTP_INTERNAL_ERROR;
    }

    cJSON_AddStringToObject(obj, "civetweb_version", CIVETWEB_VERSION);
#if 0 // FIXME
    cJSON_AddNumberToObject(obj, "amp_version", AMP_VERSION);
    cJSON_AddStringToObject(obj, "amp_uri", AMP_PROTOCOL_URL);
    cJSON_AddStringToObject(obj, "amp_version_str", AMP_VERSION_STR);
#endif

    cJSON_AddStringToObject(obj, "build_date", __DATE__);
    cJSON_AddStringToObject(obj, "build_time", __TIME__);
    SendJSON(conn, obj);
    cJSON_Delete(obj);

    return HTTP_OK;
}

static int agentsGETHandler(struct mg_connection *conn)
{
    refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));
    cJSON       *obj = cJSON_CreateObject();

    if (!obj)
    {
        /* insufficient memory? */
        mg_send_http_error(conn, HTTP_INTERNAL_ERROR, "Server error");
        return HTTP_INTERNAL_ERROR;
    }

    cJSON *agentList = cJSON_AddArrayToObject(obj, "agents");
    if (agentList == NULL)
    {
        cJSON_Delete(obj);
        mg_send_http_error(conn, HTTP_INTERNAL_ERROR, "Server error");
        return HTTP_INTERNAL_ERROR;
    }

    pthread_mutex_lock(&mgr->agents_mutex);
    refdm_agent_dict_it_t agent_it;
    for (refdm_agent_dict_it(agent_it, mgr->agents); !refdm_agent_dict_end_p(agent_it); refdm_agent_dict_next(agent_it))
    {
        const refdm_agent_t *agent = refdm_agent_dict_cref(agent_it)->value;

        cJSON *agentObj = cJSON_CreateObject();
        cJSON_AddStringToObject(agentObj, "name", string_get_cstr(agent->eid));
        cJSON_AddNumberToObject(agentObj, "rpts_count", ari_list_size(agent->rptsets));
        cJSON_AddItemToArray(agentList, agentObj);
    }
    pthread_mutex_unlock(&mgr->agents_mutex);

    SendJSON(conn, obj);
    cJSON_Delete(obj);

    return HTTP_OK;
}

// This may be called via POST /agents or PUT /agents/$name
static int agentsCreateHandler(struct mg_connection *conn, const char *eid)
{
    // Sanity-check string length
    if (eid == NULL)
    {
        mg_send_http_error(conn, HTTP_BAD_REQUEST, "Invalid EID length");
        return HTTP_BAD_REQUEST;
    }

    refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

    refdm_agent_t *agent = refdm_mgr_agent_add(mgr, eid);
    if (!agent)
    {
        mg_send_http_error(conn, HTTP_BAD_REQUEST, "Unable to register agent");
        return HTTP_BAD_REQUEST;
    }

    mg_send_http_ok(conn, "text/plain", -1);
    mg_printf(conn, "Successfully sent Raw ARI Control");
    return HTTP_OK;
}

static int agentsHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET"))
    {
        return agentsGETHandler(conn);
    }
    else if (0 == strcmp(ri->request_method, "POST"))
    {
        char buffer[EID_BUF_SIZE];
        int  dlen = mg_read(conn, buffer, sizeof(buffer) - 1);
        if (dlen < 1)
        {
            mg_send_http_error(conn, HTTP_BAD_REQUEST, "Invalid request body data (expect EID name) %d", dlen);
            return HTTP_BAD_REQUEST;
        }
        else
        {
            buffer[dlen] = '\0';
            return agentsCreateHandler(conn, buffer);
        }
    }
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only GET and PUT methods supported for this page");
        return HTTP_METHOD_NOT_ALLOWED;
    }
}

static int agentSendRaw(struct mg_connection *conn, refdm_agent_t *agent, char *hex_sep)
{
    int retval = 0;
    int res;

    ari_list_t tosend;
    ari_list_init(tosend);

    {
        m_string_t hexbuf;
        m_string_init(hexbuf);
        cace_data_t databuf;
        cace_data_init(&databuf);

        char       *part;
        const char *ctrlsep = " \f\n\r\t\v"; // Identical to isspace()
        char       *saveptr = NULL;

        part = strtok_r(hex_sep, ctrlsep, &saveptr);
        while (!retval && (part != NULL))
        {
            CACE_LOG_DEBUG("Handling message part %s", part);

            string_set_str(hexbuf, part);
            // replace the databuf contents
            res = base16_decode(&databuf, hexbuf);
            if (res)
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "One input line does not contain base-16 encoded text");
                retval = HTTP_BAD_REQUEST;
            }

            ari_t *eset = ari_list_push_back_new(tosend);

            const char *errm = NULL;
            res              = ari_cbor_decode(eset, &databuf, NULL, &errm);
            if (res)
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "Error decoding execution ARI: %s", errm);
                retval = HTTP_BAD_REQUEST;

                ARI_FREE((char *)errm);
                errm = NULL;
            }
            if (!ari_is_lit_typed(eset, ARI_TYPE_EXECSET))
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "One value is not an EXECSET");
                retval = HTTP_BAD_REQUEST;
            }

            // FIXME: what is this? ui_postprocess_ctrl(id);

            part = strtok_r(NULL, ctrlsep, &saveptr);
        }

        string_clear(hexbuf);
        cace_data_deinit(&databuf);
    }

    CACE_LOG_INFO("Sending message with %d EXECSETs", ari_list_size(tosend));
    {
        refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

        cace_amm_msg_if_metadata_t meta;
        cace_amm_msg_if_metadata_init(&meta);
        cace_data_copy_from(&meta.dest, m_string_size(agent->eid) + 1, (cace_data_ptr_t)m_string_get_cstr(agent->eid));
        res = (mgr->mif.send)(tosend, &meta, mgr->mif.ctx);
        cace_amm_msg_if_metadata_deinit(&meta);

        // FIXME ui_log_transmit_msg(agent, &esetari);
    }
    ari_list_clear(tosend);

    mg_send_http_ok(conn, "text/plain", -1);
    mg_printf(conn, "Successfully sent Raw ARI Control");
    return HTTP_OK;
}

static int agentShowTextReports(struct mg_connection *conn, refdm_agent_t *agent)
{
    CHKRET(agent, HTTP_INTERNAL_ERROR);

    m_string_t uristr;
    m_string_init(uristr);

    /* Iterate through all RPTSET for this agent in one buffer */
    ari_list_it_t rpt_it;
    for (ari_list_it(rpt_it, agent->rptsets); !ari_list_end_p(rpt_it); ari_list_next(rpt_it))
    {
        const ari_t *val = ari_list_cref(rpt_it);

        int enc_ret = ari_text_encode(uristr, val, ARI_TEXT_ENC_OPTS_DEFAULT);
        m_string_cat_cstr(uristr, "\r\n"); // HTTP convention

        if (enc_ret)
        {
            return HTTP_INTERNAL_ERROR;
        }
    }

    mg_send_http_ok(conn, "text/plain", m_string_size(uristr));
    mg_write(conn, m_string_get_cstr(uristr), m_string_size(uristr));
    m_string_clear(uristr);
    return HTTP_OK;
}

static int agentShowHexReports(struct mg_connection *conn, refdm_agent_t *agent)
{
    CHKRET(agent, HTTP_INTERNAL_ERROR);

    cJSON *obj;
    cJSON *reports;

    obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "eid", string_get_cstr(agent->eid));
    reports = cJSON_AddArrayToObject(obj, "reports");

    /* Iterate through all RPTSET for this agent. */
    ari_list_it_t rpt_it;
    for (ari_list_it(rpt_it, agent->rptsets); !ari_list_end_p(rpt_it); ari_list_next(rpt_it))
    {
        const ari_t *val = ari_list_cref(rpt_it);

        cace_data_t bytestr;
        cace_data_init(&bytestr);
        int enc_ret = ari_cbor_encode(&bytestr, val);

        m_string_t hexstr;
        m_string_init(hexstr);
        int hex_ret = base16_encode(hexstr, &bytestr, false);
        cace_data_deinit(&bytestr);

        cJSON_AddItemToArray(reports, cJSON_CreateString(m_string_get_cstr(hexstr)));
        m_string_clear(hexstr);

        if (enc_ret || hex_ret)
        {
            cJSON_Delete(obj);
            return HTTP_INTERNAL_ERROR;
        }
    }

    SendJSON(conn, obj);
    cJSON_Delete(obj);
    return HTTP_OK;
}

/** Handler for /agents/eid*
 *    Supported requests:
 *    - PUT /agents/eid/$eid/hex - Send HEX-encoded CBOR Command (hex string as request body).
 *    - PUT /agents/eid/$eid/clear_reports - Clear all received reports for this agent.
 *    - PUT /agents/eid/$eid/clear_tables - Clear all received tables for this agent.
 *    - GET /agents/eid/$eid/reports/hex - Retrieve array of reports in CBOR-encoded HEX form
 *    - GET /agents/eid/$eid/reports/text - Retrieve array of reports in ARI Text form
 */
static int agentEidHandler(struct mg_connection *conn, void *cbdata _U_)
{
    static const char *seg_sep = "/?#";
    refdm_mgr_t       *mgr     = mg_get_user_data(mg_get_context(conn));

    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);

    // Ignore common prefix with trailing slash
    const char *suburi_begin = ri->local_uri + strlen(AGENTS_EID_URI);

    // Replace path segment separators with null terminator
    size_t uri_len = strlen(suburi_begin);
    char   buf[uri_len + 1];
    // end is past the trailing null
    char *end  = stpncpy(buf, suburi_begin, uri_len + 1);
    char *curs = buf;

    const char *seg_eid_begin = NULL;
    size_t      seg_eid_len   = 0;
    if (curs)
    {
        seg_eid_begin = curs;
        seg_eid_len   = strcspn(curs, seg_sep);
        curs += seg_eid_len;
        if (curs >= end)
        {
            curs = NULL;
        }
    }
    if (curs)
    {
        *curs = '\0';
        curs += 1;
        if (curs >= end)
        {
            curs = NULL;
        }
    }

    const char *seg_cmd_begin = NULL;
    size_t      seg_cmd_len   = 0;
    if (curs)
    {
        seg_cmd_begin = curs;
        seg_cmd_len   = strcspn(curs, seg_sep);
        curs += seg_cmd_len;
        if (curs >= end)
        {
            curs = NULL;
        }
    }
    if (curs)
    {
        *curs = '\0';
        curs += 1;
        if (curs >= end)
        {
            curs = NULL;
        }
    }

    const char *seg_opt_begin = NULL;
    size_t      seg_opt_len   = 0;
    if (curs)
    {
        seg_opt_begin = curs;
        seg_opt_len   = strcspn(curs, seg_sep);
        curs += seg_opt_len;
        if (curs >= end)
        {
            curs = NULL;
        }
    }
    if (curs)
    {
        *curs = '\0';
        curs += 1;
        if (curs >= end)
        {
            curs = NULL;
        }
    }

    if (!seg_eid_begin)
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Missing EID segment");
        return HTTP_NOT_FOUND;
    }
    if (!seg_cmd_begin)
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Missing command segment");
        return HTTP_NOT_FOUND;
    }
    if (curs && (curs != end))
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Extra path segments");
        return HTTP_NOT_FOUND;
    }

    // possible to be the same length, but no longer
    char dec_eid[seg_eid_len];
    int  got = mg_url_decode(seg_eid_begin, seg_eid_len, dec_eid, sizeof(dec_eid), 0);
    if (got <= 0)
    {
        mg_send_http_error(conn, HTTP_BAD_REQUEST, "Invalid agent EID");
        return HTTP_BAD_REQUEST;
    }

    refdm_agent_t *agent = refdm_mgr_agent_get(mgr, dec_eid);
    if (agent == NULL)
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Unknown agent EID: %s", dec_eid);
        return HTTP_NOT_FOUND;
    }

    if (0 == strcmp(seg_cmd_begin, "hex"))
    {
        if (0 != strcmp(ri->request_method, "PUT"))
        {
            mg_send_http_error(conn, HTTP_NOT_FOUND, "Invalid action on hex");
            return HTTP_NOT_FOUND;
        }

        // Request body contains CBOR-encoded HEX strings as lines
        char buffer[MAX_INPUT_BYTES];
        int  dlen = mg_read(conn, buffer, sizeof(buffer) - 1);
        if (dlen <= 0)
        {
            return HTTP_BAD_REQUEST;
        }
        buffer[dlen] = '\0';

        return agentSendRaw(conn, agent, buffer);
    }
#if 0 // FIXME
    else if (0 == strcmp(seg_cmd_begin, "clear_reports"))
    {
        if (0 != strcmp(ri->request_method, "PUT"))
        {
            mg_send_http_error(conn, HTTP_NOT_FOUND, "Invalid action on clear_reports");
            return HTTP_NOT_FOUND;
        }

        ui_clear_reports(agent);
        mg_send_http_ok(conn, "text/plain", -1);
        mg_printf(conn, "Successfully cleared reports");
        return HTTP_OK;
    }
#endif
    else if (0 == strcmp(seg_cmd_begin, "reports"))
    {
        if (0 == strcmp(seg_opt_begin, "text"))
        {
            return agentShowTextReports(conn, agent);
        }
        else if (0 == strcmp(seg_opt_begin, "hex"))
        {
            return agentShowHexReports(conn, agent);
        }
        else
        {
            mg_send_http_error(conn, HTTP_NOT_FOUND, "Invalid reports option");
            return HTTP_NOT_FOUND;
        }
    }

    // Invalid request if we make it to this point
    mg_send_http_error(conn, HTTP_NOT_FOUND, "Invalid request");
    return HTTP_NOT_FOUND;
}

#if 0 // FIXME is this necessary?

/** Handler for /agents/idx*
 *    Supported requests:
 *    - PUT /agents/idx/$idx/hex - Send HEX-encoded CBOR Command (hex string as request body)
 *    - PUT /agents/idx/$idx/clear_reports - Clear all received reports for this agent.
 *    - PUT /agents/idx/$idx/clear_tables - Clear all received tables for this agent.
 *    - GET /agents/idx/$idx/reports/hex - Retrieve array of reports in CBOR-encoded HEX form
 *    - GET /agents/idx/$idx/reports/text - Retrieve array of reports in ASCII Text form (same as ui)
 *    - GET /agents/idx/$idx/reports* - Alias for hex reports. format will change in the future.
 */
static int agentIdxHandler(struct mg_connection *conn, void *cbdata)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    (void)cbdata; /* currently unused */

    uint32_t idx      = 0;
    char     cmd[32]  = "";
    char     cmd2[32] = "";

    int cnt = sscanf(ri->local_uri, AGENTS_URI "/idx/%d/%[^/?]/%[^/?]", &idx, cmd, cmd2);

    if (cnt == 2 && 0 == strcmp(ri->request_method, "PUT"))
    {
        refdm_agent_t *agent = (refdm_agent_t *)vec_at(&gMgrDB.agents, idx);

        if (idx >= vec_num_entries_ptr(&gMgrDB.agents) || agent == NULL)
        {
            mg_send_http_error(conn, HTTP_NOT_FOUND, "Invalid agent idx");
            return HTTP_NOT_FOUND;
        }
        else if (0 == strcmp(cmd, "hex"))
        {
            // URL idx field translates to agent name
            // Optional query parameter "ts" will translate to timestamp (TODO: Always 0 for initial cut)
            // Request body contains CBOR-encoded HEX string
            char buffer[MAX_INPUT_BYTES];
            int  dlen = mg_read(conn, buffer, sizeof(buffer) - 1);
            if (dlen <= 0)
            {
                return HTTP_BAD_REQUEST;
            }
            buffer[dlen] = '\0';

            return agentSendRaw(conn,
                                0, // Timestamp TODO. This will be an optional query param
                                agent, buffer);
        }
        else if (0 == strcmp(cmd, "clear_reports"))
        {
            ui_clear_reports(agent);
            mg_send_http_ok(conn, "text/plain", -1);
            mg_printf(conn, "Successfully cleared reports");
            return HTTP_OK;
        }
        else if (0 == strcmp(cmd, "clear_tables"))
        {
            ui_clear_tables(agent);
            mg_send_http_ok(conn, "text/plain", -1);
            mg_printf(conn, "Successfully cleared tables");
            return HTTP_OK;
        }
    }
    else if (cnt >= 2 && 0 == strcmp(ri->request_method, "GET"))
    {
        if (0 == strcmp(cmd, "reports"))
        {
            if (cnt == 2 || 0 == strcmp(cmd2, "text"))
            {
                return agentShowTextReports(conn, (refdm_agent_t *)vec_at(&gMgrDB.agents, idx));
            }
            else if (cnt == 3 && 0 == strcmp(cmd2, "hex"))
            {
                return agentShowRawReports(conn, (refdm_agent_t *)vec_at(&gMgrDB.agents, idx));
            }
            else if (cnt == 3 && 0 == strcmp(cmd2, "json"))
            {
                return agentShowJSONReports(conn, (refdm_agent_t *)vec_at(&gMgrDB.agents, idx));
            }
        }
    }

    // Invalid request if we make it to this point
    mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Invalid request.");
    return HTTP_METHOD_NOT_ALLOWED;
}

#endif
