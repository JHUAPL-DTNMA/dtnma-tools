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
-- Description:  inserting, updating and removing object formal and actual defintion. The backbone for the DB and all the objects that make it up. 
-- since deletes concasde if you remove the obj instance or definiton you remove that entity from the DB 
-- ==================================================================


USE amp_core;


-- ==================================================================
-- Parameters:
-- in 
-- 		p_obj_type varchar(255) - object of the type
--      p_obj_name varchar(255) -  human readable name of the new object
--      p_namespace_id int unsigned - namespace this object belongs to
-- out 
-- 		r_obj_id int unsigned - id of the new object in the database
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_obj_metadata; 
DELIMITER //
CREATE PROCEDURE SP__insert_obj_metadata(IN p_obj_type_id INT unsigned, p_obj_name varchar(255), p_namespace_id int unsigned, OUT r_obj_id int unsigned)
BEGIN

   SELECT obj_metadata_id INTO r_obj_id
              FROM amp_core.obj_metadata
              WHERE data_type_id=p_obj_type_id AND obj_name=p_obj_name AND namespace_id=p_namespace_id;

    IF (r_obj_id IS NULL) THEN
    	INSERT INTO amp_core.obj_metadata(data_type_id, obj_name, namespace_id) VALUES(p_obj_type_id, p_obj_name, p_namespace_id); 
            SET r_obj_id = LAST_INSERT_ID();
    END IF;

END //
DELIMITER ;

-- ==================================================================
-- Parameters:
-- in 
-- 		p_obj_id INT UNSIGNED - 
-- 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__delete_obj_metadata;
DELIMITER //
CREATE PROCEDURE SP__delete_obj_metadata(IN p_obj_id INT UNSIGNED)
BEGIN 
	DELETE FROM obj_metadata
WHERE obj_metadata_id = p_obj_id;

END //
DELIMITER ;



-- ==================================================================
-- Parameters:
-- in  
-- 		p_obj_metadata_id int unsigned - 
-- 		p_use_desc varchar(255) - 
-- out 
-- 		r_formal_id int unsigned - 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_obj_formal_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_obj_formal_definition(IN p_obj_metadata_id int unsigned, p_use_desc varchar(255), OUT r_formal_id int unsigned)
BEGIN

    -- Get the next available ID for automatic enumeration of new formal definitions
    SELECT data_type_id, adm_enum INTO @data_type, @adm_enum FROM vw_obj_metadata WHERE obj_metadata_id=p_obj_metadata_id;
    SELECT COALESCE(MAX(vof.obj_enum)+1,0) INTO @obj_enum FROM vw_obj_formal_def vof WHERE vof.data_type_id=@data_type AND vof.adm_enum=@adm_enum;      

	INSERT INTO amp_core.obj_formal_definition(obj_metadata_id, use_desc, obj_enum) VALUES(p_obj_metadata_id, p_use_desc, @obj_enum);
    
    SET r_formal_id = LAST_INSERT_ID();
END //
DELIMITER ;



-- ==================================================================
-- Parameters:
-- in IN p_obj_metadata_id int unsigned, p_use_desc varchar(255), OUT r_actual_id int unsigned)
--
-- out 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_obj_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_obj_actual_definition(IN p_obj_metadata_id int unsigned, p_use_desc varchar(255), OUT r_actual_id int unsigned)
BEGIN
	INSERT INTO amp_core.obj_actual_definition(obj_metadata_id, use_desc) VALUES(p_obj_metadata_id, p_use_desc); 
    SET r_actual_id = LAST_INSERT_ID();
END //
DELIMITER ;

-- ==================================================================
-- can use name or id for to delete 
-- Parameters:
-- in 
-- 		p_obj_id int unsigned - Id of the specific obj to delete 
-- 		p_obj_name varchar(255) - name of the obj to delete 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__delete_obj_formal_definition; 
DELIMITER // 
CREATE PROCEDURE SP__delete_obj_formal_definition(p_obj_id int unsigned, p_obj_name varchar(255) )
obj_delete:BEGIN
	IF( p_obj_id is NULL AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE obj_delete;
    END IF;
    
	DELETE FROM amp_core.obj_metadata WHERE obj_id = p_obj_id;
END//
DELIMITER ;
 

-- ==================================================================
-- Parameters:
-- in 
-- 		p_act_id int UNSIGNED
--  	p_obj_name varchar(255)
-- 		p_namespace_id int UNSIGNED 
--
-- ==================================================================
-- for just removing the obj instance 
-- two options for removal if you want to remove a specific instance you can specify the instance id, 
-- or if you want to remove all the instances of a specific definition you can supply the definition name
-- TODO: can add another option of adding a number and only removes up to that number of entries.
-- maybe too overloaded
DROP PROCEDURE IF EXISTS SP__delete_obj_actual_definition;
DELIMITER //
CREATE PROCEDURE SP__delete_obj_actual_definition(p_act_id int UNSIGNED, p_obj_name varchar(255) ) 
obj_inst_del:BEGIN 

	-- if its just removing one instance 
	IF( p_inst_id is not null) THEN
		DELETE FROM obj_actual_definition WHERE actual_definition_id = p_act_id;
    -- if its removing all instances with this defeintion
	ELSE -- removing all instances with this name 
		IF( p_obj_name is null or p_namespace_id) then -- nothing to do 
			leave obj_inst_del;
		END IF;
		DELETE FROM amp_core.obj_actual_definition where obj_metadata_id =(select obj_metadata_id from obj_metadat where obj_name =  p_obj_name ); 
	END IF;
END//
DELIMITER ;
