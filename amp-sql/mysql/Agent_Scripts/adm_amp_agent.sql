-- -------------------------------------------------------------------
--
-- File Name: adm_amp_agent.sql
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
# ADM: 'Amp/Agent'
use amp_core;


SET @adm_enum = 1;
CALL SP__insert_adm_defined_namespace('JHUAPL', 'Amp/Agent', 'v3.1', 'amp_agent', @adm_enum, NULL, 'The namespace of the ADM.', @amp_namespace_id);


-- #META
CALL SP__insert_obj_metadata(0, 'name', @amp_namespace_id, @agent_meta_name);
CALL SP__insert_const_actual_definition(@agent_meta_name, 'The human-readable name of the ADM.', 'STR', 'amp_agent', @agent_meta_name_did);

CALL SP__insert_obj_metadata(0, 'namespace', @amp_namespace_id, @agent_meta_namespace);
CALL SP__insert_const_actual_definition(@agent_meta_namespace, 'The namespace of the ADM.', 'STR', 'Amp/Agent', @agent_meta_namespace_did);

CALL SP__insert_obj_metadata(0, 'version', @amp_namespace_id, @agent_meta_version);
CALL SP__insert_const_actual_definition(@agent_meta_version, 'The version of the ADM.', 'STR', 'v3.1', @agent_meta_version_did);

CALL SP__insert_obj_metadata(0, 'organization', @amp_namespace_id, @agent_meta_organization);
CALL SP__insert_const_actual_definition(@agent_meta_organization, 'The name of the issuing organization of the ADM.', 'STR', 'JHUAPL', @agent_meta_organization_did);

-- #EDD
CALL SP__insert_obj_metadata(2, 'num_rpt_tpls', @amp_namespace_id, @agent_edd_num_rpt_tpls);
CALL SP__insert_edd_formal_definition(@agent_edd_num_rpt_tpls, 'This is the number of report templates known to the Agent.', NULL, 'UINT', @agent_edd_num_rpt_tpls_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_rpt_tpls, 'The singleton value for num_rpt_tpls', NULL, @agent_edd_num_rpt_tpls_aid);

CALL SP__insert_obj_metadata(2, 'num_tbl_tpls', @amp_namespace_id, @agent_edd_num_tbl_tpls);
CALL SP__insert_edd_formal_definition(@agent_edd_num_tbl_tpls, 'This is the number of table templates known to the Agent.', NULL, 'UINT', @agent_edd_num_tbl_tpls_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_tbl_tpls, 'The singleton value for num_tbl_tpls', NULL, @agent_edd_num_tbl_tpls_aid);

CALL SP__insert_obj_metadata(2, 'sent_reports', @amp_namespace_id, @agent_edd_sent_reports);
CALL SP__insert_edd_formal_definition(@agent_edd_sent_reports, 'This is the number of reports sent by the agent.', NULL, 'UINT', @agent_edd_sent_reports_did);
CALL SP__insert_edd_actual_definition(@agent_edd_sent_reports, 'The singleton value for sent_reports', NULL, @agent_edd_sent_reports_aid);

CALL SP__insert_obj_metadata(2, 'num_tbr', @amp_namespace_id, @agent_edd_num_tbr);
CALL SP__insert_edd_formal_definition(@agent_edd_num_tbr, 'This is the number of time-based rules running on the agent.', NULL, 'UINT', @agent_edd_num_tbr_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_tbr, 'The singleton value for num_tbr', NULL, @agent_edd_num_tbr_aid);

CALL SP__insert_obj_metadata(2, 'run_tbr', @amp_namespace_id, @agent_edd_run_tbr);
CALL SP__insert_edd_formal_definition(@agent_edd_run_tbr, 'This is the number of time-based rules run by the agent since the last reset.', NULL, 'UINT', @agent_edd_run_tbr_did);
CALL SP__insert_edd_actual_definition(@agent_edd_run_tbr, 'The singleton value for run_tbr', NULL, @agent_edd_run_tbr_aid);

CALL SP__insert_obj_metadata(2, 'num_sbr', @amp_namespace_id, @agent_edd_num_sbr);
CALL SP__insert_edd_formal_definition(@agent_edd_num_sbr, 'This is the number of state-based rules running on the agent.', NULL, 'UINT', @agent_edd_num_sbr_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_sbr, 'The singleton value for num_sbr', NULL, @agent_edd_num_sbr_aid);

CALL SP__insert_obj_metadata(2, 'run_sbr', @amp_namespace_id, @agent_edd_run_sbr);
CALL SP__insert_edd_formal_definition(@agent_edd_run_sbr, 'This is the number of state-based rules run by the agent since the last reset.', NULL, 'UINT', @agent_edd_run_sbr_did);
CALL SP__insert_edd_actual_definition(@agent_edd_run_sbr, 'The singleton value for run_sbr', NULL, @agent_edd_run_sbr_aid);

CALL SP__insert_obj_metadata(2, 'num_const', @amp_namespace_id, @agent_edd_num_const);
CALL SP__insert_edd_formal_definition(@agent_edd_num_const, 'This is the number of constants known by the agent.', NULL, 'UINT', @agent_edd_num_const_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_const, 'The singleton value for num_const', NULL, @agent_edd_num_const_aid);

CALL SP__insert_obj_metadata(2, 'num_var', @amp_namespace_id, @agent_edd_num_var);
CALL SP__insert_edd_formal_definition(@agent_edd_num_var, 'This is the number of variables known by the agent.', NULL, 'UINT', @agent_edd_num_var_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_var, 'The singleton value for num_var', NULL, @agent_edd_num_var_aid);

CALL SP__insert_obj_metadata(2, 'num_macros', @amp_namespace_id, @agent_edd_num_macros);
CALL SP__insert_edd_formal_definition(@agent_edd_num_macros, 'This is the number of macros known by the agent.', NULL, 'UINT', @agent_edd_num_macros_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_macros, 'The singleton value for num_macros', NULL, @agent_edd_num_macros_aid);

CALL SP__insert_obj_metadata(2, 'run_macros', @amp_namespace_id, @agent_edd_run_macros);
CALL SP__insert_edd_formal_definition(@agent_edd_run_macros, 'This is the number of macros run by the agent since the last reset.', NULL, 'UINT', @agent_edd_run_macros_did);
CALL SP__insert_edd_actual_definition(@agent_edd_run_macros, 'The singleton value for run_macros', NULL, @agent_edd_run_macros_aid);

CALL SP__insert_obj_metadata(2, 'num_controls', @amp_namespace_id, @agent_edd_num_controls);
CALL SP__insert_edd_formal_definition(@agent_edd_num_controls, 'This is the number of controls known by the agent.', NULL, 'UINT', @agent_edd_num_controls_did);
CALL SP__insert_edd_actual_definition(@agent_edd_num_controls, 'The singleton value for num_controls', NULL, @agent_edd_num_controls_aid);

CALL SP__insert_obj_metadata(2, 'run_controls', @amp_namespace_id, @agent_edd_run_controls);
CALL SP__insert_edd_formal_definition(@agent_edd_run_controls, 'This is the number of controls run by the agent since the last reset.', NULL, 'UINT', @agent_edd_run_controls_did);
CALL SP__insert_edd_actual_definition(@agent_edd_run_controls, 'The singleton value for run_controls', NULL, @agent_edd_run_controls_aid);

CALL SP__insert_obj_metadata(2, 'cur_time', @amp_namespace_id, @agent_edd_cur_time);
CALL SP__insert_edd_formal_definition(@agent_edd_cur_time, 'This is the current system time.', NULL, 'TV', @agent_edd_cur_time_did);
CALL SP__insert_edd_actual_definition(@agent_edd_cur_time, 'The singleton value for cur_time', NULL, @agent_edd_cur_time_aid);

-- #OPER
CALL SP__insert_obj_metadata(5, 'plusINT', @amp_namespace_id, @agent_op_plusint);
CALL SP__insert_tnvc_collection('operands for plusINT', @op_tnvc_id);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_plusint, 'Int32 addition', 'INT', 2, @op_tnvc_id, @agent_op_plusint_did);

CALL SP__insert_obj_metadata(5, 'plusUINT', @amp_namespace_id, @agent_op_plusuint);
CALL SP__insert_tnvc_collection('operands for plusUINT', @op_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_plusuint, 'Unsigned Int32 addition', 'UINT', 2, @op_tnvc_id, @agent_op_plusuint_did);

CALL SP__insert_obj_metadata(5, 'plusVAST', @amp_namespace_id, @agent_op_plusvast);
CALL SP__insert_tnvc_collection('operands for plusVAST', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_plusvast, 'Int64 addition', 'VAST', 2, @op_tnvc_id, @agent_op_plusvast_did);

CALL SP__insert_obj_metadata(5, 'plusUVAST', @amp_namespace_id, @agent_op_plusuvast);
CALL SP__insert_tnvc_collection('operands for plusUVAST', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_plusuvast, 'Unsigned Int64 addition', 'UVAST', 2, @op_tnvc_id, @agent_op_plusuvast_did);

CALL SP__insert_obj_metadata(5, 'plusREAL32', @amp_namespace_id, @agent_op_plusreal32);
CALL SP__insert_tnvc_collection('operands for plusREAL32', @op_tnvc_id);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_plusreal32, 'Real32 addition', 'REAL32', 2, @op_tnvc_id, @agent_op_plusreal32_did);

CALL SP__insert_obj_metadata(5, 'plusREAL64', @amp_namespace_id, @agent_op_plusreal64);
CALL SP__insert_tnvc_collection('operands for plusREAL64', @op_tnvc_id);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_plusreal64, 'Real64 addition', 'REAL64', 2, @op_tnvc_id, @agent_op_plusreal64_did);

CALL SP__insert_obj_metadata(5, 'minusINT', @amp_namespace_id, @agent_op_minusint);
CALL SP__insert_tnvc_collection('operands for minusINT', @op_tnvc_id);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_minusint, 'Int32 subtraction', 'INT', 2, @op_tnvc_id, @agent_op_minusint_did);

CALL SP__insert_obj_metadata(5, 'minusUINT', @amp_namespace_id, @agent_op_minusuint);
CALL SP__insert_tnvc_collection('operands for minusUINT', @op_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_minusuint, 'Unsigned Int32 subtraction', 'UINT', 2, @op_tnvc_id, @agent_op_minusuint_did);

CALL SP__insert_obj_metadata(5, 'minusVAST', @amp_namespace_id, @agent_op_minusvast);
CALL SP__insert_tnvc_collection('operands for minusVAST', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_minusvast, 'Int64 subtraction', 'VAST', 2, @op_tnvc_id, @agent_op_minusvast_did);

CALL SP__insert_obj_metadata(5, 'minusUVAST', @amp_namespace_id, @agent_op_minusuvast);
CALL SP__insert_tnvc_collection('operands for minusUVAST', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_minusuvast, 'Unsigned Int64 subtraction', 'UVAST', 2, @op_tnvc_id, @agent_op_minusuvast_did);

CALL SP__insert_obj_metadata(5, 'minusREAL32', @amp_namespace_id, @agent_op_minusreal32);
CALL SP__insert_tnvc_collection('operands for minusREAL32', @op_tnvc_id);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_minusreal32, 'Real32 subtraction', 'REAL32', 2, @op_tnvc_id, @agent_op_minusreal32_did);

CALL SP__insert_obj_metadata(5, 'minusREAL64', @amp_namespace_id, @agent_op_minusreal64);
CALL SP__insert_tnvc_collection('operands for minusREAL64', @op_tnvc_id);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_minusreal64, 'Real64 subtraction', 'REAL64', 2, @op_tnvc_id, @agent_op_minusreal64_did);

CALL SP__insert_obj_metadata(5, 'multINT', @amp_namespace_id, @agent_op_multint);
CALL SP__insert_tnvc_collection('operands for multINT', @op_tnvc_id);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_multint, 'Int32 multiplication', 'INT', 2, @op_tnvc_id, @agent_op_multint_did);

CALL SP__insert_obj_metadata(5, 'multUINT', @amp_namespace_id, @agent_op_multuint);
CALL SP__insert_tnvc_collection('operands for multUINT', @op_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_multuint, 'Unsigned Int32 multiplication', 'UINT', 2, @op_tnvc_id, @agent_op_multuint_did);

CALL SP__insert_obj_metadata(5, 'multVAST', @amp_namespace_id, @agent_op_multvast);
CALL SP__insert_tnvc_collection('operands for multVAST', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_multvast, 'Int64 multiplication', 'VAST', 2, @op_tnvc_id, @agent_op_multvast_did);

CALL SP__insert_obj_metadata(5, 'multUVAST', @amp_namespace_id, @agent_op_multuvast);
CALL SP__insert_tnvc_collection('operands for multUVAST', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_multuvast, 'Unsigned Int64 multiplication', 'UVAST', 2, @op_tnvc_id, @agent_op_multuvast_did);

CALL SP__insert_obj_metadata(5, 'multREAL32', @amp_namespace_id, @agent_op_multreal32);
CALL SP__insert_tnvc_collection('operands for multREAL32', @op_tnvc_id);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_multreal32, 'Real32 multiplication', 'REAL32', 2, @op_tnvc_id, @agent_op_multreal32_did);

CALL SP__insert_obj_metadata(5, 'multREAL64', @amp_namespace_id, @agent_op_multreal64);
CALL SP__insert_tnvc_collection('operands for multREAL64', @op_tnvc_id);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_multreal64, 'Real64 multiplication', 'REAL64', 2, @op_tnvc_id, @agent_op_multreal64_did);

CALL SP__insert_obj_metadata(5, 'divINT', @amp_namespace_id, @agent_op_divint);
CALL SP__insert_tnvc_collection('operands for divINT', @op_tnvc_id);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_divint, 'Int32 division', 'INT', 2, @op_tnvc_id, @agent_op_divint_did);

CALL SP__insert_obj_metadata(5, 'divUINT', @amp_namespace_id, @agent_op_divuint);
CALL SP__insert_tnvc_collection('operands for divUINT', @op_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_divuint, 'Unsigned Int32 division', 'UINT', 2, @op_tnvc_id, @agent_op_divuint_did);

CALL SP__insert_obj_metadata(5, 'divVAST', @amp_namespace_id, @agent_op_divvast);
CALL SP__insert_tnvc_collection('operands for divVAST', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_divvast, 'Int64 division', 'VAST', 2, @op_tnvc_id, @agent_op_divvast_did);

CALL SP__insert_obj_metadata(5, 'divUVAST', @amp_namespace_id, @agent_op_divuvast);
CALL SP__insert_tnvc_collection('operands for divUVAST', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_divuvast, 'Unsigned Int64 division', 'UVAST', 2, @op_tnvc_id, @agent_op_divuvast_did);

CALL SP__insert_obj_metadata(5, 'divREAL32', @amp_namespace_id, @agent_op_divreal32);
CALL SP__insert_tnvc_collection('operands for divREAL32', @op_tnvc_id);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_divreal32, 'Real32 division', 'REAL32', 2, @op_tnvc_id, @agent_op_divreal32_did);

CALL SP__insert_obj_metadata(5, 'divREAL64', @amp_namespace_id, @agent_op_divreal64);
CALL SP__insert_tnvc_collection('operands for divREAL64', @op_tnvc_id);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_divreal64, 'Real64 division', 'REAL64', 2, @op_tnvc_id, @agent_op_divreal64_did);

CALL SP__insert_obj_metadata(5, 'modINT', @amp_namespace_id, @agent_op_modint);
CALL SP__insert_tnvc_collection('operands for modINT', @op_tnvc_id);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_modint, 'Int32 modulus division', 'INT', 2, @op_tnvc_id, @agent_op_modint_did);

CALL SP__insert_obj_metadata(5, 'modUINT', @amp_namespace_id, @agent_op_moduint);
CALL SP__insert_tnvc_collection('operands for modUINT', @op_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_moduint, 'Unsigned Int32 modulus division', 'UINT', 2, @op_tnvc_id, @agent_op_moduint_did);

CALL SP__insert_obj_metadata(5, 'modVAST', @amp_namespace_id, @agent_op_modvast);
CALL SP__insert_tnvc_collection('operands for modVAST', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_modvast, 'Int64 modulus division', 'VAST', 2, @op_tnvc_id, @agent_op_modvast_did);

CALL SP__insert_obj_metadata(5, 'modUVAST', @amp_namespace_id, @agent_op_moduvast);
CALL SP__insert_tnvc_collection('operands for modUVAST', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_moduvast, 'Unsigned Int64 modulus division', 'UVAST', 2, @op_tnvc_id, @agent_op_moduvast_did);

CALL SP__insert_obj_metadata(5, 'modREAL32', @amp_namespace_id, @agent_op_modreal32);
CALL SP__insert_tnvc_collection('operands for modREAL32', @op_tnvc_id);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_modreal32, 'Real32 modulus division', 'REAL32', 2, @op_tnvc_id, @agent_op_modreal32_did);

CALL SP__insert_obj_metadata(5, 'modREAL64', @amp_namespace_id, @agent_op_modreal64);
CALL SP__insert_tnvc_collection('operands for modREAL64', @op_tnvc_id);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_modreal64, 'Real64 modulus division', 'REAL64', 2, @op_tnvc_id, @agent_op_modreal64_did);

CALL SP__insert_obj_metadata(5, 'expINT', @amp_namespace_id, @agent_op_expint);
CALL SP__insert_tnvc_collection('operands for expINT', @op_tnvc_id);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_int_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_expint, 'Int32 exponentiation', 'INT', 2, @op_tnvc_id, @agent_op_expint_did);

CALL SP__insert_obj_metadata(5, 'expUINT', @amp_namespace_id, @agent_op_expuint);
CALL SP__insert_tnvc_collection('operands for expUINT', @op_tnvc_id);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uint_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_expuint, 'Unsigned int32 exponentiation', 'UINT', 2, @op_tnvc_id, @agent_op_expuint_did);

CALL SP__insert_obj_metadata(5, 'expVAST', @amp_namespace_id, @agent_op_expvast);
CALL SP__insert_tnvc_collection('operands for expVAST', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_expvast, 'Int64 exponentiation', 'VAST', 2, @op_tnvc_id, @agent_op_expvast_did);

CALL SP__insert_obj_metadata(5, 'expUVAST', @amp_namespace_id, @agent_op_expuvast);
CALL SP__insert_tnvc_collection('operands for expUVAST', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_expuvast, 'Unsigned Int64 exponentiation', 'UVAST', 2, @op_tnvc_id, @agent_op_expuvast_did);

CALL SP__insert_obj_metadata(5, 'expREAL32', @amp_namespace_id, @agent_op_expreal32);
CALL SP__insert_tnvc_collection('operands for expREAL32', @op_tnvc_id);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real32_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_expreal32, 'Real32 exponentiation', 'REAL32', 2, @op_tnvc_id, @agent_op_expreal32_did);

CALL SP__insert_obj_metadata(5, 'expREAL64', @amp_namespace_id, @agent_op_expreal64);
CALL SP__insert_tnvc_collection('operands for expREAL64', @op_tnvc_id);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_real64_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_expreal64, 'Real64 exponentiation', 'REAL64', 2, @op_tnvc_id, @agent_op_expreal64_did);

CALL SP__insert_obj_metadata(5, 'bitAND', @amp_namespace_id, @agent_op_bitand);
CALL SP__insert_tnvc_collection('operands for bitAND', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_bitand, 'Bitwise and', 'UVAST', 2, @op_tnvc_id, @agent_op_bitand_did);

CALL SP__insert_obj_metadata(5, 'bitOR', @amp_namespace_id, @agent_op_bitor);
CALL SP__insert_tnvc_collection('operands for bitOR', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_bitor, 'Bitwise or', 'UVAST', 2, @op_tnvc_id, @agent_op_bitor_did);

CALL SP__insert_obj_metadata(5, 'bitXOR', @amp_namespace_id, @agent_op_bitxor);
CALL SP__insert_tnvc_collection('operands for bitXOR', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_bitxor, 'Bitwise xor', 'UVAST', 2, @op_tnvc_id, @agent_op_bitxor_did);

CALL SP__insert_obj_metadata(5, 'bitNOT', @amp_namespace_id, @agent_op_bitnot);
CALL SP__insert_tnvc_collection('operands for bitNOT', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_operator_actual_definition(@agent_op_bitnot, 'Bitwise not', 'UVAST', 1, @op_tnvc_id, @agent_op_bitnot_did);

CALL SP__insert_obj_metadata(5, 'logAND', @amp_namespace_id, @agent_op_logand);
CALL SP__insert_tnvc_collection('operands for logAND', @op_tnvc_id);
CALL SP__insert_tnvc_bool_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_bool_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_logand, 'Logical and', 'BOOL', 2, @op_tnvc_id, @agent_op_logand_did);

CALL SP__insert_obj_metadata(5, 'logOR', @amp_namespace_id, @agent_op_logor);
CALL SP__insert_tnvc_collection('operands for logOR', @op_tnvc_id);
CALL SP__insert_tnvc_bool_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_bool_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_logor, 'Logical or', 'BOOL', 2, @op_tnvc_id, @agent_op_logor_did);

CALL SP__insert_obj_metadata(5, 'logNOT', @amp_namespace_id, @agent_op_lognot);
CALL SP__insert_tnvc_collection('operands for logNOT', @op_tnvc_id);
CALL SP__insert_tnvc_bool_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_operator_actual_definition(@agent_op_lognot, 'Logical not', 'BOOL', 1, @op_tnvc_id, @agent_op_lognot_did);

CALL SP__insert_obj_metadata(5, 'abs', @amp_namespace_id, @agent_op_abs);
CALL SP__insert_tnvc_collection('operands for abs', @op_tnvc_id);
CALL SP__insert_tnvc_vast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_operator_actual_definition(@agent_op_abs, 'absolute value', 'UVAST', 1, @op_tnvc_id, @agent_op_abs_did);

CALL SP__insert_obj_metadata(5, 'lessThan', @amp_namespace_id, @agent_op_lessthan);
CALL SP__insert_tnvc_collection('operands for lessThan', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_lessthan, '<', 'BOOL', 2, @op_tnvc_id, @agent_op_lessthan_did);

CALL SP__insert_obj_metadata(5, 'greaterThan', @amp_namespace_id, @agent_op_greaterthan);
CALL SP__insert_tnvc_collection('operands for greaterThan', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_greaterthan, '>', 'BOOL', 2, @op_tnvc_id, @agent_op_greaterthan_did);

CALL SP__insert_obj_metadata(5, 'lessEqual', @amp_namespace_id, @agent_op_lessequal);
CALL SP__insert_tnvc_collection('operands for lessEqual', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_lessequal, '<=', 'BOOL', 2, @op_tnvc_id, @agent_op_lessequal_did);

CALL SP__insert_obj_metadata(5, 'greaterEqual', @amp_namespace_id, @agent_op_greaterequal);
CALL SP__insert_tnvc_collection('operands for greaterEqual', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_greaterequal, '>=', 'BOOL', 2, @op_tnvc_id, @agent_op_greaterequal_did);

CALL SP__insert_obj_metadata(5, 'notEqual', @amp_namespace_id, @agent_op_notequal);
CALL SP__insert_tnvc_collection('operands for notEqual', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_notequal, '!=', 'BOOL', 2, @op_tnvc_id, @agent_op_notequal_did);

CALL SP__insert_obj_metadata(5, 'Equal', @amp_namespace_id, @agent_op_equal);
CALL SP__insert_tnvc_collection('operands for Equal', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_equal, '==', 'BOOL', 2, @op_tnvc_id, @agent_op_equal_did);

CALL SP__insert_obj_metadata(5, 'bitShiftLeft', @amp_namespace_id, @agent_op_bitshiftleft);
CALL SP__insert_tnvc_collection('operands for bitShiftLeft', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_bitshiftleft, '<<', 'UVAST', 2, @op_tnvc_id, @agent_op_bitshiftleft_did);

CALL SP__insert_obj_metadata(5, 'bitShiftRight', @amp_namespace_id, @agent_op_bitshiftright);
CALL SP__insert_tnvc_collection('operands for bitShiftRight', @op_tnvc_id);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 1, null, null, @tnvc_entry1);
CALL SP__insert_tnvc_uvast_entry(@op_tnvc_id, 2, null, null, @tnvc_entry2);
CALL SP__insert_operator_actual_definition(@agent_op_bitshiftright, '>>', 'UVAST', 2, @op_tnvc_id, @agent_op_bitshiftright_did);

CALL SP__insert_obj_metadata(5, 'STOR', @amp_namespace_id, @agent_op_stor);
CALL SP__insert_tnvc_collection('operands for STOR', @op_tnvc_id);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 1, null, @tnvc_entry1);
CALL SP__insert_tnvc_unk_entry(@op_tnvc_id, 2, null, @tnvc_entry2);
-- CALL SP__insert_operator_actual_definition(@agent_op_stor, 'Store value of parm 2 in parm 1', 'UNK', 2, @op_tnvc_id, @agent_op_stor_did);


-- #VAR
-- create ac for expression
CALL SP__insert_ac_id(3, 'ac for the expression used by @agent_var_num_rules', @var_ac_id);
CALL SP__insert_ac_actual_entry(@var_ac_id, @agent_edd_num_tbr_did, 1, @r_ac_entry_id_1 );
CALL SP__insert_ac_actual_entry(@var_ac_id, @agent_edd_num_sbr_did, 2, @r_ac_entry_id_2 );
CALL SP__insert_ac_actual_entry(@var_ac_id, @agent_op_plusuint_did, 3, @r_ac_entry_id_3 );
CALL SP__insert_obj_metadata(12, 'num_rules', @amp_namespace_id, @agent_var_num_rules);
CALL SP__insert_variable_definition(@agent_var_num_rules, 'This is the number of rules known to the Agent (#TBR + #SBR).', 20, @var_ac_id, @agent_var_num_rules_did);

-- #TBLT
CALL SP__insert_obj_metadata(10, 'adms', @amp_namespace_id, @agent_tblt_adms);
CALL SP__insert_tnvc_collection('columns for the adms table', @tbl_tnvc_id);
CALL SP__insert_tnvc_str_entry(@tbl_tnvc_id, 1, 'adm_name', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@agent_tblt_adms, 'This table lists all the adms that are supported by the agent.', @tbl_tnvc_id, @agent_tblt_adms_did);

CALL SP__insert_obj_metadata(10, 'variables', @amp_namespace_id, @agent_tblt_variables);
CALL SP__insert_tnvc_collection('columns for the variables table', @tbl_tnvc_id);
CALL SP__insert_tnvc_ari_entry(@tbl_tnvc_id, 1, 'ids', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@agent_tblt_variables, 'This table lists the ARI for every variable that is known to the agent.', @tbl_tnvc_id, @agent_tblt_variables_did);

CALL SP__insert_obj_metadata(10, 'rptts', @amp_namespace_id, @agent_tblt_rptts);
CALL SP__insert_tnvc_collection('columns for the rptts table', @tbl_tnvc_id);
CALL SP__insert_tnvc_ari_entry(@tbl_tnvc_id, 1, 'ids', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@agent_tblt_rptts, 'This table lists the ARI for every report template that is known to the agent.', @tbl_tnvc_id, @agent_tblt_rptts_did);

CALL SP__insert_obj_metadata(10, 'macros', @amp_namespace_id, @agent_tblt_macros);
CALL SP__insert_tnvc_collection('columns for the macros table', @tbl_tnvc_id);
CALL SP__insert_tnvc_ari_entry(@tbl_tnvc_id, 1, 'ids', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@agent_tblt_macros, 'This table lists the ARI for every macro that is known to the agent.', @tbl_tnvc_id, @agent_tblt_macros_did);

CALL SP__insert_obj_metadata(10, 'rules', @amp_namespace_id, @agent_tblt_rules);
CALL SP__insert_tnvc_collection('columns for the rules table', @tbl_tnvc_id);
CALL SP__insert_tnvc_ari_entry(@tbl_tnvc_id, 1, 'ids', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@agent_tblt_rules, 'This table lists the ARI for every rule that is known to the agent.', @tbl_tnvc_id, @agent_tblt_rules_did);

CALL SP__insert_obj_metadata(10, 'tblts', @amp_namespace_id, @agent_tblt_tblts);
CALL SP__insert_tnvc_collection('columns for the tblts table', @tbl_tnvc_id);
CALL SP__insert_tnvc_ari_entry(@tbl_tnvc_id, 1, 'ids', null, @tnvc_entry);
CALL SP__insert_table_template_actual_definition(@agent_tblt_tblts, 'This table lists the ARI for every table template that is known to the agent.', @tbl_tnvc_id, @agent_tblt_tblts_did);


-- #RPTT

CALL SP__insert_obj_metadata(7, 'full_report', @amp_namespace_id, @agent_rpttpl_full_report);

CALL SP__insert_ac_id(16, 'ac for report template full_report', @rptt_ac_id);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_meta_name_did, 1, @r_ac_rpt_entry_1);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_meta_version_did, 2, @r_ac_rpt_entry_2);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_rpt_tpls_did, 3, @r_ac_rpt_entry_3);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_tbl_tpls_did, 4, @r_ac_rpt_entry_4);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_sent_reports_did, 5, @r_ac_rpt_entry_5);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_tbr_did, 6, @r_ac_rpt_entry_6);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_run_tbr_did, 7, @r_ac_rpt_entry_7);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_sbr_did, 8, @r_ac_rpt_entry_8);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_run_sbr_did, 9, @r_ac_rpt_entry_9);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_const_did, 10, @r_ac_rpt_entry_10);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_var_did, 11, @r_ac_rpt_entry_11);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_macros_did, 12, @r_ac_rpt_entry_12);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_run_macros_did, 13, @r_ac_rpt_entry_13);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_num_controls_did, 14, @r_ac_rpt_entry_14);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_edd_run_controls_did, 15, @r_ac_rpt_entry_15);
CALL SP__insert_ac_actual_entry(@rptt_ac_id, @agent_var_num_rules_did, 16, @r_ac_rpt_entry_16);

CALL SP__insert_report_template_formal_definition(@agent_rpttpl_full_report, 'This is all known meta-data, EDD, and VAR values known by the agent.', null, @rptt_ac_id, @agent_rpttpl_full_report_did);
CALL SP__insert_report_actual_definition(@agent_rpttpl_full_report, null, null, 'Singleton value for full_report', @agent_rpttpl_full_report_aid);

-- #CTRL
CALL SP__insert_obj_metadata(1, 'add_var', @amp_namespace_id, @agent_ctrl_add_var);
CALL SP__insert_formal_parmspec(3, 'parms for the add_var control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'id', 'ARI', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'def', 'EXPR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'type', 'BYTE', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_add_var , 'This control configures a new variable definition on the Agent.', @fp_spec_id, @agent_ctrl_add_var_did);

CALL SP__insert_obj_metadata(1, 'del_var', @amp_namespace_id, @agent_ctrl_del_var);
CALL SP__insert_formal_parmspec(1, 'parms for the del_var control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_del_var , 'This control removes one or more variable definitions from the Agent.', @fp_spec_id, @agent_ctrl_del_var_did);

CALL SP__insert_obj_metadata(1, 'add_rptt', @amp_namespace_id, @agent_ctrl_add_rptt);
CALL SP__insert_formal_parmspec(2, 'parms for the add_rptt control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'id', 'ARI', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'template', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_add_rptt , 'This control configures a new report template definition on the Agent.', @fp_spec_id, @agent_ctrl_add_rptt_did);

CALL SP__insert_obj_metadata(1, 'del_rptt', @amp_namespace_id, @agent_ctrl_del_rptt);
CALL SP__insert_formal_parmspec(1, 'parms for the del_rptt control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_del_rptt , 'This control removes one or more report template definitions from the Agent.', @fp_spec_id, @agent_ctrl_del_rptt_did);

CALL SP__insert_obj_metadata(1, 'desc_rptt', @amp_namespace_id, @agent_ctrl_desc_rptt);
CALL SP__insert_formal_parmspec(1, 'parms for the desc_rptt control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_desc_rptt , 'This control produces a detailed description of one or more report template  identifier(ARI) known to the Agent.', @fp_spec_id, @agent_ctrl_desc_rptt_did);

CALL SP__insert_obj_metadata(1, 'gen_rpts', @amp_namespace_id, @agent_ctrl_gen_rpts);
CALL SP__insert_formal_parmspec(2, 'parms for the gen_rpts control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'rxmgrs', 'TNVC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_gen_rpts , 'This control causes the Agent to produce a report entry for each identified report templates and send them to one or more identified managers(ARIs).', @fp_spec_id, @agent_ctrl_gen_rpts_did);

CALL SP__insert_obj_metadata(1, 'gen_tbls', @amp_namespace_id, @agent_ctrl_gen_tbls);
CALL SP__insert_formal_parmspec(2, 'parms for the gen_tbls control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'rxmgrs', 'TNVC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_gen_tbls , 'This control causes the Agent to produce a table for each identified table templates and send them to one or more identified managers(ARIs).', @fp_spec_id, @agent_ctrl_gen_tbls_did);

CALL SP__insert_obj_metadata(1, 'add_macro', @amp_namespace_id, @agent_ctrl_add_macro);
CALL SP__insert_formal_parmspec(3, 'parms for the add_macro control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'name', 'STR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'id', 'ARI', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'def', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_add_macro , 'This control configures a new macro definition on the Agent.', @fp_spec_id, @agent_ctrl_add_macro_did);

CALL SP__insert_obj_metadata(1, 'del_macro', @amp_namespace_id, @agent_ctrl_del_macro);
CALL SP__insert_formal_parmspec(1, 'parms for the del_macro control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_del_macro , 'This control removes one or more macro definitions from the Agent.', @fp_spec_id, @agent_ctrl_del_macro_did);

CALL SP__insert_obj_metadata(1, 'desc_macro', @amp_namespace_id, @agent_ctrl_desc_macro);
CALL SP__insert_formal_parmspec(1, 'parms for the desc_macro control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_desc_macro , 'This control produces a detailed description of one or more macro identifier(ARI) known to the Agent.', @fp_spec_id, @agent_ctrl_desc_macro_did);

CALL SP__insert_obj_metadata(1, 'add_tbr', @amp_namespace_id, @agent_ctrl_add_tbr);
CALL SP__insert_formal_parmspec(6, 'parms for the add_tbr control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'id', 'ARI', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'start', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'period', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'count', 'UVAST', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 5, 'action', 'AC', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 6, 'description', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_add_tbr , 'This control configures a new time-based rule(TBR) definition on the Agent.', @fp_spec_id, @agent_ctrl_add_tbr_did);

CALL SP__insert_obj_metadata(1, 'add_sbr', @amp_namespace_id, @agent_ctrl_add_sbr);
CALL SP__insert_formal_parmspec(7, 'parms for the add_sbr control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'id', 'ARI', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'start', 'TV', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 3, 'state', 'EXPR', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 4, 'max_eval', 'UVAST', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 5, 'count', 'UVAST', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 6, 'action', 'AC', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 7, 'description', 'STR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_add_sbr , 'This control configures a new state-based rule(SBR) definition on the Agent.', @fp_spec_id, @agent_ctrl_add_sbr_did);

CALL SP__insert_obj_metadata(1, 'del_rule', @amp_namespace_id, @agent_ctrl_del_rule);
CALL SP__insert_formal_parmspec(1, 'parms for the del_rule control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_del_rule , 'This control removes one or more rule definitions from the Agent.', @fp_spec_id, @agent_ctrl_del_rule_did);

CALL SP__insert_obj_metadata(1, 'desc_rule', @amp_namespace_id, @agent_ctrl_desc_rule);
CALL SP__insert_formal_parmspec(1, 'parms for the desc_rule control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'ids', 'AC', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_desc_rule , 'This control produces a detailed description of one or more rules known to the Agent.', @fp_spec_id, @agent_ctrl_desc_rule_did);

CALL SP__insert_obj_metadata(1, 'store_var', @amp_namespace_id, @agent_ctrl_store_var);
CALL SP__insert_formal_parmspec(2, 'parms for the store_var control', @fp_spec_id);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 1, 'id', 'ARI', null, @r_fp_ent);
CALL SP__insert_formal_parmspec_entry(@fp_spec_id, 2, 'value', 'EXPR', null, @r_fp_ent);
CALL SP__insert_control_formal_definition(@agent_ctrl_store_var , 'This control stores variables.', @fp_spec_id, @agent_ctrl_store_var_did);

CALL SP__insert_obj_metadata(1, 'reset_counts', @amp_namespace_id, @agent_ctrl_reset_counts);
CALL SP__insert_control_formal_definition(@agent_ctrl_reset_counts , 'This control resets all Agent ADM statistics reported in the Agent ADM report.', null, @agent_ctrl_reset_counts_did);


-- #CONST
CALL SP__insert_obj_metadata(0, 'amp_epoch', @amp_namespace_id, @agent_cnst_amp_epoch);
CALL SP__insert_const_actual_definition(@agent_cnst_amp_epoch, 'This constant is the time epoch for the Agent.', 'TS', '1504915200', @agent_cnst_amp_epoch_did);


-- #MAC
