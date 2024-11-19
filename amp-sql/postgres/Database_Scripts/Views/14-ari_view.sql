--
-- Copyright (c) 2023 The Johns Hopkins University Applied Physics
-- Laboratory LLC.
--
-- This file is part of the Asynchronous Network Management System (ANMS).
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--     http://www.apache.org/licenses/LICENSE-2.0
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- This work was performed for the Jet Propulsion Laboratory, California
-- Institute of Technology, sponsored by the United States Government under
-- the prime contract 80NM0018D0004 between the Caltech and NASA under
-- subcontract 1658085.
--
CREATE OR REPLACE VIEW vw_ari_union as 
SELECT ACTUAL_ARI.OBJ_METADATA_ID,
	ACTUAL_ARI.OBJ_NAME,
	AD.ADM_NAME,
	AD.TYPE_NAME,
	ACTUAL_ARI.NAMESPACE_ID,
	ACTUAL_ARI.OBJ_ACTUAL_DEFINITION_ID as obj_id,
	ACTUAL_ARI.AP_SPEC_ID AS PARM_ID,
	TRUE AS ACTUAL
FROM
	(SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			AP_SPEC_ID
		FROM VW_EDD_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			AP_SPEC_ID
		FROM VW_MAC_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			NULL AS OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			NULL AS AP_SPEC_ID
		FROM VW_OPER_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			AP_SPEC_ID
		FROM VW_RPT_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			NULL AS OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			NULL AS AP_SPEC_ID
		FROM VW_SBR_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			NULL AS OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			NULL AS AP_SPEC_ID
		FROM VW_TBLT_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			NULL AS OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			NULL AS AP_SPEC_ID
		FROM VW_TBR_ACTUAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			NULL AS OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			NULL AS AP_SPEC_ID
		FROM VW_VAR_ACTUAL
		) AS ACTUAL_ARI
INNER JOIN VW_OBJ_ACTUAL_DEF AD ON AD.OBJ_ACTUAL_DEFINITION_ID = ACTUAL_ARI.OBJ_ACTUAL_DEFINITION_ID
UNION ALL
SELECT FORMAL_ARI.OBJ_METADATA_ID,
	FORMAL_ARI.OBJ_NAME,
	FD.ADM_NAME,
	FD.TYPE_NAME,
	FORMAL_ARI.NAMESPACE_ID,
	FORMAL_ARI.OBJ_FORMAL_DEFINITION_ID as obj_id,
	FORMAL_ARI.FP_SPEC_ID AS PARM_ID,
	FALSE AS ACTUAL
FROM
	(SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
		FROM VW_CTRL_DEFINITION
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
		FROM VW_EDD_FORMAL
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
		FROM VW_MAC_DEFINITION
		UNION ALL SELECT OBJ_METADATA_ID,
			OBJ_NAME,
			NAMESPACE_ID,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
		FROM VW_RPTT_FORMAL) FORMAL_ARI
INNER JOIN VW_OBJ_FORMAL_DEF FD ON FD.OBJ_FORMAL_DEFINITION_ID = FORMAL_ARI.OBJ_FORMAL_DEFINITION_ID;


CREATE OR REPLACE VIEW vw_ari AS SELECT 
   o.obj_actual_definition_id,
   o.obj_metadata_id, o.use_desc, 
-- vof.type_name, 
vof.obj_name, 
ns.*, vof.adm_enum, vof.obj_enum,
 vof.use_desc AS type_desc, vof.adm_name AS ns_desc, -- Note: desc are for debug only
  ate.Enumeration AS adm_type,
  COALESCE(aps.tnvc_id, oad.tnvc_id) AS tnvc_id,
  aps.fp_spec_id,
  COALESCE(cad.ap_spec_id, ead.ap_spec_id, mad.ap_spec_id) AS ap_spec_id,
  COALESCE(lad.data_type_id, lcad.data_type_id, vof.data_type_id, oad.data_type_id, vad.data_type_id) AS data_type_id, -- Data Type comes from exactly on eof these (VERIFY)
  COALESCE(lcad.data_value, lad.data_value) AS data_value,
  oad.num_operands, -- TODO: This field may be redundant vs tnvc_id length
  vad.expression_id
    FROM obj_actual_definition o
    LEFT JOIN vw_obj_formal_def vof ON o.obj_metadata_id=vof.obj_metadata_id
--    LEFT JOIN obj_metadata om ON o.obj_metadata_id=om.obj_metadata_id
--    LEFT JOIN data_type dt ON om.data_type_id=dt.data_type_id
--    LEFT JOIN adm ON adm.namespace_id = om.namespace_id
    LEFT JOIN namespace ns ON ns.namespace_id=vof.namespace_id
    LEFT JOIN ADM_Type_Enumeration ate ON ate.data_type_id = vof.data_type_id

-- Types specifying actual_parmspec
    LEFT JOIN control_actual_definition cad ON cad.obj_actual_definition_id = o.obj_actual_definition_id
    LEFT JOIN edd_actual_definition ead ON ead.obj_actual_definition_id = o.obj_actual_definition_id
    LEFT JOIN macro_actual_definition mad ON mad.obj_actual_definition_id = o.obj_actual_definition_id

    LEFT JOIN actual_parmspec aps ON aps.ap_spec_id = COALESCE(cad.ap_spec_id, ead.ap_spec_id, mad.ap_spec_id)
    
    LEFT JOIN literal_actual_definition lad ON lad.obj_actual_definition_id = o.obj_actual_definition_id
    LEFT JOIN const_actual_definition lcad ON lcad.obj_actual_definition_id = o.obj_actual_definition_id
    LEFT JOIN operator_actual_definition oad ON oad.obj_actual_definition_id = o.obj_actual_definition_id
    LEFT JOIN variable_actual_definition vad ON vad.obj_actual_definition_id = o.obj_actual_definition_id
;


CREATE OR REPLACE VIEW vw_ac AS SELECT ac.*, ari.*
	FROM ari_collection_entry ac
    LEFT JOIN ari_collection_actual_entry ace ON ace.ac_entry_id=ac.ac_entry_id
    LEFT JOIN vw_ari ari ON ari.obj_actual_definition_id=ace.obj_actual_definition_id
    ORDER BY ac.ac_id DESC, ac.order_num ASC;

CREATE OR REPLACE VIEW vw_tnvc_entries AS SELECT e.*, eint.entry_value AS int_value, euint.entry_value AS uint_value, eobj.obj_actual_definition_id AS obj_value, estr.entry_value as str_value, eac.ac_id AS ac_value, etnvc.tnvc_id AS tnvc_value, ereal32.entry_value as real32_value, ereal64.entry_value as real64_value, euvast.entry_value as uvast_value, evast.entry_value as vast_value
                     FROM type_name_value_entry e 
                     LEFT JOIN type_name_value_uint_entry euint ON euint.tnv_id=e.tnv_id
                     LEFT JOIN type_name_value_int_entry eint ON eint.tnv_id=e.tnv_id
                     LEFT JOIN type_name_value_obj_entry eobj ON eobj.tnv_id=e.tnv_id
                     LEFT JOIN type_name_value_string_entry estr ON estr.tnv_id=e.tnv_id
                     LEFT JOIN type_name_value_ac_entry eac ON eac.tnv_id=e.tnv_id
                     LEFT JOIN type_name_value_tnvc_entry etnvc ON etnvc.tnv_id=e.tnv_id  
                     LEFT JOIN type_name_value_real32_entry ereal32 ON ereal32.tnv_id=e.tnv_id  
                     LEFT JOIN type_name_value_real64_entry ereal64 ON ereal64.tnv_id=e.tnv_id
                     LEFT JOIN type_name_value_uvast_entry euvast ON euvast.tnv_id=e.tnv_id  
                     LEFT JOIN type_name_value_vast_entry evast ON evast.tnv_id=e.tnv_id   
ORDER BY order_num ASC;
