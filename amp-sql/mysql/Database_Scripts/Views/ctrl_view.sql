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


CREATE OR REPLACE VIEW `vw_ctrl_definition` AS
SELECT OBJ_METADATA_ID,
       OBJ_NAME,
       NAMESPACE_ID,
       adm_name,
       OBJ_FORMAL_DEFINITION_ID,
       JOINLAST.FP_SPEC_ID,
       NUM_PARMS,
       GROUP_CONCAT(DATA_TYPE_ID) AS PARM_TYPES, -- array of parm types
 GROUP_CONCAT(PARM_NAME) AS PARM_NAMES, -- array of parm names
 GROUP_CONCAT(OBJ_ACTUAL_DEFINITION_ID) AS PARM_DEFAULTS, -- array of parm default values
 JOINLAST.USE_DESC
FROM FORMAL_PARM
INNER JOIN
    (SELECT OBJ_METADATA_ID,
            OBJ_NAME,
            NAMESPACE_ID,
            adm_name,
            OBJ_FORMAL_DEFINITION_ID,
            FORMAL_PARMSPEC.FP_SPEC_ID,
            NUM_PARMS, -- array of parm types
 JOIN3.USE_DESC
     FROM FORMAL_PARMSPEC
     INNER JOIN
         (SELECT OBJ_METADATA.OBJ_METADATA_ID,
                 OBJ_NAME,
                 NAMESPACE_ID,
                 adm_name,
                 OBJ_FORMAL_DEFINITION_ID,
                 FP_SPEC_ID,
                 JOIN2.USE_DESC
          FROM VW_OBJ_METADATA as OBJ_METADATA
          JOIN
              (SELECT OBJ_FORMAL_DEFINITION.OBJ_FORMAL_DEFINITION_ID,
                      OBJ_METADATA_ID,
                     OBJ_FORMAL_DEFINITION.USE_DESC,
                      FP_SPEC_ID
               FROM OBJ_FORMAL_DEFINITION
               JOIN CONTROL_FORMAL_DEFINITION ON CONTROL_FORMAL_DEFINITION.OBJ_FORMAL_DEFINITION_ID = OBJ_FORMAL_DEFINITION.OBJ_FORMAL_DEFINITION_ID) JOIN2 ON JOIN2.OBJ_METADATA_ID = OBJ_METADATA.OBJ_METADATA_ID) AS JOIN3 ON FORMAL_PARMSPEC.FP_SPEC_ID = JOIN3.FP_SPEC_ID) AS JOINLAST ON FORMAL_PARM.FP_SPEC_ID = JOINLAST.FP_SPEC_ID
GROUP BY JOINLAST.FP_SPEC_ID,
         OBJ_METADATA_ID,
         OBJ_NAME,
         NAMESPACE_ID,
         adm_name,
         OBJ_FORMAL_DEFINITION_ID,
         NUM_PARMS,
         JOINLAST.USE_DESC;


CREATE OR REPLACE VIEW `vw_ctrl_actual` AS
SELECT obj_metadata.obj_metadata_id,
       obj_name,
       namespace_id,
       obj_formal_definition_id,
       obj_actual_definition_id,
       ap_spec_id,
       use_desc
FROM obj_metadata
INNER JOIN
    (SELECT obj_formal_definition.obj_formal_definition_id,
            obj_formal_definition.obj_metadata_id,
            view1.use_desc,
            obj_actual_definition_id,
            ap_spec_id
     FROM obj_formal_definition
     JOIN
         (SELECT obj_actual_definition.obj_actual_definition_id,
                 obj_actual_definition.obj_metadata_id,
                 use_desc,
                 ap_spec_id
          FROM amp_core.obj_actual_definition
          JOIN amp_core.control_actual_definition ON obj_actual_definition.obj_actual_definition_id = control_actual_definition.obj_actual_definition_id) AS view1 ON view1.obj_metadata_id = amp_core.obj_formal_definition.obj_metadata_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id;