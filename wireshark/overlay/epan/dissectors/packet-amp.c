/* packet-amp.c
 * Routines for Asynchronous management Protocol dissection
 * Copyright 2018, Krishnamurthy Mayya (krishnamurthymayya@gmail.com)
 * Updated to CBOR encoding: Keith Scott, 2019 (kscott@mitre.org)
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
/* Parts of this work was performed for the Jet Propulsion
 * Laboratory, California Institute of Technology, sponsored by
 * the United States Government under the prime contract
 * 80NM0018D0004 between the Caltech and NASA under
 * subcontract 1658085.
 */

#include "config.h"

#include <epan/exceptions.h>
#include <epan/expert.h>
#include <epan/packet.h>
#include <epan/wscbor.h>
#include <math.h>
#include "packet-amp.h"

/* The AMP standard can be found here:
 * https://tools.ietf.org/html/draft-birrane-dtn-amp-04
 * https://tools.ietf.org/html/draft-birrane-dtn-amp-03
 */

#define AMP_APID 0x000 /* TODO - To be decided. Currently, the function 'dissect_amp_as_subtree' is
                          being called from dtn.c file when required to decode the bundle protocol's
                          data-payload as AMP. Later in the future, when a dedicated field is given to
                          this, this should be filled. */

#define AMP_HDR_RESERVED         0xC0
#define AMP_HDR_ACL              0x20
#define AMP_HDR_NACK             0x10
#define AMP_HDR_ACK              0x08
#define AMP_HDR_OPCODE           0x07

#define AMP_ARI_NICKNAME         0x80
#define AMP_ARI_PARAMETERS       0x40
#define AMP_ARI_ISSUER           0x20
#define AMP_ARI_TAG              0x10
#define AMP_ARI_STRUCT           0x0F
#define AMP_ARI_VALUE            0xF0

#define AMP_TNVC_RESERVED        0xF0
#define AMP_TNVC_MIXED           0x08
#define AMP_TNVC_TYPE            0x04
#define AMP_TNVC_NAME            0x02
#define AMP_TNVC_VALUE           0x01

#define AMP_MSG_REGISTER_AGENT   0x00
#define AMP_MSG_REPORT_SET       0x01
#define AMP_MSG_PERFORM_CONTROL  0x02
#define AMP_MSG_TABLE_SET        0x03

static dissector_handle_t amp_handle = NULL;
static dissector_handle_t cbor_handle = NULL;

void proto_register_amp(void);
void proto_reg_handoff_amp(void);

static int hf_amp_time_rel = -1;
static int hf_amp_time_abs = -1;
static int hf_amp_message_len = -1;
static int hf_amp_message_data = -1;
static int hf_amp_message_header = -1;
static int hf_amp_rpt_tpl = -1;
static int hf_amp_tbl_rowcount = -1;
static int hf_amp_tbl_tpl = -1;
static int hf_amp_tbl_row = -1;
static int hf_amp_pctrl_start = -1;
static int hf_amp_pctrl_ctrls = -1;
static int hf_amp_cbor_header = -1;
static int hf_amp_msg_count = -1;
static int hf_amp_primary_timestamp = -1;
static int hf_amp_agent_name = -1;
static int hf_type_ari = -1;
static int hf_ari_struct = -1;
static int hf_ari_valtype = -1;
static int hf_amp_ari_flags = -1;
static int hf_ari_flags_nickname = -1;
static int hf_ari_flags_parameters = -1;
static int hf_ari_flags_issuer = -1;
static int hf_ari_flags_tag = -1;
static int hf_ari_nickname = -1;
static int hf_nn_adm = -1;
static int hf_nn_type = -1;
static int hf_ari_obj_name = -1;
static int hf_ari_obj_name_uint = -1;
static int hf_ari_parameters = -1;
static int hf_ari_issuer = -1;
static int hf_ari_tag = -1;
static int hf_amp_type_const = -1;
static int hf_amp_type_ctrl = -1;
static int hf_amp_type_tbl = -1;
static int hf_amp_type_bstr = -1;
static int hf_amp_bstr_text = -1;
static int hf_amp_type_tstr = -1;
static int hf_amp_type_bool = -1;
static int hf_amp_type_uint = -1;
static int hf_amp_type_uvast = -1;
static int hf_amp_type_real32 = -1;
static int hf_amp_type_real64 = -1;
static int hf_amp_type_tv = -1;
static int hf_amp_type_ts = -1;
static int hf_amp_type_ari = -1;
static int hf_amp_type_ac = -1;
static int hf_amp_type_expr = -1;
static int hf_amp_type_tnvc = -1;
static int hf_amp_tnvc_flags = -1;
static int hf_amp_tnvc_flags_reserved = -1;
static int hf_amp_tnvc_flags_mixed = -1;
static int hf_amp_tnvc_flags_type = -1;
static int hf_amp_tnvc_flags_name = -1;
static int hf_amp_tnvc_flags_value = -1;
static int hf_amp_tnvc_count = -1;
static int hf_amp_tnvc_type = -1;
static int hf_amp_tnvc_name = -1;

/* Initialize the protocol and registered fields */
static int proto_amp = -1;

static gint ett_amp_ts = -1;
static gint ett_amp_bstr = -1;
static gint ett_amp_message_header = -1;
static gint ett_amp = -1;
static gint ett_amp_cbor_header = -1;
static gint ett_amp_message = -1;
static gint ett_amp_register = -1;
static gint ett_ctrls_tree = -1;
static gint ett_rpts_tree = -1;
static gint ett_amp_rpt = -1;
static gint ett_tbls_tree = -1;
static gint ett_amp_tbl = -1;
static gint ett_amp_tnvc_flags = -1;
static gint ett_amp_tnvc_types = -1;
static gint ett_amp_tnvc_names = -1;
static gint ett_amp_tnvc_values = -1;
static gint ett_type_ari = -1;
static gint ett_amp_ari_struct = -1;
static gint ett_amp_ari_flags = -1;
static gint ett_type_ac = -1;
static gint ett_type_expr = -1;
static gint ett_nickname = -1;
static gint ett_type_tnvc = -1;
static gint ett_type_tbl = -1;
static gint ett_type_const = -1;

static expert_field ei_number_overflow = EI_INIT;

static int hf_amp_reserved = -1;
static int hf_amp_acl = -1;
static int hf_amp_nack = -1;
static int hf_amp_ack = 0;
static int hf_amp_opcode = -1;
static int hf_amp_rptset_rx_list = -1;
static int hf_amp_rptset_rx_name = -1;
static int hf_amp_rptset_rpt_list = -1;
static int hf_amp_tblset_rx_list = -1;
static int hf_amp_tblset_rx_name = -1;
static int hf_amp_tblset_tbl_list = -1;

static expert_field ei_amp_cbor_malformed = EI_INIT;

static const value_string opcode[] = {
    { AMP_MSG_REGISTER_AGENT, "Register Agent" },
    { AMP_MSG_REPORT_SET, "Report Set" },
    { AMP_MSG_PERFORM_CONTROL, "Perform Control" },
    { AMP_MSG_TABLE_SET, "Table Set" },
    { 0, NULL }
};

static const value_string amp_ari_struct_type[] = {
    { 0, "CONST" },
    { 1, "CTRL" },
    { 2, "EDD" },
    { 3, "LIT" },
    { 4, "MAC" },
    { 5, "OPER" },
    { 6, "RPT" },
    { 7, "RPTT" },
    { 8, "SBR" },
    { 9, "TBL" },
    { 10, "TBLT" },
    { 11, "TBR" },
    { 12, "VAR" },
    { 16, "BOOL" },
    { 17, "BYTE" },
    { 18, "STR" },
    { 19, "INT" },
    { 20, "UINT" },
    { 21, "VAST" },
    { 22, "UVAST" },
    { 23, "REAL32" },
    { 24, "REAL64" },
    { 32, "TV" },
    { 33, "TS" },
    { 34, "TNV" },
    { 35, "TNVC" },
    { 36, "ARI" },
    { 37, "AC" },
    { 38, "EXPR" },
    { 39, "BYTESTR" },
    { 0, NULL }
};

static const value_string amp_ari_value_offset[] = {
    { 0, "BOOL" },
    { 1, "BYTE" },
    { 2, "STR" },
    { 3, "INT" },
    { 4, "UINT" },
    { 5, "VAST" },
    { 6, "UVAST" },
    { 7, "REAL32" },
    { 8, "REAL64" },
    { 0, NULL }
};

static const value_string amp_amm_type[] = {
    { 0, "CONST" },
    { 1, "CTRL" },
    { 2, "EDD" },
    { 3, "MAC" },
    { 4, "OPER" },
    { 5, "RPTT" },
    { 6, "SBR" },
    { 7, "TBLT" },
    { 8, "TBR" },
    { 9, "VAR" },
    { 10, "metadata" },
    { 0, NULL }
};

/* AMP Message Header */
static int * const amp_message_header[] = {
    &hf_amp_reserved,
    &hf_amp_acl,
    &hf_amp_nack,
    &hf_amp_ack,
    &hf_amp_opcode,
    0
};

/* TNVC Flags */
static int * const amp_tnvc_flags[] = {
    &hf_amp_tnvc_flags_reserved,
    &hf_amp_tnvc_flags_mixed,
    &hf_amp_tnvc_flags_type,
    &hf_amp_tnvc_flags_name,
    &hf_amp_tnvc_flags_value,
    0
};

/* ARI Flags */
static int * const amp_ari_flags[] = {
    &hf_ari_flags_nickname,
    &hf_ari_flags_parameters,
    &hf_ari_flags_issuer,
    &hf_ari_flags_tag,
    0
};

static int dissect_message(tvbuff_t *tvb,  packet_info *pinfo, proto_tree *tree);

static proto_item *dissect_ari(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_root);
static proto_item *dissect_ac(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_root);
static proto_item *dissect_tnvc(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_root);
static proto_item *dissect_tbl(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_root);

/** Add an AMP time value (relative or absolute encoding) as an absolute time item.
 * Time is defined in AMP Section 8.2.2.2.  Time Values (TV) and Timestamps (TS).
 */
static proto_item *
dissect_timeval(proto_tree *tree, int hf_time_format, packet_info *pinfo, tvbuff_t *tvb, wscbor_chunk_t *chunk)
{
    proto_item *item_ts;
    const guint64 *value = wscbor_require_uint64(pinfo->pool, chunk);
    DISSECTOR_ASSERT_HINT(value, "Time is not a CBOR uint");

    // If it's a relative time, convert and show the original
    nstime_t rel_time = {
        .secs = (time_t)(*value)
    };
    nstime_t abs_time;
    if (*value < 558230400) {
        // relative to packet timestamp
        nstime_sum(&abs_time, &rel_time, &(pinfo->abs_ts));

        item_ts = proto_tree_add_time(tree, hf_time_format, tvb, chunk->start, chunk->data_length, &abs_time);
        PROTO_ITEM_SET_GENERATED(item_ts);
        proto_tree *tree_ts = proto_item_add_subtree(item_ts, ett_amp_ts);

        proto_tree_add_cbor_uint64(tree_ts, hf_amp_time_rel, pinfo, tvb, chunk, value);
    }
    else {
        // relative to DTN epoch year 2000
        static const nstime_t amp_epoch = {
            .secs = 946684800
        };
        nstime_sum(&abs_time, &rel_time, &amp_epoch);

        item_ts = proto_tree_add_time(tree, hf_time_format, tvb, chunk->start, chunk->data_length, &abs_time);
        PROTO_ITEM_SET_GENERATED(item_ts);
        proto_tree *tree_ts = proto_item_add_subtree(item_ts, ett_amp_ts);

        proto_tree_add_cbor_uint64(tree_ts, hf_amp_time_abs, pinfo, tvb, chunk, value);
    }

    return item_ts;
}

static void
dissect_name_bstr(proto_tree *tree, int hfindex, packet_info *pinfo, tvbuff_t *tvb, wscbor_chunk_t *chunk, gboolean as_int)
{
    tvbuff_t *data = wscbor_require_bstr(pinfo->pool, chunk);
    proto_item *item = proto_tree_add_cbor_bstr(tree, hfindex, pinfo, tvb, chunk);

    if (data)
    {
        proto_tree *subtree = proto_item_add_subtree(item, ett_amp_bstr);
        proto_item *subitem = NULL;
        if (as_int)
        {
            gint offset = 0;
            chunk = wscbor_chunk_read(pinfo->pool, data, &offset);
            const guint64 *off_value = wscbor_require_uint64(pinfo->pool, chunk);
            subitem = proto_tree_add_cbor_uint64(subtree, hf_ari_obj_name_uint, pinfo, data, chunk, off_value);
        }
        else
        {
            subitem = proto_tree_add_item(subtree, hf_amp_bstr_text, data, 0, tvb_reported_length(data), ENC_NA);
        }
        PROTO_ITEM_SET_GENERATED(subitem);
    }
}

static void
dissect_typed_value(proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb, int *offset, const guint32 *type_id)
{
    wscbor_chunk_t *chunk;

    if (type_id)
    {
        switch (*type_id)
        {
        case 0: // CONST
            dissect_ari(tree, pinfo, tvb, offset, hf_amp_type_const);
            break;
        case 1:
            dissect_ari(tree, pinfo, tvb, offset, hf_amp_type_ctrl);
            break;
        case 9:
            dissect_tbl(tree, pinfo, tvb, offset, hf_amp_type_tbl);
            break;
        case 16: { // BOOL
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            const gboolean *value = wscbor_require_boolean(pinfo->pool, chunk);
            proto_tree_add_cbor_boolean(tree, hf_amp_type_bool, pinfo, tvb, chunk, value);
            break;
        }
        case 18: // Text
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            wscbor_require_major_type(chunk, CBOR_TYPE_STRING);
            proto_tree_add_cbor_tstr(tree, hf_amp_type_tstr, pinfo, tvb, chunk);
            break;
        case 20: { // UNIT
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            const guint64 *value = wscbor_require_uint64(pinfo->pool, chunk);
            const guint32 clamped = value ? (guint64)*value : 0;
            // special clamped display
            proto_item *subitem = proto_tree_add_uint(tree, hf_amp_type_uint, tvb, chunk->start, chunk->head_length, clamped);
            wscbor_chunk_mark_errors(pinfo, subitem, chunk);
            if (!value || (clamped != *value))
            {
                // Indicate error
                expert_add_info(pinfo, subitem, &ei_number_overflow);
            }
            break;
        }
        case 22: { // UVAST
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            const guint64 *value = wscbor_require_uint64(pinfo->pool, chunk);
            proto_tree_add_cbor_uint64(tree, hf_amp_type_uvast, pinfo, tvb, chunk, value);
            break;
        }
        case 23: { // REAL32
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            const double *value = wscbor_require_float64(pinfo->pool, chunk);
            const float clamped = value ? (float)*value : NAN;
            proto_item *subitem = proto_tree_add_cbor_float64(tree, hf_amp_type_real32, pinfo, tvb, chunk, value);
            if (!value || (clamped != *value))
            {
                // Indicate error
                expert_add_info(pinfo, subitem, &ei_number_overflow);
            }
            break;
        }
        case 24: { // REAL64
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            const double *value = wscbor_require_float64(pinfo->pool, chunk);
            proto_tree_add_cbor_float64(tree, hf_amp_type_real64, pinfo, tvb, chunk, value);
            break;
        }
        case 32: // TV
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            dissect_timeval(tree, hf_amp_type_tv, pinfo, tvb, chunk);
            break;
        case 33:
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            dissect_timeval(tree, hf_amp_type_ts, pinfo, tvb, chunk);
            break;
        case 35:
            dissect_tnvc(tree, pinfo, tvb, offset, hf_amp_type_tnvc);
            break;
        case 36:
            dissect_ari(tree, pinfo, tvb, offset, hf_amp_type_ari);
            break;
        case 37:
            dissect_ac(tree, pinfo, tvb, offset, hf_amp_type_ac);
            break;
        case 38: {
            proto_item *item_expr = proto_tree_add_item(tree, hf_amp_type_expr, tvb, *offset, 0, ENC_NA);
            proto_item *tree_expr = proto_item_add_subtree(item_expr, ett_type_expr);

            guint32 *res_type_id = wmem_new(pinfo->pool, guint32);
            proto_tree_add_item_ret_uint(tree_expr, hf_amp_tnvc_type, tvb, *offset, 1, ENC_BIG_ENDIAN, res_type_id);
            *offset += 1;

            dissect_ac(tree_expr, pinfo, tvb, offset, hf_amp_type_ac);
            break;
        }
        case 39:
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            wscbor_require_major_type(chunk, CBOR_TYPE_BYTESTRING);
            proto_tree_add_cbor_bstr(tree, hf_amp_type_bstr, pinfo, tvb, chunk);
            break;
        default:
            DISSECTOR_ASSERT_CMPUINT(G_MAXUINT32, <, (*type_id));
            break;
        }
    }
    else
    {
        // raw CBOR
        tvbuff_t *subtvb = tvb_new_subset_remaining(tvb, *offset);
        gint sublen = call_dissector_only(cbor_handle, subtvb, pinfo, tree, NULL);
        if (sublen <= 0)
        {
            return;
        }
        *offset += sublen;
    }
}

static proto_item *
dissect_tnvc(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_root)
{
    wscbor_chunk_t *chunk;
    proto_item *item_root = proto_tree_add_item(tree, hf_root, tvb, *offset, 0, ENC_NA);
    proto_tree *tree_root = proto_item_add_subtree(item_root, ett_type_tnvc);

    // TNVC Flags
    guint64 flagsval;
    proto_tree_add_bitmask_ret_uint64(
        tree_root, tvb, *offset, hf_amp_tnvc_flags, ett_amp_tnvc_flags,
        amp_tnvc_flags, ENC_BIG_ENDIAN, &flagsval
    );
    *offset += 1;

    guint64 *item_count = NULL;
    if (flagsval & ~AMP_TNVC_RESERVED)
    {
        // any flag is set
        chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
        item_count = wscbor_require_uint64(pinfo->pool, chunk);
        proto_tree_add_cbor_uint64(tree_root, hf_amp_tnvc_count, pinfo, tvb, chunk, item_count);
    }
    if (!item_count)
    {
        // Implied empty
        item_count = wmem_new(pinfo->pool, guint64);
        *item_count = 0;
        PROTO_ITEM_SET_GENERATED(proto_tree_add_uint64(tree_root, hf_amp_tnvc_count, tvb, 0, 0, *item_count));
    }
    proto_item_append_text(item_root, ", Value count: %" PRIu64, *item_count);

    wmem_list_t *type_list = wmem_list_new(pinfo->pool);
    if (flagsval & AMP_TNVC_TYPE)
    {
        proto_tree *tree_types = proto_tree_add_subtree(tree_root, tvb, *offset, *item_count, ett_amp_tnvc_types, NULL, "Type List");
        for (guint64 ix = 0; ix < *item_count; ++ix)
        {
            guint32 *type_id = wmem_new(pinfo->pool, guint32);
            proto_tree_add_item_ret_uint(tree_types, hf_amp_tnvc_type, tvb, *offset, 1, ENC_BIG_ENDIAN, type_id);
            *offset += 1;

            wmem_list_append(type_list, type_id);
        }
    }

    if (flagsval & AMP_TNVC_NAME)
    {
        proto_tree *tree_names = proto_tree_add_subtree(tree_root, tvb, *offset, *item_count, ett_amp_tnvc_types, NULL, "Name List");
        for (guint64 ix = 0; ix < *item_count; ++ix)
        {
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            wscbor_require_major_type(chunk, CBOR_TYPE_STRING);
            proto_tree_add_cbor_tstr(tree_names, hf_amp_tnvc_name, pinfo, tvb, chunk);
        }
    }

    if (flagsval & AMP_TNVC_VALUE)
    {
        proto_item *item_values;
        proto_tree *tree_values = proto_tree_add_subtree_format(tree_root, tvb, *offset, 0, ett_amp_tnvc_types, &item_values, "Value List, Count: %" PRIu64, *item_count);
        wmem_list_frame_t *type_it = wmem_list_head(type_list);
        for (guint64 ix = 0; ix < *item_count; ++ix)
        {
            const guint32 *type_id = type_it ? (guint32*)wmem_list_frame_data(type_it) : NULL;

            dissect_typed_value(tree_values, pinfo, tvb, offset, type_id);

            if (type_it)
            {
                type_it = wmem_list_frame_next(type_it);
            }
        }
        proto_item_set_end(item_values, tvb, *offset);
    }

    if (flagsval & AMP_TNVC_MIXED)
    {
        DISSECTOR_ASSERT_HINT(false, "No mixed TNVC handling");
    }

    proto_item_set_end(item_root, tvb, *offset);
    return item_root;
}

static proto_item *
dissect_ari(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_ari)
{
    proto_item *item_ari = proto_tree_add_item(tree, hf_ari, tvb, *offset, -1, ENC_NA);
    proto_tree *tree_ari = proto_item_add_subtree(item_ari, ett_type_ari);

    guint64 structval;
    proto_tree_add_bits_ret_val(tree_ari, hf_ari_struct, tvb, (*offset) * 8 + 4, 4, &structval, ENC_BIG_ENDIAN);
    if (structval == 3)
    {
        // Literal
        guint64 typeval;
        proto_tree_add_bits_ret_val(tree_ari, hf_ari_valtype, tvb, (*offset) * 8, 4, &typeval, ENC_BIG_ENDIAN);
        *offset += 1;

        guint32 type_id = typeval + 16;
        dissect_typed_value(tree_ari, pinfo, tvb, offset, &type_id);
    }
    else
    {
        // NOT literal
        guint64 flagsval;
        wscbor_chunk_t *chunk;
        proto_tree_add_bitmask_ret_uint64(tree_ari, tvb, *offset, hf_amp_ari_flags, ett_amp_ari_flags,
                                          amp_ari_flags, ENC_BIG_ENDIAN, &flagsval);
        *offset += 1;
        flagsval <<= 4; // shift back to byte aligned flags

        if (flagsval & AMP_ARI_NICKNAME)
        {
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            guint64 *nnval = wscbor_require_uint64(pinfo->pool, chunk);
            proto_item *item_nn = proto_tree_add_cbor_uint64(tree_ari, hf_ari_nickname, pinfo, tvb, chunk, nnval);

            if (nnval)
            {
                proto_tree *tree_nn = proto_item_add_subtree(item_nn, ett_nickname);
                PROTO_ITEM_SET_GENERATED(
                    proto_tree_add_uint64(tree_nn, hf_nn_adm, tvb, 0, 0, *nnval / 20)
                );
                PROTO_ITEM_SET_GENERATED(
                    proto_tree_add_uint(tree_nn, hf_nn_type, tvb, 0, 0, (guint32)(*nnval % 20))
                );
            }
        }

        chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
        dissect_name_bstr(tree_ari, hf_ari_obj_name, pinfo, tvb, chunk, !(flagsval & AMP_ARI_ISSUER));

        if (flagsval & AMP_ARI_PARAMETERS)
        {
            dissect_tnvc(tree_ari, pinfo, tvb, offset, hf_ari_parameters);
        }
        if (flagsval & AMP_ARI_ISSUER)
        {
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            dissect_name_bstr(tree_ari, hf_ari_issuer, pinfo, tvb, chunk, false);
        }
        if (flagsval & AMP_ARI_TAG)
        {
            chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
            dissect_name_bstr(tree_ari, hf_ari_tag, pinfo, tvb, chunk, false);
        }
    }

    proto_item_set_end(item_ari, tvb, *offset);
    return item_ari;
}

static proto_item *
dissect_ac(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_ac)
{
    wscbor_chunk_t *chunk;
    chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
    wscbor_require_array(chunk);
    proto_item *item_ac = proto_tree_add_cbor_container(tree, hf_ac, pinfo, tvb, chunk);
    if (!wscbor_has_errors(chunk))
    {
        proto_tree *tree_ac = proto_item_add_subtree(item_ac, ett_type_ac);
        const guint64 array_size = chunk->head_value;
        for (guint64 itemIx = 0; itemIx < array_size; ++itemIx)
        {
            dissect_ari(tree_ac, pinfo, tvb, offset, hf_type_ari);
        }
    }

    proto_item_set_end(item_ac, tvb, *offset);
    return item_ac;
}

static proto_item *
dissect_tbl(proto_tree *tree, packet_info *pinfo _U_, tvbuff_t *tvb, int *offset, int hf_root)
{
    wscbor_chunk_t *chunk;
    chunk = wscbor_chunk_read(pinfo->pool, tvb, offset);
    wscbor_require_array(chunk);
    proto_item *item_tbl = proto_tree_add_cbor_container(tree, hf_root, pinfo, tvb, chunk);
    if (!wscbor_has_errors(chunk))
    {
        proto_tree *tree_tbl = proto_item_add_subtree(item_tbl, ett_type_tbl);
        const guint64 row_count = chunk->head_value - 1;

        PROTO_ITEM_SET_GENERATED(
            proto_tree_add_uint64(tree_tbl, hf_amp_tbl_rowcount, tvb, chunk->start, chunk->head_length, row_count)
        );
        proto_item_append_text(item_tbl, ", Row count: %" PRIu64, row_count);

        dissect_ari(tree_tbl, pinfo, tvb, offset, hf_amp_tbl_tpl);

        for (guint64 ix = 0; ix < row_count; ++ix)
        {
            dissect_tnvc(tree_tbl, pinfo, tvb, offset, hf_amp_tbl_row);
        }
    }
    proto_item_set_end(item_tbl, tvb, *offset);
    return item_tbl;
}

static int
dissect_amp_as_subtree(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset)
{
    wscbor_chunk_t *chunk;
    guint64 msg_count = 0;

    // First byte is the main CBOR type (probably an array of things)
    chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
    if (!wscbor_require_array(chunk))
    {
        wscbor_chunk_mark_errors(pinfo, proto_tree_get_parent(tree), chunk);
        return 0;
    }
    msg_count = chunk->head_value - 1;
    PROTO_ITEM_SET_GENERATED(
        proto_tree_add_uint64(tree, hf_amp_msg_count, tvb, chunk->start, chunk->head_length, msg_count)
    );

    // Item 0 is the timestamp
    chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
    dissect_timeval(tree, hf_amp_primary_timestamp, pinfo, tvb, chunk);

    for (guint64 msgIx = 0; msgIx < msg_count; msgIx++) {
        // Get the bytestring object that gives the total length of the AMP message
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);

        // all message work happens within this sub-TVB, container offset has already shifted
        tvbuff_t *msgtvb = wscbor_require_bstr(pinfo->pool, chunk);

        proto_item *item_msg;
        proto_tree *tree_msg = proto_tree_add_subtree(tree, tvb, chunk->start, chunk->data_length, ett_amp_message, &item_msg, "AMP Message");
        const guint64 msglen = tvb_reported_length(msgtvb);
        PROTO_ITEM_SET_GENERATED(
            proto_tree_add_cbor_uint64(tree_msg, hf_amp_message_len, pinfo, tvb, chunk, &msglen)
        );
        PROTO_ITEM_SET_GENERATED(
            proto_tree_add_cbor_bstr(tree_msg, hf_amp_message_data, pinfo, tvb, chunk)
        );
        dissect_message(msgtvb, pinfo, tree_msg);
    }

    return offset;
}

static int
dissect_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree_msg)
{
    guint offset = 0;
    wscbor_chunk_t *chunk;
    proto_item *item_msg = proto_tree_get_parent(tree_msg);

    // The first byte of this byte string (the AMP message) is going to be the message header
    // This is just a byte, not a CBOR uint8
    int amp_header = tvb_get_guint8(tvb, offset);
    int msg_opcode = amp_header & 0x07;

    proto_tree_add_bitmask(tree_msg, tvb, offset, hf_amp_message_header, ett_amp_message_header,
                           amp_message_header, ENC_BIG_ENDIAN);
    offset += 1;

    const char *msg_type_name = val_to_str_const(msg_opcode, opcode, "Unknown Type");
    proto_item_append_text(item_msg, ": %s", msg_type_name);
    col_append_sep_str(pinfo->cinfo, COL_INFO, NULL, msg_type_name);

    switch (msg_opcode)
    {
    case AMP_MSG_REGISTER_AGENT: // Register agent
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        wscbor_require_major_type(chunk, CBOR_TYPE_STRING);
        proto_tree_add_cbor_tstr(tree_msg, hf_amp_agent_name, pinfo, tvb, chunk);
        break;

    case AMP_MSG_REPORT_SET: { // Report set

        // Rx Names
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        wscbor_require_array(chunk);
        proto_item *names_list = proto_tree_add_cbor_container(tree_msg, hf_amp_rptset_rx_list, pinfo, tvb, chunk);
        if (!wscbor_has_errors(chunk))
        {
            proto_tree *names_tree = proto_item_add_subtree(names_list, ett_rpts_tree);
            const guint64 array_size = chunk->head_value;
            for (guint64 nameIx = 0; nameIx < array_size; ++nameIx)
            {
                chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
                wscbor_require_major_type(chunk, CBOR_TYPE_STRING);
                proto_tree_add_cbor_tstr(names_tree, hf_amp_rptset_rx_name, pinfo, tvb, chunk);
            }
        }
        proto_item_set_end(names_list, tvb, offset);

        // How many reports?
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        wscbor_require_array(chunk);
        proto_item *rpts_list = proto_tree_add_cbor_container(tree_msg, hf_amp_rptset_rpt_list, pinfo, tvb, chunk);
        if (!wscbor_has_errors(chunk))
        {
            proto_tree *rpts_tree = proto_item_add_subtree(rpts_list, ett_rpts_tree);
            const guint64 array_size = chunk->head_value;
            for (guint64 nameIx = 0; nameIx < array_size; ++nameIx)
            {
                // Internals of each report per section 8.4.7 of https://tools.ietf.org/pdf/draft-birrane-dtn-amp-07.pdf
                // Each Report is a:
                //     Tempate [ARI]
                //     Timestamp [TS] (opt)
                //     Entries [TNVC] (bytestring containing a TNVC)

                proto_item *item_rpt;
                proto_tree *tree_rpt = proto_tree_add_subtree(rpts_tree, tvb, offset, 0, ett_amp_rpt, &item_rpt, "Report");

                chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
                wscbor_require_array_size(chunk, 2, 3);
                if (!wscbor_chunk_mark_errors(pinfo, tree_rpt, chunk))
                {
                    const gboolean reportHasTimestamp = chunk->head_value > 2;

                    dissect_ari(tree_rpt, pinfo, tvb, &offset, hf_amp_rpt_tpl);

                    if ( reportHasTimestamp )
                    {
                        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
                        dissect_timeval(tree_rpt, hf_amp_primary_timestamp, pinfo, tvb, chunk);
                    }

                    dissect_tnvc(tree_rpt, pinfo, tvb, &offset, hf_amp_type_tnvc);
                }
                proto_item_set_end(item_rpt, tvb, offset);
            }
        }
        proto_item_set_end(rpts_list, tvb, offset);
        break;
    }
    case AMP_MSG_TABLE_SET: { // Table set

        // Rx Names
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        wscbor_require_array(chunk);
        proto_item *names_list = proto_tree_add_cbor_container(tree_msg, hf_amp_tblset_rx_list, pinfo, tvb, chunk);
        if (!wscbor_has_errors(chunk))
        {
            proto_tree *names_tree = proto_item_add_subtree(names_list, ett_tbls_tree);
            const guint64 array_size = chunk->head_value;
            for (guint64 nameIx = 0; nameIx < array_size; ++nameIx)
            {
                chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
                wscbor_require_major_type(chunk, CBOR_TYPE_STRING);
                proto_tree_add_cbor_tstr(names_tree, hf_amp_tblset_rx_name, pinfo, tvb, chunk);
            }
        }
        proto_item_set_end(names_list, tvb, offset);

        // How many reports?
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        wscbor_require_array(chunk);
        proto_item *tbls_list = proto_tree_add_cbor_container(tree_msg, hf_amp_tblset_tbl_list, pinfo, tvb, chunk);
        if (!wscbor_has_errors(chunk))
        {
            proto_tree *tbls_tree = proto_item_add_subtree(tbls_list, ett_tbls_tree);
            const guint64 array_size = chunk->head_value;
            for (guint64 nameIx = 0; nameIx < array_size; ++nameIx)
            {
                // Internals of each report per section 8.4.7 of https://tools.ietf.org/pdf/draft-birrane-dtn-amp-07.pdf
                // Each Report is a:
                //     Tempate [ARI]
                //     Entries [TNVC] (bytestring containing a TNVC)
                //      ...
                //     Entries [TNVC] (bytestring containing a TNVC)

                proto_item *item_tbl;
                proto_tree *tree_tbl = proto_tree_add_subtree(tbls_tree, tvb, offset, 0, ett_amp_rpt, &item_tbl, "Table");

                chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
                wscbor_require_array(chunk);
                if (!wscbor_chunk_mark_errors(pinfo, tree_tbl, chunk))
                {
                    dissect_ari(tree_tbl, pinfo, tvb, &offset, hf_amp_tbl_tpl);

                    const guint64 tbl_size = chunk->head_value;
                    for (guint64 rowIx = 1; rowIx < tbl_size; ++rowIx)
                    {
                        dissect_tnvc(tree_tbl, pinfo, tvb, &offset, hf_amp_type_tnvc);
                    }
                }
                proto_item_set_end(item_tbl, tvb, offset);
            }
        }
        proto_item_set_end(tbls_list, tvb, offset);
        break;
    }
    case AMP_MSG_PERFORM_CONTROL: { // Perform Control
        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        dissect_timeval(tree_msg, hf_amp_pctrl_start, pinfo, tvb, chunk);

        chunk = wscbor_chunk_read(pinfo->pool, tvb, &offset);
        wscbor_require_array(chunk);
        proto_item *ctrls_list = proto_tree_add_cbor_container(tree_msg, hf_amp_pctrl_ctrls, pinfo, tvb, chunk);
        if (!wscbor_has_errors(chunk))
        {
            proto_tree *ctrls_tree = proto_item_add_subtree(ctrls_list, ett_ctrls_tree);
            for (guint64 ctrlIx = 0; ctrlIx < chunk->head_value; ++ctrlIx)
            {
                dissect_ari(ctrls_tree, pinfo, tvb, &offset, hf_amp_type_ctrl);
            }
        }
        proto_item_set_end(ctrls_list, tvb, offset);
        break;
    }
    default:
        break;
    }

    return offset;
}

/* Code to actually dissect the packets */
static int
dissect_amp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
  int offset = 0;
  proto_item  *amp_packet;
  proto_item  *amp_tree;
  gint sublen;

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "AMP");
  col_clear(pinfo->cinfo, COL_INFO);

  amp_packet = proto_tree_add_item(tree, proto_amp, tvb, 0, -1, ENC_NA);
  amp_tree   = proto_item_add_subtree(amp_packet, ett_amp);

  sublen = dissect_amp_as_subtree (tvb, pinfo, amp_tree, offset);
  proto_item_set_len(amp_packet, sublen);
  return sublen;
}

void
proto_register_amp(void)
{
    static hf_register_info hf[] = {

        { &hf_amp_time_rel,
        { "Relative Time", "amp.time.relative",
          FT_UINT64, BASE_DEC|BASE_UNIT_STRING, &units_seconds, 0x0, NULL, HFILL}
        },
        { &hf_amp_time_abs,
        { "Absolute Time", "amp.time.absolute",
          FT_UINT64, BASE_DEC|BASE_UNIT_STRING, &units_seconds, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_bstr,
        { "BYTESTR", "amp.type.bytestr",
          FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_amp_type_tstr,
        { "TEXTSTR", "amp.type.textstr",
          FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_amp_bstr_text,
        { "As Text", "amp.bstr_text",
          FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_amp_message_len,
        { "Message Length", "amp.message.length",
          FT_UINT64, BASE_DEC|BASE_UNIT_STRING, &units_byte_bytes, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_message_data,
        { "Message Data", "amp.message.data",
          FT_BYTES, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_message_header,
        { "Message Flags", "amp.message.header",
          FT_UINT8, BASE_DEC_HEX, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_rpt_tpl,
        { "Template (ARI)", "amp.rpt.template",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_tbl_rowcount,
        { "Row Count", "amp.tbl.row_count",
            FT_UINT64, BASE_DEC, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_tbl_tpl,
        { "Template (ARI)", "amp.tbl.template",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_tbl_row,
        { "Table Row", "amp.tbl.row",
            FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_pctrl_start,
        { "Start Time", "amp.pctrl.start",
            FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_pctrl_ctrls,
        { "Control List, Count", "amp.pctrl.controls",
            FT_UINT64, BASE_DEC, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_cbor_header,
        { "CBOR-Header", "amp.cbor_header",
            FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_amp_msg_count,
        { "Message Count", "amp.msg_count",
          FT_UINT64, BASE_DEC, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_primary_timestamp,
        { "Timestamp", "amp.primary_timestamp",
          FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_const,
        { "CONST (ARI)", "amp.type.const",
          FT_BYTES, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_type_ctrl,
        { "CTRL (ARI)", "amp.type.ctrl",
          FT_BYTES, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_type_tbl,
        { "TBL", "amp.type.tbl",
          FT_NONE, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_type_bool,
        { "BOOL", "amp.type.bool",
          FT_BOOLEAN, 8, TFS(&tfs_true_false), 0x0, NULL, HFILL}
        },
        { &hf_amp_type_uint,
        { "UINT", "amp.type.uint",
          FT_UINT32, BASE_DEC_HEX, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_uvast,
        { "UVAST", "amp.type.uvast",
          FT_UINT64, BASE_DEC_HEX, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_real32,
        { "REAL32", "amp.type.real32",
          FT_DOUBLE, BASE_NONE, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_real64,
        { "REAL64", "amp.type.real64",
          FT_DOUBLE, BASE_NONE, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_tv,
        { "TV", "amp.type.tv",
          FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_ts,
        { "TS", "amp.type.ts",
          FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0x0, NULL, HFILL}
        },
        { &hf_amp_type_tnvc,
        { "TNVC", "amp.type.tnvc",
          FT_NONE, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_flags,
        { "TNVC Flags", "amp.tnvc.flags",
          FT_UINT8, BASE_DEC_HEX, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_flags_reserved,
        { "Reserved", "amp.tnvc.flags.reserved",
          FT_UINT8, BASE_DEC, NULL, AMP_TNVC_RESERVED,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_flags_mixed,
        { "Mixed", "amp.tnvc.flags.mixed",
          FT_BOOLEAN, 8, TFS(&tfs_yes_no), AMP_TNVC_MIXED,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_flags_type,
        { "Type", "amp.tnvc.flags.type",
          FT_BOOLEAN, 8, TFS(&tfs_yes_no), AMP_TNVC_TYPE,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_flags_name,
        { "Name", "amp.tnvc.flags.name",
          FT_BOOLEAN, 8, TFS(&tfs_yes_no), AMP_TNVC_NAME,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_flags_value,
        { "Value", "amp.tnvc.flags.value",
          FT_BOOLEAN, 8, TFS(&tfs_yes_no), AMP_TNVC_VALUE,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_count,
        { "Item Count", "amp.tnvc.count",
          FT_UINT64, BASE_DEC, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_type,
        { "Type", "amp.tnvc.type",
          FT_UINT8, BASE_DEC, VALS(amp_ari_struct_type), 0x0,
          NULL, HFILL }
        },
        { &hf_amp_tnvc_name,
        { "Name", "amp.tnvc.name",
          FT_STRING, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_type_ari,
          { "ARI", "amp.ari",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_ari_struct,
        { "Struct Type", "amp.ari.struct",
          FT_UINT8, BASE_DEC, VALS(amp_ari_struct_type), 0x0,
          NULL, HFILL }
        },
        { &hf_ari_valtype,
        { "Value Type Offset", "amp.ari.valtype",
          FT_UINT8, BASE_DEC, VALS(amp_ari_value_offset), 0x0,
          NULL, HFILL }
        },
        { &hf_amp_ari_flags,
        { "ARI Flags", "amp.ari.flags",
          FT_UINT8, BASE_DEC_HEX, NULL, 0xF0,
          NULL, HFILL }
        },
        { &hf_ari_flags_nickname,
        { "Nickname", "amp.ari.flags.nickname",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_ARI_NICKNAME,
          NULL, HFILL }
        },
        { &hf_ari_flags_parameters,
        { "Parameters", "amp.ari.flags.parameters",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_ARI_PARAMETERS,
          NULL, HFILL }
        },
        { &hf_ari_flags_issuer,
        { "Issuer", "amp.ari.flags.issuer",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_ARI_ISSUER,
          NULL, HFILL }
        },
        { &hf_ari_flags_tag,
        { "Tag", "amp.ari.flags.tag",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_ARI_TAG,
          NULL, HFILL }
        },
        { &hf_ari_nickname,
        { "Nickname", "amp.ari.nickname",
          FT_UINT64, BASE_DEC_HEX, NULL, 0x0, NULL, HFILL }
        },
        { &hf_nn_adm,
        { "ADM ID", "amp.nickname.adm",
          FT_UINT64, BASE_DEC_HEX, NULL, 0x0, NULL, HFILL }
        },
        { &hf_nn_type,
        { "Object Type", "amp.nickname.objtype",
          FT_UINT8, BASE_DEC, VALS(amp_amm_type), 0x0, NULL, HFILL }
        },
        { &hf_ari_obj_name,
        { "Object Name", "amp.ari.name",
          FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_ari_obj_name_uint,
        { "As Offset", "amp.ari.name.uint",
          FT_UINT64, BASE_DEC, NULL, 0x0, NULL, HFILL }
        },
        { &hf_ari_parameters,
        { "Parameters", "amp.ari.parameters",
          FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_ari_issuer,
        { "Issuer", "amp.ari.issuer",
          FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_ari_tag,
        { "Tag", "amp.ari.tag",
          FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }
        },
        { &hf_amp_type_ari,
        { "ARI", "amp.type.ari",
          FT_BYTES, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_type_ac,
        { "AC, ARI count", "amp.type.ac",
          FT_UINT64, BASE_DEC, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_type_expr,
        { "EXPR", "amp.type.expr",
          FT_NONE, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
        },
        { &hf_amp_reserved,
        { "Reserved", "amp.reserved",
          FT_UINT8, BASE_DEC, NULL, AMP_HDR_RESERVED,
          NULL, HFILL }
        },
        { &hf_amp_acl,
          { "ACL", "amp.acl",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_HDR_ACL,
          NULL, HFILL }
        },
        { &hf_amp_nack,
          { "NACK", "amp.nack",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_HDR_NACK,
          NULL, HFILL }
        },
        { &hf_amp_ack,
          { "ACK", "amp.ack",
          FT_BOOLEAN, 8, TFS(&tfs_present_not_present), AMP_HDR_ACK,
          NULL, HFILL }
        },
        { &hf_amp_opcode,
          { "Opcode", "amp.opcode",
          FT_UINT8, BASE_DEC, VALS(opcode), AMP_HDR_OPCODE,
          NULL, HFILL }
        },
        {&hf_amp_agent_name,
         {"Agent-Name", "amp.agent_name",
          FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL}
        },
        {&hf_amp_rptset_rx_list,
         {"Receivers, Count", "amp.rptset.rx_count",
          FT_UINT64, BASE_DEC, NULL, 0x0, NULL, HFILL}
        },
        {&hf_amp_rptset_rx_name,
         {"Receiver Name", "amp.rptset.rx_name",
          FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL}
        },
        {&hf_amp_rptset_rpt_list,
         {"Reports, Count", "amp.rptset.rpt_count",
          FT_UINT64, BASE_DEC, NULL, 0x0, NULL, HFILL}
        },
        {&hf_amp_tblset_rx_list,
         {"Receivers, Count", "amp.tblset.rx_count",
          FT_UINT64, BASE_DEC, NULL, 0x0, NULL, HFILL}
        },
        {&hf_amp_tblset_rx_name,
         {"Receiver Name", "amp.tblset.rx_name",
          FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL}
        },
        {&hf_amp_tblset_tbl_list,
         {"Tables, Count", "amp.tblset.rpt_count",
          FT_UINT64, BASE_DEC, NULL, 0x0, NULL, HFILL}
        },

    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_amp,
        &ett_amp_ts,
        &ett_amp_bstr,
        &ett_amp_message_header,
        &ett_amp_cbor_header,
        &ett_amp_message,
        &ett_amp_register,
        &ett_ctrls_tree,
        &ett_rpts_tree,
        &ett_amp_rpt,
        &ett_tbls_tree,
        &ett_amp_tbl,
        &ett_amp_tnvc_flags,
        &ett_amp_tnvc_types,
        &ett_amp_tnvc_names,
        &ett_amp_tnvc_values,
        &ett_amp_ari_struct,
        &ett_amp_ari_flags,
        &ett_nickname,
        &ett_type_ari,
        &ett_type_ac,
        &ett_type_expr,
        &ett_type_tnvc,
        &ett_type_tbl,
        &ett_type_const
    };

    static ei_register_info ei[] = {
        { &ei_amp_cbor_malformed, { "amp.cbor.malformed", PI_MALFORMED, PI_ERROR, "Malformed CBOR object", EXPFILL }},
        {&ei_number_overflow, {"amp.uint_overflow", PI_MALFORMED, PI_WARN, "UINT value larger than 2^32-1", EXPFILL}},
    };

    /* Register the protocol name and description */
    proto_amp = proto_register_protocol("Asynchronous Management Protocol", "AMP", "amp");

    /* Required function calls to register the header fields and subtrees used */
    proto_register_field_array(proto_amp, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
    expert_module_t *expert_amp = expert_register_protocol(proto_amp);
    expert_register_field_array(expert_amp, ei, array_length(ei));

    amp_handle = register_dissector("amp", dissect_amp, proto_amp);
}

void
proto_reg_handoff_amp(void)
{
    cbor_handle = find_dissector("cbor");

    dissector_add_uint("ccsds.apid", AMP_APID, amp_handle);
    dissector_add_for_decode_as_with_preference("udp.port", amp_handle);
    dissector_add_for_decode_as("bpv7.payload.dtn_serv", amp_handle);
    dissector_add_for_decode_as_with_preference("bpv7.payload.ipn_serv", amp_handle);
}

/*
 * Editor modelines - http://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
