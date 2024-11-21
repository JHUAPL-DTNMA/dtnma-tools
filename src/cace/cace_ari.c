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
#include <cace/config.h>
#include <cace/ari.h>
#include <cace/ari/text.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text_util.h>
#include <qcbor/qcbor_decode.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>

typedef enum
{
    /// Default invalid value
    ARI_FORM_INVALID,
    /** Detect the input by its initial characters as either text or hex
     * and use the opposite on output.
     */
    ARI_FORM_AUTO,
    /// Use text form ARIs
    ARI_FORM_TEXT,
    /// Use binary form ARIs
    ARI_FORM_CBOR,
    /// Use hex-encoded binary form ARIs
    ARI_FORM_CBORHEX,
} ari_form_t;

static ari_form_t get_form(const char *text)
{
    if (strcasecmp(text, "auto") == 0)
    {
        return ARI_FORM_AUTO;
    }
    else if (strcasecmp(text, "text") == 0)
    {
        return ARI_FORM_TEXT;
    }
    else if (strcasecmp(text, "cbor") == 0)
    {
        return ARI_FORM_CBOR;
    }
    else if (strcasecmp(text, "cborhex") == 0)
    {
        return ARI_FORM_CBORHEX;
    }
    fprintf(stderr, "Invalid ARI encoding form: %s\n", text);
    return ARI_FORM_INVALID;
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

/** Read a single value and indicate whether to continue reading.
 *
 * @return Zero upon success.
 * A negative value to indicate end-of-input.
 */
static int read_text(ari_t *inval, FILE *source)
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
    res              = ari_text_decode(inval, intext, &errm);
    string_clear(intext);
    if (res)
    {
        fprintf(stderr, "Failed to decode text ARI (err %d): %s\n", res, errm);
        return 2;
    }

    return 0;
}

#define CBOR_STORE_WANT 1024

static int read_cbor(ari_t *inval, FILE *source)
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

#if 0
    QCBORDecodeContext dec;
    UsefulBufC indata = { .ptr = store.ptr, .len = store.len };
    QCBORDecode_Init(&dec, indata, QCBOR_DECODE_MODE_NORMAL);

    QCBORItem decitem;
    QCBORDecode_VGetNextConsume(&dec, &decitem);

    size_t used;
    int res = QCBORDecode_PartialFinish(&dec, &used);
    if (res && (res != QCBOR_ERR_EXTRA_BYTES))
    {
      // cannot handle this case and cannot reset the input cursor
      cace_data_clear(&store);
      fprintf(stderr, "Failed to seek CBOR item (%d): %s\n", res, qcbor_err_to_str(res));
      return 2;
    }
#else
        int    res;
        size_t used;
#endif
        const char *errm = NULL;
        res              = ari_cbor_decode(inval, &store, &used, &errm);
        if (used)
        {
            cace_data_extend_front(&store, -used);
        }

        if (res)
        {
            fprintf(stderr, "Failed to decode CBOR ARI (err %d): %s\n", res, errm);
        }
        if (errm)
        {
            M_MEMORY_FREE((char *)errm);
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
static int read_cborhex(ari_t *inval, FILE *source)
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
    res = base16_decode(&cbordata, inhex);
    string_clear(inhex);
    if (res)
    {
        fprintf(stderr, "Failed to decode hex input (err %d)\n", res);
        return 2;
    }

    const char *errm = NULL;
    res              = ari_cbor_decode(inval, &cbordata, NULL, &errm);
    cace_data_deinit(&cbordata);
    if (res)
    {
        fprintf(stderr, "Failed to decode CBOR ARI (err %d): %s\n", res, errm);
        return 2;
    }

    return 0;
}

static int read_auto(ari_form_t *inform, ari_form_t *outform, ari_t *inval, FILE *source)
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
        *inform  = ARI_FORM_TEXT;
        *outform = ARI_FORM_CBORHEX;

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
        *inform  = ARI_FORM_CBORHEX;
        *outform = ARI_FORM_TEXT;

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

static int write_text(const ari_t *val, FILE *dest, ari_text_enc_opts_t opts)
{
    string_t outtext;
    string_init(outtext);

    int res = ari_text_encode(outtext, val, opts);
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

static int write_cbor(const ari_t *val, FILE *dest)
{
    cace_data_t cbordata;
    cace_data_init(&cbordata);
    int res = ari_cbor_encode(&cbordata, val);
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

static int write_cborhex(const ari_t *val, FILE *dest)
{
    cace_data_t cbordata;
    cace_data_init(&cbordata);
    int res = ari_cbor_encode(&cbordata, val);
    if (res)
    {
        return 1;
    }

    string_t outhex;
    string_init(outhex);
    res = base16_encode(outhex, &cbordata, true);
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

int main(int argc, char *argv[])
{
    FILE               *source    = stdin;
    ari_form_t          inform    = ARI_FORM_AUTO;
    FILE               *dest      = stdout;
    ari_form_t          outform   = ARI_FORM_AUTO;
    ari_text_enc_opts_t text_opts = ARI_TEXT_ENC_OPTS_DEFAULT;

#ifdef HAVE_GETOPT_LONG
    static const struct option longopts[] = {
        { "source", required_argument, 0, 's' },
        { "inform", required_argument, 0, 'i' },
        { "dest", required_argument, 0, 'd' },
        { "outform", required_argument, 0, 'o' },
        { NULL, 0, NULL, 0 },
    };
#endif /* HAVE_GETOPT_LONG */

    bool cont   = true;
    int  retval = 0;
    while (cont)
    {
#ifdef HAVE_GETOPT_LONG
        int option_index = 0;
        int res          = getopt_long(argc, argv, ":s:i:d:o:", longopts, &option_index);
#else
        int    res = getopt(argc, argv, ":s:i:d:o:");
#endif /* HAVE_GETOPT_LONG */

        if (res == -1)
        {
            break;
        }
        switch (res)
        {
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
                if (inform == ARI_FORM_INVALID)
                {
                    retval = 1;
                    cont   = false;
                }
                break;
            case 'o':
                outform = get_form(optarg);
                if (outform == ARI_FORM_INVALID)
                {
                    retval = 1;
                    cont   = false;
                }
                break;
            default:
                fprintf(
                    stderr,
                    "Usage: %s [--source {filename or -}] [--inform {auto,text,cbor,cborhex}] [--dest {filename or -}] "
                    "[--outform {auto,text,cbor,cborhex}]\n",
                    argv[0]);
                retval = 1;
                cont   = false;
                break;
        }
    }
    if (retval)
    {
        fprintf(stderr, "Failed to handle program options\n");
        cont = false;
    }
    else
    {
        cont = true;
    }

    // handle each input line/item in-turn
    int failures = 0;
    while (cont)
    {
        // read input ARI
        ari_t inval = ARI_INIT_UNDEFINED;
        int   res   = 0;
        switch (inform)
        {
            case ARI_FORM_AUTO:
                res = read_auto(&inform, &outform, &inval, source);
                break;
            case ARI_FORM_TEXT:
                res = read_text(&inval, source);
                break;
            case ARI_FORM_CBOR:
                res = read_cbor(&inval, source);
                break;
            case ARI_FORM_CBORHEX:
                res = read_cborhex(&inval, source);
                break;
            case ARI_FORM_INVALID:
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
        ari_t *outval = &inval;

        // output the value
        switch (outform)
        {
            case ARI_FORM_TEXT:
                write_text(outval, dest, text_opts);
                break;
            case ARI_FORM_CBOR:
                write_cbor(outval, dest);
                break;
            case ARI_FORM_CBORHEX:
                write_cborhex(outval, dest);
                break;
            case ARI_FORM_AUTO:
            case ARI_FORM_INVALID:
                // not supposed to happen
                cont = false;
                break;
        }

        ari_deinit(&inval);
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
    return retval;
}
