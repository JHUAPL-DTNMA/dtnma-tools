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
use amp_core;

CREATE OR REPLACE VIEW vw_obj_metadata AS
       SELECT om.*, adm.adm_name, adm.adm_enum, adm.adm_enum_label, adm.use_desc
       FROM obj_metadata om
       LEFT JOIN adm ON adm.namespace_id=om.namespace_id;
CREATE OR REPLACE VIEW vw_obj_formal_def AS
       SELECT vom.*, ofd.obj_formal_definition_id, ofd.use_desc AS "formal_desc", ofd.obj_enum
       FROM obj_formal_definition ofd
       LEFT JOIN vw_obj_metadata vom ON ofd.obj_metadata_id=vom.obj_metadata_id;
CREATE OR REPLACE VIEW vw_obj_actual_def AS
       SELECT VOM.OBJ_METADATA_ID,
	DT.TYPE_NAME,
	VOM.OBJ_NAME,
	VOM.NAMESPACE_ID,
	VOM.ADM_NAME,
	VOM.ADM_ENUM,
	VOM.ADM_ENUM_LABEL,
	VOM.USE_DESC,
	VOM.OBJ_ACTUAL_DEFINITION_ID,
	VOM.ACTUAL_DESC
FROM
	(SELECT VOM.OBJ_METADATA_ID,
			VOM.DATA_TYPE_ID,
			VOM.OBJ_NAME,
			VOM.NAMESPACE_ID,
			VOM.ADM_NAME,
			VOM.ADM_ENUM,
			VOM.ADM_ENUM_LABEL,
			VOM.USE_DESC,
			OAD.OBJ_ACTUAL_DEFINITION_ID,
			OAD.USE_DESC AS ACTUAL_DESC
		FROM OBJ_ACTUAL_DEFINITION OAD
		LEFT JOIN VW_OBJ_METADATA VOM ON OAD.OBJ_METADATA_ID = VOM.OBJ_METADATA_ID) VOM
LEFT JOIN DATA_TYPE DT ON VOM.DATA_TYPE_ID = DT.DATA_TYPE_ID;