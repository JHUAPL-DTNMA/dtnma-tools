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

-- STORED PROCEDURE(S) for creating an ari collection and entries

-- ==================================================================
-- SP__insert_ac_id 
-- adds an ari collection to the database
-- Parameters:
-- in 
-- 		p_num_entries int(10) unsigned - number of entries in the ac
-- 		p_use_desc varchar(255) - human readable description
-- OUT 
-- 		r_ac_id int(10) unsigned- id of the ac
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_ac_id; 
DELIMITER //
CREATE PROCEDURE SP__insert_ac_id(IN p_num_entries int(10) unsigned, p_use_desc varchar(255),  OUT r_ac_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.ari_collection(num_entries, use_desc) VALUES(p_num_entries, p_use_desc); 
    SET r_ac_id = LAST_INSERT_ID();
END //
DELIMITER ;


-- ==================================================================
-- SP__insert_ac_formal_entry 
-- adds a formal ari entry into the database. 
-- stops if the order_num is > the number of entries for the target ac
-- Parameters:
-- in 
-- 		p_ac_id int(10) unsigned - id of the ari collection this entry belongs to
-- 		p_definition_id int(10) unsigned - id of the definition   
-- 		p_order_num int(10) unsigned - order number
-- OUT 
-- 		r_ac_entry_id int(10) unsigned - entry id 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_ac_formal_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_ac_formal_entry(IN p_ac_id int(10) unsigned, p_definition_id int(10) unsigned,  p_order_num int(10) unsigned, OUT r_ac_entry_id int(10) unsigned)
BEGIN 
	/*IF p_order_num < (select num_entries from ari_collection where ari_collection.ac_id = p_definition_id) THEN 
    BEGIN*/
		INSERT INTO amp_core.ari_collection_entry(ac_id, order_num) VALUES(p_ac_id, p_order_num);
		SET r_ac_entry_id = LAST_INSERT_ID();
		INSERT INTO amp_core.ari_collection_formal_entry(ac_entry_id, obj_formal_definition_id) VALUES(r_ac_entry_id, p_definition_id); 
	/*END;
    END IF;*/
END //
DELIMITER ;


-- ==================================================================
-- SP__insert_ac_actual_entry 
-- adds a actual ari entry into the database. 
-- stops if the order_num is > the number of entries for the target ac
-- Parameters:
-- in 
-- 		p_ac_id int(10) unsigned - id of the ari collection this entry belongs to
-- 		p_definition_id int(10) unsigned - id of the definition   
-- 		p_order_num int(10) unsigned - order number
-- OUT 
-- 		r_ac_entry_id int(10) unsigned - entry id 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_ac_actual_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_ac_actual_entry(IN p_ac_id int(10) unsigned, p_definition_id int(10) unsigned, p_order_num int(10) unsigned, 
OUT r_ac_entry_id int(10) unsigned)
BEGIN 
	-- IF p_order_num < (select num_entries from ari_collection where ari_collection.ac_id = p_definition_id) THEN 
    -- BEGIN
		INSERT INTO amp_core.ari_collection_entry(ac_id, order_num) VALUES(p_ac_id, p_order_num);
		SET r_ac_entry_id = LAST_INSERT_ID();
		INSERT INTO amp_core.ari_collection_actual_entry(ac_entry_id, obj_actual_definition_id) VALUES(r_ac_entry_id, p_definition_id); 
-- END;
   -- END IF;
END //
DELIMITER ;

-- DROP PROCEDURE IF EXISTS SP__insert_ari_collection;
-- DELIMITER //
-- CREATE PROCEDURE SP__insert_ari_collection(IN p_num_entries int(10) unsigned, p_definition_ids_list varchar(10000), p_instance_ids_list varchar(10000), p_use_desc varchar(255), OUT r_ac_id int(10) unsigned) 
-- BEGIN 
-- 	CALL SP__insert_ac_id(p_num_entries, p_use_desc, r_ac_id); 
--     -- TODO -- verify that lists are same size or NULL prior to processing, backout transaction on failure  and return NULL
--     SET @ac_id  = r_ac_id; 
--     SET @s = 'INSERT INTO amp_core.ari_collection_entry(ac_id, definition_id, instance_id, order_num) VALUES'; 
-- 	
-- 	SET @loops = 1; 
-- --     -- SET @def_id = NULL; 
-- --     -- SET @inst_id = NULL; 
--     WHILE @loops < p_num_entries DO
-- 		BEGIN
-- 			-- @def_id
--             IF p_definition_ids_list IS NOT NULL THEN
-- 				BEGIN
-- 				SET @def_id = TRIM(SUBSTRING_INDEX(p_definition_ids_list, ',', 1)); 
-- 				SET p_definition_ids_list = REPLACE(p_definition_ids_list, CONCAT(@def_id, ','), '');
-- 				-- IF strcmp(@def_id, 'NULL') THEN
-- 					-- SET @def_id = NULL; 
-- 				-- ELSE
-- 					SET @def_id = CAST(@def_id AS UNSIGNED); 
-- 				-- END IF;
--                 END; 
-- 			ELSE 
-- 				BEGIN
-- 				 SET @def_id = 'NULL';
-- 				END; 
--             END IF; 
--             
--             -- @inst_id
--             IF p_instance_ids_list IS NOT NULL THEN 
-- 				BEGIN
-- 				SET @inst_id = TRIM(SUBSTRING_INDEX(p_instance_ids_list, ',', 1)); 
-- 				SET p_instance_ids_list = REPLACE(p_instance_ids_list, CONCAT(@inst_id, ','), '');
-- 				-- IF strcmp(@inst_id, 'NULL') THEN 
-- 					-- SET @inst_id = NULL; 
-- 				-- ELSE
-- 					SET @inst_id = CAST(@inst_id AS UNSIGNED); 
-- 				-- END IF;
--                 END;
-- 			ELSE 
-- 				BEGIN
-- 				SET @inst_id = 'NULL'; 
--                 END; 
-- 			END IF; 
-- 			
--             SET @s = CONCAT(@s, '(', @ac_id, ',', @def_id , ',', @inst_id, ',', @loops, '),');
--            -- SELECT @s; 
--  			SET @loops = @loops + 1; 
--          END; 
--     END WHILE; 
--     
--     -- @def_id
--             IF p_definition_ids_list IS NOT NULL THEN
-- 				BEGIN
-- 				SET @def_id = TRIM(SUBSTRING_INDEX(p_definition_ids_list, ',', 1)); 
-- 				SET p_definition_ids_list = REPLACE(p_definition_ids_list, CONCAT(@def_id, ','), '');
-- 			-- 	IF strcmp(@def_id, 'NULL') THEN
-- 				-- 	SET @def_id = NULL; 
-- 			--	ELSE
-- 					SET @def_id = CAST(@def_id AS UNSIGNED); 
-- 				-- END IF;
--                 END; 
-- 			ELSE 
-- 				BEGIN
-- 				 SET @def_id = 'NULL'; 
-- 				END; 
--             END IF; 
--             
--             -- @inst_id
--             IF p_instance_ids_list IS NOT NULL THEN 
-- 				BEGIN
-- 				SET @inst_id = TRIM(SUBSTRING_INDEX(p_instance_ids_list, ',', 1)); 
-- 				SET p_instance_ids_list = REPLACE(p_instance_ids_list, CONCAT(@inst_id, ','), '');
-- 				-- IF strcmp(@inst_id, 'NULL') THEN 
-- 				--	SET @inst_id = NULL; 
-- 				-- ELSE
-- 					SET @inst_id = CAST(@inst_id AS UNSIGNED); 
-- 				-- END IF;
--                 END; 
-- 			 ELSE 
-- 				BEGIN
-- 			 	SET @inst_id = 'NULL'; 
--                 END;
-- 			END IF; 
-- 			
--             SET @s = CONCAT(@s, '(', @ac_id, ',', @def_id, ',', @inst_id, ',', @loops, ')');
--           -- SELECT @s; 
-- 	PREPARE stmt FROM @s; 
-- 	EXECUTE stmt; 
-- END //
DELIMITER ;


