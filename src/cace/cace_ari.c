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
#include <cace/config.h>
#include <cace/ari.h>
#include <cace/ari/text.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text_util.h>
#include <cace/util/logging.h>
#include <qcbor/qcbor_decode.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

typedef enum
{
    /// Default invalid value
    CACE_ARI_FORM_INVALID,
    /** Detect the input by its initial characters as either text or hex
     * and use the opposite on output.
     */
    CACE_ARI_FORM_AUTO,
    /// Use text form ARIs
    CACE_ARI_FORM_TEXT,
    /// Use binary form ARIs
    CACE_ARI_FORM_CBOR,
    /// Use hex-encoded binary form ARIs
    CACE_ARI_FORM_CBORHEX,
} cace_ari_form_t;

static cace_ari_form_t get_form(const char *text)
{
    if (strcasecmp(text, "auto") == 0)
    {
        return CACE_ARI_FORM_AUTO;
    }
    else if (strcasecmp(text, "text") == 0)
    {
        return CACE_ARI_FORM_TEXT;
    }
    else if (strcasecmp(text, "cbor") == 0)
    {
        return CACE_ARI_FORM_CBOR;
    }
    else if (strcasecmp(text, "cborhex") == 0)
    {
        return CACE_ARI_FORM_CBORHEX;
    }
    fprintf(stderr, "Invalid ARI encoding form: %s\n", text);
    return CACE_ARI_FORM_INVALID;
}

static FILE *get_file(const char *name, const char *mode)
{
    if (!mode)
    {
        return NULL;
    }
    if (!name || (strcmp(name, "-") == 0))
    {
        if (mode[0] == 'r')
        {
            return stdin;
        }
        else
        {
            return stdout;
        }
    }
    FILE *file = fopen(name, mode);
    if (!file)
    {
        char buf[256];
        strerror_r(errno, buf, sizeof(buf));
        fprintf(stderr, "Failed to open file %s (err %d): %s\n", name, errno, buf);
    }
    return file;
}

#if defined(ARI_TEXT_PARSE)
/** Read a single value and indicate whether to continue reading.
 *
 * @return Zero upon success.
 * A negative value to indicate end-of-input.
 */
static int read_text(cace_ari_t *inval, FILE *source)
{
    char  *buf = NULL;
    size_t len = 0;
    int    res = getline(&buf, &len, source);
    if (res < 0)
    {
        // end of file
        return -1;
    }

    string_t intext;
    string_init_set_str(intext, buf);
    free(buf);

    const char *errm = NULL;
    res              = cace_ari_text_decode(inval, intext, &errm);
    string_clear(intext);
    if (res)
    {
        fprintf(stderr, "Failed to decode text ARI (err %d): %s\n", res, errm);
        if (errm != NULL)
            CACE_FREE((char *)errm);
        return 2;
    }

    return 0;
}
#endif /* ARI_TEXT_PARSE */

#define CBOR_STORE_WANT 1024

static int read_cbor(cace_ari_t *inval, FILE *source)
{
    // skip over a single item
    static cace_data_t store = CACE_DATA_INIT_NULL;

    while (true)
    {
        if (store.len < CBOR_STORE_WANT)
        {
            uint8_t buf[CBOR_STORE_WANT];
            size_t  got = fread(buf, 1, sizeof(buf), source);
            if ((got == 0) && (store.len == 0))
            {
                return -1;
            }
            cace_data_append_from(&store, got, buf);
        }

        int         res;
        size_t      used;
        const char *errm = NULL;
        res              = cace_ari_cbor_decode(inval, &store, &used, &errm);
        if (used)
        {
            // chop off used data
            cace_data_extend_front(&store, -used);
        }

        if (res)
        {
            fprintf(stderr, "Failed to decode CBOR ARI (err %d): %s\n", res, errm);
        }
        if (errm)
        {
            CACE_FREE((char *)errm);
        }
        if (res)
        {
            return 3;
        }

        // got the data and decoded the inval
        break;
    }

    return 0;
}

/// @overload
static int read_cborhex(cace_ari_t *inval, FILE *source)
{
    char  *buf = NULL;
    size_t len = 0;
    int    res = getline(&buf, &len, source);
    if (res < 0)
    {
        return -1;
    }

    string_t inhex;
    string_init_set_str(inhex, buf);
    string_strim(inhex); // trim spaces
    free(buf);

    cace_data_t cbordata;
    cace_data_init(&cbordata);
    res = cace_base16_decode(&cbordata, inhex);
    string_clear(inhex);
    if (res)
    {
        fprintf(stderr, "Failed to decode hex input (err %d)\n", res);
        return 2;
    }

    const char *errm = NULL;
    res              = cace_ari_cbor_decode(inval, &cbordata, NULL, &errm);
    cace_data_deinit(&cbordata);
    if (res)
    {
        fprintf(stderr, "Failed to decode CBOR ARI (err %d): %s\n", res, errm);
        if (errm != NULL)
            CACE_FREE((char *)errm);
        return 2;
    }

    return 0;
}

#if defined(ARI_TEXT_PARSE)
static int read_auto(cace_ari_form_t *inform, cace_ari_form_t *outform, cace_ari_t *inval, FILE *source)
{
    // check only the first line
    char  *buf = NULL;
    size_t len = 0;
    int    res = getline(&buf, &len, source);
    if (res < 0)
    {
        // end of file
        return -1;
    }

    if ((len >= 4) && (strncasecmp(buf, "ari:", 4) == 0))
    {
        *inform  = CACE_ARI_FORM_TEXT;
        *outform = CACE_ARI_FORM_CBORHEX;

        FILE *tmp = fmemopen(buf, len, "rb");
        if (!tmp)
        {
            return 2;
        }
        res = read_text(inval, tmp);
        fclose(tmp);
    }
    else
    {
        *inform  = CACE_ARI_FORM_CBORHEX;
        *outform = CACE_ARI_FORM_TEXT;

        FILE *tmp = fmemopen(buf, len, "rb");
        if (!tmp)
        {
            return 2;
        }
        res = read_cborhex(inval, tmp);
        fclose(tmp);
    }
    return res;
}
#endif /* ARI_TEXT_PARSE */

static int write_text(const cace_ari_t *val, FILE *dest, cace_ari_text_enc_opts_t opts)
{
    string_t outtext;
    string_init(outtext);

    int res = cace_ari_text_encode(outtext, val, opts);
    if (res)
    {
        fprintf(stderr, "Failed to encode text ARI (err %d)\n", res);
        return 1;
    }

    res = fprintf(dest, "%s\n", string_get_cstr(outtext));
    string_clear(outtext);
    if (res < 1)
    {
        char buf[256];
        strerror_r(errno, buf, sizeof(buf));
        fprintf(stderr, "Failed to write output (err %d): %s\n", errno, buf);
        return 2;
    }

    return 0;
}

static int write_cbor(const cace_ari_t *val, FILE *dest)
{
    cace_data_t cbordata;
    cace_data_init(&cbordata);
    int res = cace_ari_cbor_encode(&cbordata, val);
    if (res)
    {
        return 1;
    }

    res = fwrite(cbordata.ptr, 1, cbordata.len, dest);
    cace_data_deinit(&cbordata);
    if (res < 1)
    {
        char buf[256];
        strerror_r(errno, buf, sizeof(buf));
        fprintf(stderr, "Failed to write output (err %d): %s\n", errno, buf);
        return 3;
    }

    return 0;
}

static int write_cborhex(const cace_ari_t *val, FILE *dest)
{
    cace_data_t cbordata;
    cace_data_init(&cbordata);
    int res = cace_ari_cbor_encode(&cbordata, val);
    if (res)
    {
        return 1;
    }

    string_t outhex;
    string_init(outhex);
    res = cace_base16_encode(outhex, &cbordata, true);
    cace_data_deinit(&cbordata);
    if (res)
    {
        return 2;
    }

    res = fprintf(dest, "%s\n", string_get_cstr(outhex));
    string_clear(outhex);
    if (res < 1)
    {
        char buf[256];
        strerror_r(errno, buf, sizeof(buf));
        fprintf(stderr, "Failed to write output (err %d): %s\n", errno, buf);
        return 3;
    }

    return 0;
}

static void show_usage(const char *argv0)
{
    fprintf(stderr,
            "Usage: %s {-l <log-level>} "
            "[--source {filename or -}] "
#if defined(ARI_TEXT_PARSE)
            "[--inform {auto,text,cbor,cborhex}] "
#else
            "[--inform {cbor,cborhex}] "
#endif /* ARI_TEXT_PARSE */
            "[--dest {filename or -}] "
            "[--outform {auto,text,cbor,cborhex}]\n",
            argv0);
}

int main(int argc, char *argv[])
{
    int                      log_limit = LOG_WARNING;
    FILE                    *source    = stdin;
    cace_ari_form_t          inform    = CACE_ARI_FORM_AUTO;
    FILE                    *dest      = stdout;
    cace_ari_form_t          outform   = CACE_ARI_FORM_AUTO;
    cace_ari_text_enc_opts_t text_opts = CACE_ARI_TEXT_ENC_OPTS_DEFAULT;

    cace_openlog();

#ifdef HAVE_GETOPT_LONG
    static const struct option longopts[] = {
        { "help", no_argument, NULL, 'h' },
        { "log-level", required_argument, NULL, 'l' },
        { "source", required_argument, NULL, 's' },
        { "inform", required_argument, NULL, 'i' },
        { "dest", required_argument, NULL, 'd' },
        { "outform", required_argument, NULL, 'o' },
        { NULL, 0, NULL, 0 },
    };
#endif /* HAVE_GETOPT_LONG */

    bool cont   = true;
    int  retval = 0;
    while (cont)
    {
#ifdef HAVE_GETOPT_LONG
        int option_index = 0;
        int res          = getopt_long(argc, argv, ":hl:s:i:d:o:", longopts, &option_index);
#else
        int res = getopt(argc, argv, ":hl:s:i:d:o:");
#endif /* HAVE_GETOPT_LONG */

        if (res == -1)
        {
            break;
        }
        switch (res)
        {
            case 'l':
                if (cace_log_get_severity(&log_limit, optarg))
                {
                    fprintf(stderr, "Invalid log severity: %s\n", optarg);
                    retval = 1;
                    cont   = false;
                }
                break;
            case 's':
                source = get_file(optarg, "r");
                if (!source)
                {
                    retval = 1;
                    cont   = false;
                }
                break;
            case 'd':
                dest = get_file(optarg, "w");
                if (!dest)
                {
                    retval = 1;
                    cont   = false;
                }
                break;
            case 'i':
                inform = get_form(optarg);
#if !defined(ARI_TEXT_PARSE)
                if (inform == CACE_ARI_FORM_INVALID || inform == CACE_ARI_FORM_TEXT || inform == CACE_ARI_FORM_AUTO)
                {
                    retval = 1;
                    cont   = false;
                }
#endif /* ARI_TEXT_PARSE */
                if (inform == CACE_ARI_FORM_INVALID)
                {
                    retval = 1;
                    cont   = false;
                }
                break;
            case 'o':
                outform = get_form(optarg);
                if (outform == CACE_ARI_FORM_INVALID)
                {
                    retval = 1;
                    cont   = false;
                }
                break;
            case 'h':
                show_usage(argv[0]);
                cont = false;
                // still exit code zero
                break;
            default:
                retval = 1;
                cont   = false;
                break;
        }
    }
    if (retval)
    {
        fprintf(stderr, "Failed to handle program options\n\n");
        show_usage(argv[0]);
        cont = false;
    }
    cace_log_set_least_severity(log_limit);
    CACE_LOG_DEBUG("Starting up with log limit %d", log_limit);

    // handle each input line/item in-turn
    int failures = 0;
    while (cont)
    {
        // read input ARI
        cace_ari_t inval = CACE_ARI_INIT_UNDEFINED;
        int        res   = 0;
        switch (inform)
        {
            case CACE_ARI_FORM_AUTO:
#if defined(ARI_TEXT_PARSE)
                res = read_auto(&inform, &outform, &inval, source);
#endif /* ARI_TEXT_PARSE */
                break;
            case CACE_ARI_FORM_TEXT:
#if defined(ARI_TEXT_PARSE)
                res = read_text(&inval, source);
#endif /* ARI_TEXT_PARSE */
                break;
            case CACE_ARI_FORM_CBOR:
                res = read_cbor(&inval, source);
                break;
            case CACE_ARI_FORM_CBORHEX:
                res = read_cborhex(&inval, source);
                break;
            case CACE_ARI_FORM_INVALID:
                // not supposed to happen
                cont = false;
                break;
        }
        if (res < 0)
        {
            // the input has ended
            cont = false;
            break;
        }
        else if (res)
        {
            // no further processing if input is not valid
            // but continue to read inputs
            failures += 1;
            continue;
        }

        // process to an output
        cace_ari_t *outval = &inval;

        // output the value
        switch (outform)
        {
            case CACE_ARI_FORM_TEXT:
                write_text(outval, dest, text_opts);
                break;
            case CACE_ARI_FORM_CBOR:
                write_cbor(outval, dest);
                break;
            case CACE_ARI_FORM_CBORHEX:
                write_cborhex(outval, dest);
                break;
            case CACE_ARI_FORM_AUTO:
            case CACE_ARI_FORM_INVALID:
                // not supposed to happen
                cont = false;
                break;
        }
        fflush(dest);

        cace_ari_deinit(&inval);
    }
    if (failures)
    {
        retval = 2 + failures;
    }

    if (source != stdin)
    {
        fclose(source);
    }
    if (dest != stdout)
    {
        fclose(dest);
    }

    cace_closelog();
    return retval;
}
