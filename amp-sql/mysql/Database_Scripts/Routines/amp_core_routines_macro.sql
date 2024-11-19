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

-- STORED PROCEDURE(S) for creating updating and deleting macro definitions and actuals

-- ==================================================================
-- SP__insert_macro_formal_definition
-- IN
-- 		p_obj_id int(10) unsigned - id of the metadata info 
-- 		p_use_desc varchar(255) - human readable describtion
--      p_fp_spec_id int(10) unsigned - formal parmspec of the macro
--      p_max_call_depth int(10) unsigned - max call depth of the macro
--      p_definition_ac int(10) unsigned - ari collection definining the macro
-- OUT
-- 		r_definition_id int(10) unsigned - id of the new formal definition	 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_macro_formal_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_macro_formal_definition(IN p_obj_id int(10) unsigned, p_use_desc varchar(255), p_fp_spec_id int(10) unsigned, p_max_call_depth int(10) unsigned, p_definition_ac int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.macro_formal_definition(obj_formal_definition_id, fp_spec_id, ac_id, max_call_depth) VALUES(r_definition_id, p_fp_spec_id, p_definition_ac, p_max_call_depth); 
END //
DELIMITER ;


-- ==================================================================
-- SP__delete_mac_formal_defintion
-- IN 
--      p_obj_id int(10) unsigned - id of the macro to be deleted
-- 	 	p_obj_name VARCHAR(255) - name of the macro to delete
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__delete_mac_formal_defintion;
DELIMITER // 
CREATE PROCEDURE SP__delete_mac_formal_defintion(IN p_obj_id int(10) unsigned, p_obj_name VARCHAR(255))
mac_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE mac_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_mac_formal where obj_id = p_obj_id);
    END IF ;
    
	-- deleting all actuals 
	CALL SP__delete_macro_actual(null, p_obj_name);
	
	SET @def_id = (select definition_id from obj_formal_definition where obj_id = p_obj_id);
	SET @fp_id = (SELECT fp_spec_id from macro_formal_definition where definition_id = @def_id );
    SET @ac_id = (SELECT ac_id FROM macro_formal_definition where definition_id = @def_id );
    
	DELETE FROM formal_parmspec WHERE fp_spec_id = @fp_id;
	DELETE FROM ari_collection WHERE ac_id = @ac_id;

	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
END // 
DELIMITER ;


-- ==================================================================
-- SP__insert_macro_actual_definition
-- -- IN
-- 		p_obj_id int(10) unsigned - id of the metadata info 
--      p_ap_spec_id int(10) unsigned - actual parmspec id
--      p_actual_ac int(10) unsigned - ari collection containg all actual ARI for this macro
--      p_use_desc varchar(255) - human readable description
-- OUT
-- 		r_actual_id int(10) unsigned - id of the new actual definition	
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_macro_actual_definition; 
DELIMITER //
CREATE PROCEDURE SP__insert_macro_actual_definition(IN p_obj_definition_id int(10) unsigned, p_ap_spec_id int(10) unsigned, p_actual_ac int(10) unsigned, p_use_desc varchar(255), OUT r_actual_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_id); 
    INSERT INTO amp_core.macro_actual(obj_actual_definition_id, ap_spec_id, ac_id) VALUES(r_actual_id, p_ap_spec_id, p_actual_ac);  
END // 
DELIMITER ; 
 
-- ==================================================================
-- SP__delete_mac_actual_definition
-- IN 
--      p_obj_id int(10) unsigned - id of the macro to be deleted
-- 	 	p_obj_name VARCHAR(255) - name of the macro to delete
-- ==================================================================
-- for actual can supply the definiton name to remove all the actuals of that definition or can remove 
DROP PROCEDURE IF EXISTS SP__delete_mac_actual_definition;
DELIMITER //
CREATE PROCEDURE SP__delete_mac_actual_definition(IN p_inst_id int(10) unsigned, p_obj_name varchar(255))
mac_inst_del:BEGIN
	
	DECLARE done INT DEFAULT FALSE;
    DECLARE actual_id_hold, ap_spec_id_hold, ac_id_hold int(10) unsigned;
    DECLARE actual_cursor CURSOR
			FOR SELECT actual_id, ap_spec_id, ac_id  WHERE obj_name = p_obj_name;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;	
    
    -- only one thing to remove 
	If( p_inst_id is not null) then 
		SET @ap_id = (SELECT ap_spec_id from macro_actual_definition where actual_id = p_inst_id );
		IF( @ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = @ap_id;
		END IF;
		DELETE FROM macro_actual 
WHERE
    intance_id = p_inst_id;
		CALL SP__delete_obj_actual(p_inst_id, p_obj_name);
	ELSE -- removing all actuals with this name 
		IF( p_obj_name is null) then -- nothing to do 
			leave mac_inst_del;
		END IF;
		OPEN actual_cursor;
        read_loop: LOOP
			FETCH NEXT FROM actual_cursor INTO 
				actual_id_hold, ap_spec_id_hold, ac_id_hold;
			IF done THEN
				LEAVE read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;
			DELETE FROM ari_collection WHERE ac_id = ac_id_hold;

			DELETE FROM macro_actual_definition
WHERE
    amp_core.macro_actual.actual_id = actual_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_cursor;
END // 
DELIMITER ;
