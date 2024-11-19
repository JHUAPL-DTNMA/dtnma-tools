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


-- ==================================================================
-- Author: David Linko	
-- 
-- Description:  inserting, updating and removing constant formal and actual definitions
-- using the obj routines
-- 
-- ==================================================================

USE amp_core;


-- ==================================================================
-- SP__insert_const_actual_definition;
-- Parameters:
-- in 
--     p_obj_id int unsigned - id for the object metadata
--      p_use_desc varchar(255) - humanreadable description of the constant 
-- 		p_data_type varchar(255) -  name of the data type for the constant 
--  	p_data_value_string varchar(255) - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id int(10) unsigned id of the actual defintion entry 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_const_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_const_actual_definition(IN p_obj_id int(10) unsigned, p_use_desc varchar(255), p_data_type varchar(255), p_data_value_string varchar(255), OUT r_actual_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_actual_definition_id); 
    Set @data_id  = (SELECT data_type_id FROM amp_core.data_type WHERE type_name  = p_data_type);
    INSERT INTO amp_core.const_actual_definition(obj_actual_definition_id, data_type_id, data_value) VALUES(r_actual_definition_id, @data_id, p_data_value_string); 
END //
DELIMITER ;


-- ==================================================================
-- SP__delete_const_actual_definition
-- cna us either hte name or the id of the constant to delete 
-- Parameters:
-- in 
--  	p_obj_id int unsigned -  id of the constan to delete 
-- 		p_obj_name varchar(255) -   name of the constant to delete
-- ==================================================================
DROP PROCEDURE IF EXiSTS SP__delete_const_actual_definition;
DELIMITER //
CREATE PROCEDURE SP__delete_const_actual_definition(IN p_obj_id int(10) unsigned, p_obj_name VARCHAR(255))
BEGIN
	IF (p_obj_id != null) THEN
		SET @metadata_id = (SELECT obj_metadata_id FROM obj_actual_definition WHERE obj_actual_definition_id = p_obj_id );
	ELSE
		IF (p_obj_name != NULL) THEN 
			SET @metadata_id = (SELECT obj_metadata_id FROM obj_metadata WHERE obj_name = p_obj_name); 
        END IF;
	END IF;
    CALL SP__delete_obj_metadata(@metadata_id);
    
END//
DELIMITER ;
