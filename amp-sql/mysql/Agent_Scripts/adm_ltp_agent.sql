-- -------------------------------------------------------------------
--
-- File Name: adm_ltp_agent.sql
--
-- Description: TODO
--
-- Notes: TODO
--
-- Assumptions: TODO
--
-- Modification History: 
-- YYYY-MM-DD    AUTHOR          DESCRIPTION
-- ----------    --------------  ------------------------------------
-- 2020-11-23    AUTO            Auto-generated SQL file 
--
-- -------------------------------------------------------------------
# ADM: 'DTN/ltp_agent'
use amp_core;


SET @adm_enum = 3;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'DTN/ltp_agent', 'v0.0', 'ltp_agent', @adm_enum, NULL, 'The namespace of the ADM.', @dtn_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @dtn_namespace_id, @ltp_agent_meta_name);
CALL SP__insert_const_actual_definition(@ltp_agent_meta_name, 'The human-readable name of the ADM.', 'STR', 'ltp_agent', @ltp_agent_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @dtn_namespace_id, @ltp_agent_meta_namespace);
CALL SP__insert_const_actual_definition(@ltp_agent_meta_namespace, 'The namespace of the ADM.', 'STR', 'DTN/ltp_agent', @ltp_agent_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @dtn_namespace_id, @ltp_agent_meta_version);
CALL SP__insert_const_actual_definition(@ltp_agent_meta_version, 'The version of the ADM.', 'STR', 'v0.0', @ltp_agent_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @dtn_namespace_id, @ltp_agent_meta_organization);
CALL SP__insert_const_actual_definition(@ltp_agent_meta_organization, 'The name of the issuing organization of the ADM.', 'STR', 'JHUAPL', @ltp_agent_meta_organization_did);

-- #EDD
CALL SP__insert_obj_metadata(2, 'span_remote_engine_nbr', @dtn_namespace_id, @ltp_agent_edd_span_remote_engine_nbr);
CALL SP__insert_formal_parmspec(1, 'parms for span_remote_engine_nbr', @ltp_agent_edd_span_remote_engine_nbr_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_remote_engine_nbr_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_remote_engine_nbr, 'The remote engine number of this span.', @ltp_agent_edd_span_remote_engine_nbr_fp, 'UINT', @ltp_agent_edd_span_remote_engine_nbr_did);

CALL SP__insert_obj_metadata(2, 'span_cur_expt_sess', @dtn_namespace_id, @ltp_agent_edd_span_cur_expt_sess);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_expt_sess', @ltp_agent_edd_span_cur_expt_sess_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_cur_expt_sess_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_cur_expt_sess, 'Expected sessions on this span.', @ltp_agent_edd_span_cur_expt_sess_fp, 'UINT', @ltp_agent_edd_span_cur_expt_sess_did);

CALL SP__insert_obj_metadata(2, 'span_cur_out_seg', @dtn_namespace_id, @ltp_agent_edd_span_cur_out_seg);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_out_seg', @ltp_agent_edd_span_cur_out_seg_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_cur_out_seg_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_cur_out_seg, 'The current number of outbound segments for this span.', @ltp_agent_edd_span_cur_out_seg_fp, 'UINT', @ltp_agent_edd_span_cur_out_seg_did);

CALL SP__insert_obj_metadata(2, 'span_cur_imp_sess', @dtn_namespace_id, @ltp_agent_edd_span_cur_imp_sess);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_imp_sess', @ltp_agent_edd_span_cur_imp_sess_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_cur_imp_sess_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_cur_imp_sess, 'The current number of import segments for this span.', @ltp_agent_edd_span_cur_imp_sess_fp, 'UINT', @ltp_agent_edd_span_cur_imp_sess_did);

CALL SP__insert_obj_metadata(2, 'span_cur_in_seg', @dtn_namespace_id, @ltp_agent_edd_span_cur_in_seg);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_in_seg', @ltp_agent_edd_span_cur_in_seg_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_cur_in_seg_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_cur_in_seg, 'The current number of inbound segments for this span.', @ltp_agent_edd_span_cur_in_seg_fp, 'UINT', @ltp_agent_edd_span_cur_in_seg_did);

CALL SP__insert_obj_metadata(2, 'span_reset_time', @dtn_namespace_id, @ltp_agent_edd_span_reset_time);
CALL SP__insert_formal_parmspec(1, 'parms for span_reset_time', @ltp_agent_edd_span_reset_time_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_reset_time_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_reset_time, 'The last time the span counters were reset.', @ltp_agent_edd_span_reset_time_fp, 'UVAST', @ltp_agent_edd_span_reset_time_did);

CALL SP__insert_obj_metadata(2, 'span_out_seg_q_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_seg_q_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_q_cnt', @ltp_agent_edd_span_out_seg_q_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_seg_q_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_seg_q_cnt, 'The output segment queued count for the span.', @ltp_agent_edd_span_out_seg_q_cnt_fp, 'UINT', @ltp_agent_edd_span_out_seg_q_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_seg_q_bytes', @dtn_namespace_id, @ltp_agent_edd_span_out_seg_q_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_q_bytes', @ltp_agent_edd_span_out_seg_q_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_seg_q_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_seg_q_bytes, 'The output segment queued bytes for the span.', @ltp_agent_edd_span_out_seg_q_bytes_fp, 'UINT', @ltp_agent_edd_span_out_seg_q_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_out_seg_pop_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_seg_pop_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_pop_cnt', @ltp_agent_edd_span_out_seg_pop_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_seg_pop_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_seg_pop_cnt, 'The output segment popped count for the span.', @ltp_agent_edd_span_out_seg_pop_cnt_fp, 'UINT', @ltp_agent_edd_span_out_seg_pop_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_seg_pop_bytes', @dtn_namespace_id, @ltp_agent_edd_span_out_seg_pop_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_pop_bytes', @ltp_agent_edd_span_out_seg_pop_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_seg_pop_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_seg_pop_bytes, 'The output segment popped bytes for the span.', @ltp_agent_edd_span_out_seg_pop_bytes_fp, 'UINT', @ltp_agent_edd_span_out_seg_pop_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_out_ckpt_xmit_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_ckpt_xmit_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_ckpt_xmit_cnt', @ltp_agent_edd_span_out_ckpt_xmit_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_ckpt_xmit_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_ckpt_xmit_cnt, 'The output checkpoint transmit count for the span.', @ltp_agent_edd_span_out_ckpt_xmit_cnt_fp, 'UINT', @ltp_agent_edd_span_out_ckpt_xmit_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_pos_ack_rx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_pos_ack_rx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_pos_ack_rx_cnt', @ltp_agent_edd_span_out_pos_ack_rx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_pos_ack_rx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_pos_ack_rx_cnt, 'The output positive acknowledgement received count for the span.', @ltp_agent_edd_span_out_pos_ack_rx_cnt_fp, 'UINT', @ltp_agent_edd_span_out_pos_ack_rx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_neg_ack_rx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_neg_ack_rx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_neg_ack_rx_cnt', @ltp_agent_edd_span_out_neg_ack_rx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_neg_ack_rx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_neg_ack_rx_cnt, 'The output negative acknowledgement received count for the span.', @ltp_agent_edd_span_out_neg_ack_rx_cnt_fp, 'UINT', @ltp_agent_edd_span_out_neg_ack_rx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_cancel_rx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_cancel_rx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_cancel_rx_cnt', @ltp_agent_edd_span_out_cancel_rx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_cancel_rx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_cancel_rx_cnt, 'The output cancelled received count for the span.', @ltp_agent_edd_span_out_cancel_rx_cnt_fp, 'UINT', @ltp_agent_edd_span_out_cancel_rx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_ckpt_rexmit_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_ckpt_rexmit_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_ckpt_rexmit_cnt', @ltp_agent_edd_span_out_ckpt_rexmit_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_ckpt_rexmit_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_ckpt_rexmit_cnt, 'The output checkpoint retransmit count for the span.', @ltp_agent_edd_span_out_ckpt_rexmit_cnt_fp, 'UINT', @ltp_agent_edd_span_out_ckpt_rexmit_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_cancel_xmit_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_cancel_xmit_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_cancel_xmit_cnt', @ltp_agent_edd_span_out_cancel_xmit_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_cancel_xmit_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_cancel_xmit_cnt, 'The output cancel retransmit count for the span.', @ltp_agent_edd_span_out_cancel_xmit_cnt_fp, 'UINT', @ltp_agent_edd_span_out_cancel_xmit_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_out_complete_cnt', @dtn_namespace_id, @ltp_agent_edd_span_out_complete_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_complete_cnt', @ltp_agent_edd_span_out_complete_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_out_complete_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_out_complete_cnt, 'The output completed count for the span.', @ltp_agent_edd_span_out_complete_cnt_fp, 'UINT', @ltp_agent_edd_span_out_complete_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_red_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_red_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_red_cnt', @ltp_agent_edd_span_in_seg_rx_red_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_red_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_red_cnt, 'The input segment received red count for the span.', @ltp_agent_edd_span_in_seg_rx_red_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_red_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_red_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_red_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_red_bytes', @ltp_agent_edd_span_in_seg_rx_red_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_red_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_red_bytes, 'The input segment received red bytes for the span.', @ltp_agent_edd_span_in_seg_rx_red_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_red_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_green_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_green_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_green_cnt', @ltp_agent_edd_span_in_seg_rx_green_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_green_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_green_cnt, 'The input segment received green count for the span.', @ltp_agent_edd_span_in_seg_rx_green_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_green_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_green_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_green_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_green_bytes', @ltp_agent_edd_span_in_seg_rx_green_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_green_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_green_bytes, 'The input segment received green bytes for the span.', @ltp_agent_edd_span_in_seg_rx_green_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_green_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_redundant_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_redundant_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_redundant_cnt', @ltp_agent_edd_span_in_seg_rx_redundant_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_redundant_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_redundant_cnt, 'The input segment received redundant count for the span.', @ltp_agent_edd_span_in_seg_rx_redundant_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_redundant_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_redundant_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_redundant_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_redundant_bytes', @ltp_agent_edd_span_in_seg_rx_redundant_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_redundant_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_redundant_bytes, 'The input segment received redundant bytes for the span.', @ltp_agent_edd_span_in_seg_rx_redundant_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_redundant_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_mal_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_mal_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_mal_cnt', @ltp_agent_edd_span_in_seg_rx_mal_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_mal_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_mal_cnt, 'The input segment malformed count for the span.', @ltp_agent_edd_span_in_seg_rx_mal_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_mal_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_mal_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_mal_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_mal_bytes', @ltp_agent_edd_span_in_seg_rx_mal_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_mal_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_mal_bytes, 'The input segment malformed bytes for the span.', @ltp_agent_edd_span_in_seg_rx_mal_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_mal_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_unk_sender_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_unk_sender_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_sender_cnt', @ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_unk_sender_cnt, 'The input segment unknown sender count for the span.', @ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_unk_sender_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_unk_sender_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_sender_bytes', @ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_unk_sender_bytes, 'The input segment unknown sender bytes for the span.', @ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_unk_client_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_unk_client_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_client_cnt', @ltp_agent_edd_span_in_seg_rx_unk_client_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_unk_client_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_unk_client_cnt, 'The input segment unknown client count for the span.', @ltp_agent_edd_span_in_seg_rx_unk_client_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_unk_client_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_rx_unk_client_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_rx_unk_client_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_client_bytes', @ltp_agent_edd_span_in_seg_rx_unk_client_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_rx_unk_client_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_rx_unk_client_bytes, 'The input segment unknown client bytes for the span.', @ltp_agent_edd_span_in_seg_rx_unk_client_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_rx_unk_client_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_stray_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_stray_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_stray_cnt', @ltp_agent_edd_span_in_seg_stray_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_stray_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_stray_cnt, 'The input segment stray count for the span.', @ltp_agent_edd_span_in_seg_stray_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_stray_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_stray_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_stray_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_stray_bytes', @ltp_agent_edd_span_in_seg_stray_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_stray_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_stray_bytes, 'The input segment stray bytes for the span.', @ltp_agent_edd_span_in_seg_stray_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_stray_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_miscolor_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_miscolor_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_miscolor_cnt', @ltp_agent_edd_span_in_seg_miscolor_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_miscolor_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_miscolor_cnt, 'The input segment miscolored count for the span.', @ltp_agent_edd_span_in_seg_miscolor_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_miscolor_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_miscolor_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_miscolor_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_miscolor_bytes', @ltp_agent_edd_span_in_seg_miscolor_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_miscolor_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_miscolor_bytes, 'The input segment miscolored bytes for the span.', @ltp_agent_edd_span_in_seg_miscolor_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_miscolor_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_closed_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_closed_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_closed_cnt', @ltp_agent_edd_span_in_seg_closed_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_closed_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_closed_cnt, 'The input segment closed count for the span.', @ltp_agent_edd_span_in_seg_closed_cnt_fp, 'UINT', @ltp_agent_edd_span_in_seg_closed_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_seg_closed_bytes', @dtn_namespace_id, @ltp_agent_edd_span_in_seg_closed_bytes);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_closed_bytes', @ltp_agent_edd_span_in_seg_closed_bytes_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_seg_closed_bytes_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_seg_closed_bytes, 'The input segment closed bytes for the span.', @ltp_agent_edd_span_in_seg_closed_bytes_fp, 'UINT', @ltp_agent_edd_span_in_seg_closed_bytes_did);

CALL SP__insert_obj_metadata(2, 'span_in_ckpt_rx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_ckpt_rx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_ckpt_rx_cnt', @ltp_agent_edd_span_in_ckpt_rx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_ckpt_rx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_ckpt_rx_cnt, 'The input checkpoint receive count for the span.', @ltp_agent_edd_span_in_ckpt_rx_cnt_fp, 'UINT', @ltp_agent_edd_span_in_ckpt_rx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_pos_ack_tx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_pos_ack_tx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_pos_ack_tx_cnt', @ltp_agent_edd_span_in_pos_ack_tx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_pos_ack_tx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_pos_ack_tx_cnt, 'The input positive acknolwedgement transmitted count for the span.', @ltp_agent_edd_span_in_pos_ack_tx_cnt_fp, 'UINT', @ltp_agent_edd_span_in_pos_ack_tx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_neg_ack_tx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_neg_ack_tx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_neg_ack_tx_cnt', @ltp_agent_edd_span_in_neg_ack_tx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_neg_ack_tx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_neg_ack_tx_cnt, 'The input negative acknolwedgement transmitted count for the span.', @ltp_agent_edd_span_in_neg_ack_tx_cnt_fp, 'UINT', @ltp_agent_edd_span_in_neg_ack_tx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_cancel_tx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_cancel_tx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_cancel_tx_cnt', @ltp_agent_edd_span_in_cancel_tx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_cancel_tx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_cancel_tx_cnt, 'The input cancel transmitted count for the span.', @ltp_agent_edd_span_in_cancel_tx_cnt_fp, 'UINT', @ltp_agent_edd_span_in_cancel_tx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_ack_retx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_ack_retx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_ack_retx_cnt', @ltp_agent_edd_span_in_ack_retx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_ack_retx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_ack_retx_cnt, 'The input acknolwedgement retransmit count for the span.', @ltp_agent_edd_span_in_ack_retx_cnt_fp, 'UINT', @ltp_agent_edd_span_in_ack_retx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_cancel_rx_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_cancel_rx_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_cancel_rx_cnt', @ltp_agent_edd_span_in_cancel_rx_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_cancel_rx_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_cancel_rx_cnt, 'The input cancel receive count for the span.', @ltp_agent_edd_span_in_cancel_rx_cnt_fp, 'UINT', @ltp_agent_edd_span_in_cancel_rx_cnt_did);

CALL SP__insert_obj_metadata(2, 'span_in_complete_cnt', @dtn_namespace_id, @ltp_agent_edd_span_in_complete_cnt);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_complete_cnt', @ltp_agent_edd_span_in_complete_cnt_fp);
CALL SP__insert_formal_parmspec_entry(@ltp_agent_edd_span_in_complete_cnt_fp, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@ltp_agent_edd_span_in_complete_cnt, 'The input completed count for the span.', @ltp_agent_edd_span_in_complete_cnt_fp, 'UINT', @ltp_agent_edd_span_in_complete_cnt_did);

-- #OPER

-- #VAR

-- #TBLT
CALL SP__insert_obj_metadata(10, 'engines', @dtn_namespace_id, @ltp_agent_tblt_engines);
CALL SP__insert_tnvc_collection('columns for the engines table', @tbl_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@tbl_tnvc_id, 1, 'peer_engine_nbr', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ltp_agent_tblt_engines, 'This table lists all known remote engine ids.', @tbl_tnvc_id, @ltp_agent_tblt_engines_did);


-- #RPTT

CALL SP__insert_obj_metadata(7, 'endpointReport', @dtn_namespace_id, @ltp_agent_rpttpl_endpointreport);
CALL SP__insert_formal_parmspec(1, 'parms for span_remote_engine_nbr', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_remote_engine_nbr', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_remote_engine_nbr_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_remote_engine_nbr_did, NULL, @ap_spec_id, @ltp_agent_edd_span_remote_engine_nbr_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_expt_sess', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_cur_expt_sess', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_cur_expt_sess_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_cur_expt_sess_did, NULL, @ap_spec_id, @ltp_agent_edd_span_cur_expt_sess_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_out_seg', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_cur_out_seg', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_cur_out_seg_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_cur_out_seg_did, NULL, @ap_spec_id, @ltp_agent_edd_span_cur_out_seg_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_imp_sess', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_cur_imp_sess', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_cur_imp_sess_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_cur_imp_sess_did, NULL, @ap_spec_id, @ltp_agent_edd_span_cur_imp_sess_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_cur_in_seg', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_cur_in_seg', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_cur_in_seg_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_cur_in_seg_did, NULL, @ap_spec_id, @ltp_agent_edd_span_cur_in_seg_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_reset_time', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_reset_time', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_reset_time_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_reset_time_did, NULL, @ap_spec_id, @ltp_agent_edd_span_reset_time_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_q_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_seg_q_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_seg_q_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_seg_q_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_seg_q_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_q_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_seg_q_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_seg_q_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_seg_q_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_seg_q_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_pop_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_seg_pop_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_seg_pop_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_seg_pop_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_seg_pop_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_seg_pop_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_seg_pop_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_seg_pop_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_seg_pop_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_seg_pop_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_ckpt_xmit_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_ckpt_xmit_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_ckpt_xmit_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_ckpt_xmit_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_ckpt_xmit_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_pos_ack_rx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_pos_ack_rx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_pos_ack_rx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_pos_ack_rx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_pos_ack_rx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_neg_ack_rx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_neg_ack_rx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_neg_ack_rx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_neg_ack_rx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_neg_ack_rx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_cancel_rx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_cancel_rx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_cancel_rx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_cancel_rx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_cancel_rx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_ckpt_rexmit_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_ckpt_rexmit_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_ckpt_rexmit_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_ckpt_rexmit_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_ckpt_rexmit_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_cancel_xmit_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_cancel_xmit_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_cancel_xmit_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_cancel_xmit_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_cancel_xmit_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_out_complete_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_out_complete_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_out_complete_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_out_complete_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_out_complete_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_red_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_red_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_red_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_red_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_red_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_red_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_red_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_red_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_red_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_red_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_green_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_green_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_green_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_green_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_green_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_green_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_green_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_green_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_green_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_green_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_redundant_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_redundant_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_redundant_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_redundant_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_redundant_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_redundant_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_redundant_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_redundant_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_redundant_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_redundant_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_mal_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_mal_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_mal_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_mal_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_mal_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_mal_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_mal_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_mal_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_mal_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_mal_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_sender_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_unk_sender_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_sender_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_unk_sender_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_client_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_unk_client_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_unk_client_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_unk_client_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_unk_client_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_rx_unk_client_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_rx_unk_client_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_rx_unk_client_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_rx_unk_client_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_rx_unk_client_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_stray_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_stray_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_stray_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_stray_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_stray_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_stray_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_stray_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_stray_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_stray_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_stray_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_miscolor_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_miscolor_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_miscolor_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_miscolor_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_miscolor_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_miscolor_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_miscolor_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_miscolor_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_miscolor_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_miscolor_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_closed_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_closed_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_closed_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_closed_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_closed_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_seg_closed_bytes', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_seg_closed_bytes', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_seg_closed_bytes_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_seg_closed_bytes_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_seg_closed_bytes_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_ckpt_rx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_ckpt_rx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_ckpt_rx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_ckpt_rx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_ckpt_rx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_pos_ack_tx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_pos_ack_tx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_pos_ack_tx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_pos_ack_tx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_pos_ack_tx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_neg_ack_tx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_neg_ack_tx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_neg_ack_tx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_neg_ack_tx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_neg_ack_tx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_cancel_tx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_cancel_tx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_cancel_tx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_cancel_tx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_cancel_tx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_ack_retx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_ack_retx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_ack_retx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_ack_retx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_ack_retx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_cancel_rx_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_cancel_rx_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_cancel_rx_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_cancel_rx_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_cancel_rx_cnt_aid_ltp_span_1);
CALL SP__insert_formal_parmspec(1, 'parms for span_in_complete_cnt', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'span_in_complete_cnt', 'UINT', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@ltp_agent_edd_span_in_complete_cnt_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'UINT', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@ltp_agent_edd_span_in_complete_cnt_did, NULL, @ap_spec_id, @ltp_agent_edd_span_in_complete_cnt_aid_ltp_span_1);

CALL SP__insert_ac_id(42, 'ac for report template endpointReport', @rptt_ac_id);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_remote_engine_nbr_did, 1, @r_ac_rpt_entry_1);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_cur_expt_sess_did, 2, @r_ac_rpt_entry_2);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_cur_out_seg_did, 3, @r_ac_rpt_entry_3);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_cur_imp_sess_did, 4, @r_ac_rpt_entry_4);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_cur_in_seg_did, 5, @r_ac_rpt_entry_5);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_reset_time_did, 6, @r_ac_rpt_entry_6);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_seg_q_cnt_did, 7, @r_ac_rpt_entry_7);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_seg_q_bytes_did, 8, @r_ac_rpt_entry_8);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_seg_pop_cnt_did, 9, @r_ac_rpt_entry_9);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_seg_pop_bytes_did, 10, @r_ac_rpt_entry_10);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_ckpt_xmit_cnt_did, 11, @r_ac_rpt_entry_11);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_pos_ack_rx_cnt_did, 12, @r_ac_rpt_entry_12);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_neg_ack_rx_cnt_did, 13, @r_ac_rpt_entry_13);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_cancel_rx_cnt_did, 14, @r_ac_rpt_entry_14);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_ckpt_rexmit_cnt_did, 15, @r_ac_rpt_entry_15);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_cancel_xmit_cnt_did, 16, @r_ac_rpt_entry_16);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_out_complete_cnt_did, 17, @r_ac_rpt_entry_17);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_red_cnt_did, 18, @r_ac_rpt_entry_18);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_red_bytes_did, 19, @r_ac_rpt_entry_19);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_green_cnt_did, 20, @r_ac_rpt_entry_20);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_green_bytes_did, 21, @r_ac_rpt_entry_21);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_redundant_cnt_did, 22, @r_ac_rpt_entry_22);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_redundant_bytes_did, 23, @r_ac_rpt_entry_23);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_mal_cnt_did, 24, @r_ac_rpt_entry_24);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_mal_bytes_did, 25, @r_ac_rpt_entry_25);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_unk_sender_cnt_did, 26, @r_ac_rpt_entry_26);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_unk_sender_bytes_did, 27, @r_ac_rpt_entry_27);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_unk_client_cnt_did, 28, @r_ac_rpt_entry_28);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_rx_unk_client_bytes_did, 29, @r_ac_rpt_entry_29);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_stray_cnt_did, 30, @r_ac_rpt_entry_30);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_stray_bytes_did, 31, @r_ac_rpt_entry_31);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_miscolor_cnt_did, 32, @r_ac_rpt_entry_32);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_miscolor_bytes_did, 33, @r_ac_rpt_entry_33);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_closed_cnt_did, 34, @r_ac_rpt_entry_34);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_seg_closed_bytes_did, 35, @r_ac_rpt_entry_35);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_ckpt_rx_cnt_did, 36, @r_ac_rpt_entry_36);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_pos_ack_tx_cnt_did, 37, @r_ac_rpt_entry_37);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_neg_ack_tx_cnt_did, 38, @r_ac_rpt_entry_38);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_cancel_tx_cnt_did, 39, @r_ac_rpt_entry_39);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_ack_retx_cnt_did, 40, @r_ac_rpt_entry_40);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_cancel_rx_cnt_did, 41, @r_ac_rpt_entry_41);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @ltp_agent_edd_span_in_complete_cnt_did, 42, @r_ac_rpt_entry_42);

CALL SP__insert_report_template_formal_definition(@ltp_agent_rpttpl_endpointreport, 'This is all known endpoint information', null, @rptt_ac_id, @ltp_agent_rpttpl_endpointreport_did);
CALL SP__insert_report_actual_definition(@ltp_agent_rpttpl_endpointreport, null, null, 'Singleton value for endpointReport', @ltp_agent_rpttpl_endpointreport_aid);

-- #CTRL
CALL SP__insert_obj_metadata(1, 'reset', @dtn_namespace_id, @ltp_agent_ctrl_reset);
CALL SP__insert_formal_parmspec(1, 'parms for the reset control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_span', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ltp_agent_ctrl_reset , 'Resets the counters associated with the engine and updates the last reset time for the span to be the time when this control was run.', @fp_spec_id, @ltp_agent_ctrl_reset_did);


-- #CONST

-- #MAC
