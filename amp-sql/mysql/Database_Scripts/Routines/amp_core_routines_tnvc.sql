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

-- STORED PROCEDURE(S) for inserting type name value collections into db


-- ==================================================================
-- SP__insert_tnvc
-- inserts a new tnv collection definition into the db
-- IN
-- 		p_num_entries int unsigned - number of entries in the collection
-- 		p_use_desc varchar(255) -  human readble description for the collection
-- OUT 
-- 		r_tnvc_id int unsigned - id of the collection
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_tnvc_collection; 
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_collection(IN p_use_desc varchar(255), OUT r_tnvc_id int unsigned)
BEGIN
	INSERT INTO amp_core.type_name_value_collection(use_desc) VALUES(p_use_desc);
    SET r_tnvc_id = LAST_INSERT_ID();
END //
DELIMITER ;

-- ==================================================================
-- sp for inserting a single entry into a tnvc 
-- SP__insert_tnvc_entry
-- IN 
-- 		p_tnvc_id int unsigned - id of tnvc this entry belongs to 
-- 		p_order_num int unsigned - order number of this entry 
-- 		p_data_type_name varchar(255) -  data type name 
-- 		p_data_name varchar(255) - name of the tnvc 
-- 		p_definition_id int unsigned - definition of the object could be literal
-- OUT 
-- 		r_tnvc_entry_id int unsigned - id of this entrty
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_tnvc_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned, p_data_type_name varchar(255), p_data_name varchar(255), OUT r_tnvc_entry_id int unsigned)
BEGIN

    IF p_order_num IS NULL THEN
       SELECT order_num+1 INTO p_order_num FROM type_name_value_entry WHERE tnvc_id=p_tnvc_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_order_num IS NULL THEN
       SET p_order_num = 0;
    END IF; 


	INSERT INTO `amp_core`.`type_name_value_entry`
(`tnvc_id`,
`order_num`,
`data_type_id`,
`data_name`)
VALUES
(p_tnvc_id,
p_order_num,
(SELECT data_type_id FROM data_type WHERE type_name = UPPER(p_data_type_name)),
p_data_name);

SET r_tnvc_entry_id = LAST_INSERT_ID();

END //
DELIMITER ;

DROP PROCEDURE IF EXISTS SP__insert_tnvc_entry_id; 
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_entry_id(IN p_tnvc_id int unsigned, p_order_num int unsigned, p_data_type_id int unsigned, p_data_name varchar(255), OUT r_tnvc_entry_id int unsigned)
BEGIN

    IF p_order_num IS NULL THEN
       SELECT order_num+1 INTO p_order_num FROM type_name_value_entry WHERE tnvc_id=p_tnvc_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_order_num IS NULL THEN
       SET p_order_num = 0;
    END IF; 


INSERT INTO `amp_core`.`type_name_value_entry`
(`tnvc_id`,
`order_num`,
`data_type_id`,
`data_name`)
VALUES
(p_tnvc_id,
p_order_num,
p_data_type_id,
p_data_name);

SET r_tnvc_entry_id = LAST_INSERT_ID();

END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_obj_entry
--  insert a new tnvc entry that is an ADM object
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   int unsigned - id for this actual object
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_obj_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_obj_entry(IN p_tnvc_id int unsigned,
                                                  p_order_num int unsigned,
                                                  p_data_type varchar(255),
                                                  p_entry_value int unsigned,
                                                  OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, p_data_type, NULL, r_tnvc_entry_id);
    INSERT INTO type_name_value_obj_entry (tnv_id, obj_actual_definition_id) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;


DROP PROCEDURE IF EXISTS SP__insert_tnvc_ac_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_ac_entry(IN p_tnvc_id int unsigned,
                                                  p_order_num int unsigned,
                                                  p_data_name varchar(255),
                                                  p_entry_value int unsigned,
                                                  OUT r_tnvc_entry_id int unsigned )
BEGIN

    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 37, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_ac_entry (tnv_id, ac_id) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

DROP PROCEDURE IF EXISTS SP__insert_tnvc_tnvc_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_tnvc_entry(IN p_tnvc_id int unsigned,
                                                  p_order_num int unsigned,
                                                  p_data_name varchar(255), 
                                                  p_entry_value int unsigned,
                                                  OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 35, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_tnvc_entry (tnv_id, tnvc_id) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;


-- ==================================================================
--  SP__insert_tnvc_ari_entry
--  insert a new tnvc entry that is an ADM object
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   int unsigned - id for this actual object
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_ari_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_ari_entry(IN p_tnvc_id int unsigned,
                                                  p_order_num int unsigned,
                                                  p_data_name varchar(255),
                                                  p_entry_value int unsigned,
                                                  OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'ari', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_obj_entry (tnv_id, obj_actual_definition_id) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_unk_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_unk_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_unk_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'unk', p_data_name, r_tnvc_entry_id);
END //
DELIMITER ;


-- ==================================================================
--  SP__insert_tnvc_int_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   int- value for this int_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_int_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_int_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value int, OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 19, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_int_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_uint_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   int unsigned - value for this uint_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_uint_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_uint_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value int unsigned , OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 20, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_uint_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_vast_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   bigint - value for this vast_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_vast_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_vast_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value bigint , OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 21, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_vast_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_uvast_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   bigint unsigned - value for this uvast_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_uvast_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_uvast_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value bigint unsigned , OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 22, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_uvast_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_tv_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   bigint unsigned - value for this uvast_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_tv_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_tv_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value bigint unsigned , OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 32, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_uvast_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_uvast_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   bigint unsigned - value for this uvast_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_ts_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_ts_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value bigint unsigned , OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 33, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_uvast_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_real32_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   float- value for this real32_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_real32_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_real32_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value float, OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 23, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_real32_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_real64_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   double- value for this real64_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_real64_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_real64_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value double, OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 24, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_real64_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_str_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   varchar(255)- value for this string_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_str_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_str_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value varchar(255), OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 18, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_string_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_bool_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   bool- value for this bool_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_bool_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_bool_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value bool, OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 16, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_bool_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;

-- ==================================================================
--  SP__insert_tnvc_byte_entry
--  insert a new tnvc entry for the specific primitive data type
--  in
--      p_tnv_entry_id int unsigned - the id for this tnv entry
--      p_entry_value   tinyint- value for this byte_entry
-- ================================================================== 
DROP PROCEDURE IF EXISTS SP__insert_tnvc_byte_entry;
DELIMITER //
CREATE PROCEDURE SP__insert_tnvc_byte_entry(IN p_tnvc_id int unsigned, p_order_num int unsigned,  p_data_name varchar(255),  p_entry_value tinyint, OUT r_tnvc_entry_id int unsigned )
BEGIN
    CALL SP__insert_tnvc_entry_id(p_tnvc_id, p_order_num, 17, p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_byte_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END //
DELIMITER ;






-- ==================================================================
-- SP__insert_tnv_collection
-- inserts entries into the collection using ',' lists
-- IN
-- 		p_num_entries int unsigned - number of entries in the collection
-- 		p_data_types_list varchar(10000) - list of the data types  
-- 		p_data_names_list varchar(10000) - list of the names
-- 		p_data_values_list varchar(10000) - list of the values
-- OUT 
-- 		r_tnvc_id int unsigned - id of the collection
-- ==================================================================
-- DROP PROCEDURE IF EXISTS SP__insert_tnv_collection; 
-- DELIMITER //
-- CREATE PROCEDURE SP__insert_tnv_collection(IN p_num_entries int unsigned, p_data_types_list varchar(10000), p_data_names_list varchar(10000), p_data_values_list varchar(10000), OUT r_tnvc_id int unsigned)
-- BEGIN 
-- 	CALL SP__insert_tnvc(p_num_entries, r_tnvc_id); 
--     SET @tnvc_id = r_tnvc_id; 
--     SET @s = 'INSERT INTO amp_core.type_name_value(tnvc_id, order_num, data_type_id, data_name, obj_actual_definition_id) VALUES'; 
--     SET @loops = 1; 
--     WHILE @loops < p_num_entries DO 
-- 		BEGIN
-- 			-- @data_type
-- 				IF p_data_types_list IS NOT NULL THEN 
-- 					BEGIN 
-- 						SET @data_type = TRIM(SUBSTRING_INDEX(p_data_types_list, ',', 1));
-- 						SET p_data_types_list = REPLACE(p_data_types_list, CONCAT(@data_type, ','), '');
-- 				    END; 
-- 				ELSE 
-- 					BEGIN
-- 						SET @data_type = 'NULL'; 
--                     END; 
-- 				END IF;
--                 
--  			-- @data_name
-- 				IF p_data_names_list IS NOT NULL THEN 
-- 					BEGIN 
-- 						SET @data_name = TRIM(SUBSTRING_INDEX(p_data_names_list, ',', 1)); 
-- 						SET p_data_names_list = REPLACE(p_data_names_list, CONCAT(@data_name, ','), '');
--                     END;
-- 				ELSE 
-- 					BEGIN
-- 						SET @data_name = 'NULL'; 
--                     END; 
--                 END IF;
--                 
--             -- @data_value
-- 				IF p_data_values_list IS NOT NULL THEN 
-- 					BEGIN 
-- 						SET @data_value = TRIM(SUBSTRING_INDEX(p_data_values_list, ',', 1));
-- 						SET p_data_values_list = REPLACE(p_data_values_list, CONCAT(@data_value, ','), '');
--                     END;
-- 				ELSE 
-- 					BEGIN
-- 						SET @data_value = 'NULL'; 
--                     END; 
-- 				END IF; 
-- 				SET @s = CONCAT(@s, '(', @tnvc_id, ',', @loops, ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @data_name, '"', ',', '"', @data_value, '"', '),');
--                 SET @loops = @loops + 1; 
--         END; 
--     END WHILE; 
--  
--     -- @data_type
--     IF p_data_types_list IS NOT NULL THEN
-- 		BEGIN 
-- 			SET @data_type = TRIM((SUBSTRING_INDEX(p_data_types_list, ',', 1)));
-- 		END; 
--     ELSE
-- 		BEGIN
-- 			SET @data_type = 'NULL'; 
--         END; 
--     END IF; 
--     
-- 	-- @data_name
--     IF p_data_names_list IS NOT NULL THEN
-- 		BEGIN 
-- 			SET @data_name = TRIM(SUBSTRING_INDEX(p_data_names_list, ',', 1));
-- 		END;
-- 	ELSE 
-- 		BEGIN
-- 			SET @data_name = 'NULL'; 
--         END; 
-- 	END IF; 
--     
-- 	-- @data_value
--     IF p_data_values_list IS NOT NULL THEN
-- 		BEGIN 
-- 			SET @data_value = TRIM(SUBSTRING_INDEX(p_data_values_list, ',', 1));
-- 		END; 
-- 	ELSE
-- 		BEGIN
-- 			SET @data_value = 'NULL'; 
--         END; 
--     END IF; 
-- 	SET @s = CONCAT(@s, '(', @tnvc_id, ',', @loops, ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @data_name, '"', ',', '"', @data_value, '"', ')');
-- 	PREPARE stmt FROM @s; 
--     EXECUTE stmt; 

-- END //
-- DELIMITER ;

