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
#include <m-bstring.h>
// CivetWeb includes
#include <cjson/cJSON.h>
#include <civetweb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/// Chunking size for receiving request bodies
#define REQUEST_BODY_CHUNK 4096

#define BASE_API_URI      "/nm/api"
#define VERSION_URI       BASE_API_URI "/version$"
#define AGENTS_URI        BASE_API_URI "/agents$"
#define AGENTS_IDX_PREFIX BASE_API_URI "/agents/idx/"
#define AGENTS_EID_PREFIX BASE_API_URI "/agents/eid/"

static int requireContentType(struct mg_connection *conn, const char *match)
{
    const char *ctype = mg_get_header(conn, "content-type");
    if (!ctype)
    {
        return 2;
    }
    size_t ctype_len = strlen(ctype);
    size_t match_len = strlen(match);
    if (0 != strncasecmp(ctype, match, match_len))
    {
        return 3;
    }
    if (ctype_len > match_len)
    {
        if (ctype[match_len] != ';')
        {
            return 3;
        }
    }
    return 0;
}

static int readRequstBody(struct mg_connection *conn, m_bstring_t data)
{
    uint8_t chunk[REQUEST_BODY_CHUNK];

    while (true)
    {
        int got = mg_read(conn, chunk, sizeof(chunk));
        if (got == 0)
        {
            break;
        }
        else if (got < 0)
        {
            return HTTP_BAD_REQUEST;
        }
        m_bstring_push_back_bytes(data, got, chunk);
    }
    return 0;
}

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

/*** Start REST API Handlers ***/

static int versionHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcasecmp(ri->request_method, "GET"))
    {
        cJSON *obj = cJSON_CreateObject();
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
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only GET method supported");
        return HTTP_METHOD_NOT_ALLOWED;
    }
}

static int agentsGetHandler(struct mg_connection *conn)
{
    refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

    cJSON *obj = cJSON_CreateObject();
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

    pthread_mutex_lock(&mgr->agent_mutex);
    refdm_agent_list_it_t agent_it;
    for (refdm_agent_list_it(agent_it, mgr->agent_list); !refdm_agent_list_end_p(agent_it);
         refdm_agent_list_next(agent_it))
    {
        const refdm_agent_t *agent = *refdm_agent_list_ref(agent_it);

        cJSON *agentObj = cJSON_CreateObject();
        cJSON_AddStringToObject(agentObj, "name", string_get_cstr(agent->eid));
        cJSON_AddNumberToObject(agentObj, "rpts_count", cace_ari_list_size(agent->rptsets));
        cJSON_AddItemToArray(agentList, agentObj);
    }
    pthread_mutex_unlock(&mgr->agent_mutex);

    SendJSON(conn, obj);
    cJSON_Delete(obj);

    return HTTP_OK;
}

// This may be called via POST /agents
static int agentsPostHandler(struct mg_connection *conn)
{
    if (requireContentType(conn, "text/plain"))
    {
        mg_send_http_error(conn, HTTP_UNSUP_MEDIA_TYPE, "Only text/plain supported");
        return HTTP_UNSUP_MEDIA_TYPE;
    }

    // Request body contains direct EID text
    m_bstring_t body;
    m_bstring_init(body);
    int retval = readRequstBody(conn, body);

    m_string_t eid;
    m_string_init(eid);
    if (!retval)
    {
        m_bstring_push_back(body, '\0');

        m_string_set_cstr(eid, (const char *)m_bstring_view(body, 0, m_bstring_size(body)));
        m_string_strim(eid);

        // Sanity-check string length
        if (m_string_size(eid) <= 1)
        {
            mg_send_http_error(conn, HTTP_UNPROCESSABLE_CNT, "Invalid request body data (expect EID name)");
            retval = HTTP_UNPROCESSABLE_CNT;
        }
    }

    if (!retval)
    {
        refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

        refdm_agent_t *agent = refdm_mgr_agent_add(mgr, m_string_get_cstr(eid));
        if (!agent)
        {
            mg_send_http_error(conn, HTTP_BAD_REQUEST, "Unable to register agent");
            retval = HTTP_BAD_REQUEST;
        }
    }

    if (!retval)
    {
        m_string_t resp;
        m_string_init_printf(resp, "Successfully created agent %s", m_string_get_cstr(eid));

        // FIXME should really be a 207
        mg_send_http_ok(conn, "text/plain", m_string_size(resp));
        mg_printf(conn, "%s", m_string_get_cstr(resp));
        m_string_clear(resp);
        retval = HTTP_OK;
    }

    m_string_clear(eid);
    m_bstring_clear(body);
    return retval;
}

static int agentsHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcasecmp(ri->request_method, "GET"))
    {
        return agentsGetHandler(conn);
    }
    else if (0 == strcasecmp(ri->request_method, "POST"))
    {
        return agentsPostHandler(conn);
    }
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only GET and PUT methods supported");
        return HTTP_METHOD_NOT_ALLOWED;
    }
}

static int agentParseHex(struct mg_connection *conn, cace_ari_list_t tosend)
{
    int retval = 0;
    int res;

    // Request body contains hex-encoded binary-encoded ARIs as lines
    m_bstring_t body;
    m_bstring_init(body);
    retval = readRequstBody(conn, body);

    if (!retval)
    {
        m_bstring_push_back(body, '\0');

        m_string_t hexbuf;
        m_string_init(hexbuf);
        cace_data_t databuf;
        cace_data_init(&databuf);
        const char *errm = NULL;

        static const char *arisep = " \f\n\r\t\v"; // Identical to isspace()

        const size_t body_len = m_bstring_size(body);
        const char  *curs     = (const char *)m_bstring_view(body, 0, body_len);
        const char  *end      = curs + body_len;
        while (!retval && (curs < end))
        {
            size_t part_len = strcspn(curs, arisep);
            if (part_len == 0)
            {
                break;
            }
            // skip over optional prefix
            if (strncasecmp(curs, "0x", 2) == 0)
            {
                curs += 2;
                part_len -= 2;
            }

            m_string_set_cstrn(hexbuf, curs, part_len);
            CACE_LOG_DEBUG("Handling message part %s", m_string_get_cstr(hexbuf));

            // replace the databuf contents
            res = cace_base16_decode(&databuf, hexbuf);
            if (res)
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "One input line does not contain base-16 encoded text: %s",
                                   m_string_get_cstr(hexbuf));
                retval = HTTP_BAD_REQUEST;
            }

            cace_ari_t *item = cace_ari_list_push_back_new(tosend);

            res = cace_ari_cbor_decode(item, &databuf, NULL, &errm);
            if (res)
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "Error decoding execution ARI: %s", errm);
                retval = HTTP_BAD_REQUEST;

                CACE_FREE((char *)errm);
                errm = NULL;
            }
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                string_t buf;
                string_init(buf);
                cace_ari_text_encode(buf, item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_DEBUG("decoded ARI as %s", string_get_cstr(buf));
                string_clear(buf);
            }

            if (!cace_ari_is_lit_typed(item, CACE_ARI_TYPE_EXECSET))
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "One value is not an EXECSET");
                retval = HTTP_BAD_REQUEST;
            }

            // FIXME: what is this? ui_postprocess_ctrl(id);

            curs += part_len;
            size_t sep_len = strspn(curs, arisep);
            curs += sep_len;
        }

        cace_data_deinit(&databuf);
        string_clear(hexbuf);
    }

    m_bstring_clear(body);
    return retval;
}

static int agentParseText(struct mg_connection *conn, cace_ari_list_t tosend)
{
    int retval = 0;
    int res;

    // Request body contains URI text-encoded ARIs as lines
    m_bstring_t body;
    m_bstring_init(body);
    retval = readRequstBody(conn, body);

    if (!retval)
    {
        m_bstring_push_back(body, '\0');

        m_string_t linebuf;
        m_string_init(linebuf);
        const char *errm = NULL;

        static const char *ctrlsep = "\n\r"; // only newline

        const size_t body_len = m_bstring_size(body);
        const char  *curs     = (const char *)m_bstring_view(body, 0, body_len);
        const char  *end      = curs + body_len;
        while (!retval && (curs < end))
        {
            size_t part_len = strcspn(curs, ctrlsep);
            if (part_len == 0)
            {
                break;
            }

            m_string_set_cstrn(linebuf, curs, part_len);
            CACE_LOG_DEBUG("Handling message line %s", m_string_get_cstr(linebuf));

            cace_ari_t *eset = cace_ari_list_push_back_new(tosend);

            res = cace_ari_text_decode(eset, linebuf, &errm);
            if (res)
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "Error decoding execution ARI: %s", errm);
                retval = HTTP_BAD_REQUEST;

                CACE_FREE((char *)errm);
                errm = NULL;
            }
            if (!cace_ari_is_lit_typed(eset, CACE_ARI_TYPE_EXECSET))
            {
                mg_send_http_error(conn, HTTP_BAD_REQUEST, "One value is not an EXECSET");
                retval = HTTP_BAD_REQUEST;
            }

            // FIXME: what is this? ui_postprocess_ctrl(id);

            curs += part_len;
            size_t sep_len = strspn(curs, ctrlsep);
            curs += sep_len;
        }

        string_clear(linebuf);
    }

    m_bstring_clear(body);
    return retval;
}

static int agentSendItems(struct mg_connection *conn, refdm_agent_t *agent, cace_ari_list_t tosend)
{
    int retval = 0;
    CACE_LOG_INFO("Sending message with %d EXECSETs", cace_ari_list_size(tosend));
    {
        refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

        cace_amm_msg_if_metadata_t meta;
        cace_amm_msg_if_metadata_init(&meta);
        cace_ari_set_tstr(&meta.dest, m_string_get_cstr(agent->eid), true);

        int res = (mgr->mif.send)(tosend, &meta, mgr->mif.ctx);
        cace_amm_msg_if_metadata_deinit(&meta);
        if (res)
        {
            CACE_LOG_ERR("failed to send with status %d", res);
            mg_send_http_error(conn, HTTP_INTERNAL_ERROR, "Failed sending to agent");
            retval = HTTP_INTERNAL_ERROR;
        }

        // FIXME ui_log_transmit_msg(agent, &esetari);
    }

    if (!retval)
    {
        const char *resp = "Successfully sent EXECSETs";
        cace_ari_list_clear(tosend);

        mg_send_http_ok(conn, "text/plain", strlen(resp));
        mg_printf(conn, "%s", resp);
        retval = HTTP_OK;
    }
    return retval;
}

static int agentShowTextReports(struct mg_connection *conn, refdm_agent_t *agent)
{
    CHKRET(agent, HTTP_INTERNAL_ERROR);
    int retval = 0;

    m_string_t body;
    m_string_init(body);

    /* Iterate through all RPTSET for this agent in one buffer */
    cace_ari_list_it_t rpt_it;
    for (cace_ari_list_it(rpt_it, agent->rptsets); !cace_ari_list_end_p(rpt_it); cace_ari_list_next(rpt_it))
    {
        const cace_ari_t *val = cace_ari_list_cref(rpt_it);

        m_string_t uristr;
        m_string_init(uristr);
        int enc_ret = cace_ari_text_encode(uristr, val, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

        m_string_cat(body, uristr);
        m_string_clear(uristr);
        m_string_cat_cstr(body, "\r\n"); // HTTP convention

        if (enc_ret)
        {
            mg_send_http_error(conn, HTTP_INTERNAL_ERROR, "encoding failure");
            retval = HTTP_INTERNAL_ERROR;
            break;
        }
    }

    if (!retval)
    {
        mg_send_http_ok(conn, "text/uri-list", m_string_size(body));
        mg_write(conn, m_string_get_cstr(body), m_string_size(body));
        retval = HTTP_OK;
    }
    m_string_clear(body);
    return retval;
}

static int agentShowHexReports(struct mg_connection *conn, refdm_agent_t *agent)
{
    CHKRET(agent, HTTP_INTERNAL_ERROR);
    int retval = 0;

    m_string_t body;
    m_string_init(body);

    /* Iterate through all RPTSET for this agent. */
    cace_ari_list_it_t rpt_it;
    for (cace_ari_list_it(rpt_it, agent->rptsets); !cace_ari_list_end_p(rpt_it); cace_ari_list_next(rpt_it))
    {
        const cace_ari_t *val = cace_ari_list_cref(rpt_it);

        cace_data_t bytestr;
        cace_data_init(&bytestr);
        int enc_ret = cace_ari_cbor_encode(&bytestr, val);

        m_string_t hexstr;
        m_string_init(hexstr);
        int hex_ret = cace_base16_encode(hexstr, &bytestr, false);
        cace_data_deinit(&bytestr);

        m_string_cat(body, hexstr);
        m_string_clear(hexstr);
        m_string_cat_cstr(body, "\r\n"); // HTTP convention

        if (enc_ret || hex_ret)
        {
            mg_send_http_error(conn, HTTP_INTERNAL_ERROR, "encoding failure");
            retval = HTTP_INTERNAL_ERROR;
            break;
        }
    }

    if (!retval)
    {
        mg_send_http_ok(conn, "text/plain", m_string_size(body));
        mg_write(conn, m_string_get_cstr(body), m_string_size(body));
        retval = HTTP_OK;
    }

    m_string_clear(body);
    return retval;
}

/// Characters disallowed in URI segments (per RFC 3986) to know where they end
static const char *uri_seg_sep = "/?#";

static int getSegmentDecoded(struct mg_connection *conn, const char *prefix, m_string_t seg)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    // Ignore common prefix with trailing slash
    const char *suburi_begin = ri->local_uri + strlen(prefix);

    m_string_t rawbuf;
    m_string_init_set_cstr(rawbuf, suburi_begin);
    size_t seg_len = m_string_cspn(rawbuf, uri_seg_sep);
    m_string_left(rawbuf, seg_len);

    if (m_string_empty_p(rawbuf))
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Missing or empty segment");
        return HTTP_NOT_FOUND;
    }

    // possible to be the same length, with trailing null, but no longer
    char decbuf[seg_len + 1];
    int  got = mg_url_decode(m_string_get_cstr(rawbuf), seg_len, decbuf, seg_len + 1, 0);
    m_string_clear(rawbuf);
    if (got <= 0)
    {
        mg_send_http_error(conn, HTTP_BAD_REQUEST, "Invalid segment percent encoding");
        return HTTP_BAD_REQUEST;
    }

    m_string_set_cstr(seg, decbuf);
    return 0;
}

static int getAgentFromEid(struct mg_connection *conn, const char *prefix, refdm_agent_t **agent)
{
    m_string_t eid;
    m_string_init(eid);
    int res = getSegmentDecoded(conn, prefix, eid);
    if (res)
    {
        m_string_clear(eid);
        return res;
    }

    refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

    *agent = refdm_mgr_agent_get_eid(mgr, m_string_get_cstr(eid));
    m_string_clear(eid);
    if (*agent == NULL)
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Unknown agent EID: %s", m_string_get_cstr(eid));
        return HTTP_NOT_FOUND;
    }

    return 0;
}

static int getFormParam(struct mg_connection *conn, char *form, size_t form_len)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (ri->query_string)
    {
        int res = mg_get_var(ri->query_string, strlen(ri->query_string), "form", form, form_len);
        if ((res == -2) || ((strcasecmp(form, "text") != 0) && (strcasecmp(form, "hex") != 0)))
        {
            mg_send_http_error(conn, HTTP_BAD_REQUEST, "Form parameter must be either text or hex");
            return HTTP_BAD_REQUEST;
        }
    }
    CACE_LOG_DEBUG("Parsed form=%s", form);

    return 0;
}

/** The ./send resource of either agent form.
 */
static int agentAnySendHandler(struct mg_connection *conn, refdm_agent_t *agent)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    char form[10] = "text"; // size enough to hold valid values

    int retval = getFormParam(conn, form, sizeof(form));
    if (retval)
    {
        return retval;
    }

    if (0 == strcasecmp(ri->request_method, "POST"))
    {
        cace_ari_list_t tosend;
        cace_ari_list_init(tosend);
        if (strcasecmp(form, "text") == 0)
        {
            // either is acceptable
            if (requireContentType(conn, "text/uri-list") && requireContentType(conn, "text/plain"))
            {
                mg_send_http_error(conn, HTTP_UNSUP_MEDIA_TYPE, "Only text/uri-list or text/plain supported");
                retval = HTTP_UNSUP_MEDIA_TYPE;
            }
            if (!retval)
            {
                retval = agentParseText(conn, tosend);
            }
        }
        else if (strcasecmp(form, "hex") == 0)
        {
            if (requireContentType(conn, "text/plain"))
            {
                mg_send_http_error(conn, HTTP_UNSUP_MEDIA_TYPE, "Only text/plain supported");
                retval = HTTP_UNSUP_MEDIA_TYPE;
            }
            if (!retval)
            {
                retval = agentParseHex(conn, tosend);
            }
        }
        if (retval)
        {
            cace_ari_list_clear(tosend);
            return retval;
        }
        return agentSendItems(conn, agent, tosend);
    }
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only POST method supported");
        return HTTP_METHOD_NOT_ALLOWED;
    }
}

/** Handler /agents/eid/$eid/send - Send EXECSET encoded according to query key "form"
 */
static int agentEidSendHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);
    refdm_agent_t *agent = NULL;

    int retval = getAgentFromEid(conn, AGENTS_EID_PREFIX, &agent);
    if (retval)
    {
        return retval;
    }

    return agentAnySendHandler(conn, agent);
}

/** Handler /agents/eid/$eid/clear_reports - Clear all received reports for this agent.
 */
static int agentEidClearReportsHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);
    refdm_agent_t *agent = NULL;

    int res = getAgentFromEid(conn, AGENTS_EID_PREFIX, &agent);
    if (res)
    {
        return res;
    }

    if (0 == strcasecmp(ri->request_method, "POST"))
    {
        refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));
        refdm_mgr_clear_reports(mgr, agent);

        const char *resp = "Successfully cleared reports";
        mg_send_http_ok(conn, "text/plain", strlen(resp));
        mg_printf(conn, "%s", resp);
        return HTTP_OK;
    }
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only POST method supported");
        return HTTP_METHOD_NOT_ALLOWED;
    }
}

/** The ./reports resource of either agent form.
 */
static int agentAnyReportsHandler(struct mg_connection *conn, refdm_agent_t *agent)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);

    char form[10] = "text"; // size enough to hold valid values

    int retval = getFormParam(conn, form, sizeof(form));
    if (retval)
    {
        return retval;
    }

    if (0 == strcasecmp(ri->request_method, "GET"))
    {
        if (strcasecmp(form, "text") == 0)
        {
            retval = agentShowTextReports(conn, agent);
        }
        else if (strcasecmp(form, "hex") == 0)
        {
            retval = agentShowHexReports(conn, agent);
        }
    }
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only GET method supported");
        retval = HTTP_METHOD_NOT_ALLOWED;
    }
    return retval;
}

/** Handler /agents/eid/$eid/reports/hex - Retrieve array of reports in CBOR-encoded HEX form
 */
static int agentEidReportsHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);
    refdm_agent_t *agent = NULL;

    int res = getAgentFromEid(conn, AGENTS_EID_PREFIX, &agent);
    if (res)
    {
        return res;
    }

    return agentAnyReportsHandler(conn, agent);
}

static int getAgentFromIdx(struct mg_connection *conn, const char *prefix, refdm_agent_t **agent)
{
    m_string_t idx;
    m_string_init(idx);
    int res = getSegmentDecoded(conn, prefix, idx);
    if (res)
    {
        m_string_clear(idx);
        return res;
    }

    const char *seg_idx_begin = m_string_get_cstr(idx);
    const char *seg_idx_end   = seg_idx_begin + m_string_size(idx);
    char       *seg_idx_used;
    size_t      index = strtoull(seg_idx_begin, &seg_idx_used, 10);
    m_string_clear(idx);
    if (seg_idx_used < seg_idx_end)
    {
        // the entire segment was not converted
        mg_send_http_error(conn, HTTP_BAD_REQUEST, "Invalid agent index text: %s", seg_idx_begin);
        return HTTP_BAD_REQUEST;
    }

    refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));

    *agent = refdm_mgr_agent_get_index(mgr, index);
    if (*agent == NULL)
    {
        mg_send_http_error(conn, HTTP_NOT_FOUND, "Unknown agent index: %zu", index);
        return HTTP_NOT_FOUND;
    }

    return 0;
}

/** Handler /agents/idx/$idx/hex - Send HEX-encoded CBOR Command (hex string as request body).
 */
static int agentIdxSendHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);
    refdm_agent_t *agent = NULL;

    int res = getAgentFromIdx(conn, AGENTS_IDX_PREFIX, &agent);
    if (res)
    {
        return res;
    }

    return agentAnySendHandler(conn, agent);
}

/** Handler /agents/idx/$idx/clear_reports - Clear all received reports for this agent.
 */
static int agentIdxClearReportsHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);
    refdm_agent_t *agent = NULL;

    int res = getAgentFromIdx(conn, AGENTS_IDX_PREFIX, &agent);
    if (res)
    {
        return res;
    }

    if (0 == strcasecmp(ri->request_method, "POST"))
    {
        refdm_mgr_t *mgr = mg_get_user_data(mg_get_context(conn));
        refdm_mgr_clear_reports(mgr, agent);

        const char *resp = "Successfully cleared reports";
        mg_send_http_ok(conn, "text/plain", strlen(resp));
        mg_printf(conn, "%s", resp);
        return HTTP_OK;
    }
    else
    {
        mg_send_http_error(conn, HTTP_METHOD_NOT_ALLOWED, "Only POST method supported");
        return HTTP_METHOD_NOT_ALLOWED;
    }
}

/** Handler /agents/idx/$idx/reports/hex - Retrieve array of reports in CBOR-encoded HEX form
 */
static int agentIdxReportsHandler(struct mg_connection *conn, void *cbdata _U_)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    CACE_LOG_DEBUG("Handling local_uri %s", ri->local_uri);
    refdm_agent_t *agent = NULL;

    int res = getAgentFromIdx(conn, AGENTS_IDX_PREFIX, &agent);
    if (res)
    {
        return res;
    }

    return agentAnyReportsHandler(conn, agent);
}

int refdm_nm_rest_start(struct mg_context **ctx, refdm_mgr_t *mgr)
{
    CHKERR1(ctx);
    CHKERR1(mgr);

    char port_buf[6]; // holds uint16_t
    snprintf(port_buf, sizeof(port_buf), "%u", mgr->rest_listen_port);

    const char *options[] = { "listening_ports",
                              port_buf,
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
    unsigned            features = 0;
    int                 err      = 0;

/* Check if libcivetweb has been built with all required features. */
#ifndef NO_SSL
    if (!mg_check_feature(MG_FEATURES_SSL))
    {
        CACE_LOG_ERR("Embedded example built with SSL support, "
                     "but civetweb library build without.");
        err = 3;
    }
    features |= MG_FEATURES_SSL;
#endif

    unsigned got = mg_init_library(features);
    if (got != features)
    {
        CACE_LOG_ERR("REST server failed to start with requested feature flags %u got %u", features, got);
        err = 2;
    }

    if (err)
    {
        return err;
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
        return 4;
    }

    /* Add URL Handlers.   */
    mg_set_request_handler(*ctx, VERSION_URI, versionHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_URI, agentsHandler, 0);

    mg_set_request_handler(*ctx, AGENTS_EID_PREFIX "*/clear_reports$", agentEidClearReportsHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_EID_PREFIX "*/send$", agentEidSendHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_EID_PREFIX "*/reports$", agentEidReportsHandler, 0);

    mg_set_request_handler(*ctx, AGENTS_IDX_PREFIX "*/clear_reports$", agentIdxClearReportsHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_IDX_PREFIX "*/send$", agentIdxSendHandler, 0);
    mg_set_request_handler(*ctx, AGENTS_IDX_PREFIX "*/reports$", agentIdxReportsHandler, 0);

    CACE_LOG_INFO("REST API Server Started on port %s", port_buf);
    return 0;
}

void refdm_nm_rest_stop(struct mg_context *ctx)
{
    CHKVOID(ctx);
    mg_stop(ctx);
}
