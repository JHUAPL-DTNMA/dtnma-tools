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

-- STORED PROCEDURE(S) for adding updating and deleting time base rule defintions and instances 

-- =================
-- for adding state based rules into the database 
-- SP__insert_sbr_actual_definition 
-- IN 
-- 		p_obj_id int(10) unsigned - metadata id of this SBR
-- 		p_use_desc varchar(255)- human readable description
-- 		p_expr_id int(10) UNSIGNED, - id of the expresion for this rule
-- 		p_ac_id int(10) UNSIGNED, - id of the ari collection that defines the action of this rule 
-- 		p_start_time time - whem this rule starts 
-- OUT 
-- 		r_definition_id int(10) unsigned - id of the start 
-- ====================================
DROP PROCEDURE IF EXISTS SP__insert_sbr_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_sbr_actual_definition(IN p_obj_id int(10) unsigned, p_use_desc varchar(255),  p_expr_id int(10) UNSIGNED, p_ac_id int(10) UNSIGNED, p_start_time time , OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.sbr_actual_definition(obj_actual_definition_id, expression_id, run_count, start_time, ac_id) VALUES(r_definition_id, p_expr_id, p_run_count, p_start_time, p_ac_id);
END //
DELIMITER ;


-- ==================================================================
-- SP__delete_sbr_actual_definition;
-- Parameters:
-- in 
--      p_obj_id int unsigned - id for the sbr to delete 
-- 		p_obj_name varchar(255) -  name of the sbr to delete --
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__delete_sbr_actual_definition;
DELIMITER // 
CREATE PROCEDURE SP__delete_sbr_actual_definition(IN p_obj_id int(10) unsigned, p_obj_name VARCHAR(255))
sbr_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE sbr_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_sbr_formal where obj_id = p_obj_id);
    END IF ;
    
	SET @exp_id = (SELECT expression_id FROM vw_sbr_actual WHERE obj_actual_definition = p_obj_id); 
	SET @ac_id = (SELECT ac_id FROM vw_sbr_actual WHERE obj_actual_definition = p_obj_id); 
	
    DELETE FROM ari_collection WHERE ac_id = @ac_id;
    DELETE FROM expression WHERE expression_id = @exp_id;
	
    CALL SP__delete_obj_atual_defintion(p_obj_id, p_obj_name);
END //
DELIMITER ;
