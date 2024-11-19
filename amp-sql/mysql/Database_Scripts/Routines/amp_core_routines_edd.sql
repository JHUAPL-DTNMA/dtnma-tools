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
-- Description:  inserting, updating and removing EDD formal and actual definitions
-- using the obj routines
-- 
-- ==================================================================

USE amp_core;


-- ==================================================================
-- SP__insert_edd_formal_definition;
-- Parameters:
-- in 
--     p_obj_id int unsigned - id for the object metadata
--      p_use_desc varchar(255) - humanreadable description of the edd 
-- 		p_data_type varchar(255) -  name of the data type for the edd 
--  	p_data_value_string varchar(255) - blob conating the encoded value of the edd
-- out
-- 		r_actual_definition_id int(10) unsigned id of the actual defintion entry 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_edd_formal_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_edd_formal_definition(IN p_obj_id int(10) unsigned, p_use_desc varchar(255), p_fp_spec_id int(10) unsigned, p_external_data_type varchar(255), OUT r_formal_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_formal_definition_id); 
    INSERT INTO amp_core.edd_formal_definition(obj_formal_definition_id, fp_spec_id, data_type_id) VALUES(r_formal_definition_id, p_fp_spec_id, (SELECT data_type_id FROM amp_core.data_type WHERE type_name  = p_external_data_type)); 
END //
DELIMITER ;



-- ==================================================================
-- SP__delete_edd_actual_definition;
-- Parameters:
-- in 
--      p_obj_id int unsigned - id for the edd to delete 
-- 		p_obj_name varchar(255) -  name of the edd  to delete 
--
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__delete_edd_formal_defintion;
DELIMITER // 
CREATE PROCEDURE SP__delete_edd_formal_defintion(IN p_obj_id int(10) unsigned, p_obj_name VARCHAR(255))
edd_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE edd_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_edd_formal where obj_id = p_obj_id);
    END IF ;
    
	-- deleting all instances 
	CALL SP__delete_edd_actual_definition(null, p_obj_name);
    
	SET @def_id = (select obj_formal_definition_id from obj_formal_definition where obj_metadata_id = p_obj_id);
	SET @fp_id = (SELECT fp_spec_id from edd_formal_definition where obj_formal_definition_id = @def_id );
	DELETE FROM formal_parmspec WHERE fp_spec_id = @fp_id;
    
	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
END // 
DELIMITER ;


-- ==================================================================
-- SP__insert_edd_actual_definition;
-- Parameters:
-- in 
--     p_obj_id int unsigned - id for the object metadata
--      p_use_desc varchar(255) - humanreadable description of the constant 
-- 		p_data_type varchar(255) -  name of the data type for the constant 
--  	p_data_value_string varchar(255) - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id int(10) unsigned id of the actual defintion entry 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_edd_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_edd_actual_definition(IN p_obj_definition_id int(10) unsigned, p_use_desc varchar(255), p_ap_spec_id int(10) unsigned,  OUT r_actual_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_definition_id); 
    INSERT INTO amp_core.edd_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_actual_definition_id, p_ap_spec_id);  
END // 
DELIMITER ; 


-- ==================================================================
-- SP__delete_edd_actual_definition;
-- Parameters:
-- in 
--     p_obj_id int unsigned - id for the object metadata
--      p_use_desc varchar(255) - humanreadable description of the constant 
-- 		p_data_type varchar(255) -  name of the data type for the constant 
--  	p_data_value_string varchar(255) - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id int(10) unsigned id of the actual defintion entry 
-- ==================================================================
-- for instance can supply the definiton name to remove all the instances of that definition or can remove 
DROP PROCEDURE IF EXISTS SP__delete_edd_actual_definition;
DELIMITER //
CREATE PROCEDURE SP__delete_edd_actual_definition(IN p_actual_definition_id int(10) unsigned, p_obj_name varchar(255))
edd_actual_definition_del:BEGIN
	
	DECLARE done INT DEFAULT FALSE;
    DECLARE actual_definition_id_hold, ap_spec_id_hold int(10) unsigned;
    DECLARE actual_definition_cursor CURSOR
			FOR SELECT actual_definition_id, ap_spec_id FROM vw_edd_actual WHERE obj_name = p_obj_name;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;	
    
    -- only one thing to remove 
	If( p_inst_id is not null) then 
		SET @ap_id = (SELECT ap_spec_id from edd_actual_definition where actual_definition_id = p_inst_id );
		IF( @ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = @ap_id;
		END IF;
		DELETE FROM edd_actual_definition 
WHERE
    intance_id = p_actual_definition_id;
		CALL SP__delete_obj_actual_definition(p_actual_definition_id, p_obj_name);
	ELSE -- removing all instances with this name 
		IF( p_obj_name is null) then -- nothing to do 
			leave edd_actual_definition_del;
		END IF;
		OPEN actual_definition_cursor;
        read_loop: LOOP
			FETCH NEXT FROM actual_definition_cursor INTO 
				actual_definition_id_hold, ap_spec_id_hold;
			IF done THEN
				LEAVE read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;

			DELETE FROM edd_actual_definition
WHERE
    amp_core.edd_actual_definition.actual_definition_id = actual_definition_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_definition_cursor;
END // 
DELIMITER ;
