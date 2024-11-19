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

-- views for parameters

CREATE OR REPLACE VIEW VW_FORMAL_PARAMETERS AS
SELECT FP.FP_SPEC_ID,
	NUM_PARMS,
	FPS.USE_DESC,
	GROUP_CONCAT(PARM_NAME) AS PARM_NAMES,
	GROUP_CONCAT(TYPE_NAME) AS PARM_TYPE_NAME,
	GROUP_CONCAT(OBJ_ACTUAL_DEFINITION_ID) AS PARM_VALUE_IDS
FROM FORMAL_PARM FP
JOIN FORMAL_PARMSPEC FPS ON FP.FP_SPEC_ID = FPS.FP_SPEC_ID
JOIN DATA_TYPE DT ON FP.DATA_TYPE_ID = DT.DATA_TYPE_ID
GROUP BY FP.FP_SPEC_ID,
	NUM_PARMS,
	FPS.USE_DESC ;

-- actual_parmspec view

CREATE OR REPLACE VIEW VW_ACTUAL_PARAMETERS AS
SELECT AP_SPEC_ID,
	AP.TNVC_ID,
	FP_SPEC_ID,
	USE_DESC,
	GROUP_CONCAT(fp_id) AS FP_VALUES,
	GROUP_CONCAT(STR_VALUE) AS STR_VALUES,
	GROUP_CONCAT(UINT_VALUE) AS UINT_VALUES,
	GROUP_CONCAT(INT_VALUE) AS INT_VALUES,
	GROUP_CONCAT(OBJ_VALUE) AS OBJ_VALUES,
	GROUP_CONCAT(AC_VALUE) AS AC_VALUES,
	GROUP_CONCAT(TNVC_VALUE) AS TNVC_VALUES,
	GROUP_CONCAT(REAL32_VALUE) AS REAL32_VALUES,
	GROUP_CONCAT(REAL64_VALUE) AS REAL64_VALUES,
	GROUP_CONCAT(UVAST_VALUE) AS UVAST_VALUES,
	GROUP_CONCAT(VAST_VALUE) AS VAST_VALUES
FROM ACTUAL_PARMSPEC AP
INNER JOIN VW_TNVC_ENTRIES TNVC ON AP.TNVC_ID = TNVC.TNVC_ID
GROUP BY AP.TNVC_ID,
	AP.AP_SPEC_ID,
	FP_SPEC_ID,
	USE_DESC