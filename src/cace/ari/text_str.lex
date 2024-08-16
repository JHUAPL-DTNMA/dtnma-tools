%option reentrant bison-bridge noyywrap
%option nodefault
%option noinput nounput
%option noyyalloc noyyfree noyyrealloc
%option prefix="cace_ari_text_str_"
%option extra-type="cace_ari_text_str_t *"

%{
#include "cace/ari/text_str_parse.h"
#include "cace/ari/text_util.h"
#include "cace/config.h"
#include "cace/util.h"
#include <stdio.h>

static int ari_valseg_decode(string_t out, const char *in, size_t inlen);
%}

/* This is the same as RFC 3986 'segment-nz' production with some excluded
 * for ARI container recursion: "(" ")" ";" "="
 */
VALSEG ([a-zA-Z0-9\-\._~\!\"\'\*\+\:@]|%[0-9a-fA-F]{2})+

/* States for type-specific matching with overlapping patterns. */
%s ARI_OBJREF
%s ARI_TYPEDLIT

/* Tokens named similarly to Python anms-ace:ace.ari_text.lexmod */
%%

[ \t\n] // ignore all whitespace

"(" { BEGIN(INITIAL); return T_LPAREN; }
")" { BEGIN(INITIAL); return T_RPAREN; }
"," { BEGIN(INITIAL); return T_COMMA; }
"=" { return T_EQ; }
";" { return T_SC; }

<INITIAL>"ari:"/.+  {
    // Trailing match-all forces this to have highest precedence
    return T_ARI_PREFIX;
}

<INITIAL>\/\/{VALSEG} {
    // Just a namespace reference and nothing after it
    char *curs = yytext + 2;

    {
        string_t decoded;
        if (ari_valseg_decode(decoded, curs, yyleng - 2))
        {
            cace_ari_text_str_error(yyscanner, yyextra, "NS-ID segment failed to percent-decode");
            return YYerror;
        }
        ari_idseg_init_from_text(&(yylval->idseg), decoded);
    }
    BEGIN(ARI_OBJREF);
    return T_IDSEG;
}
<INITIAL>"." {
    // A null namespace segment
    ari_idseg_init(&(yylval->idseg));
    BEGIN(ARI_OBJREF);
    return T_IDSEG;
}

<ARI_OBJREF>"/" {
    // Ignore trailing slash
}

<ARI_OBJREF>\/{VALSEG} {
    // Match one path segment including leading slash
    char *curs = yytext + 1;

    {
        string_t decoded;
        if (ari_valseg_decode(decoded, curs, yyleng - 1))
        {
            cace_ari_text_str_error(yyscanner, yyextra, "ID segment failed to percent-decode");
            return YYerror;
        }
        ari_idseg_init_from_text(&(yylval->objpath.ns_id), decoded);
    }

    return T_IDSEG;
}

<INITIAL>\/{VALSEG}\//. {
    // Literal type ID, match leading and trailing slash also
    ari_idseg_t typeid;
    {
        string_t decoded;
        if (ari_valseg_decode(decoded, yytext + 1, yyleng - 3))
        {
            cace_ari_text_str_error(yyscanner, yyextra, "TYPE-ID segment failed to percent-decode");
            return YYerror;
        }
        ari_idseg_init_from_text(&typeid, decoded);
    }

    // decode type ID to get into proper type-specific state
    switch (typeid.form)
    {
    case ARI_IDSEG_NULL:
        break;
    case ARI_IDSEG_TEXT:
    {
        int res = ari_type_from_name(&(yylval->ari_type), string_get_cstr(typeid.as_text));
        if (res)
        {
            ari_idseg_deinit(&typeid);
            cace_ari_text_str_error(yyscanner, yyextra, "TYPE-ID has invalid name");
            return YYerror;
        }
        break;
    }
    case ARI_IDSEG_INT:
        yylval->ari_type = typeid.as_int;
        break;
    }
    ari_idseg_deinit(&typeid);

    // containers get different tokens
    switch (yylval->ari_type)
    {
        case ARI_TYPE_AC:
            return T_LITTYPE_AC;
        case ARI_TYPE_AM:
            return T_LITTYPE_AM;
        case ARI_TYPE_TBL:
            return T_LITTYPE_TBL;
        case ARI_TYPE_EXECSET:
            return T_LITTYPE_EXECSET;
        case ARI_TYPE_RPTSET:
            return T_LITTYPE_RPTSET;
        default:
            // next is the literal value
            BEGIN(ARI_TYPEDLIT);
            return T_LITTYPE;
    }
}

<INITIAL,ARI_TYPEDLIT>{VALSEG} {
//    fprintf(stderr, "VALSEG from: %s\n", yytext);
    const char *text_begin = yytext;
    const size_t text_len = yyleng;

    string_t *value = &(yylval->text);
    string_init(*value);
    if (text_len > 0)
    {
        cace_data_t text_view;
        cace_data_init_view(&text_view, text_len + 1, (cace_data_ptr_t)text_begin);

        uri_percent_decode(*value, &text_view);
    }

    // done with this typed value
    BEGIN(INITIAL);
    return T_VALSEG;
}

<*>\0 {
    // ignore trailing null
}

<*>. {
    return YYerror;
}

%%

void *cace_ari_text_str_alloc(yy_size_t size, yyscan_t yyscanner _U_)
{
    return ARI_MALLOC(size);
}

void *cace_ari_text_str_realloc(void *ptr, yy_size_t size, yyscan_t yyscanner _U_)
{
    return ARI_REALLOC(ptr, size);
}

void cace_ari_text_str_free(void *ptr, yyscan_t yyscanner _U_)
{
    ARI_FREE(ptr);
}

static int ari_valseg_decode(string_t out, const char *in, size_t inlen)
{
    cace_data_t text_view;
    cace_data_init_view(&text_view, inlen + 1, (cace_data_ptr_t)in);

    string_init(out);
    if (uri_percent_decode(out, &text_view))
    {
        string_clear(out);
        return 2;
    }
    return 0;
}
