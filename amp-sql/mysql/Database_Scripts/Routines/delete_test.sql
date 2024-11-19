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
# for testing the delete function 

USE amp_core;

set @p_obj_name = 'bundles_by_priority';
select * from obj_definition join obj_identifier on obj_definition.obj_id = obj_identifier.obj_id;
-- select definition_id from obj_definition where obj_id = (SELECT obj_id FROM amp_core.obj_identifier WHERE obj_name = @p_obj_name);

-- set  @fp_spec_id_control_definitionobj_definitionformal_parmspecedd1 = (select fp_spec_id from amp_core.edd_definition where definition_id = (select definition_id from obj_definition where obj_id = (SELECT obj_id FROM amp_core.obj_identifier WHERE obj_name = @p_obj_name)));
-- CALL SP__insert_actual_parms_set(1, @fp_spec_id_edd1, 'UINT', '1', @ap_spec_id); 
-- CALL SP__insert_edd_instance(@edd_definition_id_9, @ap_spec_id,NULL, @edd_inst_id_1);


-- Select * from amp_core.obj_definition;
-- SELECT obj_id FROM amp_core.obj_identifier WHERE obj_name = @p_obj_name;
-- SELECT * from amp_core.obj_definition where obj_id = (SELECT obj_id FROM amp_core.obj_identifier WHERE obj_name = @p_obj_name);

SELECT * FROM vw_edd_instance;
-- CALL SP__delete_obj_definition(0, null, @p_obj_name);
call SP__delete_edd_instance(null, 'bundles_by_priority');
-- CALL SP__delete_edd_instance(null, @p_o-- bj_name);

SELECT * FROM vw_edd_instance;


