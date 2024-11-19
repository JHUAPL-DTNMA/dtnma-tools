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
CREATE OR REPLACE VIEW vw_ctrl_definition AS
 SELECT joinlast.obj_metadata_id,
    joinlast.obj_name,
    joinlast.namespace_id,
    joinlast.adm_name,
    joinlast.obj_formal_definition_id,
    joinlast.fp_spec_id,
    joinlast.num_parms,
    string_agg(formal_parm.data_type_id::character varying::text, ','::text) AS parm_types,
    string_agg(formal_parm.parm_name::text, ','::text) AS parm_names,
    string_agg(formal_parm.obj_actual_definition_id::character varying::text, ','::text) AS parm_defaults,
    joinlast.use_desc
   FROM formal_parm
     JOIN ( SELECT join3.obj_metadata_id,
            join3.obj_name,
            join3.namespace_id,
            join3.adm_name,
            join3.obj_formal_definition_id,
            formal_parmspec.fp_spec_id,
            formal_parmspec.num_parms,
            join3.use_desc
           FROM formal_parmspec
             JOIN ( SELECT obj_metadata.obj_metadata_id,
                    obj_metadata.obj_name,
                    obj_metadata.namespace_id,
                   obj_metadata.adm_name,
                    join2.obj_formal_definition_id,
                    join2.fp_spec_id,
                    join2.use_desc
                   FROM vw_obj_metadata as obj_metadata
                     JOIN ( SELECT obj_formal_definition.obj_formal_definition_id,
                            obj_formal_definition.obj_metadata_id,
                            obj_formal_definition.use_desc,
                            control_formal_definition.fp_spec_id
                           FROM obj_formal_definition
                             JOIN control_formal_definition ON control_formal_definition.obj_formal_definition_id = obj_formal_definition.obj_formal_definition_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id) join3 ON formal_parmspec.fp_spec_id = join3.fp_spec_id) joinlast ON formal_parm.fp_spec_id = joinlast.fp_spec_id
  GROUP BY joinlast.fp_spec_id, joinlast.obj_metadata_id, joinlast.obj_name, joinlast.namespace_id, joinlast.adm_name, joinlast.obj_formal_definition_id, joinlast.num_parms, joinlast.use_desc;


CREATE OR REPLACE VIEW vw_ctrl_actual AS
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
          FROM obj_actual_definition
          JOIN control_actual_definition ON obj_actual_definition.obj_actual_definition_id = control_actual_definition.obj_actual_definition_id) AS view1 ON view1.obj_metadata_id = obj_formal_definition.obj_metadata_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id;