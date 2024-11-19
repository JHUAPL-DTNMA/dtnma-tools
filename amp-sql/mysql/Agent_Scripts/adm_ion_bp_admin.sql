-- -------------------------------------------------------------------
--
-- File Name: adm_ion_bp_admin.sql
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
# ADM: 'DTN/ION/bpadmin'
use amp_core;


SET @adm_enum = 5;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'DTN/ION/bpadmin', 'v0.0', 'ion_bp_admin', @adm_enum, NULL, 'The namespace of the ADM', @dtn_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @dtn_namespace_id, @ion_bpadmin_meta_name);
CALL SP__insert_const_actual_definition(@ion_bpadmin_meta_name, 'The human-readable name of the ADM.', 'STR', 'ion_bp_admin', @ion_bpadmin_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @dtn_namespace_id, @ion_bpadmin_meta_namespace);
CALL SP__insert_const_actual_definition(@ion_bpadmin_meta_namespace, 'The namespace of the ADM', 'STR', 'DTN/ION/bpadmin', @ion_bpadmin_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @dtn_namespace_id, @ion_bpadmin_meta_version);
CALL SP__insert_const_actual_definition(@ion_bpadmin_meta_version, 'The version of the ADM', 'STR', 'v0.0', @ion_bpadmin_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @dtn_namespace_id, @ion_bpadmin_meta_organization);
CALL SP__insert_const_actual_definition(@ion_bpadmin_meta_organization, 'The name of the issuing organization of the ADM', 'STR', 'JHUAPL', @ion_bpadmin_meta_organization_did);

-- #EDD
CALL SP__insert_obj_metadata(2, 'bp_version', @dtn_namespace_id, @ion_bpadmin_edd_bp_version);
CALL SP__insert_edd_formal_definition(@ion_bpadmin_edd_bp_version, 'Version of installed ION BP Admin utility.', NULL, 'STR', @ion_bpadmin_edd_bp_version_did);
CALL SP__insert_edd_actual_definition(@ion_bpadmin_edd_bp_version, 'The singleton value for bp_version', NULL, @ion_bpadmin_edd_bp_version_aid);

-- #OPER

-- #VAR

-- #TBLT
CALL SP__insert_obj_metadata(10, 'endpoints', @dtn_namespace_id, @ion_bpadmin_tblt_endpoints);
CALL SP__insert_tnvc_collection('columns for the endpoints table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'scheme_name', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 2, 'endpoint_nss', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'app_pid', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 4, 'recv_rule', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 5, 'rcv_script', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_bpadmin_tblt_endpoints, 'Local endpoints, regardless of scheme name.', @tbl_tnvc_id, @ion_bpadmin_tblt_endpoints_did);

CALL SP__insert_obj_metadata(10, 'inducts', @dtn_namespace_id, @ion_bpadmin_tblt_inducts);
CALL SP__insert_tnvc_collection('columns for the inducts table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'protocol_name', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 2, 'duct_name', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 3, 'cli_control', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_bpadmin_tblt_inducts, 'Inducts established locally for the indicated CL protocol.', @tbl_tnvc_id, @ion_bpadmin_tblt_inducts_did);

CALL SP__insert_obj_metadata(10, 'outducts', @dtn_namespace_id, @ion_bpadmin_tblt_outducts);
CALL SP__insert_tnvc_collection('columns for the outducts table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'protocol_name', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 2, 'duct_name', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'clo_pid', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 4, 'clo_control', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 5, 'max_payload_length', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_bpadmin_tblt_outducts, 'If protocolName is specified, this table lists all outducts established locally for the indicated CL protocol. Otherwise, it lists all locally established outducts, regardless of their protocol.', @tbl_tnvc_id, @ion_bpadmin_tblt_outducts_did);

CALL SP__insert_obj_metadata(10, 'protocols', @dtn_namespace_id, @ion_bpadmin_tblt_protocols);
CALL SP__insert_tnvc_collection('columns for the protocols table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'name', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 2, 'payload_bpf', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'overhead_bpf', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 4, 'protocol class', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_bpadmin_tblt_protocols, 'Convergence layer protocols that can currently be utilized at the local node.', @tbl_tnvc_id, @ion_bpadmin_tblt_protocols_did);

CALL SP__insert_obj_metadata(10, 'schemes', @dtn_namespace_id, @ion_bpadmin_tblt_schemes);
CALL SP__insert_tnvc_collection('columns for the schemes table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'scheme_name', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 2, 'fwd_pid', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 3, 'fwd_cmd', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 4, 'admin_app_pid', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 5, 'admin_app_cmd', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_bpadmin_tblt_schemes, 'Declared endpoint naming schemes.', @tbl_tnvc_id, @ion_bpadmin_tblt_schemes_did);

CALL SP__insert_obj_metadata(10, 'egress_plans', @dtn_namespace_id, @ion_bpadmin_tblt_egress_plans);
CALL SP__insert_tnvc_collection('columns for the egress_plans table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'neighbor_eid', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 2, 'clm_pid', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'nominal_rate', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_bpadmin_tblt_egress_plans, 'Egress plans.', @tbl_tnvc_id, @ion_bpadmin_tblt_egress_plans_did);


-- #RPTT

-- #CTRL
CALL SP__insert_obj_metadata(1, 'endpoint_add', @dtn_namespace_id, @ion_bpadmin_ctrl_endpoint_add);
CALL SP__insert_formal_parmspec(3, 'parms for the endpoint_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'endpoint_id', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'type', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'rcv_script', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_endpoint_add , 'Establish DTN endpoint named endpointId on the local node. The remaining parameters indicate what is to be done when bundles destined for this endpoint arrive at a time when no application has the endpoint open for bundle reception. If type is ''x'', then such bundles are to be discarded silently and immediately. If type is ''q'', then such bundles are to be enqueued for later delivery and, if recvScript is provided, recvScript is to be executed.', @fp_spec_id, @ion_bpadmin_ctrl_endpoint_add_did);

CALL SP__insert_obj_metadata(1, 'endpoint_change', @dtn_namespace_id, @ion_bpadmin_ctrl_endpoint_change);
CALL SP__insert_formal_parmspec(3, 'parms for the endpoint_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'endpoint_id', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'type', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'rcv_script', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_endpoint_change , 'Change the action taken when bundles destined for this endpoint arrive at a time when no application has the endpoint open for bundle reception.', @fp_spec_id, @ion_bpadmin_ctrl_endpoint_change_did);

CALL SP__insert_obj_metadata(1, 'endpoint_del', @dtn_namespace_id, @ion_bpadmin_ctrl_endpoint_del);
CALL SP__insert_formal_parmspec(1, 'parms for the endpoint_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'endpoint_id', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_endpoint_del , 'Delete the endpoint identified by endpointId. The control will fail if any bundles are currently pending delivery to this endpoint.', @fp_spec_id, @ion_bpadmin_ctrl_endpoint_del_did);

CALL SP__insert_obj_metadata(1, 'induct_add', @dtn_namespace_id, @ion_bpadmin_ctrl_induct_add);
CALL SP__insert_formal_parmspec(3, 'parms for the induct_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'cli_control', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_induct_add , 'Establish a duct for reception of bundles via the indicated CL protocol. The duct''s data acquisition structure is used and populated by the induct task whose operation is initiated by cliControl at the time the duct is started.', @fp_spec_id, @ion_bpadmin_ctrl_induct_add_did);

CALL SP__insert_obj_metadata(1, 'induct_change', @dtn_namespace_id, @ion_bpadmin_ctrl_induct_change);
CALL SP__insert_formal_parmspec(3, 'parms for the induct_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'cli_control', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_induct_change , 'Change the control used to initiate operation of the induct task for the indicated duct.', @fp_spec_id, @ion_bpadmin_ctrl_induct_change_did);

CALL SP__insert_obj_metadata(1, 'induct_del', @dtn_namespace_id, @ion_bpadmin_ctrl_induct_del);
CALL SP__insert_formal_parmspec(2, 'parms for the induct_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_induct_del , 'Delete the induct identified by protocolName and ductName. The control will fail if any bundles are currently pending acquisition via this induct.', @fp_spec_id, @ion_bpadmin_ctrl_induct_del_did);

CALL SP__insert_obj_metadata(1, 'induct_start', @dtn_namespace_id, @ion_bpadmin_ctrl_induct_start);
CALL SP__insert_formal_parmspec(2, 'parms for the induct_start control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_induct_start , 'Start the indicated induct task as defined for the indicated CL protocol on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_induct_start_did);

CALL SP__insert_obj_metadata(1, 'induct_stop', @dtn_namespace_id, @ion_bpadmin_ctrl_induct_stop);
CALL SP__insert_formal_parmspec(2, 'parms for the induct_stop control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_induct_stop , 'Stop the indicated induct task as defined for the indicated CL protocol on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_induct_stop_did);

CALL SP__insert_obj_metadata(1, 'manage_heap_max', @dtn_namespace_id, @ion_bpadmin_ctrl_manage_heap_max);
CALL SP__insert_formal_parmspec(1, 'parms for the manage_heap_max control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'max_database_heap_per_acquisition', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_manage_heap_max , 'Declare the maximum number of bytes of SDR heap space that will be occupied by any single bundle acquisition activity (nominally the acquisition of a single bundle, but this is at the discretion of the convergence-layer input task). All data acquired in excess of this limit will be written to a temporary file pending extraction and dispatching of the acquired bundle or bundles. The default is the minimum allowed value (560 bytes), which is the approximate size of a ZCO file reference object; this is the minimum SDR heap space occupancy in the event that all acquisition is into a file.', @fp_spec_id, @ion_bpadmin_ctrl_manage_heap_max_did);

CALL SP__insert_obj_metadata(1, 'outduct_add', @dtn_namespace_id, @ion_bpadmin_ctrl_outduct_add);
CALL SP__insert_formal_parmspec(4, 'parms for the outduct_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'clo_command', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'max_payload_length', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_outduct_add , 'Establish a duct for transmission of bundles via the indicated CL protocol. the duct''s data transmission structure is serviced by the outduct task whose operation is initiated by CLOcommand at the time the duct is started. A value of zero for maxPayloadLength indicates that bundles of any size can be accomodated; this is the default.', @fp_spec_id, @ion_bpadmin_ctrl_outduct_add_did);

CALL SP__insert_obj_metadata(1, 'outduct_change', @dtn_namespace_id, @ion_bpadmin_ctrl_outduct_change);
CALL SP__insert_formal_parmspec(4, 'parms for the outduct_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'clo_control', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'max_payload_length', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_outduct_change , 'Set new values for the indicated duct''s payload size limit and the control that is used to initiate operation of the outduct task for this duct.', @fp_spec_id, @ion_bpadmin_ctrl_outduct_change_did);

CALL SP__insert_obj_metadata(1, 'outduct_del', @dtn_namespace_id, @ion_bpadmin_ctrl_outduct_del);
CALL SP__insert_formal_parmspec(2, 'parms for the outduct_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_outduct_del , 'Delete the outduct identified by protocolName and ductName. The control will fail if any bundles are currently pending transmission via this outduct.', @fp_spec_id, @ion_bpadmin_ctrl_outduct_del_did);

CALL SP__insert_obj_metadata(1, 'outduct_start', @dtn_namespace_id, @ion_bpadmin_ctrl_outduct_start);
CALL SP__insert_formal_parmspec(2, 'parms for the outduct_start control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_outduct_start , 'Start the indicated outduct task as defined for the indicated CL protocol on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_outduct_start_did);

CALL SP__insert_obj_metadata(1, 'egress_plan_block', @dtn_namespace_id, @ion_bpadmin_ctrl_egress_plan_block);
CALL SP__insert_formal_parmspec(1, 'parms for the egress_plan_block control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'plan_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_egress_plan_block , 'Disable transmission of bundles queued for transmission to the indicated node and reforwards all non-critical bundles currently queued for transmission to this node. This may result in some or all of these bundles being enqueued for transmission to the psuedo-node limbo.', @fp_spec_id, @ion_bpadmin_ctrl_egress_plan_block_did);

CALL SP__insert_obj_metadata(1, 'egress_plan_unblock', @dtn_namespace_id, @ion_bpadmin_ctrl_egress_plan_unblock);
CALL SP__insert_formal_parmspec(1, 'parms for the egress_plan_unblock control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'plan_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_egress_plan_unblock , 'Re-enable transmission of bundles to the indicated node and reforwards all bundles in limbo in the hope that the unblocking of this egress plan will enable some of them to be transmitted.', @fp_spec_id, @ion_bpadmin_ctrl_egress_plan_unblock_did);

CALL SP__insert_obj_metadata(1, 'outduct_stop', @dtn_namespace_id, @ion_bpadmin_ctrl_outduct_stop);
CALL SP__insert_formal_parmspec(2, 'parms for the outduct_stop control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'duct_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_outduct_stop , 'Stop the indicated outduct task as defined for the indicated CL protocol on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_outduct_stop_did);

CALL SP__insert_obj_metadata(1, 'protocol_add', @dtn_namespace_id, @ion_bpadmin_ctrl_protocol_add);
CALL SP__insert_formal_parmspec(4, 'parms for the protocol_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'payload_bytes_per_frame', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'overhead_bytes_per_frame', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'nominal_data_rate', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_protocol_add , 'Establish access to the named convergence layer protocol at the local node. The payloadBytesPerFrame and overheadBytesPerFrame arguments are used in calculating the estimated transmission capacity consumption of each bundle, to aid in route computation and congesting forecasting. The optional nominalDataRate argument overrides the hard coded default continuous data rate for the indicated protocol for purposes of rate control. For all promiscuous prototocols-that is, protocols whose outducts are not specifically dedicated to transmission to a single identified convergence-layer protocol endpoint- the protocol''s applicable nominal continuous data rate is the data rate that is always used for rate control over links served by that protocol; data rates are not extracted from contact graph information. This is because only the induct and outduct throttles for non-promiscuous protocols (LTP, TCP) can be dynamically adjusted in response to changes in data rate between the local node and its neighbors, as enacted per the contact plan. Even for an outduct of a non-promiscuous protocol the nominal data rate may be the authority for rate control, in the event that the contact plan lacks identified contacts with the node to which the outduct is mapped.', @fp_spec_id, @ion_bpadmin_ctrl_protocol_add_did);

CALL SP__insert_obj_metadata(1, 'protocol_del', @dtn_namespace_id, @ion_bpadmin_ctrl_protocol_del);
CALL SP__insert_formal_parmspec(1, 'parms for the protocol_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_protocol_del , 'Delete the convergence layer protocol identified by protocolName. The control will fail if any ducts are still locally declared for this protocol.', @fp_spec_id, @ion_bpadmin_ctrl_protocol_del_did);

CALL SP__insert_obj_metadata(1, 'protocol_start', @dtn_namespace_id, @ion_bpadmin_ctrl_protocol_start);
CALL SP__insert_formal_parmspec(1, 'parms for the protocol_start control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_protocol_start , 'Start all induct and outduct tasks for inducts and outducts that have been defined for the indicated CL protocol on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_protocol_start_did);

CALL SP__insert_obj_metadata(1, 'protocol_stop', @dtn_namespace_id, @ion_bpadmin_ctrl_protocol_stop);
CALL SP__insert_formal_parmspec(1, 'parms for the protocol_stop control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'protocol_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_protocol_stop , 'Stop all induct and outduct tasks for inducts and outducts that have been defined for the indicated CL protocol on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_protocol_stop_did);

CALL SP__insert_obj_metadata(1, 'scheme_add', @dtn_namespace_id, @ion_bpadmin_ctrl_scheme_add);
CALL SP__insert_formal_parmspec(3, 'parms for the scheme_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'scheme_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'forwarder_control', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'admin_app_control', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_scheme_add , 'Declares an endpoint naming scheme for use in endpoint IDs, which are structured as URIs: schemeName:schemeSpecificPart. forwarderControl will be executed when the scheme is started on this node, to initiate operation of a forwarding daemon for this scheme. adminAppControl will also be executed when the scheme is started on this node, to initiate operation of a daemon that opens a custodian endpoint identified within this scheme so that it can recieve and process custody signals and bundle status reports.', @fp_spec_id, @ion_bpadmin_ctrl_scheme_add_did);

CALL SP__insert_obj_metadata(1, 'scheme_change', @dtn_namespace_id, @ion_bpadmin_ctrl_scheme_change);
CALL SP__insert_formal_parmspec(3, 'parms for the scheme_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'scheme_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'forwarder_control', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'admin_app_control', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_scheme_change , 'Set the indicated scheme''s forwarderControl and adminAppControl to the strings provided as arguments.', @fp_spec_id, @ion_bpadmin_ctrl_scheme_change_did);

CALL SP__insert_obj_metadata(1, 'scheme_del', @dtn_namespace_id, @ion_bpadmin_ctrl_scheme_del);
CALL SP__insert_formal_parmspec(1, 'parms for the scheme_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'scheme_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_scheme_del , 'Delete the scheme identified by schemeName. The control will fail if any bundles identified in this scheme are pending forwarding, transmission, or delivery.', @fp_spec_id, @ion_bpadmin_ctrl_scheme_del_did);

CALL SP__insert_obj_metadata(1, 'scheme_start', @dtn_namespace_id, @ion_bpadmin_ctrl_scheme_start);
CALL SP__insert_formal_parmspec(1, 'parms for the scheme_start control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'scheme_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_scheme_start , 'Start the forwarder and administrative endpoint tasks for the indicated scheme task on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_scheme_start_did);

CALL SP__insert_obj_metadata(1, 'scheme_stop', @dtn_namespace_id, @ion_bpadmin_ctrl_scheme_stop);
CALL SP__insert_formal_parmspec(1, 'parms for the scheme_stop control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'scheme_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_scheme_stop , 'Stop the forwarder and administrative endpoint tasks for the indicated scheme task on the local node.', @fp_spec_id, @ion_bpadmin_ctrl_scheme_stop_did);

CALL SP__insert_obj_metadata(1, 'watch', @dtn_namespace_id, @ion_bpadmin_ctrl_watch);
CALL SP__insert_formal_parmspec(2, 'parms for the watch control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'status', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'activity_spec', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_bpadmin_ctrl_watch , 'Enable/Disable production of a continuous stream of user selected Bundle Protocol activity indication characters. A watch parameter of 1 selects all BP activity indication characters, 0 deselects allBP activity indication characters; any other activitySpec such as acz~ selects all activity indication characters in the string, deselecting all others. BP will print each selected activity indication character to stdout every time a processing event of the associated type occurs: a new bundle is queued for forwarding, b bundle is queued for transmission, c bundle is popped from its transmission queue, m custody acceptance signal is recieved, w custody of bundle is accepted, x custody of bundle is refused, y bundle is accepted upon arrival, z bundle is queued for delivery to an application, ~ bundle is abandoned (discarded) on attempt to forward it, ! bundle is destroyed due to TTL expiration, &amp; custody refusal signal is recieved, # bundle is queued for re-forwarding due to CL protocol failures, j bundle is placed in ''limbo'' for possible future reforwarding, k bundle is removed from ''limbo'' and queued for reforwarding, $ bundle''s custodial retransmission timeout interval expired.', @fp_spec_id, @ion_bpadmin_ctrl_watch_did);


-- #CONST

-- #MAC
