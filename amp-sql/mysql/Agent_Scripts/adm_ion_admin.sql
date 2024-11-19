-- -------------------------------------------------------------------
--
-- File Name: adm_ion_admin.sql
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
# ADM: 'DTN/ION/ionadmin'
use amp_core;


SET @adm_enum = 7;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'DTN/ION/ionadmin', 'v0.0', 'ion_admin', @adm_enum, NULL, 'The namespace of the ADM.', @dtn_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @dtn_namespace_id, @ion_ionadmin_meta_name);
CALL SP__insert_const_actual_definition(@ion_ionadmin_meta_name, 'The human-readable name of the ADM.', 'STR', 'ion_admin', @ion_ionadmin_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @dtn_namespace_id, @ion_ionadmin_meta_namespace);
CALL SP__insert_const_actual_definition(@ion_ionadmin_meta_namespace, 'The namespace of the ADM.', 'STR', 'DTN/ION/ionadmin', @ion_ionadmin_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @dtn_namespace_id, @ion_ionadmin_meta_version);
CALL SP__insert_const_actual_definition(@ion_ionadmin_meta_version, 'The version of the ADM.', 'STR', 'v0.0', @ion_ionadmin_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @dtn_namespace_id, @ion_ionadmin_meta_organization);
CALL SP__insert_const_actual_definition(@ion_ionadmin_meta_organization, 'The name of the issuing organization of the ADM.', 'STR', 'JHUAPL', @ion_ionadmin_meta_organization_did);

-- #EDD
CALL SP__insert_obj_metadata(2, 'clock_error', @dtn_namespace_id, @ion_ionadmin_edd_clock_error);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_clock_error, 'This is how accurate the ION Agent''s clock is described as number of seconds, an absolute value.', NULL, 'UINT', @ion_ionadmin_edd_clock_error_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_clock_error, 'The singleton value for clock_error', NULL, @ion_ionadmin_edd_clock_error_aid);

CALL SP__insert_obj_metadata(2, 'clock_sync', @dtn_namespace_id, @ion_ionadmin_edd_clock_sync);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_clock_sync, 'This is whether or not the the computer on which the local ION node is running has a synchronized clock.', NULL, 'UINT', @ion_ionadmin_edd_clock_sync_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_clock_sync, 'The singleton value for clock_sync', NULL, @ion_ionadmin_edd_clock_sync_aid);

CALL SP__insert_obj_metadata(2, 'congestion_alarm_control', @dtn_namespace_id, @ion_ionadmin_edd_congestion_alarm_control);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_congestion_alarm_control, 'This is whether or not the node has a control that will set off alarm if it will become congested at some future time.', NULL, 'UINT', @ion_ionadmin_edd_congestion_alarm_control_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_congestion_alarm_control, 'The singleton value for congestion_alarm_control', NULL, @ion_ionadmin_edd_congestion_alarm_control_aid);

CALL SP__insert_obj_metadata(2, 'congestion_end_time_forecasts', @dtn_namespace_id, @ion_ionadmin_edd_congestion_end_time_forecasts);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_congestion_end_time_forecasts, 'This is the time horizon beyond which we don''t attempt to forecast congestion', NULL, 'UINT', @ion_ionadmin_edd_congestion_end_time_forecasts_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_congestion_end_time_forecasts, 'The singleton value for congestion_end_time_forecasts', NULL, @ion_ionadmin_edd_congestion_end_time_forecasts_aid);

CALL SP__insert_obj_metadata(2, 'consumption_rate', @dtn_namespace_id, @ion_ionadmin_edd_consumption_rate);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_consumption_rate, 'This is the mean rate of continuous data delivery to local BP applications.', NULL, 'UINT', @ion_ionadmin_edd_consumption_rate_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_consumption_rate, 'The singleton value for consumption_rate', NULL, @ion_ionadmin_edd_consumption_rate_aid);

CALL SP__insert_obj_metadata(2, 'inbound_file_system_occupancy_limit', @dtn_namespace_id, @ion_ionadmin_edd_inbound_file_system_occupancy_limit);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_inbound_file_system_occupancy_limit, 'This is the maximum number of megabytes of storage space in ION''s local file system that can be used for the storage of inbound zero-copy objects. The default heap limit is 1 Terabyte.', NULL, 'UINT', @ion_ionadmin_edd_inbound_file_system_occupancy_limit_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_inbound_file_system_occupancy_limit, 'The singleton value for inbound_file_system_occupancy_limit', NULL, @ion_ionadmin_edd_inbound_file_system_occupancy_limit_aid);

CALL SP__insert_obj_metadata(2, 'inbound_heap_occupancy_limit', @dtn_namespace_id, @ion_ionadmin_edd_inbound_heap_occupancy_limit);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_inbound_heap_occupancy_limit, 'This is the maximum number of megabytes of storage space in ION''s SDR non-volatile heap that can be used for the storage of inbound zero-copy objects. The default heap limit is 20% of the SDR data space''s total heap size.', NULL, 'UINT', @ion_ionadmin_edd_inbound_heap_occupancy_limit_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_inbound_heap_occupancy_limit, 'The singleton value for inbound_heap_occupancy_limit', NULL, @ion_ionadmin_edd_inbound_heap_occupancy_limit_aid);

CALL SP__insert_obj_metadata(2, 'number', @dtn_namespace_id, @ion_ionadmin_edd_number);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_number, 'This is a CBHE node number which uniquely identifies the node in the delay-tolerant network.', NULL, 'UINT', @ion_ionadmin_edd_number_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_number, 'The singleton value for number', NULL, @ion_ionadmin_edd_number_aid);

CALL SP__insert_obj_metadata(2, 'outbound_file_system_occupancy_limit', @dtn_namespace_id, @ion_ionadmin_edd_outbound_file_system_occupancy_limit);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_outbound_file_system_occupancy_limit, 'This is the maximum number of megabytes of storage space in ION''s local file system that can be used for the storage of outbound zero-copy objects. The default heap limit is 1 Terabyte.', NULL, 'UINT', @ion_ionadmin_edd_outbound_file_system_occupancy_limit_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_outbound_file_system_occupancy_limit, 'The singleton value for outbound_file_system_occupancy_limit', NULL, @ion_ionadmin_edd_outbound_file_system_occupancy_limit_aid);

CALL SP__insert_obj_metadata(2, 'outbound_heap_occupancy_limit', @dtn_namespace_id, @ion_ionadmin_edd_outbound_heap_occupancy_limit);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_outbound_heap_occupancy_limit, 'This is the maximum number of megabytes of storage space in ION''s SDR non-volatile heap that can be used for the storage of outbound zero-copy objects. The default heap limit is 20% of the SDR data space''s total heap size.', NULL, 'UINT', @ion_ionadmin_edd_outbound_heap_occupancy_limit_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_outbound_heap_occupancy_limit, 'The singleton value for outbound_heap_occupancy_limit', NULL, @ion_ionadmin_edd_outbound_heap_occupancy_limit_aid);

CALL SP__insert_obj_metadata(2, 'production_rate', @dtn_namespace_id, @ion_ionadmin_edd_production_rate);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_production_rate, 'This is the rate of local data production.', NULL, 'UINT', @ion_ionadmin_edd_production_rate_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_production_rate, 'The singleton value for production_rate', NULL, @ion_ionadmin_edd_production_rate_aid);

CALL SP__insert_obj_metadata(2, 'ref_time', @dtn_namespace_id, @ion_ionadmin_edd_ref_time);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_ref_time, 'This is the reference time that will be used for interpreting relative time values from now until the next revision of reference time.', NULL, 'TV', @ion_ionadmin_edd_ref_time_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_ref_time, 'The singleton value for ref_time', NULL, @ion_ionadmin_edd_ref_time_aid);

CALL SP__insert_obj_metadata(2, 'time_delta', @dtn_namespace_id, @ion_ionadmin_edd_time_delta);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_time_delta, 'The time delta is used to compensate for error (drift) in clocks, particularly spacecraft clocks. The hardware clock on a spacecraft might gain or lose a few seconds every month, to the point at which its understanding of the current time - as reported out by the operating system - might differ significantly from the actual value of Unix Epoch time as reported by authoritative clocks on Earth. To compensate for this difference without correcting the clock itself (which can be difficult and dangerous), ION simply adds the time delta to the Epoch time reported by the operating system.', NULL, 'UINT', @ion_ionadmin_edd_time_delta_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_time_delta, 'The singleton value for time_delta', NULL, @ion_ionadmin_edd_time_delta_aid);

CALL SP__insert_obj_metadata(2, 'version', @dtn_namespace_id, @ion_ionadmin_edd_version);
CALL SP__insert_edd_formal_definition(@ion_ionadmin_edd_version, 'This is the version of ION that is currently installed.', NULL, 'STR', @ion_ionadmin_edd_version_did);
CALL SP__insert_edd_actual_definition(@ion_ionadmin_edd_version, 'The singleton value for version', NULL, @ion_ionadmin_edd_version_aid);

-- #OPER

-- #VAR

-- #TBLT
CALL SP__insert_obj_metadata(10, 'contacts', @dtn_namespace_id, @ion_ionadmin_tblt_contacts);
CALL SP__insert_tnvc_collection('columns for the contacts table', @tbl_tnvc_id);
CALL SP__insert_tnvc_tv_entry(@tbl_tnvc_id, 1, 'start_time', null, @tnvc_entry);
CALL SP__insert_tnvc_tv_entry(@tbl_tnvc_id, 2, 'stop_time', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'source_node', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 4, 'dest_node', null, @tnvc_entry);
CALL SP__insert_tnvc_uvast_entry(@tbl_tnvc_id, 5, 'xmit_data', null, @tnvc_entry);
CALL SP__insert_tnvc_uvast_entry(@tbl_tnvc_id, 6, 'confidence', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_ionadmin_tblt_contacts, 'This table shows all scheduled periods of data transmission.', @tbl_tnvc_id, @ion_ionadmin_tblt_contacts_did);

CALL SP__insert_obj_metadata(10, 'ranges', @dtn_namespace_id, @ion_ionadmin_tblt_ranges);
CALL SP__insert_tnvc_collection('columns for the ranges table', @tbl_tnvc_id);
CALL SP__insert_tnvc_tv_entry(@tbl_tnvc_id, 1, 'start', null, @tnvc_entry);
CALL SP__insert_tnvc_tv_entry(@tbl_tnvc_id, 2, 'stop', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'node', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 4, 'other_node', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 5, 'distance', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_ionadmin_tblt_ranges, 'This table shows all predicted periods of constant distance between nodes.', @tbl_tnvc_id, @ion_ionadmin_tblt_ranges_did);


-- #RPTT

-- #CTRL
CALL SP__insert_obj_metadata(1, 'node_init', @dtn_namespace_id, @ion_ionadmin_ctrl_node_init);
CALL SP__insert_formal_parmspec(2, 'parms for the node_init control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'node_nbr', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'config_file', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_init , 'Until this control is executed, the local ION node does not exist and most ionadmin controls will fail. The control configures the local node to be identified by node_number, a CBHE node number which uniquely identifies the node in the delay-tolerant network.  It also configures ION''s data space (SDR) and shared working-memory region.  For this purpose it uses a set of default settings if no argument follows node_number or if the argument following node_number is ''''; otherwise it uses the configuration settings found in a configuration file.  If configuration file name is provided, then the configuration file''s name is implicitly ''hostname.ionconfig''; otherwise, ion_config_filename is taken to be the explicit configuration file name.', @fp_spec_id, @ion_ionadmin_ctrl_node_init_did);

CALL SP__insert_obj_metadata(1, 'node_clock_error_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_clock_error_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_clock_error_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'known_maximum_clock_error', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_clock_error_set , 'This management control sets ION''s understanding of the accuracy of the scheduled start and stop times of planned contacts, in seconds.  The default value is 1.', @fp_spec_id, @ion_ionadmin_ctrl_node_clock_error_set_did);

CALL SP__insert_obj_metadata(1, 'node_clock_sync_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_clock_sync_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_clock_sync_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'new_state', 'BOOL', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_clock_sync_set , 'This management control reports whether or not the computer on which the local ION node is running has a synchronized clock.', @fp_spec_id, @ion_ionadmin_ctrl_node_clock_sync_set_did);

CALL SP__insert_obj_metadata(1, 'node_congestion_alarm_control_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_congestion_alarm_control_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_congestion_alarm_control_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'congestion_alarm_control', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_congestion_alarm_control_set , 'This management control establishes a control which will automatically be executed whenever ionadmin predicts that the node will become congested at some future time.', @fp_spec_id, @ion_ionadmin_ctrl_node_congestion_alarm_control_set_did);

CALL SP__insert_obj_metadata(1, 'node_congestion_end_time_forecasts_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_congestion_end_time_forecasts_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_congestion_end_time_forecasts_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'end_time_for_congestion_forcasts', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_congestion_end_time_forecasts_set , 'This management control sets the end time for computed congestion forecasts. Setting congestion forecast horizon to zero sets the congestion forecast end time to infinite time in the future: if there is any predicted net growth in bundle storage space occupancy at all, following the end of the last scheduled contact, then eventual congestion will be predicted. The default value is zero, i.e., no end time.', @fp_spec_id, @ion_ionadmin_ctrl_node_congestion_end_time_forecasts_set_did);

CALL SP__insert_obj_metadata(1, 'node_consumption_rate_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_consumption_rate_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_consumption_rate_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'planned_data_consumption_rate', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_consumption_rate_set , 'This management control sets ION''s expectation of the mean rate of continuous data delivery to local BP applications throughout the period of time over which congestion forecasts are computed. For nodes that function only as routers this variable will normally be zero. A value of -1, which is the default, indicates that the rate of local data consumption is unknown; in that case local data consumption is not considered in the computation of congestion forecasts.', @fp_spec_id, @ion_ionadmin_ctrl_node_consumption_rate_set_did);

CALL SP__insert_obj_metadata(1, 'node_contact_add', @dtn_namespace_id, @ion_ionadmin_ctrl_node_contact_add);
CALL SP__insert_formal_parmspec(6, 'parms for the node_contact_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'start', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'stop', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'from_node_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'to_node_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 5, 'data_rate', 'UVAST', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 6, 'prob', 'UVAST', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_contact_add , 'This control schedules a period of data transmission from source_node to dest_node. The period of transmission will begin at start_time and end at stop_time, and the rate of data transmission will be xmit_data_rate bytes/second. Our confidence in the contact defaults to 1.0, indicating that the contact is scheduled - not that non-occurrence of the contact is impossible, just that occurrence of the contact is planned and scheduled rather than merely imputed from past node behavior. In the latter case, confidence indicates our estimation of the likelihood of this potential contact.', @fp_spec_id, @ion_ionadmin_ctrl_node_contact_add_did);

CALL SP__insert_obj_metadata(1, 'node_contact_del', @dtn_namespace_id, @ion_ionadmin_ctrl_node_contact_del);
CALL SP__insert_formal_parmspec(3, 'parms for the node_contact_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'start', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'node_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'dest', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_contact_del , 'This control deletes the scheduled period of data transmission from source_node to dest_node starting at start_time. To delete all contacts between some pair of nodes, use ''*'' as start_time.', @fp_spec_id, @ion_ionadmin_ctrl_node_contact_del_did);

CALL SP__insert_obj_metadata(1, 'node_inbound_heap_occupancy_limit_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_inbound_heap_occupancy_limit_set);
CALL SP__insert_formal_parmspec(2, 'parms for the node_inbound_heap_occupancy_limit_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'heap_occupancy_limit', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'file_system_occupancy_limit', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_inbound_heap_occupancy_limit_set , 'This management control sets the maximum number of megabytes of storage space in ION''s SDR non-volatile heap that can be used for the storage of inbound zero-copy objects. A value of -1 for either limit signifies ''leave unchanged''. The default heap limit is 30% of the SDR data space''s total heap size.', @fp_spec_id, @ion_ionadmin_ctrl_node_inbound_heap_occupancy_limit_set_did);

CALL SP__insert_obj_metadata(1, 'node_outbound_heap_occupancy_limit_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_outbound_heap_occupancy_limit_set);
CALL SP__insert_formal_parmspec(2, 'parms for the node_outbound_heap_occupancy_limit_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'heap_occupancy_limit', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'file_system_occupancy_limit', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_outbound_heap_occupancy_limit_set , 'This management control sets the maximum number of megabytes of storage space in ION''s SDR non-volatile heap that can be used for the storage of outbound zero-copy objects.  A value of  -1 for either limit signifies ''leave unchanged''. The default heap  limit is 30% of the SDR data space''s total heap size.', @fp_spec_id, @ion_ionadmin_ctrl_node_outbound_heap_occupancy_limit_set_did);

CALL SP__insert_obj_metadata(1, 'node_production_rate_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_production_rate_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_production_rate_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'planned_data_production_rate', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_production_rate_set , 'This management control sets ION''s expectation of the mean rate of continuous data origination by local BP applications throughout the period of time over which congestion forecasts are computed. For nodes that function only as routers this variable will normally be zero. A value of -1, which is the default, indicates that the rate of local data production is unknown; in that case local data production is not considered in the computation of congestion forecasts.', @fp_spec_id, @ion_ionadmin_ctrl_node_production_rate_set_did);

CALL SP__insert_obj_metadata(1, 'node_range_add', @dtn_namespace_id, @ion_ionadmin_ctrl_node_range_add);
CALL SP__insert_formal_parmspec(5, 'parms for the node_range_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'start', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'stop', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'node', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'other_node', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 5, 'distance', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_range_add , 'This control predicts a period of time during which the distance from node to other_node will be constant to within one light second. The period will begin at start_time and end at stop_time, and the distance between the nodes during that time will be distance light seconds.', @fp_spec_id, @ion_ionadmin_ctrl_node_range_add_did);

CALL SP__insert_obj_metadata(1, 'node_range_del', @dtn_namespace_id, @ion_ionadmin_ctrl_node_range_del);
CALL SP__insert_formal_parmspec(3, 'parms for the node_range_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'start', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'node', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'other_node', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_range_del , 'This control deletes the predicted period of constant distance between node and other_node starting at start_time. To delete all ranges between some pair of nodes, use ''*'' as start_time.', @fp_spec_id, @ion_ionadmin_ctrl_node_range_del_did);

CALL SP__insert_obj_metadata(1, 'node_ref_time_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_ref_time_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_ref_time_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'time', 'TV', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_ref_time_set , 'This is used to set the reference time that will be used for interpreting relative time values from now until the next revision of reference time. Note that the new reference time can be a relative time, i.e., an offset beyond the current reference time.', @fp_spec_id, @ion_ionadmin_ctrl_node_ref_time_set_did);

CALL SP__insert_obj_metadata(1, 'node_time_delta_set', @dtn_namespace_id, @ion_ionadmin_ctrl_node_time_delta_set);
CALL SP__insert_formal_parmspec(1, 'parms for the node_time_delta_set control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'local_time_sec_after_epoch', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionadmin_ctrl_node_time_delta_set , 'This management control sets ION''s understanding of the current difference between correct time and the Unix Epoch time values reported by the clock for the local ION node''s computer. This delta is automatically applied to locally obtained time values whenever ION needs to know the current time.', @fp_spec_id, @ion_ionadmin_ctrl_node_time_delta_set_did);


-- #CONST

-- #MAC
