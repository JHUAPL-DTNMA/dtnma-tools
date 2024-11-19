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

-- STORED PROCEDURE(S) for the actaul parameters specefications and actual parameter sets. has real data 


-- ==================================================================
-- SP__insert_actual_parmspec
--  inserting an actual parmspec into db
-- IN 
-- 		p_fp_spec_id int(10) unsigned - the id of the formal parm spec for this actual parmspec
-- 		p_tnvc_id int(10) unsigned - TNVC corresponding to actual parameter definition
-- 		p_use_desc VARCHAR(255) - human readable describtion
-- OUT 
-- 		r_ap_spec_id int(10) unsigned - id of the parmspec in the db 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_actual_parmspec_tnvc; 
DELIMITER //
CREATE PROCEDURE SP__insert_actual_parmspec_tnvc(IN p_fp_spec_id int unsigned, p_tnvc_id int unsigned, p_use_desc VARCHAR(255), OUT r_ap_spec_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.actual_parmspec(fp_spec_id, tnvc_id, use_desc) VALUES(p_fp_spec_id, p_tnvc_id, p_use_desc); 
    SET r_ap_spec_id = LAST_INSERT_ID();
END //
DELIMITER ;

-- ==================================================================
-- SP__insert_actual_parmspec
--  inserting an actual parmspec into db
-- IN 
-- 		p_fp_spec_id int(10) unsigned - the id of the formal parm spec for this actual parmspec
-- 		p_num_parms int(10) unsigned - number of parms 
-- 		p_use_desc VARCHAR(255) - human readable describtion
-- OUT 
-- 		r_ap_spec_id int(10) unsigned - id of the parmspec in the db 
-- ==================================================================
-- TODO: p_num_parms argument is deprecated and will be removed
DROP PROCEDURE IF EXISTS SP__insert_actual_parmspec; 
DELIMITER //
CREATE PROCEDURE SP__insert_actual_parmspec(IN p_fp_spec_id int unsigned, p_num_parms int unsigned, p_use_desc VARCHAR(255), OUT r_ap_spec_id int unsigned)
BEGIN
    DECLARE tnvc_id INT;
    CALL SP__insert_tnvc_collection(p_use_desc, tnvc_id);
    CALL SP__insert_actual_parmspec_tnvc(p_fp_spec_id, tnvc_id, p_use_desc, r_ap_spec_id);
END //
DELIMITER ;

-- ==================================================================
-- SP__insert_actual_parms_object
--  inserting an actual parm object into spec
-- IN 
-- 		p_ap_spec_id int(10) unsigned -  id of the spec this object is being added 
-- 		p_order_num int(10) unsigned -  order number
-- 		p_data_type_id int(10) unsigned - the id of the datatype in the data type table
-- 		p_obj_actual_definition int(10) unsigned - id of the object for the parm
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_actual_parms_object; 
DELIMITER //
CREATE PROCEDURE SP__insert_actual_parms_object(IN p_ap_spec_id INT unsigned, p_order_num int unsigned, p_data_type_id varchar(255) , p_obj_actual_definition INT UNSIGNED)
BEGIN
    DECLARE ap_tnvc_id, r_tnvc_entry_id INT;
    SELECT tnvc_id INTO ap_tnvc_id FROM amp_core.actual_parmspec WHERE ap_spec_id = p_ap_spec_id;


    CALL SP__insert_tnvc_obj_entry(ap_tnvc_id, p_order_num, p_data_type_id, p_obj_actual_definition, r_tnvc_entry_id);



END //
DELIMITER ;

-- ==================================================================
-- SP__insert_actual_parms_names
--  inserting an actual parm reference by name into spec. This parm gets it value from the object that defines this parm spec 
-- IN 
-- 		p_ap_spec_id int(10) unsigned -  id of the spec this object is being added 
-- 		p_order_num int(10) unsigned -  order number
-- 		p_data_type_id int(10) unsigned - the id of the datatype in the data type table
-- 		p_fp_id int(10) unsigned - id of the formal parm this parm reference
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_actual_parms_names; 
DELIMITER //
CREATE PROCEDURE SP__insert_actual_parms_names(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_data_type_id varchar(255), p_fp_id INT(10) UNSIGNED)
BEGIN 
    DECLARE ap_tnvc_id INT;
    DECLARE dt_id INT;
    
    SELECT tnvc_id INTO ap_tnvc_id FROM amp_core.actual_parmspec WHERE ap_spec_id = p_ap_spec_id;

    SELECT data_type_id INTO dt_id FROM data_type WHERE type_name = p_data_type_id;

    INSERT INTO type_name_value_entry(tnvc_id, order_num, data_type_id, data_name, fp_id) VALUES(ap_tnvc_id, p_order_num, dt_id, p_data_Type_id, p_fp_id);
    


END //
DELIMITER ;

-- ==================================================================
-- SP__insert_actual_parms_tnvc
--  inserting an actual parm tnvc into spec.
-- IN 
-- 		p_ap_spec_id int(10) unsigned -  id of the spec this object is being added 
-- 		p_order_num int(10) unsigned -  order number
-- 		p_data_type_id int(10) unsigned - the id of the datatype in the data type table
-- 		p_tnvc_id int(10) unsigned - id of the type name value collection
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_actual_parms_tnvc; 
DELIMITER //
CREATE PROCEDURE SP__insert_actual_parms_tnvc(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_tnvc_id INT(10) UNSIGNED)
BEGIN
    DECLARE ap_tnvc_id, r_entry_id INT;
    SELECT tnvc_id INTO ap_tnvc_id FROM amp_core.actual_parmspec WHERE ap_spec_id = p_ap_spec_id;

    CALL SP__insert_tnvc_tnvc_entry(ap_tnvc_id, p_order_num, p_tnvc_id, r_entry_id);
END //
DELIMITER ;

-- ==================================================================
-- SP__insert_actual_parms_ac
--  inserting an actual parm ac into spec.
-- IN 
-- 		p_ap_spec_id int(10) unsigned -  id of the spec this object is being added 
-- 		p_order_num int(10) unsigned -  order number
-- 		p_data_type_id int(10) unsigned - the id of the datatype in the data type table
-- 		p_ac_id int(10) unsigned - id of the ari collection
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_actual_parms_ac; 
DELIMITER //
CREATE PROCEDURE SP__insert_actual_parms_ac(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_ac_id INT(10) UNSIGNED)
BEGIN 
    DECLARE ap_tnvc_id, r_entry_id INT;
    SELECT ap_tnvc_id=tnvc_id FROM amp_core.actual_parmspec WHERE ap_spec_id = p_ap_spec_id;

    CALL SP__insert_tnvc_ac_entry(ap_tnvc_id, p_order_num, p_ac_id, r_entry_id);

END //
DELIMITER ;



-- was used before one there wasnt two types of actual_parms so it was easy to parse a 
-- a list of parms 
--
-- DROP PROCEDURE IF EXISTS SP__insert_actual_parms_set; 
-- DELIMITER //
-- CREATE PROCEDURE SP__insert_actual_parms_set(IN p_num_parms int(10) unsigned, p_fp_spec_id int(10) unsigned, p_data_types_list varchar(10000), p_data_values_list varchar(10000), OUT r_ap_spec_id int(10) unsigned)
-- BEGIN 
-- 	CALL SP__insert_actual_parmspec(p_fp_spec_id, r_ap_spec_id); 
--     SET @ap_spec_id = r_ap_spec_id; 
--     SET @s = 'INSERT INTO amp_core.actual_parm(order_num, ap_type, data_value, ap_spec_id) VALUES'; 
--     SET @loops = 1; 
--     WHILE @loops < p_num_parms DO 
-- 		BEGIN
-- 			-- @data_type
-- 				SET @data_type = TRIM(SUBSTRING_INDEX(p_data_types_list, ',', 1));
-- 				SET p_data_types_list = REPLACE(p_data_types_list, CONCAT(@data_type, ','), ''); 
--     
--             -- @data_value
-- 				SET @data_value = TRIM(SUBSTRING_INDEX(p_data_values_list, ',', 1));
--                 SET p_data_values_list = REPLACE(p_data_values_list, CONCAT(@data_value, ','), '');
--             
-- 				SET @s = CONCAT(@s, '(', @loops, ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @data_value, '"', ',', @ap_spec_id, '),');
--                 SET @loops = @loops + 1; 
--         END; 
--     END WHILE; 
--  
--     -- @data_type
-- 	SET @data_type = TRIM((SUBSTRING_INDEX(p_data_types_list, ',', 1)));
--     
-- 	
-- 	-- @data_value
-- 	SET @data_value = TRIM(SUBSTRING_INDEX(p_data_values_list, ',', 1));

-- 	SET @s = CONCAT(@s, '(', @loops, ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @data_value, '"', ',', @ap_spec_id, ')');
-- 	PREPARE stmt FROM @s; 
--     EXECUTE stmt; 

-- END //
-- DELIMITER ;

