-- -------------------------------------------------------------------
--
-- File Name: adm_sbsp.sql
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
# ADM: 'DTN/sbsp'
use amp_core;


SET @adm_enum = 10;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'DTN/sbsp', 'v1.0', 'sbsp', @adm_enum, NULL, 'The namespace of the ADM.', @dtn_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @dtn_namespace_id, @sbsp_meta_name);
CALL SP__insert_const_actual_definition(@sbsp_meta_name, 'The human-readable name of the ADM.', 'STR', 'sbsp', @sbsp_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @dtn_namespace_id, @sbsp_meta_namespace);
CALL SP__insert_const_actual_definition(@sbsp_meta_namespace, 'The namespace of the ADM.', 'STR', 'DTN/sbsp', @sbsp_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @dtn_namespace_id, @sbsp_meta_version);
CALL SP__insert_const_actual_definition(@sbsp_meta_version, 'The version of the ADM.', 'STR', 'v1.0', @sbsp_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @dtn_namespace_id, @sbsp_meta_organization);
CALL SP__insert_const_actual_definition(@sbsp_meta_organization, 'The name of the issuing organization of the ADM.', 'STR', 'JHUAPL', @sbsp_meta_organization_did);

-- #EDD
CALL SP__insert_obj_metadata(2, 'num_good_tx_bcb_blk', @dtn_namespace_id, @sbsp_edd_num_good_tx_bcb_blk);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bcb_blk, 'Total successfully Tx Bundle Confidentiality blocks', NULL, 'UINT', @sbsp_edd_num_good_tx_bcb_blk_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bcb_blk, 'The singleton value for num_good_tx_bcb_blk', NULL, @sbsp_edd_num_good_tx_bcb_blk_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bcb_blk', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bcb_blk);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bcb_blk, 'Total unsuccessfully Tx Block Confidentiality Block (BCB) blocks', NULL, 'UINT', @sbsp_edd_num_bad_tx_bcb_blk_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bcb_blk, 'The singleton value for num_bad_tx_bcb_blk', NULL, @sbsp_edd_num_bad_tx_bcb_blk_aid);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bcb_blk', @dtn_namespace_id, @sbsp_edd_num_good_rx_bcb_blk);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bcb_blk, 'Total successfully Rx BCB blocks', NULL, 'UINT', @sbsp_edd_num_good_rx_bcb_blk_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bcb_blk, 'The singleton value for num_good_rx_bcb_blk', NULL, @sbsp_edd_num_good_rx_bcb_blk_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bcb_blk', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bcb_blk);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bcb_blk, 'Total unsuccessfully Rx BCB blocks', NULL, 'UINT', @sbsp_edd_num_bad_rx_bcb_blk_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bcb_blk, 'The singleton value for num_bad_rx_bcb_blk', NULL, @sbsp_edd_num_bad_rx_bcb_blk_aid);

CALL SP__insert_obj_metadata(2, 'num_missing_rx_bcb_blks', @dtn_namespace_id, @sbsp_edd_num_missing_rx_bcb_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_missing_rx_bcb_blks, 'Total missing-on-RX BCB blocks', NULL, 'UINT', @sbsp_edd_num_missing_rx_bcb_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_missing_rx_bcb_blks, 'The singleton value for num_missing_rx_bcb_blks', NULL, @sbsp_edd_num_missing_rx_bcb_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_fwd_bcb_blks', @dtn_namespace_id, @sbsp_edd_num_fwd_bcb_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bcb_blks, 'Total forward BCB blocks', NULL, 'UINT', @sbsp_edd_num_fwd_bcb_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bcb_blks, 'The singleton value for num_fwd_bcb_blks', NULL, @sbsp_edd_num_fwd_bcb_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bcb_bytes', @dtn_namespace_id, @sbsp_edd_num_good_tx_bcb_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bcb_bytes, 'Total successfully Tx BCB bytes', NULL, 'UINT', @sbsp_edd_num_good_tx_bcb_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bcb_bytes, 'The singleton value for num_good_tx_bcb_bytes', NULL, @sbsp_edd_num_good_tx_bcb_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bcb_bytes', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bcb_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bcb_bytes, 'Total unsuccessfully Tx BCB bytes', NULL, 'UINT', @sbsp_edd_num_bad_tx_bcb_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bcb_bytes, 'The singleton value for num_bad_tx_bcb_bytes', NULL, @sbsp_edd_num_bad_tx_bcb_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bcb_blks', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bcb_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bcb_blks, 'Total unsuccessfully Tx BCB blocks', NULL, 'UINT', @sbsp_edd_num_bad_tx_bcb_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bcb_blks, 'The singleton value for num_bad_tx_bcb_blks', NULL, @sbsp_edd_num_bad_tx_bcb_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bcb_bytes', @dtn_namespace_id, @sbsp_edd_num_good_rx_bcb_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bcb_bytes, 'Total successfully Rx BCB bytes', NULL, 'UINT', @sbsp_edd_num_good_rx_bcb_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bcb_bytes, 'The singleton value for num_good_rx_bcb_bytes', NULL, @sbsp_edd_num_good_rx_bcb_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bcb_bytes', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bcb_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bcb_bytes, 'Total unsuccessfully Rx BCB bytes', NULL, 'UINT', @sbsp_edd_num_bad_rx_bcb_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bcb_bytes, 'The singleton value for num_bad_rx_bcb_bytes', NULL, @sbsp_edd_num_bad_rx_bcb_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_missing_rx_bcb_bytes', @dtn_namespace_id, @sbsp_edd_num_missing_rx_bcb_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_missing_rx_bcb_bytes, 'Total missing-on-Rx BCB bytes', NULL, 'UINT', @sbsp_edd_num_missing_rx_bcb_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_missing_rx_bcb_bytes, 'The singleton value for num_missing_rx_bcb_bytes', NULL, @sbsp_edd_num_missing_rx_bcb_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_fwd_bcb_bytes', @dtn_namespace_id, @sbsp_edd_num_fwd_bcb_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bcb_bytes, 'Total forwarded BCB bytes', NULL, 'UINT', @sbsp_edd_num_fwd_bcb_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bcb_bytes, 'The singleton value for num_fwd_bcb_bytes', NULL, @sbsp_edd_num_fwd_bcb_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bib_blks', @dtn_namespace_id, @sbsp_edd_num_good_tx_bib_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bib_blks, 'Total successfully Tx Block Integrity Block (BIB) blocks', NULL, 'UINT', @sbsp_edd_num_good_tx_bib_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bib_blks, 'The singleton value for num_good_tx_bib_blks', NULL, @sbsp_edd_num_good_tx_bib_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bib_blks', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bib_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bib_blks, 'Total unsuccessfully Tx BIB blocks', NULL, 'UINT', @sbsp_edd_num_bad_tx_bib_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bib_blks, 'The singleton value for num_bad_tx_bib_blks', NULL, @sbsp_edd_num_bad_tx_bib_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bib_blks', @dtn_namespace_id, @sbsp_edd_num_good_rx_bib_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bib_blks, 'Total successfully Rx BIB blocks', NULL, 'UINT', @sbsp_edd_num_good_rx_bib_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bib_blks, 'The singleton value for num_good_rx_bib_blks', NULL, @sbsp_edd_num_good_rx_bib_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bib_blks', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bib_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bib_blks, 'Total unsuccessfully Rx BIB blocks', NULL, 'UINT', @sbsp_edd_num_bad_rx_bib_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bib_blks, 'The singleton value for num_bad_rx_bib_blks', NULL, @sbsp_edd_num_bad_rx_bib_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_miss_rx_bib_blks', @dtn_namespace_id, @sbsp_edd_num_miss_rx_bib_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_miss_rx_bib_blks, 'Total missing-on-Rx BIB blocks', NULL, 'UINT', @sbsp_edd_num_miss_rx_bib_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_miss_rx_bib_blks, 'The singleton value for num_miss_rx_bib_blks', NULL, @sbsp_edd_num_miss_rx_bib_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_fwd_bib_blks', @dtn_namespace_id, @sbsp_edd_num_fwd_bib_blks);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bib_blks, 'Total forwarded BIB blocks', NULL, 'UINT', @sbsp_edd_num_fwd_bib_blks_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bib_blks, 'The singleton value for num_fwd_bib_blks', NULL, @sbsp_edd_num_fwd_bib_blks_aid);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bib_bytes', @dtn_namespace_id, @sbsp_edd_num_good_tx_bib_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bib_bytes, 'Total successfully Tx BIB bytes', NULL, 'UINT', @sbsp_edd_num_good_tx_bib_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bib_bytes, 'The singleton value for num_good_tx_bib_bytes', NULL, @sbsp_edd_num_good_tx_bib_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bib_bytes', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bib_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bib_bytes, 'Total unsuccessfully Tx BIB bytes', NULL, 'UINT', @sbsp_edd_num_bad_tx_bib_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bib_bytes, 'The singleton value for num_bad_tx_bib_bytes', NULL, @sbsp_edd_num_bad_tx_bib_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bib_bytes', @dtn_namespace_id, @sbsp_edd_num_good_rx_bib_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bib_bytes, 'Total successfully Rx BIB bytes', NULL, 'UINT', @sbsp_edd_num_good_rx_bib_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bib_bytes, 'The singleton value for num_good_rx_bib_bytes', NULL, @sbsp_edd_num_good_rx_bib_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bib_bytes', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bib_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bib_bytes, 'Total unsuccessfully Rx BIB bytes', NULL, 'UINT', @sbsp_edd_num_bad_rx_bib_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bib_bytes, 'The singleton value for num_bad_rx_bib_bytes', NULL, @sbsp_edd_num_bad_rx_bib_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_miss_rx_bib_bytes', @dtn_namespace_id, @sbsp_edd_num_miss_rx_bib_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_miss_rx_bib_bytes, 'Total missing-on-Rx BIB bytes', NULL, 'UINT', @sbsp_edd_num_miss_rx_bib_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_miss_rx_bib_bytes, 'The singleton value for num_miss_rx_bib_bytes', NULL, @sbsp_edd_num_miss_rx_bib_bytes_aid);

CALL SP__insert_obj_metadata(2, 'num_fwd_bib_bytes', @dtn_namespace_id, @sbsp_edd_num_fwd_bib_bytes);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bib_bytes, 'Total forwarded BIB bytes', NULL, 'UINT', @sbsp_edd_num_fwd_bib_bytes_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bib_bytes, 'The singleton value for num_fwd_bib_bytes', NULL, @sbsp_edd_num_fwd_bib_bytes_aid);

CALL SP__insert_obj_metadata(2, 'last_update', @dtn_namespace_id, @sbsp_edd_last_update);
CALL SP__insert_edd_formal_definition(@sbsp_edd_last_update, 'Last sbsp update', NULL, 'TV', @sbsp_edd_last_update_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_last_update, 'The singleton value for last_update', NULL, @sbsp_edd_last_update_aid);

CALL SP__insert_obj_metadata(2, 'num_known_keys', @dtn_namespace_id, @sbsp_edd_num_known_keys);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_known_keys, 'Number of known keys', NULL, 'UINT', @sbsp_edd_num_known_keys_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_known_keys, 'The singleton value for num_known_keys', NULL, @sbsp_edd_num_known_keys_aid);

CALL SP__insert_obj_metadata(2, 'key_names', @dtn_namespace_id, @sbsp_edd_key_names);
CALL SP__insert_edd_formal_definition(@sbsp_edd_key_names, 'Known key names', NULL, 'STR', @sbsp_edd_key_names_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_key_names, 'The singleton value for key_names', NULL, @sbsp_edd_key_names_aid);

CALL SP__insert_obj_metadata(2, 'ciphersuite_names', @dtn_namespace_id, @sbsp_edd_ciphersuite_names);
CALL SP__insert_edd_formal_definition(@sbsp_edd_ciphersuite_names, 'Known ciphersuite names', NULL, 'STR', @sbsp_edd_ciphersuite_names_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_ciphersuite_names, 'The singleton value for ciphersuite_names', NULL, @sbsp_edd_ciphersuite_names_aid);

CALL SP__insert_obj_metadata(2, 'rule_source', @dtn_namespace_id, @sbsp_edd_rule_source);
CALL SP__insert_edd_formal_definition(@sbsp_edd_rule_source, 'Known rule sources', NULL, 'STR', @sbsp_edd_rule_source_did);
CALL SP__insert_edd_actual_definition(@sbsp_edd_rule_source, 'The singleton value for rule_source', NULL, @sbsp_edd_rule_source_aid);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bcb_blks_src', @dtn_namespace_id, @sbsp_edd_num_good_tx_bcb_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bcb_blks_src', @sbsp_edd_num_good_tx_bcb_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_tx_bcb_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bcb_blks_src, 'Number of successfully Tx BCB blocks from SRC', @sbsp_edd_num_good_tx_bcb_blks_src_fp, 'UINT', @sbsp_edd_num_good_tx_bcb_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bcb_blks_src', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bcb_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bcb_blks_src', @sbsp_edd_num_bad_tx_bcb_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_tx_bcb_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bcb_blks_src, 'Number of failed TX BCB blocks from SRC', @sbsp_edd_num_bad_tx_bcb_blks_src_fp, 'UINT', @sbsp_edd_num_bad_tx_bcb_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bcb_blks_src', @dtn_namespace_id, @sbsp_edd_num_good_rx_bcb_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bcb_blks_src', @sbsp_edd_num_good_rx_bcb_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_rx_bcb_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bcb_blks_src, 'Number of successfully Rx BCB blocks from SRC', @sbsp_edd_num_good_rx_bcb_blks_src_fp, 'UINT', @sbsp_edd_num_good_rx_bcb_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bcb_blks_src', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bcb_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bcb_blks_src', @sbsp_edd_num_bad_rx_bcb_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_rx_bcb_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bcb_blks_src, 'Number of failed RX BCB blocks from SRC', @sbsp_edd_num_bad_rx_bcb_blks_src_fp, 'UINT', @sbsp_edd_num_bad_rx_bcb_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_missing_rx_bcb_blks_src', @dtn_namespace_id, @sbsp_edd_num_missing_rx_bcb_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_missing_rx_bcb_blks_src', @sbsp_edd_num_missing_rx_bcb_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_missing_rx_bcb_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_missing_rx_bcb_blks_src, 'Number of missing-onRX BCB blocks from SRC', @sbsp_edd_num_missing_rx_bcb_blks_src_fp, 'UINT', @sbsp_edd_num_missing_rx_bcb_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_fwd_bcb_blks_src', @dtn_namespace_id, @sbsp_edd_num_fwd_bcb_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bcb_blks_src', @sbsp_edd_num_fwd_bcb_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_fwd_bcb_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bcb_blks_src, 'Number of forwarded BCB blocks from SRC', @sbsp_edd_num_fwd_bcb_blks_src_fp, 'UINT', @sbsp_edd_num_fwd_bcb_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bcb_bytes_src', @dtn_namespace_id, @sbsp_edd_num_good_tx_bcb_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bcb_bytes_src', @sbsp_edd_num_good_tx_bcb_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_tx_bcb_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bcb_bytes_src, 'Number of successfully Tx bcb bytes from SRC', @sbsp_edd_num_good_tx_bcb_bytes_src_fp, 'UINT', @sbsp_edd_num_good_tx_bcb_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bcb_bytes_src', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bcb_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bcb_bytes_src', @sbsp_edd_num_bad_tx_bcb_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_tx_bcb_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bcb_bytes_src, 'Number of failed Tx bcb bytes from SRC', @sbsp_edd_num_bad_tx_bcb_bytes_src_fp, 'UINT', @sbsp_edd_num_bad_tx_bcb_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bcb_bytes_src', @dtn_namespace_id, @sbsp_edd_num_good_rx_bcb_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bcb_bytes_src', @sbsp_edd_num_good_rx_bcb_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_rx_bcb_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bcb_bytes_src, 'Number of successfully Rx bcb bytes from SRC', @sbsp_edd_num_good_rx_bcb_bytes_src_fp, 'UINT', @sbsp_edd_num_good_rx_bcb_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bcb_bytes_src', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bcb_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bcb_bytes_src', @sbsp_edd_num_bad_rx_bcb_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_rx_bcb_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bcb_bytes_src, 'Number of failed Rx bcb bytes from SRC', @sbsp_edd_num_bad_rx_bcb_bytes_src_fp, 'UINT', @sbsp_edd_num_bad_rx_bcb_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_missing_rx_bcb_bytes_src', @dtn_namespace_id, @sbsp_edd_num_missing_rx_bcb_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_missing_rx_bcb_bytes_src', @sbsp_edd_num_missing_rx_bcb_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_missing_rx_bcb_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_missing_rx_bcb_bytes_src, 'Number of missing-on-Rx bcb bytes from SRC', @sbsp_edd_num_missing_rx_bcb_bytes_src_fp, 'UINT', @sbsp_edd_num_missing_rx_bcb_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_fwd_bcb_bytes_src', @dtn_namespace_id, @sbsp_edd_num_fwd_bcb_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bcb_bytes_src', @sbsp_edd_num_fwd_bcb_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_fwd_bcb_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bcb_bytes_src, 'Number of forwarded bcb bytes from SRC', @sbsp_edd_num_fwd_bcb_bytes_src_fp, 'UINT', @sbsp_edd_num_fwd_bcb_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bib_blks_src', @dtn_namespace_id, @sbsp_edd_num_good_tx_bib_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bib_blks_src', @sbsp_edd_num_good_tx_bib_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_tx_bib_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bib_blks_src, 'Number of successfully Tx BIB blocks from SRC', @sbsp_edd_num_good_tx_bib_blks_src_fp, 'UINT', @sbsp_edd_num_good_tx_bib_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bib_blks_src', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bib_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bib_blks_src', @sbsp_edd_num_bad_tx_bib_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_tx_bib_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bib_blks_src, 'Number of failed Tx BIB blocks from SRC', @sbsp_edd_num_bad_tx_bib_blks_src_fp, 'UINT', @sbsp_edd_num_bad_tx_bib_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bib_blks_src', @dtn_namespace_id, @sbsp_edd_num_good_rx_bib_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bib_blks_src', @sbsp_edd_num_good_rx_bib_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_rx_bib_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bib_blks_src, 'Number of successfully Rx BIB blocks from SRC', @sbsp_edd_num_good_rx_bib_blks_src_fp, 'UINT', @sbsp_edd_num_good_rx_bib_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bib_blks_src', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bib_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bib_blks_src', @sbsp_edd_num_bad_rx_bib_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_rx_bib_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bib_blks_src, 'Number of failed Rx BIB blocks from SRC', @sbsp_edd_num_bad_rx_bib_blks_src_fp, 'UINT', @sbsp_edd_num_bad_rx_bib_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_miss_rx_bib_blks_src', @dtn_namespace_id, @sbsp_edd_num_miss_rx_bib_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_miss_rx_bib_blks_src', @sbsp_edd_num_miss_rx_bib_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_miss_rx_bib_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_miss_rx_bib_blks_src, 'Number of missing-on-Rx BIB blocks from SRC', @sbsp_edd_num_miss_rx_bib_blks_src_fp, 'UINT', @sbsp_edd_num_miss_rx_bib_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_fwd_bib_blks_src', @dtn_namespace_id, @sbsp_edd_num_fwd_bib_blks_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bib_blks_src', @sbsp_edd_num_fwd_bib_blks_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_fwd_bib_blks_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bib_blks_src, 'Number of forwarded BIB blocks from SRC', @sbsp_edd_num_fwd_bib_blks_src_fp, 'UINT', @sbsp_edd_num_fwd_bib_blks_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_tx_bib_bytes_src', @dtn_namespace_id, @sbsp_edd_num_good_tx_bib_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bib_bytes_src', @sbsp_edd_num_good_tx_bib_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_tx_bib_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_tx_bib_bytes_src, 'Number of successfully Tx BIB bytes from SRC', @sbsp_edd_num_good_tx_bib_bytes_src_fp, 'UINT', @sbsp_edd_num_good_tx_bib_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_tx_bib_bytes_src', @dtn_namespace_id, @sbsp_edd_num_bad_tx_bib_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bib_bytes_src', @sbsp_edd_num_bad_tx_bib_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_tx_bib_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_tx_bib_bytes_src, 'Number of failed Tx BIB bytes from SRC', @sbsp_edd_num_bad_tx_bib_bytes_src_fp, 'UINT', @sbsp_edd_num_bad_tx_bib_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_good_rx_bib_bytes_src', @dtn_namespace_id, @sbsp_edd_num_good_rx_bib_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bib_bytes_src', @sbsp_edd_num_good_rx_bib_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_good_rx_bib_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_good_rx_bib_bytes_src, 'Number of successfully Rx BIB bytes from SRC', @sbsp_edd_num_good_rx_bib_bytes_src_fp, 'UINT', @sbsp_edd_num_good_rx_bib_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_bad_rx_bib_bytes_src', @dtn_namespace_id, @sbsp_edd_num_bad_rx_bib_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bib_bytes_src', @sbsp_edd_num_bad_rx_bib_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_bad_rx_bib_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_bad_rx_bib_bytes_src, 'Number of failed Rx BIB bytes from SRC', @sbsp_edd_num_bad_rx_bib_bytes_src_fp, 'UINT', @sbsp_edd_num_bad_rx_bib_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_missing_rx_bib_bytes_src', @dtn_namespace_id, @sbsp_edd_num_missing_rx_bib_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_missing_rx_bib_bytes_src', @sbsp_edd_num_missing_rx_bib_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_missing_rx_bib_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_missing_rx_bib_bytes_src, 'Number of missing-on-Rx BIB bytes from SRC', @sbsp_edd_num_missing_rx_bib_bytes_src_fp, 'UINT', @sbsp_edd_num_missing_rx_bib_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'num_fwd_bib_bytes_src', @dtn_namespace_id, @sbsp_edd_num_fwd_bib_bytes_src);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bib_bytes_src', @sbsp_edd_num_fwd_bib_bytes_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_num_fwd_bib_bytes_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_num_fwd_bib_bytes_src, 'Number of forwarded BIB bytes from SRC', @sbsp_edd_num_fwd_bib_bytes_src_fp, 'UINT', @sbsp_edd_num_fwd_bib_bytes_src_did);

CALL SP__insert_obj_metadata(2, 'last_update_src', @dtn_namespace_id, @sbsp_edd_last_update_src);
CALL SP__insert_formal_parmspec(1, 'parms for last_update_src', @sbsp_edd_last_update_src_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_last_update_src_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_last_update_src, 'Last sbsp update from SRC', @sbsp_edd_last_update_src_fp, 'TV', @sbsp_edd_last_update_src_did);

CALL SP__insert_obj_metadata(2, 'last_reset', @dtn_namespace_id, @sbsp_edd_last_reset);
CALL SP__insert_formal_parmspec(1, 'parms for last_reset', @sbsp_edd_last_reset_fp);
CALL SP__insert_formal_parmspec_entry(@sbsp_edd_last_reset_fp, 1, 'Src', 'STR', null, @r_fp_ent);
CALL SP__insert_edd_formal_definition(@sbsp_edd_last_reset, 'Last reset', @sbsp_edd_last_reset_fp, 'TV', @sbsp_edd_last_reset_did);

-- #OPER

-- #VAR
-- create ac for expression
SELECT @agent_op_plusuint_did := obj_actual_definition_id FROM obj_actual_definition INNER JOIN obj_metadata ON obj_actual_definition.obj_metadata_id = obj_metadata.obj_metadata_id WHERE obj_name = "plusUINT";
CALL SP__insert_ac_id(3, 'ac for the expression used by @sbsp_var_total_bad_tx_blks', @var_ac_id);
CALL SP__insert_ac_actual_entry(@var_ac_id, @sbsp_edd_num_bad_tx_bib_blks_did, 1, @r_ac_entry_id_1 );
CALL SP__insert_ac_actual_entry(@var_ac_id, @sbsp_edd_num_bad_tx_bcb_blks_did, 2, @r_ac_entry_id_2 );
CALL SP__insert_ac_actual_entry(@var_ac_id, @agent_op_plusuint_did, 3, @r_ac_entry_id_3 );
CALL SP__insert_obj_metadata(12, 'total_bad_tx_blks', @dtn_namespace_id, @sbsp_var_total_bad_tx_blks);
CALL SP__insert_variable_definition(@sbsp_var_total_bad_tx_blks, 'This is the number of failed TX blocks (# failed BIB + # failed bcb).', 20, @var_ac_id, @sbsp_var_total_bad_tx_blks_did);

-- #TBLT
CALL SP__insert_obj_metadata(10, 'bib_rules', @dtn_namespace_id, @sbsp_tblt_bib_rules);
CALL SP__insert_tnvc_collection('columns for the bib_rules table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'SrcEid', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 2, 'DestEid', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'TgtBlk', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 4, 'csName', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 5, 'keyName', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@sbsp_tblt_bib_rules, 'BIB Rules.', @tbl_tnvc_id, @sbsp_tblt_bib_rules_did);

CALL SP__insert_obj_metadata(10, 'bcb_rules', @dtn_namespace_id, @sbsp_tblt_bcb_rules);
CALL SP__insert_tnvc_collection('columns for the bcb_rules table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'SrcEid', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 2, 'DestEid', null, @tnvc_entry);
CALL SP__insert_tnvc_uint_entry(@tbl_tnvc_id, 3, 'TgtBlk', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 4, 'csName', null, @tnvc_entry);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 5, 'keyName', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@sbsp_tblt_bcb_rules, 'BCB Rules.', @tbl_tnvc_id, @sbsp_tblt_bcb_rules_did);


-- #RPTT

CALL SP__insert_obj_metadata(7, 'full_report', @dtn_namespace_id, @sbsp_rpttpl_full_report);

CALL SP__insert_ac_id(29, 'ac for report template full_report', @rptt_ac_id);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bcb_blk_did, 1, @r_ac_rpt_entry_1);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bcb_blk_did, 2, @r_ac_rpt_entry_2);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bcb_blk_did, 3, @r_ac_rpt_entry_3);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bcb_blk_did, 4, @r_ac_rpt_entry_4);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_missing_rx_bcb_blks_did, 5, @r_ac_rpt_entry_5);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bcb_blks_did, 6, @r_ac_rpt_entry_6);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bcb_bytes_did, 7, @r_ac_rpt_entry_7);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bcb_bytes_did, 8, @r_ac_rpt_entry_8);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bcb_bytes_did, 9, @r_ac_rpt_entry_9);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bcb_bytes_did, 10, @r_ac_rpt_entry_10);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_missing_rx_bcb_bytes_did, 11, @r_ac_rpt_entry_11);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bcb_bytes_did, 12, @r_ac_rpt_entry_12);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bib_blks_did, 13, @r_ac_rpt_entry_13);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bib_blks_did, 14, @r_ac_rpt_entry_14);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bib_blks_did, 15, @r_ac_rpt_entry_15);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bib_blks_did, 16, @r_ac_rpt_entry_16);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_miss_rx_bib_blks_did, 17, @r_ac_rpt_entry_17);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bib_blks_did, 18, @r_ac_rpt_entry_18);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bib_bytes_did, 19, @r_ac_rpt_entry_19);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bib_bytes_did, 20, @r_ac_rpt_entry_20);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bib_bytes_did, 21, @r_ac_rpt_entry_21);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bib_bytes_did, 22, @r_ac_rpt_entry_22);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_miss_rx_bib_bytes_did, 23, @r_ac_rpt_entry_23);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bib_bytes_did, 24, @r_ac_rpt_entry_24);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_last_update_did, 25, @r_ac_rpt_entry_25);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_known_keys_did, 26, @r_ac_rpt_entry_26);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_key_names_did, 27, @r_ac_rpt_entry_27);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_ciphersuite_names_did, 28, @r_ac_rpt_entry_28);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_rule_source_did, 29, @r_ac_rpt_entry_29);

CALL SP__insert_report_template_formal_definition(@sbsp_rpttpl_full_report, 'all known meta-data, externally defined data, and variables', null, @rptt_ac_id, @sbsp_rpttpl_full_report_did);
CALL SP__insert_report_actual_definition(@sbsp_rpttpl_full_report, null, null, 'Singleton value for full_report', @sbsp_rpttpl_full_report_aid);

CALL SP__insert_obj_metadata(7, 'source_report', @dtn_namespace_id, @sbsp_rpttpl_source_report);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bcb_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_tx_bcb_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_tx_bcb_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bcb_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_tx_bcb_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bcb_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_tx_bcb_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_tx_bcb_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bcb_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_tx_bcb_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bcb_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_rx_bcb_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_rx_bcb_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bcb_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_rx_bcb_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bcb_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_rx_bcb_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_rx_bcb_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bcb_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_rx_bcb_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_missing_rx_bcb_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_missing_rx_bcb_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_missing_rx_bcb_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_missing_rx_bcb_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_missing_rx_bcb_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bcb_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_fwd_bcb_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_fwd_bcb_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bcb_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_fwd_bcb_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bcb_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_tx_bcb_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_tx_bcb_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bcb_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_tx_bcb_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bcb_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_tx_bcb_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_tx_bcb_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bcb_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_tx_bcb_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bcb_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_rx_bcb_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_rx_bcb_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bcb_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_rx_bcb_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bcb_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_rx_bcb_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_rx_bcb_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bcb_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_rx_bcb_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_missing_rx_bcb_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_missing_rx_bcb_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_missing_rx_bcb_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_missing_rx_bcb_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_missing_rx_bcb_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bcb_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_fwd_bcb_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_fwd_bcb_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bcb_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_fwd_bcb_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bib_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_tx_bib_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_tx_bib_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bib_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_tx_bib_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bib_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_tx_bib_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_tx_bib_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bib_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_tx_bib_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bib_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_rx_bib_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_rx_bib_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bib_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_rx_bib_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bib_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_rx_bib_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_rx_bib_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bib_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_rx_bib_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_miss_rx_bib_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_miss_rx_bib_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_miss_rx_bib_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_miss_rx_bib_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_miss_rx_bib_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bib_blks_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_fwd_bib_blks_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_fwd_bib_blks_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bib_blks_src_did, NULL, @ap_spec_id, @sbsp_edd_num_fwd_bib_blks_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_tx_bib_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_tx_bib_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_tx_bib_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_tx_bib_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_tx_bib_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_tx_bib_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_tx_bib_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_tx_bib_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_tx_bib_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_tx_bib_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_good_rx_bib_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_good_rx_bib_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_good_rx_bib_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_good_rx_bib_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_good_rx_bib_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_bad_rx_bib_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_bad_rx_bib_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_bad_rx_bib_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_bad_rx_bib_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_bad_rx_bib_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_missing_rx_bib_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_missing_rx_bib_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_missing_rx_bib_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_missing_rx_bib_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_missing_rx_bib_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for num_fwd_bib_bytes_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'num_fwd_bib_bytes_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_num_fwd_bib_bytes_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_num_fwd_bib_bytes_src_did, NULL, @ap_spec_id, @sbsp_edd_num_fwd_bib_bytes_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for last_update_src', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'last_update_src', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_last_update_src_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_last_update_src_did, NULL, @ap_spec_id, @sbsp_edd_last_update_src_aid_source_1);
CALL SP__insert_formal_parmspec(1, 'parms for last_reset', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'last_reset', 'STR', null, @r_fp_ent);

CALL SP__insert_actual_parmspec(@sbsp_edd_last_reset_fp, 1, '', @ap_spec_id);
CALL SP__insert_actual_parms_names(@ap_spec_id, 1, 'STR', @fp_spec_id);
CALL SP__insert_edd_actual_definition(@sbsp_edd_last_reset_did, NULL, @ap_spec_id, @sbsp_edd_last_reset_aid_source_1);

CALL SP__insert_ac_id(26, 'ac for report template source_report', @rptt_ac_id);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bcb_blks_src_did, 1, @r_ac_rpt_entry_1);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bcb_blks_src_did, 2, @r_ac_rpt_entry_2);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bcb_blks_src_did, 3, @r_ac_rpt_entry_3);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bcb_blks_src_did, 4, @r_ac_rpt_entry_4);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_missing_rx_bcb_blks_src_did, 5, @r_ac_rpt_entry_5);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bcb_blks_src_did, 6, @r_ac_rpt_entry_6);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bcb_bytes_src_did, 7, @r_ac_rpt_entry_7);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bcb_bytes_src_did, 8, @r_ac_rpt_entry_8);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bcb_bytes_src_did, 9, @r_ac_rpt_entry_9);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bcb_bytes_src_did, 10, @r_ac_rpt_entry_10);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_missing_rx_bcb_bytes_src_did, 11, @r_ac_rpt_entry_11);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bcb_bytes_src_did, 12, @r_ac_rpt_entry_12);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bib_blks_src_did, 13, @r_ac_rpt_entry_13);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bib_blks_src_did, 14, @r_ac_rpt_entry_14);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bib_blks_src_did, 15, @r_ac_rpt_entry_15);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bib_blks_src_did, 16, @r_ac_rpt_entry_16);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_miss_rx_bib_blks_src_did, 17, @r_ac_rpt_entry_17);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bib_blks_src_did, 18, @r_ac_rpt_entry_18);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_tx_bib_bytes_src_did, 19, @r_ac_rpt_entry_19);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_tx_bib_bytes_src_did, 20, @r_ac_rpt_entry_20);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_good_rx_bib_bytes_src_did, 21, @r_ac_rpt_entry_21);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_bad_rx_bib_bytes_src_did, 22, @r_ac_rpt_entry_22);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_missing_rx_bib_bytes_src_did, 23, @r_ac_rpt_entry_23);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_num_fwd_bib_bytes_src_did, 24, @r_ac_rpt_entry_24);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_last_update_src_did, 25, @r_ac_rpt_entry_25);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @sbsp_edd_last_reset_did, 26, @r_ac_rpt_entry_26);

CALL SP__insert_report_template_formal_definition(@sbsp_rpttpl_source_report, 'security info by source', null, @rptt_ac_id, @sbsp_rpttpl_source_report_did);
CALL SP__insert_report_actual_definition(@sbsp_rpttpl_source_report, null, null, 'Singleton value for source_report', @sbsp_rpttpl_source_report_aid);

-- #CTRL
CALL SP__insert_obj_metadata(1, 'rst_all_cnts', @dtn_namespace_id, @sbsp_ctrl_rst_all_cnts);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_rst_all_cnts , 'This control causes the Agent to reset all counts associated with block or byte statistics and to set the Last Reset Time of the sbsp EDD data to the time when the control was run.', null, @sbsp_ctrl_rst_all_cnts_did);

CALL SP__insert_obj_metadata(1, 'rst_src_cnts', @dtn_namespace_id, @sbsp_ctrl_rst_src_cnts);
CALL SP__insert_formal_parmspec(1, 'parms for the rst_src_cnts control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'src', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_rst_src_cnts , 'This control causes the Agent to reset all counts (blocks and bytes) associated with a given bundle source and set the Last Reset Time of the source statistics to the time when the control was run.', @fp_spec_id, @sbsp_ctrl_rst_src_cnts_did);

CALL SP__insert_obj_metadata(1, 'delete_key', @dtn_namespace_id, @sbsp_ctrl_delete_key);
CALL SP__insert_formal_parmspec(1, 'parms for the delete_key control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_delete_key , 'This control deletes a key from the sbsp system.', @fp_spec_id, @sbsp_ctrl_delete_key_did);

CALL SP__insert_obj_metadata(1, 'add_key', @dtn_namespace_id, @sbsp_ctrl_add_key);
CALL SP__insert_formal_parmspec(2, 'parms for the add_key control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'keyData', 'BYTESTR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_add_key , 'This control adds a key to the sbsp system.', @fp_spec_id, @sbsp_ctrl_add_key_did);

CALL SP__insert_obj_metadata(1, 'add_bib_rule', @dtn_namespace_id, @sbsp_ctrl_add_bib_rule);
CALL SP__insert_formal_parmspec(5, 'parms for the add_bib_rule control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'source', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'destination', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'target', 'INT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'ciphersuiteId', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 5, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_add_bib_rule , 'This control configures policy on the sbsp protocol implementation that describes how BIB blocks should be applied to bundles in the system. This policy is captured as a rule which states when transmitting a bundle from the given source endpoint ID to the given destination endpoint ID, blocks of type target should have a BIB added to them using the given ciphersuite and the given key.', @fp_spec_id, @sbsp_ctrl_add_bib_rule_did);

CALL SP__insert_obj_metadata(1, 'del_bib_rule', @dtn_namespace_id, @sbsp_ctrl_del_bib_rule);
CALL SP__insert_formal_parmspec(3, 'parms for the del_bib_rule control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'source', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'destination', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'target', 'INT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_del_bib_rule , 'This control removes any configured policy on the sbsp protocol implementation that describes how BIB blocks should be applied to bundles in the system. A BIB policy is uniquely identified by a source endpoint Id, a destination Id, and a target block type.', @fp_spec_id, @sbsp_ctrl_del_bib_rule_did);

CALL SP__insert_obj_metadata(1, 'add_bcb_rule', @dtn_namespace_id, @sbsp_ctrl_add_bcb_rule);
CALL SP__insert_formal_parmspec(5, 'parms for the add_bcb_rule control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'source', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'destination', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'target', 'INT', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'ciphersuiteId', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 5, 'key_name', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_add_bcb_rule , 'This control configures policy on the sbsp protocol implementation that describes how BCB blocks should be applied to bundles in the system. This policy is captured as a rule which states when transmitting a bundle from the given source endpoint id to the given destination endpoint id, blocks of type target should have a bcb added to them using the given ciphersuite and the given key.', @fp_spec_id, @sbsp_ctrl_add_bcb_rule_did);

CALL SP__insert_obj_metadata(1, 'del_bcb_rule', @dtn_namespace_id, @sbsp_ctrl_del_bcb_rule);
CALL SP__insert_formal_parmspec(3, 'parms for the del_bcb_rule control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'source', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'destination', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'target', 'INT', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@sbsp_ctrl_del_bcb_rule , 'This control removes any configured policy on the sbsp protocol implementation that describes how BCB blocks should be applied to bundles in the system. A bcb policy is uniquely identified by a source endpoint id, a destination endpoint id, and a target block type.', @fp_spec_id, @sbsp_ctrl_del_bcb_rule_did);


-- #CONST

-- #MAC
