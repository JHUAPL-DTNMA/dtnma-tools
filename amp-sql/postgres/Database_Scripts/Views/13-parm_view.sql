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
	STRING_AGG(PARM_NAME::CHARACTER varying, ',' ORDER BY order_num)  AS PARM_NAMES,
	STRING_AGG(TYPE_NAME::CHARACTER varying, ',' ORDER BY order_num)  AS PARM_TYPE_NAME,
	STRING_AGG(OBJ_ACTUAL_DEFINITION_ID::CHARACTER varying, ',' ORDER BY order_num)  AS PARM_VALUE_IDS
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
	STRING_AGG(fp_id::CHARACTER varying, ',' ORDER BY order_num) AS FP_VALUES,
	STRING_AGG(STR_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS STR_VALUES,
	STRING_AGG(UINT_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS UINT_VALUES,
	STRING_AGG(INT_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS INT_VALUES,
	STRING_AGG(OBJ_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS OBJ_VALUES,
	STRING_AGG(AC_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS AC_VALUES,
	STRING_AGG(TNVC_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS TNVC_VALUES,
	STRING_AGG(REAL32_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS REAL32_VALUES,
	STRING_AGG(REAL64_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS REAL64_VALUES,
	STRING_AGG(UVAST_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS UVAST_VALUES,
	STRING_AGG(VAST_VALUE::CHARACTER varying, ',' ORDER BY order_num) AS VAST_VALUES
FROM ACTUAL_PARMSPEC AP
INNER JOIN VW_TNVC_ENTRIES TNVC ON AP.TNVC_ID = TNVC.TNVC_ID
GROUP BY AP.TNVC_ID,
	AP.AP_SPEC_ID,
	FP_SPEC_ID,
	USE_DESC