%option reentrant bison-bridge noyywrap
%option nodefault
%option noinput nounput
%option noyyalloc noyyfree noyyrealloc
%option prefix="cace_ari_text_val_"
%option extra-type="cace_ari_text_val_t *"

%{
#include "text_val_parse.h"
#include "cace/ari/text_util.h"
#include "cace/config.h"
#include "cace/util/defs.h"
#include <m-string.h>

struct yyguts_t;
/** Use the extra data to set the initial state.
 */
static void cace_ari_text_val_startcond(yyscan_t *scanner);
#define YY_USER_INIT cace_ari_text_val_startcond(&yyscanner)
%}

%x PRIMITIVE
%x LT_NULL
%x LT_BOOL
%x LT_ANYINT
%x LT_ANYFLOAT
%x LT_TEXTSTR
%x LT_BYTESTR
%x LT_TIMEPOINT
%x LT_TIMEDIFF
%x LT_LABEL
%x LT_CBOR
%x LT_ARITYPE

HEXDIG [0-9a-fA-F]
DECDIG [0-9]

/* Tokens named similarly to Python anms-ace:ace.ari_text.util */
%%

<PRIMITIVE>(?i:undefined) {
    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_UNDEFINED,
    };
    return T_UNDEFINED;
}
<PRIMITIVE,LT_NULL>(?i:null) {
    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_NULL,
    };
    return T_NULL;
}
<PRIMITIVE,LT_BOOL>(?i:false) {
    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_BOOL,
        .value.as_bool = false,
    };
    return T_BOOL;
}
<PRIMITIVE,LT_BOOL>(?i:true) {
    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_BOOL,
        .value.as_bool = true,
    };
    return T_BOOL;
}

<LT_LABEL>(0|[1-9]{DECDIG}*) {
        string_t text;
          string_init_set_str(text, yytext);

        uint64_t val;
        int ret = cace_ari_uint64_decode(&val, text);
        string_clear(text);
        if (ret)
        {
            cace_ari_text_val_error(yyscanner, yyextra, "number too large to parse");
            return YYerror;
        }

        // prefer signed integer use
        if (val <= INT64_MAX)
        {
            yylval->lit = (cace_ari_lit_t){
                .prim_type = CACE_ARI_PRIM_INT64,
                .value.as_int64 = (int64_t)val,
            };
            return T_INT;
        }
        else
        {
            yylval->lit = (cace_ari_lit_t){
                .prim_type = CACE_ARI_PRIM_UINT64,
                .value.as_uint64 = val,
            };
            return T_UINT;
        }
}
<PRIMITIVE,LT_ANYINT,LT_ARITYPE>[+-]?(0[bB][01]+|0[xX]{HEXDIG}+|{DECDIG}+) {
    // infer the base from the text
    if (yytext[0] == '-')
    {
        string_t text;
        string_init_set_str(text, yytext + 1);
        uint64_t neg;
        int ret = cace_ari_uint64_decode(&neg, text);
        string_clear(text);
        if (ret)
        {
            cace_ari_text_val_error(yyscanner, yyextra, "number too large to parse");
            return YYerror;
        }

        // work around signed negation overflow
        if (neg > (uint64_t)(-(INT64_MIN + 1)) + 1)
        {
            cace_ari_text_val_error(yyscanner, yyextra, "negative int too large");
            return YYerror;
        }

        yylval->lit = (cace_ari_lit_t){
            .prim_type = CACE_ARI_PRIM_INT64,
            .value.as_int64 = -(int64_t)neg,
        };
        return T_INT;
    }
    else
    {
        string_t text;
        if (yytext[0] == '+') { 
          string_init_set_str(text, yytext + 1);
        } else {
          string_init_set_str(text, yytext);
        }
        uint64_t val;
        int ret = cace_ari_uint64_decode(&val, text);
        string_clear(text);
        if (ret)
        {
            cace_ari_text_val_error(yyscanner, yyextra, "number too large to parse");
            return YYerror;
        }

        // prefer signed integer use
        if (val <= INT64_MAX)
        {
            yylval->lit = (cace_ari_lit_t){
                .prim_type = CACE_ARI_PRIM_INT64,
                .value.as_int64 = (int64_t)val,
            };
            return T_INT;
        }
        else
        {
            yylval->lit = (cace_ari_lit_t){
                .prim_type = CACE_ARI_PRIM_UINT64,
                .value.as_uint64 = val,
            };
            return T_UINT;
        }
    }
}

<PRIMITIVE,LT_ANYFLOAT>[+-]?(({DECDIG}+|{DECDIG}*\.{DECDIG}*)([eE][+-]?{DECDIG}+)|0[xX]({HEXDIG}+|{HEXDIG}*\.{HEXDIG}*)([pP][+-]?{DECDIG})?|{DECDIG}*(\.{DECDIG}*)?|(?i:Infinity))|(?i:NaN) {
    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_FLOAT64,
        .value.as_float64 = strtod(yytext, NULL),
    };
    return T_FLOAT;
}

<PRIMITIVE,LT_TEXTSTR,LT_LABEL>\"(\\.|[^"])*\" {
    char *text_begin = yytext + 1;
    char *text_end = yytext + yyleng - 1;
    const size_t text_len = text_end - text_begin;
    *text_end = '\0';

    cace_data_t data;
    cace_data_init(&data);
    if (text_len > 0)
    {
        cace_data_t text_view;
        cace_data_init_view(&text_view, text_len, (cace_data_ptr_t)text_begin);

        string_t text_unesc;
        string_init(text_unesc);
        cace_slash_unescape(text_unesc, &text_view);
        cace_data_deinit(&text_view);

        cace_data_from_m_string(&data, text_unesc);
        string_clear(text_unesc);
    }

    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_TSTR,
        .value.as_data = data,
    };
    return T_TSTR;
}

<PRIMITIVE,LT_BYTESTR,LT_CBOR>(h|b64)?'((\\.|[^'])*)' {
    char *form_begin = yytext;
    char *form_end = strchr(yytext, '\'');
    *form_end = '\0';

    char *text_begin = form_end + 1;
    char *text_end = yytext + yyleng - 1;
    const size_t text_len = text_end - text_begin;
    *text_end = '\0';

    cace_data_t data;
    cace_data_init(&data);
    if (text_len > 0)
    {
        if (strcmp(form_begin, "h") == 0)
        {
            string_t clean;
            string_init(clean);
            cace_strip_space(clean, text_begin, text_len);
            int ret = cace_base16_decode(&data, clean);
            string_clear(clean);
            if (ret)
            {
                cace_data_deinit(&data);
                cace_ari_text_val_error(yyscanner, yyextra, "base16 failed to decode");
                return YYerror;
            }
        }
        else if (strcmp(form_begin, "b64") == 0)
        {
            string_t clean;
            string_init(clean);
            cace_strip_space(clean, text_begin, text_len);
            int ret = cace_base64_decode(&data, clean);
            string_clear(clean);
            if (ret)
            {
                cace_data_deinit(&data);
                cace_ari_text_val_error(yyscanner, yyextra, "base64 failed to decode");
                return YYerror;
            }
        }
        else
        {
            cace_data_t text_view;
            cace_data_init_view(&text_view, text_len, (cace_data_ptr_t)text_begin);

            string_t text_unesc;
            string_init(text_unesc);
            cace_slash_unescape(text_unesc, &text_view);
            cace_data_deinit(&text_view);
        
            // direct copy
            cace_data_from_m_string(&data, text_unesc);
            string_clear(text_unesc);
            // strip the trailing null
            cace_data_extend_back(&data, -1);
        }
    }

    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_BSTR,
        .value.as_data = data,
    };
    return T_BSTR;
}

<LT_TIMEPOINT>({DECDIG}{4})\-?({DECDIG}{2})\-?({DECDIG}{2})T({DECDIG}{2}):?({DECDIG}{2}):?({DECDIG}{2})(\.({DECDIG}{1,6}))?Z {
    cace_data_t text_view;
    cace_data_init_view(&text_view, yyleng, (cace_data_ptr_t)yytext);

    struct timespec val;
    int ret = cace_utctime_decode(&val, &text_view);
    cace_data_deinit(&text_view);
    if (ret)
    {
        return YYerror;
    }

    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_TIMESPEC,
        .value.as_timespec = val,
    };
    return T_TIMEPOINT;
}

<LT_TIMEDIFF>([+-])?P(({DECDIG}+)D)?T(({DECDIG}+)H)?(({DECDIG}+)M)?(({DECDIG}+)(\.({DECDIG}{1,6}))?S)? {
    cace_data_t text_view;
    cace_data_init_view(&text_view, yyleng, (cace_data_ptr_t)yytext);

    struct timespec val;
    int ret = cace_timeperiod_decode(&val, &text_view);
    cace_data_deinit(&text_view);
    if (ret)
    {
        return YYerror;
    }

    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_TIMESPEC,
        .value.as_timespec = val,
    };
    return T_TIMEDIFF;
}

<LT_TIMEPOINT,LT_TIMEDIFF>([+-])?{DECDIG}*(\.{DECDIG}*)? {
    cace_data_t text_view;
    cace_data_init_view(&text_view, yyleng, (cace_data_ptr_t)yytext);
    
    struct timespec val;
    int ret = cace_decfrac_decode(&val, &text_view);
    cace_data_deinit(&text_view);
    if (ret)
    {
        return YYerror;
    }

    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_TIMESPEC,
        .value.as_timespec = val,
    };
    return T_DECFRAC;
}

<PRIMITIVE,LT_TEXTSTR,LT_LABEL,LT_ARITYPE>[a-zA-Z_][a-zA-Z0-9_\-\.]* {
    // Special case of unquoted text
    cace_data_t data;
    cace_data_init(&data);
    cace_data_copy_from(&data, yyleng + 1, (cace_data_ptr_t)yytext);

    yylval->lit = (cace_ari_lit_t){
        .prim_type = CACE_ARI_PRIM_TSTR,
        .value.as_data = data,
    };
    return T_IDENTITY;
}

<*>\0|\n {
    // ignore trailing null
}

<*>. {
    string_t str;
    string_init(str);
    string_cat_printf(str, "Unexpected character at %d: %s (%d)",
                      yycolumn, yytext, yytext[0]);

    cace_ari_text_val_error(yyscanner, yyextra, string_get_cstr(str));
    string_clear(str);
    return YYerror;
}

%%

void *cace_ari_text_val_alloc(yy_size_t size, yyscan_t yyscanner _U_)
{
    return CACE_MALLOC(size);
}

void *cace_ari_text_val_realloc(void *ptr, yy_size_t size, yyscan_t yyscanner _U_)
{
    return CACE_REALLOC(ptr, size);
}

void cace_ari_text_val_free(void *ptr, yyscan_t yyscanner _U_)
{
    CACE_FREE(ptr);
}

void cace_ari_text_val_startcond(yyscan_t *scanner)
{
    const cace_ari_text_val_t *input = cace_ari_text_val_get_extra(*scanner);
    const int32_t *ari_type = input->lit->has_ari_type ? &(input->lit->ari_type) : NULL;

    struct yyguts_t *yyg = (struct yyguts_t*)*scanner;
//    fprintf(stderr, "Context ari_type_t: %d\n", ari_type ? *ari_type : -1);
    if (!ari_type)
    {
        BEGIN(PRIMITIVE);
        return;
    }
    switch (*ari_type)
    {
        case CACE_ARI_TYPE_NULL:
            BEGIN(LT_NULL);
            break;
        case CACE_ARI_TYPE_BOOL:
            BEGIN(LT_BOOL);
            break;
        case CACE_ARI_TYPE_BYTE:
        case CACE_ARI_TYPE_UINT:
        case CACE_ARI_TYPE_UVAST:
            BEGIN(LT_ANYINT);
            break;
        case CACE_ARI_TYPE_INT:
        case CACE_ARI_TYPE_VAST:
            BEGIN(LT_ANYINT);
            break;
        case CACE_ARI_TYPE_REAL32:
        case CACE_ARI_TYPE_REAL64:
            BEGIN(LT_ANYFLOAT);
            break;
        case CACE_ARI_TYPE_TEXTSTR:
            BEGIN(LT_TEXTSTR);
            break;
        case CACE_ARI_TYPE_BYTESTR:
            BEGIN(LT_BYTESTR);
            break;

        case CACE_ARI_TYPE_TP:
            BEGIN(LT_TIMEPOINT);
            break;
        case CACE_ARI_TYPE_TD:
            BEGIN(LT_TIMEDIFF);
            break;
        case CACE_ARI_TYPE_LABEL:
            BEGIN(LT_LABEL);
            break;
        case CACE_ARI_TYPE_CBOR:
            BEGIN(LT_CBOR);
            break;
        case CACE_ARI_TYPE_ARITYPE:
            BEGIN(LT_ARITYPE);
            break;

        default:
            break;
    }
}
