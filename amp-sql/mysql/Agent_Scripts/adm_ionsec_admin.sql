-- -------------------------------------------------------------------
--
-- File Name: adm_ionsec_admin.sql
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
# ADM: 'DTN/ION/ionsecadmin'
use amp_core;


SET @adm_enum = 8;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'DTN/ION/ionsecadmin', 'v0.0', 'ionsec_admin', @adm_enum, NULL, 'The namespace of the ADM.', @dtn_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @dtn_namespace_id, @ion_ionsecadmin_meta_name);
CALL SP__insert_const_actual_definition(@ion_ionsecadmin_meta_name, 'The human-readable name of the ADM.', 'STR', 'ionsec_admin', @ion_ionsecadmin_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @dtn_namespace_id, @ion_ionsecadmin_meta_namespace);
CALL SP__insert_const_actual_definition(@ion_ionsecadmin_meta_namespace, 'The namespace of the ADM.', 'STR', 'DTN/ION/ionsecadmin', @ion_ionsecadmin_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @dtn_namespace_id, @ion_ionsecadmin_meta_version);
CALL SP__insert_const_actual_definition(@ion_ionsecadmin_meta_version, 'The version of the ADM.', 'STR', 'v0.0', @ion_ionsecadmin_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @dtn_namespace_id, @ion_ionsecadmin_meta_organization);
CALL SP__insert_const_actual_definition(@ion_ionsecadmin_meta_organization, 'The name of the issuing organization of the ADM.', 'STR', 'JHUAPL', @ion_ionsecadmin_meta_organization_did);

-- #EDD
-- #OPER

-- #VAR

-- #TBLT
CALL SP__insert_obj_metadata(10, 'ltp_rx_rules', @dtn_namespace_id, @ion_ionsecadmin_tblt_ltp_rx_rules);
CALL SP__insert_tnvc_collection('columns for the ltp_rx_rules table', @tbl_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 1, 'ltp_engine_id', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 2, 'ciphersuite_nbr', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 3, 'key_name', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_ionsecadmin_tblt_ltp_rx_rules, 'This table lists all LTP segment authentication rulesin the security policy database.', @tbl_tnvc_id, @ion_ionsecadmin_tblt_ltp_rx_rules_did);

CALL SP__insert_obj_metadata(10, 'ltp_tx_rules', @dtn_namespace_id, @ion_ionsecadmin_tblt_ltp_tx_rules);
CALL SP__insert_tnvc_collection('columns for the ltp_tx_rules table', @tbl_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 1, 'ltp_engine_id', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 2, 'ciphersuite_nbr', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 3, 'key_name', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@ion_ionsecadmin_tblt_ltp_tx_rules, 'This table lists all LTP segment signing rules in the security policy database.', @tbl_tnvc_id, @ion_ionsecadmin_tblt_ltp_tx_rules_did);


-- #RPTT

-- #CTRL
CALL SP__insert_obj_metadata(1, 'key_add', @dtn_namespace_id, @ion_ionsecadmin_ctrl_key_add);
CALL SP__insert_formal_parmspec(2, 'parms for the key_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'key_value', 'BYTESTR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_key_add , 'This control adds a named key value to the security policy database. The content of file_name is taken as the value of the key.Named keys can be referenced by other elements of thesecurity policy database.', @fp_spec_id, @ion_ionsecadmin_ctrl_key_add_did);

CALL SP__insert_obj_metadata(1, 'key_change', @dtn_namespace_id, @ion_ionsecadmin_ctrl_key_change);
CALL SP__insert_formal_parmspec(2, 'parms for the key_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'key_value', 'BYTESTR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_key_change , 'This control changes the value of the named key, obtaining the new key value from the content of file_name.', @fp_spec_id, @ion_ionsecadmin_ctrl_key_change_did);

CALL SP__insert_obj_metadata(1, 'key_del', @dtn_namespace_id, @ion_ionsecadmin_ctrl_key_del);
CALL SP__insert_formal_parmspec(1, 'parms for the key_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_key_del , 'This control deletes the key identified by name.', @fp_spec_id, @ion_ionsecadmin_ctrl_key_del_did);

CALL SP__insert_obj_metadata(1, 'ltp_rx_rule_add', @dtn_namespace_id, @ion_ionsecadmin_ctrl_ltp_rx_rule_add);
CALL SP__insert_formal_parmspec(3, 'parms for the ltp_rx_rule_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_engine_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'ciphersuite_nbr', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_ltp_rx_rule_add , 'This control adds a rule specifying the manner in which LTP segment authentication will be applied to LTP segmentsrecieved from the indicated LTP engine. A segment from the indicated LTP engine will only be deemed authentic if it contains an authentication extension computed via the ciphersuite identified by ciphersuite_nbr using the applicable key value. If ciphersuite_nbr is 255 then the applicable key value is a hard-coded constant and key_name must be omitted; otherwise key_nameis required and the applicable key value is the current value of the key named key_name in the local security policy database. Valid values of ciphersuite_nbr are: 0: HMAC-SHA1-80 1: RSA-SHA256 255: NULL', @fp_spec_id, @ion_ionsecadmin_ctrl_ltp_rx_rule_add_did);

CALL SP__insert_obj_metadata(1, 'ltp_rx_rule_change', @dtn_namespace_id, @ion_ionsecadmin_ctrl_ltp_rx_rule_change);
CALL SP__insert_formal_parmspec(3, 'parms for the ltp_rx_rule_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_engine_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'ciphersuite_nbr', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_ltp_rx_rule_change , 'This control changes the parameters of the LTP segment authentication rule for the indicated LTP engine.', @fp_spec_id, @ion_ionsecadmin_ctrl_ltp_rx_rule_change_did);

CALL SP__insert_obj_metadata(1, 'ltp_rx_rule_del', @dtn_namespace_id, @ion_ionsecadmin_ctrl_ltp_rx_rule_del);
CALL SP__insert_formal_parmspec(1, 'parms for the ltp_rx_rule_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_engine_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_ltp_rx_rule_del , 'This control deletes the LTP segment authentication rule for the indicated LTP engine.', @fp_spec_id, @ion_ionsecadmin_ctrl_ltp_rx_rule_del_did);

CALL SP__insert_obj_metadata(1, 'ltp_tx_rule_add', @dtn_namespace_id, @ion_ionsecadmin_ctrl_ltp_tx_rule_add);
CALL SP__insert_formal_parmspec(3, 'parms for the ltp_tx_rule_add control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_engine_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'ciphersuite_nbr', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_ltp_tx_rule_add , 'This control adds a rule specifying the manner in which LTP segments transmitted to the indicated LTP engine mustbe signed. Signing a segment destined for the indicated LTP engineentails computing an authentication extension via the ciphersuite identified by ciphersuite_nbr using the applicable key value. If ciphersuite_nbr is 255 then the applicable key value is a hard-coded constant and key_name must be omitted; otherwise key_nameis required and the applicable key value is the current value of the key named key_name in the local security policy database.Valid values of ciphersuite_nbr are: 0:HMAC_SHA1-80 1: RSA_SHA256 255: NULL', @fp_spec_id, @ion_ionsecadmin_ctrl_ltp_tx_rule_add_did);

CALL SP__insert_obj_metadata(1, 'ltp_tx_rule_change', @dtn_namespace_id, @ion_ionsecadmin_ctrl_ltp_tx_rule_change);
CALL SP__insert_formal_parmspec(3, 'parms for the ltp_tx_rule_change control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_engine_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'ciphersuite_nbr', 'UINT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_ltp_tx_rule_change , 'This control changes the parameters of the LTP segment signing rule for the indicated LTP engine.', @fp_spec_id, @ion_ionsecadmin_ctrl_ltp_tx_rule_change_did);

CALL SP__insert_obj_metadata(1, 'ltp_tx_rule_del', @dtn_namespace_id, @ion_ionsecadmin_ctrl_ltp_tx_rule_del);
CALL SP__insert_formal_parmspec(1, 'parms for the ltp_tx_rule_del control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ltp_engine_id', 'UINT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_ltp_tx_rule_del , 'This control deletes the LTP segment signing rule forthe indicated LTP engine.', @fp_spec_id, @ion_ionsecadmin_ctrl_ltp_tx_rule_del_did);

CALL SP__insert_obj_metadata(1, 'list_keys', @dtn_namespace_id, @ion_ionsecadmin_ctrl_list_keys);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_list_keys , 'This control lists the names of keys available in the key policy database.', null, @ion_ionsecadmin_ctrl_list_keys_did);

CALL SP__insert_obj_metadata(1, 'list_ltp_rx_rules', @dtn_namespace_id, @ion_ionsecadmin_ctrl_list_ltp_rx_rules);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_list_ltp_rx_rules , 'This control lists all LTP segment authentication rules in the security policy database.', null, @ion_ionsecadmin_ctrl_list_ltp_rx_rules_did);

CALL SP__insert_obj_metadata(1, 'list_ltp_tx_rules', @dtn_namespace_id, @ion_ionsecadmin_ctrl_list_ltp_tx_rules);
CALL SP__insert_control_formal_definition(@ion_ionsecadmin_ctrl_list_ltp_tx_rules , 'This control lists all LTP segment signing rules in the security policy database.', null, @ion_ionsecadmin_ctrl_list_ltp_tx_rules_did);


-- #CONST

-- #MAC
