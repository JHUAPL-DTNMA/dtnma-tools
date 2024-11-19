-- -------------------------------------------------------------------
--
-- File Name: adm_bp_agent.sql
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
# ADM: 'DTN/bp_agent'
use amp_core;


SET @adm_enum = 2;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'DTN/bp_agent', 'v0.1', 'bp_agent', @adm_enum, NULL, 'The namespace of the ADM.', @dtn_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @dtn_namespace_id, @bp_agent_meta_name);
CALL SP__insert_const_actual_definition(@bp_agent_meta_name, 'The human-readable name of the ADM.', 'STR', 'bp_agent', @bp_agent_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @dtn_namespace_id, @bp_agent_meta_namespace);
CALL SP__insert_const_actual_definition(@bp_agent_meta_namespace, 'The namespace of the ADM.', 'STR', 'DTN/bp_agent', @bp_agent_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @dtn_namespace_id, @bp_agent_meta_version);
CALL SP__insert_const_actual_definition(@bp_agent_meta_version, 'The version of the ADM', 'STR', 'v0.1', @bp_agent_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @dtn_namespace_id, @bp_agent_meta_organization);
CALL SP__insert_const_actual_definition(@bp_agent_meta_organization, 'The name of the issuing organization of the ADM.', 'STR', 'JHUAPL', @bp_agent_meta_organization_did);

-- #EDD
CALL SP__insert_obj_metadata(2, 'bp_node_id', @dtn_namespace_id, @bp_agent_edd_bp_node_id);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_bp_node_id, 'The node administration endpoint', NULL, 'STR', @bp_agent_edd_bp_node_id_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bp_node_id, 'The singleton value for bp_node_id', NULL, @bp_agent_edd_bp_node_id_aid);

CALL SP__insert_obj_metadata(2, 'bp_node_version', @dtn_namespace_id, @bp_agent_edd_bp_node_version);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_bp_node_version, 'The latest version of the BP supported by this node', NULL, 'STR', @bp_agent_edd_bp_node_version_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bp_node_version, 'The singleton value for bp_node_version', NULL, @bp_agent_edd_bp_node_version_aid);

CALL SP__insert_obj_metadata(2, 'available_storage', @dtn_namespace_id, @bp_agent_edd_available_storage);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_available_storage, 'Bytes available for bundle storage', NULL, 'UVAST', @bp_agent_edd_available_storage_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_available_storage, 'The singleton value for available_storage', NULL, @bp_agent_edd_available_storage_aid);

CALL SP__insert_obj_metadata(2, 'last_reset_time', @dtn_namespace_id, @bp_agent_edd_last_reset_time);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_last_reset_time, 'The last time that BP counters were reset, either due to execution of a reset control or a restart of the node itself', NULL, 'UVAST', @bp_agent_edd_last_reset_time_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_last_reset_time, 'The singleton value for last_reset_time', NULL, @bp_agent_edd_last_reset_time_aid);

CALL SP__insert_obj_metadata(2, 'num_registrations', @dtn_namespace_id, @bp_agent_edd_num_registrations);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_registrations, 'number of registrations', NULL, 'UINT', @bp_agent_edd_num_registrations_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_registrations, 'The singleton value for num_registrations', NULL, @bp_agent_edd_num_registrations_aid);

CALL SP__insert_obj_metadata(2, 'num_pend_fwd', @dtn_namespace_id, @bp_agent_edd_num_pend_fwd);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_pend_fwd, 'number of bundles pending forwarding', NULL, 'UINT', @bp_agent_edd_num_pend_fwd_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_pend_fwd, 'The singleton value for num_pend_fwd', NULL, @bp_agent_edd_num_pend_fwd_aid);

CALL SP__insert_obj_metadata(2, 'num_pend_dis', @dtn_namespace_id, @bp_agent_edd_num_pend_dis);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_pend_dis, 'number of bundles awaiting dispatch', NULL, 'UINT', @bp_agent_edd_num_pend_dis_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_pend_dis, 'The singleton value for num_pend_dis', NULL, @bp_agent_edd_num_pend_dis_aid);

CALL SP__insert_obj_metadata(2, 'num_in_cust', @dtn_namespace_id, @bp_agent_edd_num_in_cust);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_in_cust, 'number of bundles', NULL, 'UINT', @bp_agent_edd_num_in_cust_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_in_cust, 'The singleton value for num_in_cust', NULL, @bp_agent_edd_num_in_cust_aid);

CALL SP__insert_obj_metadata(2, 'num_pend_reassembly', @dtn_namespace_id, @bp_agent_edd_num_pend_reassembly);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_pend_reassembly, 'number of bundles pending reassembly', NULL, 'UINT', @bp_agent_edd_num_pend_reassembly_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_pend_reassembly, 'The singleton value for num_pend_reassembly', NULL, @bp_agent_edd_num_pend_reassembly_aid);

CALL SP__insert_obj_metadata(2, 'bundles_by_priority', @dtn_namespace_id, @bp_agent_edd_bundles_by_priority);
CALL SP__insert_formal_parmspec(1, 'parms for bundles_by_priority', @bp_agent_edd_bundles_by_priority_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_bundles_by_priority_fp, 1, 'mask', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_bundles_by_priority, 'number of bundles for the given priority. Priority is given as a priority mask where Bulk=0x1, normal=0x2, express=0x4. Any bundles matching any of the masked priorities will be included in the returned count', @bp_agent_edd_bundles_by_priority_fp, 'UINT', @bp_agent_edd_bundles_by_priority_did);

CALL SP__insert_obj_metadata(2, 'bytes_by_priority', @dtn_namespace_id, @bp_agent_edd_bytes_by_priority);
CALL SP__insert_formal_parmspec(1, 'parms for bytes_by_priority', @bp_agent_edd_bytes_by_priority_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_bytes_by_priority_fp, 1, 'mask', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_bytes_by_priority, 'number of bytes of the given priority. Priority is given as a priority mask where bulk=0x1, normal=0x2, express=0x4. Any bundles matching any of the masked priorities will be included in the returned count.', @bp_agent_edd_bytes_by_priority_fp, 'UINT', @bp_agent_edd_bytes_by_priority_did);

CALL SP__insert_obj_metadata(2, 'src_bundles_by_priority', @dtn_namespace_id, @bp_agent_edd_src_bundles_by_priority);
CALL SP__insert_formal_parmspec(1, 'parms for src_bundles_by_priority', @bp_agent_edd_src_bundles_by_priority_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_src_bundles_by_priority_fp, 1, 'mask', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_src_bundles_by_priority, 'number of bundles sourced by this node of the given priority. Priority is given as a priority mask where bulk=0x1, normal=0x2, express=0x4. Any bundles sourced by this node and matching any of the masked priorities will be included in the returned count.', @bp_agent_edd_src_bundles_by_priority_fp, 'UINT', @bp_agent_edd_src_bundles_by_priority_did);

CALL SP__insert_obj_metadata(2, 'src_bytes_by_priority', @dtn_namespace_id, @bp_agent_edd_src_bytes_by_priority);
CALL SP__insert_formal_parmspec(1, 'parms for src_bytes_by_priority', @bp_agent_edd_src_bytes_by_priority_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_src_bytes_by_priority_fp, 1, 'mask', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_src_bytes_by_priority, 'number of bytes sourced by this node of the given priority. Priority is given as a priority mask where bulk=0x1, normal=0x2, express=0x4. Any bundles sourced by this node and matching any of the masked priorities will be included in the returned count', @bp_agent_edd_src_bytes_by_priority_fp, 'UINT', @bp_agent_edd_src_bytes_by_priority_did);

CALL SP__insert_obj_metadata(2, 'num_fragmented_bundles', @dtn_namespace_id, @bp_agent_edd_num_fragmented_bundles);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_fragmented_bundles, 'number of fragmented bundles', NULL, 'UINT', @bp_agent_edd_num_fragmented_bundles_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_fragmented_bundles, 'The singleton value for num_fragmented_bundles', NULL, @bp_agent_edd_num_fragmented_bundles_aid);

CALL SP__insert_obj_metadata(2, 'num_fragments_produced', @dtn_namespace_id, @bp_agent_edd_num_fragments_produced);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_fragments_produced, 'number of bundles with fragmentary payloads produced by this node', NULL, 'UINT', @bp_agent_edd_num_fragments_produced_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_fragments_produced, 'The singleton value for num_fragments_produced', NULL, @bp_agent_edd_num_fragments_produced_aid);

CALL SP__insert_obj_metadata(2, 'num_failed_by_reason', @dtn_namespace_id, @bp_agent_edd_num_failed_by_reason);
CALL SP__insert_formal_parmspec(1, 'parms for num_failed_by_reason', @bp_agent_edd_num_failed_by_reason_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_num_failed_by_reason_fp, 1, 'mask', 'UINT', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_failed_by_reason, 'number of bundles failed for any of the given reasons. (noInfo=0x1, Expired=0x2, UniFwd=0x4, Cancelled=0x8, NoStorage=0x10, BadEID=0x20, NoRoute=0x40, NoContact=0x80, BadBlock=0x100)', @bp_agent_edd_num_failed_by_reason_fp, 'UINT', @bp_agent_edd_num_failed_by_reason_did);

CALL SP__insert_obj_metadata(2, 'num_bundles_deleted', @dtn_namespace_id, @bp_agent_edd_num_bundles_deleted);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_num_bundles_deleted, 'number of bundles deleted by this node', NULL, 'UINT', @bp_agent_edd_num_bundles_deleted_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_bundles_deleted, 'The singleton value for num_bundles_deleted', NULL, @bp_agent_edd_num_bundles_deleted_aid);

CALL SP__insert_obj_metadata(2, 'failed_custody_bundles', @dtn_namespace_id, @bp_agent_edd_failed_custody_bundles);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_failed_custody_bundles, 'number of bundle fails at this node', NULL, 'UINT', @bp_agent_edd_failed_custody_bundles_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_failed_custody_bundles, 'The singleton value for failed_custody_bundles', NULL, @bp_agent_edd_failed_custody_bundles_aid);

CALL SP__insert_obj_metadata(2, 'failed_custody_bytes', @dtn_namespace_id, @bp_agent_edd_failed_custody_bytes);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_failed_custody_bytes, 'number bytes of fails at this node', NULL, 'UINT', @bp_agent_edd_failed_custody_bytes_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_failed_custody_bytes, 'The singleton value for failed_custody_bytes', NULL, @bp_agent_edd_failed_custody_bytes_aid);

CALL SP__insert_obj_metadata(2, 'failed_forward_bundles', @dtn_namespace_id, @bp_agent_edd_failed_forward_bundles);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_failed_forward_bundles, 'number bundles not forwarded by this node', NULL, 'UINT', @bp_agent_edd_failed_forward_bundles_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_failed_forward_bundles, 'The singleton value for failed_forward_bundles', NULL, @bp_agent_edd_failed_forward_bundles_aid);

CALL SP__insert_obj_metadata(2, 'failed_forward_bytes', @dtn_namespace_id, @bp_agent_edd_failed_forward_bytes);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_failed_forward_bytes, 'number of bytes not forwaded by this node', NULL, 'UINT', @bp_agent_edd_failed_forward_bytes_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_failed_forward_bytes, 'The singleton value for failed_forward_bytes', NULL, @bp_agent_edd_failed_forward_bytes_aid);

CALL SP__insert_obj_metadata(2, 'abandoned_bundles', @dtn_namespace_id, @bp_agent_edd_abandoned_bundles);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_abandoned_bundles, 'number of bundles abandoned by this node', NULL, 'UINT', @bp_agent_edd_abandoned_bundles_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_abandoned_bundles, 'The singleton value for abandoned_bundles', NULL, @bp_agent_edd_abandoned_bundles_aid);

CALL SP__insert_obj_metadata(2, 'abandoned_bytes', @dtn_namespace_id, @bp_agent_edd_abandoned_bytes);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_abandoned_bytes, 'number of bytes abandoned by this node', NULL, 'UINT', @bp_agent_edd_abandoned_bytes_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_abandoned_bytes, 'The singleton value for abandoned_bytes', NULL, @bp_agent_edd_abandoned_bytes_aid);

CALL SP__insert_obj_metadata(2, 'discarded_bundles', @dtn_namespace_id, @bp_agent_edd_discarded_bundles);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_discarded_bundles, 'number of bundles discarded by this node', NULL, 'UINT', @bp_agent_edd_discarded_bundles_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_discarded_bundles, 'The singleton value for discarded_bundles', NULL, @bp_agent_edd_discarded_bundles_aid);

CALL SP__insert_obj_metadata(2, 'discarded_bytes', @dtn_namespace_id, @bp_agent_edd_discarded_bytes);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_discarded_bytes, 'number of bytes discarded by this node', NULL, 'UINT', @bp_agent_edd_discarded_bytes_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_discarded_bytes, 'The singleton value for discarded_bytes', NULL, @bp_agent_edd_discarded_bytes_aid);

CALL SP__insert_obj_metadata(2, 'endpoint_names', @dtn_namespace_id, @bp_agent_edd_endpoint_names);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_endpoint_names, 'CSV list of endpoint names for this node', NULL, 'STR', @bp_agent_edd_endpoint_names_did);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_endpoint_names, 'The singleton value for endpoint_names', NULL, @bp_agent_edd_endpoint_names_aid);

CALL SP__insert_obj_metadata(2, 'endpoint_active', @dtn_namespace_id, @bp_agent_edd_endpoint_active);
CALL SP__insert_formal_parmspec(1, 'parms for endpoint_active', @bp_agent_edd_endpoint_active_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_endpoint_active_fp, 1, 'endpoint_name', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_endpoint_active, 'is the given endpoint active? (0=no)', @bp_agent_edd_endpoint_active_fp, 'UINT', @bp_agent_edd_endpoint_active_did);

CALL SP__insert_obj_metadata(2, 'endpoint_singleton', @dtn_namespace_id, @bp_agent_edd_endpoint_singleton);
CALL SP__insert_formal_parmspec(1, 'parms for endpoint_singleton', @bp_agent_edd_endpoint_singleton_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_endpoint_singleton_fp, 1, 'endpoint_name', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_endpoint_singleton, 'is the given endpoint singleton? (0=no)', @bp_agent_edd_endpoint_singleton_fp, 'UINT', @bp_agent_edd_endpoint_singleton_did);

CALL SP__insert_obj_metadata(2, 'endpoint_policy', @dtn_namespace_id, @bp_agent_edd_endpoint_policy);
CALL SP__insert_formal_parmspec(1, 'parms for endpoint_policy', @bp_agent_edd_endpoint_policy_fp);
CALL SP__insert_formal_parmspec_entry(@bp_agent_edd_endpoint_policy_fp, 1, 'endpoint_name', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@bp_agent_edd_endpoint_policy, 'Does the endpoint abandon on fail (0=no)', @bp_agent_edd_endpoint_policy_fp, 'UINT', @bp_agent_edd_endpoint_policy_did);

-- #OPER

-- #VAR

-- #TBLT

-- #RPTT

CALL SP__insert_obj_metadata(7, 'full_report', @dtn_namespace_id, @bp_agent_rpttpl_full_report);

CALL SP__insert_actual_parmspec(@bp_agent_edd_bundles_by_priority_fp, 1, 'actual parms for bundles_by_priority passed 1', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 1', @dtn_namespace_id, @p_lit_meta_1_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_1_id, 'literal value 1', 'UINT', '1', @r_lit_1_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_1_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bundles_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_bundles_by_priority_aid_1_1);

CALL SP__insert_actual_parmspec(@bp_agent_edd_bundles_by_priority_fp, 1, 'actual parms for bundles_by_priority passed 2', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 2', @dtn_namespace_id, @p_lit_meta_2_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_2_id, 'literal value 2', 'UINT', '2', @r_lit_2_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_2_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bundles_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_bundles_by_priority_aid_1_2);

CALL SP__insert_actual_parmspec(@bp_agent_edd_bundles_by_priority_fp, 1, 'actual parms for bundles_by_priority passed 4', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 4', @dtn_namespace_id, @p_lit_meta_4_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_4_id, 'literal value 4', 'UINT', '4', @r_lit_4_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_4_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bundles_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_bundles_by_priority_aid_1_4);

CALL SP__insert_actual_parmspec(@bp_agent_edd_bytes_by_priority_fp, 1, 'actual parms for bytes_by_priority passed 1', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 1', @dtn_namespace_id, @p_lit_meta_1_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_1_id, 'literal value 1', 'UINT', '1', @r_lit_1_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_1_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bytes_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_bytes_by_priority_aid_1_1);

CALL SP__insert_actual_parmspec(@bp_agent_edd_bytes_by_priority_fp, 1, 'actual parms for bytes_by_priority passed 2', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 2', @dtn_namespace_id, @p_lit_meta_2_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_2_id, 'literal value 2', 'UINT', '2', @r_lit_2_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_2_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bytes_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_bytes_by_priority_aid_1_2);

CALL SP__insert_actual_parmspec(@bp_agent_edd_bytes_by_priority_fp, 1, 'actual parms for bytes_by_priority passed 4', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 4', @dtn_namespace_id, @p_lit_meta_4_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_4_id, 'literal value 4', 'UINT', '4', @r_lit_4_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_4_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_bytes_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_bytes_by_priority_aid_1_4);

CALL SP__insert_actual_parmspec(@bp_agent_edd_src_bundles_by_priority_fp, 1, 'actual parms for src_bundles_by_priority passed 1', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 1', @dtn_namespace_id, @p_lit_meta_1_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_1_id, 'literal value 1', 'UINT', '1', @r_lit_1_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_1_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_src_bundles_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_src_bundles_by_priority_aid_1_1);

CALL SP__insert_actual_parmspec(@bp_agent_edd_src_bundles_by_priority_fp, 1, 'actual parms for src_bundles_by_priority passed 2', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 2', @dtn_namespace_id, @p_lit_meta_2_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_2_id, 'literal value 2', 'UINT', '2', @r_lit_2_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_2_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_src_bundles_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_src_bundles_by_priority_aid_1_2);

CALL SP__insert_actual_parmspec(@bp_agent_edd_src_bundles_by_priority_fp, 1, 'actual parms for src_bundles_by_priority passed 4', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 4', @dtn_namespace_id, @p_lit_meta_4_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_4_id, 'literal value 4', 'UINT', '4', @r_lit_4_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_4_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_src_bundles_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_src_bundles_by_priority_aid_1_4);

CALL SP__insert_actual_parmspec(@bp_agent_edd_src_bytes_by_priority_fp, 1, 'actual parms for src_bytes_by_priority passed 1', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 1', @dtn_namespace_id, @p_lit_meta_1_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_1_id, 'literal value 1', 'UINT', '1', @r_lit_1_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_1_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_src_bytes_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_src_bytes_by_priority_aid_1_1);

CALL SP__insert_actual_parmspec(@bp_agent_edd_src_bytes_by_priority_fp, 1, 'actual parms for src_bytes_by_priority passed 2', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 2', @dtn_namespace_id, @p_lit_meta_2_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_2_id, 'literal value 2', 'UINT', '2', @r_lit_2_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_2_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_src_bytes_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_src_bytes_by_priority_aid_1_2);

CALL SP__insert_actual_parmspec(@bp_agent_edd_src_bytes_by_priority_fp, 1, 'actual parms for src_bytes_by_priority passed 4', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 4', @dtn_namespace_id, @p_lit_meta_4_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_4_id, 'literal value 4', 'UINT', '4', @r_lit_4_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_4_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_src_bytes_by_priority_did, NULL, @ap_spec_id, @bp_agent_edd_src_bytes_by_priority_aid_1_4);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 1', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 1', @dtn_namespace_id, @p_lit_meta_1_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_1_id, 'literal value 1', 'UINT', '1', @r_lit_1_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_1_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_1);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 2', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 2', @dtn_namespace_id, @p_lit_meta_2_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_2_id, 'literal value 2', 'UINT', '2', @r_lit_2_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_2_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_2);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 4', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 4', @dtn_namespace_id, @p_lit_meta_4_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_4_id, 'literal value 4', 'UINT', '4', @r_lit_4_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_4_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_4);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 8', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 8', @dtn_namespace_id, @p_lit_meta_8_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_8_id, 'literal value 8', 'UINT', '8', @r_lit_8_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_8_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_8);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 16', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 16', @dtn_namespace_id, @p_lit_meta_16_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_16_id, 'literal value 16', 'UINT', '16', @r_lit_16_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_16_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_16);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 32', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 32', @dtn_namespace_id, @p_lit_meta_32_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_32_id, 'literal value 32', 'UINT', '32', @r_lit_32_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_32_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_32);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 64', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 64', @dtn_namespace_id, @p_lit_meta_64_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_64_id, 'literal value 64', 'UINT', '64', @r_lit_64_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_64_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_64);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 128', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 128', @dtn_namespace_id, @p_lit_meta_128_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_128_id, 'literal value 128', 'UINT', '128', @r_lit_128_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_128_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_128);

CALL SP__insert_actual_parmspec(@bp_agent_edd_num_failed_by_reason_fp, 1, 'actual parms for num_failed_by_reason passed 256', @ap_spec_id);
CALL SP__insert_obj_metadata(3, 'Literal value 256', @dtn_namespace_id, @p_lit_meta_256_id);
CALL SP__insert_literal_actual_definition(@p_lit_meta_256_id, 'literal value 256', 'UINT', '256', @r_lit_256_id);
CALL SP__insert_actual_parms_object(@ap_spec_id, 1, 'UINT', @r_lit_256_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_num_failed_by_reason_did, NULL, @ap_spec_id, @bp_agent_edd_num_failed_by_reason_aid_1_256);

CALL SP__insert_ac_id(43, 'ac for report template full_report', @rptt_ac_id);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_meta_name_did, 1, @r_ac_rpt_entry_1);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_meta_version_did, 2, @r_ac_rpt_entry_2);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bp_node_id_did, 3, @r_ac_rpt_entry_3);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bp_node_version_did, 4, @r_ac_rpt_entry_4);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_available_storage_did, 5, @r_ac_rpt_entry_5);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_last_reset_time_did, 6, @r_ac_rpt_entry_6);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_registrations_did, 7, @r_ac_rpt_entry_7);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_pend_fwd_did, 8, @r_ac_rpt_entry_8);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_pend_dis_did, 9, @r_ac_rpt_entry_9);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_in_cust_did, 10, @r_ac_rpt_entry_10);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_pend_reassembly_did, 11, @r_ac_rpt_entry_11);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bundles_by_priority_did, 12, @r_ac_rpt_entry_12);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bundles_by_priority_did, 13, @r_ac_rpt_entry_13);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bundles_by_priority_did, 14, @r_ac_rpt_entry_14);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bytes_by_priority_did, 15, @r_ac_rpt_entry_15);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bytes_by_priority_did, 16, @r_ac_rpt_entry_16);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_bytes_by_priority_did, 17, @r_ac_rpt_entry_17);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_src_bundles_by_priority_did, 18, @r_ac_rpt_entry_18);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_src_bundles_by_priority_did, 19, @r_ac_rpt_entry_19);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_src_bundles_by_priority_did, 20, @r_ac_rpt_entry_20);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_src_bytes_by_priority_did, 21, @r_ac_rpt_entry_21);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_src_bytes_by_priority_did, 22, @r_ac_rpt_entry_22);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_src_bytes_by_priority_did, 23, @r_ac_rpt_entry_23);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_fragmented_bundles_did, 24, @r_ac_rpt_entry_24);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_fragments_produced_did, 25, @r_ac_rpt_entry_25);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 26, @r_ac_rpt_entry_26);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 27, @r_ac_rpt_entry_27);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 28, @r_ac_rpt_entry_28);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 29, @r_ac_rpt_entry_29);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 30, @r_ac_rpt_entry_30);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 31, @r_ac_rpt_entry_31);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 32, @r_ac_rpt_entry_32);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 33, @r_ac_rpt_entry_33);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_failed_by_reason_did, 34, @r_ac_rpt_entry_34);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_num_bundles_deleted_did, 35, @r_ac_rpt_entry_35);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_failed_custody_bundles_did, 36, @r_ac_rpt_entry_36);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_failed_custody_bytes_did, 37, @r_ac_rpt_entry_37);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_failed_forward_bundles_did, 38, @r_ac_rpt_entry_38);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_failed_forward_bytes_did, 39, @r_ac_rpt_entry_39);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_abandoned_bundles_did, 40, @r_ac_rpt_entry_40);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_discarded_bundles_did, 41, @r_ac_rpt_entry_41);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_discarded_bytes_did, 42, @r_ac_rpt_entry_42);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_endpoint_names_did, 43, @r_ac_rpt_entry_43);

CALL SP__insert_report_template_formal_definition(@bp_agent_rpttpl_full_report, 'This is all known meta-data, EDD, and VAR values known by the agent.', null, @rptt_ac_id, @bp_agent_rpttpl_full_report_did);
CALL SP__insert_report_actual_definition(@bp_agent_rpttpl_full_report, null, null, 'Singleton value for full_report', @bp_agent_rpttpl_full_report_aid);

CALL SP__insert_obj_metadata(7, 'endpoint_report', @dtn_namespace_id, @bp_agent_rpttpl_endpoint_report);
CALL SP__insert_formal_parmspec(1, 'parms for endpoint_active', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'endpoint_active', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@bp_agent_edd_endpoint_active_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_endpoint_active_did, NULL, @ap_spec_id, @bp_agent_edd_endpoint_active_aid_endpoint_id_1);
CALL SP__insert_formal_parmspec(1, 'parms for endpoint_singleton', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'endpoint_singleton', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@bp_agent_edd_endpoint_singleton_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_endpoint_singleton_did, NULL, @ap_spec_id, @bp_agent_edd_endpoint_singleton_aid_endpoint_id_1);
CALL SP__insert_formal_parmspec(1, 'parms for endpoint_policy', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'endpoint_policy', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@bp_agent_edd_endpoint_policy_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@bp_agent_edd_endpoint_policy_did, NULL, @ap_spec_id, @bp_agent_edd_endpoint_policy_aid_endpoint_id_1);

CALL SP__insert_ac_id(3, 'ac for report template endpoint_report', @rptt_ac_id);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_endpoint_active_did, 1, @r_ac_rpt_entry_1);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_endpoint_singleton_did, 2, @r_ac_rpt_entry_2);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @bp_agent_edd_endpoint_policy_did, 3, @r_ac_rpt_entry_3);

CALL SP__insert_report_template_formal_definition(@bp_agent_rpttpl_endpoint_report, 'This is all known endpoint information', @fp_spec_id, @rptt_ac_id, @bp_agent_rpttpl_endpoint_report_did);
-- CALL SP__insert_report_actual_definition(@bp_agent_rpttpl_endpoint_report, null, null, 'Singleton value for endpoint_report', @bp_agent_rpttpl_endpoint_report_aid);

-- #CTRL
CALL SP__insert_obj_metadata(1, 'reset_all_counts', @dtn_namespace_id, @bp_agent_ctrl_reset_all_counts);
CALL SP__insert_control_formal_definition(@bp_agent_ctrl_reset_all_counts , 'This control causes the Agent to reset all counts associated with bundle or byte statistics and to set the last reset time of the BP primitive data to the time when the control was run', null, @bp_agent_ctrl_reset_all_counts_did);


-- #CONST

-- #MAC
