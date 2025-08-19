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
#include "cace/util/defs.h"
#include <stdio.h>

static int cace_ari_valseg_decode(string_t out, const char *in, size_t inlen);
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

<INITIAL>\//\/ {
    // Match both slashes but capture only the first one
    BEGIN(ARI_OBJREF);
    return T_REF_ABS;
}
<INITIAL>"." {
    // A relative path segment
    BEGIN(ARI_OBJREF);
    return T_REF_REL_DOT;
}
<INITIAL>".." {
    // A relative path segment
    BEGIN(ARI_OBJREF);
    return T_REF_REL_DOTDOT;
}

<ARI_OBJREF>"/" {
    // Ignore trailing slash
}

<ARI_OBJREF>\/{VALSEG} {
    // Match one path segment including leading slash
    char *curs = yytext + 1;

    {
        m_string_t decoded;
        if (cace_ari_valseg_decode(decoded, curs, yyleng - 1))
        {
            cace_ari_text_str_error(yyscanner, yyextra, "ID segment failed to percent-decode");
            return YYerror;
        }
        cace_ari_idseg_init_text(&(yylval->idseg), decoded);
    }

    return T_IDSEG;
}

<INITIAL>\/{VALSEG}\//. {
    // Literal type ID, match leading and trailing slash also
    cace_ari_idseg_t typeid;
    {
        string_t decoded;
        if (cace_ari_valseg_decode(decoded, yytext + 1, yyleng - 3))
        {
            cace_ari_text_str_error(yyscanner, yyextra, "TYPE-ID segment failed to percent-decode");
            return YYerror;
        }
        cace_string_toupper(decoded);
        cace_ari_idseg_init_text(&typeid, decoded);
    }

    // decode type ID to get into proper type-specific state
    cace_ari_idseg_derive_form(&typeid);
    switch (typeid.form)
    {
    case CACE_ARI_IDSEG_NULL:
        break;
    case CACE_ARI_IDSEG_TEXT:
    {
        int res = cace_ari_type_from_name(&(yylval->ari_type), string_get_cstr(typeid.as_text));
        if (res)
        {
            cace_ari_idseg_deinit(&typeid);
            cace_ari_text_str_error(yyscanner, yyextra, "TYPE-ID has invalid name");
            return YYerror;
        }
        break;
    }
    case CACE_ARI_IDSEG_INT:
        yylval->ari_type = typeid.as_int;
        break;
    }
    cace_ari_idseg_deinit(&typeid);

    // containers get different tokens
    switch (yylval->ari_type)
    {
        case CACE_ARI_TYPE_AC:
            return T_LITTYPE_AC;
        case CACE_ARI_TYPE_AM:
            return T_LITTYPE_AM;
        case CACE_ARI_TYPE_TBL:
            return T_LITTYPE_TBL;
        case CACE_ARI_TYPE_EXECSET:
            return T_LITTYPE_EXECSET;
        case CACE_ARI_TYPE_RPTSET:
            return T_LITTYPE_RPTSET;
        default:
            // next is the literal value
            BEGIN(ARI_TYPEDLIT);
            return T_LITTYPE;
    }
}

<INITIAL,ARI_TYPEDLIT>{VALSEG} {
    const char *text_begin = yytext;
    const size_t text_len = yyleng;

    string_t *value = &(yylval->text);
    string_init(*value);
    if (text_len > 0)
    {
        cace_data_t text_view;
        cace_data_init_view(&text_view, text_len + 1, (cace_data_ptr_t)text_begin);

        cace_uri_percent_decode(*value, &text_view);
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
    return CACE_MALLOC(size);
}

void *cace_ari_text_str_realloc(void *ptr, yy_size_t size, yyscan_t yyscanner _U_)
{
    return CACE_REALLOC(ptr, size);
}

void cace_ari_text_str_free(void *ptr, yyscan_t yyscanner _U_)
{
    CACE_FREE(ptr);
}

static int cace_ari_valseg_decode(string_t out, const char *in, size_t inlen)
{
    cace_data_t text_view;
    cace_data_init_view(&text_view, inlen + 1, (cace_data_ptr_t)in);

    string_init(out);
    if (cace_uri_percent_decode(out, &text_view))
    {
        string_clear(out);
        return 2;
    }
    return 0;
}
