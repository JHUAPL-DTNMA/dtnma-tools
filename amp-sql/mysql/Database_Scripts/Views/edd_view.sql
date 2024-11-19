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
CREATE OR REPLACE VIEW `vw_edd_formal` AS
    SELECT 
        obj_metadata.obj_metadata_id,
        obj_name,
        namespace_id,
        obj_formal_definition_id,
        fp_spec_id,
        use_desc
    FROM
        obj_metadata
            JOIN
        (SELECT 
            obj_formal_definition.obj_formal_definition_id, obj_metadata_id, use_desc, fp_spec_id
        FROM
            obj_formal_definition
        JOIN edd_formal_definition ON edd_formal_definition.obj_formal_definition_id = 
        obj_formal_definition.obj_formal_definition_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id;


CREATE OR REPLACE VIEW `vw_edd_actual` AS
    SELECT 
        obj_metadata.obj_metadata_id,
        obj_name,
        namespace_id,
        obj_formal_definition_id,
        obj_actual_definition_id,
        ap_spec_id,
        use_desc
    FROM
        obj_metadata
            INNER JOIN
        (SELECT 
            obj_formal_definition.obj_formal_definition_id,
                obj_formal_definition.obj_metadata_id,
                view1.use_desc,
                obj_actual_definition_id,
                ap_spec_id
        FROM
            obj_formal_definition
        JOIN (SELECT 
            obj_actual_definition.obj_actual_definition_id,
			obj_actual_definition.obj_metadata_id,
                use_desc,
                ap_spec_id
        FROM
            amp_core.obj_actual_definition
        JOIN amp_core.edd_actual_definition ON obj_actual_definition.obj_actual_definition_id = edd_actual_definition.obj_actual_definition_id) AS view1 ON view1.obj_metadata_id = amp_core.obj_formal_definition.obj_metadata_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id;