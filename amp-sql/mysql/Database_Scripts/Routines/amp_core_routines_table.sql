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
USE amp_core;

-- STORED PROCEDURE(S) for adding deleting and updating Table template defintions and instances 


-- ==================================================================
-- SP__insert_table_template_actual_definition
-- IN
-- 		p_obj_id int(10) unsigned - obj_metadata id
-- 		p_use_desc varchar(255) - human readble use desc
-- 		p_num_columns int(10) unsigned -  number of columns in the table
-- 		p_column_names_list varchar(10000) -  list of column names 
-- 		p_column_types_list varchar(10000) - list of column type
-- OUT 
-- 		r_definition_id int(10) unsigned - id of the the new table
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_table_template_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_table_template_actual_definition(IN p_obj_id int(10) unsigned, p_use_desc varchar(255), p_columns_id int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
    INSERT INTO amp_core.table_template_actual_definition(obj_actual_definition_id, tnvc_id) VALUES(r_definition_id, p_columns_id); 
END //
DELIMITER ;

-- ==================================================================
-- SP__insert_table_template_actual_definition 
-- IN 
-- 		p_obj_id int(10) unsigned - id of the table to delete 
-- 		p_obj_name varchar(255) -- name of the object to delete 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__delete_table_template_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__delete_table_template_actual_definition(IN p_obj_id int(10) unsigned)
BEGIN
	IF p_obj_id != NULL THEN BEGIN
		SET @tnvc_id = (select tnvc_id from table_template_actual_definition where obj_actual_definition_id = p_obj_id);
	END;
	ELSE BEGIN
		SET @tnvc_id = 
			(select tnvc_id from table_template_actual_definition where obj_atual_definition_id = 
				(select obj_actual_definition_id  from obj_actual_definition where obj_metadata_id = 
					(select obj_metadata_id from obj_metadata where obj_name = p_obj_name)));
	END;
   END IF;
   CALL SP__delete_tnvc(@tnvc_id);
   CALL SP__delete_obj_actual_definition(p_obj_id, null);
   
END //
DELIMITER ;



-- DROP PROCEDURE IF EXISTS SP__insert_table_instance; 
-- DELIMITER //
-- -- table_definition_id,  use_desc, 
-- CREATE PROCEDURE SP__insert_table_instance(IN p_obj_definition_id int(10) unsigned,  p_use_desc varchar(255), p_row_values_list varchar(10000), OUT r_instance_id int(10) unsigned)
-- BEGIN
-- 	
--     -- have to visit how to store multiple rows is is just one long value collection splitting at every num_colmns?
-- 	CALL SP__insert_obj_instance(p_obj_definition_id, p_use_desc, r_instance_id); 
--     SET @n_rows = (select num_entries from amp_core.type_name_value_collection where tnvc_id =(select columns_list from amp_core.table_template_definition where definition_id = p_obj_definition_id));
--     CALL  SP__insert_tnv_collection(@num_rows, null, null, p_row_values_list , @tnvc_id);  
--     INSERT INTO amp_core.table_instance(instance_id, ap_spec_id) VALUES(r_instance_id, p_ap_spec_id);   
-- END //
-- DELIMITER ;
