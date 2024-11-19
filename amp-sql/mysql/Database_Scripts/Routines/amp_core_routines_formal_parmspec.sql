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

-- STORED PROCEDURE(S) for the formal parameters specefications. Stores type name information. 


-- ==================================================================
-- SP__insert_formal_parmspec
-- inserts a new formal parmspec in the db
-- IN 
-- 		p_num_parms int(10) unsigned - number if parms in the spec
-- 		p_use_desc varchar(255) - human readable describtion
-- OUT 
-- 		r_fp_spec_id int(10) unsigned -  the id of the spec 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_formal_parmspec; 
DELIMITER //
CREATE PROCEDURE SP__insert_formal_parmspec(IN p_num_parms int(10) unsigned, p_use_desc varchar(255),  OUT r_fp_spec_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.formal_parmspec(num_parms, use_desc) VALUES(p_num_parms, p_use_desc); 
    SET r_fp_spec_id = LAST_INSERT_ID();
END //
DELIMITER ;



-- ==================================================================
-- SP__insert_formal_parmspec_entry
-- insert a single entry into a formal parm
-- IN 
--      p_fp_spec_id int(10) unsigned - id of the formal parmspec for this entry    
--      p_order_num int(10) unsigned - order of the entry in the parmspec
--      p_parm_name varchar(255) - name of the parm used for parm by name 
--      p_data_type VARCHAR(255) - data type of the parm
--      p_obj_definition_id int(10) unsigned -  optional default value of this parm
-- OUT 
--      r_fp_id int(10) unsigned
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_formal_parmspec_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_formal_parmspec_entry(IN p_fp_spec_id int(10) unsigned,  p_order_num int(10) unsigned, p_parm_name varchar(255), p_data_type VARCHAR(255), p_obj_definition_id int(10) unsigned, OUT r_fp_id int(10) unsigned)
BEGIN
	INSERT INTO formal_parm
(
fp_spec_id,
order_num,
parm_name,
data_type_id,
obj_actual_definition_id)
VALUES
(p_fp_spec_id,
p_order_num,
p_parm_name,
(select data_type_id from data_type where type_name = p_data_type),
p_obj_definition_id);

SET r_fp_id = LAST_INSERT_ID();
END //
DELIMITER ;



-- ==================================================================
-- SP__insert_formal_parmspec
-- inserts a list of formal parms into a spec, uses three ',' delimenated lists to 
-- store type name and default value info for the formal parms
-- IN 
-- 		p_num_parms int(10) unsigned - number of parms in the parmspec
-- 		p_use_desc varchar(255) - human readable description
-- 		p_data_types_list varchar(10000 ) - list of types for the parms 
-- 		p_parm_names_list varchar(10000) - list of the names for the parms
-- 		p_default_values_list varchar(10000) - list of the default values
-- OUT 
-- 		r_fp_spec_id int(10) unsigned - formal parmspec id
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_formal_parms_set; 
DELIMITER //
CREATE PROCEDURE SP__insert_formal_parms_set(IN p_num_parms int(10) unsigned, p_use_desc varchar(255), p_data_types_list varchar(10000), p_parm_names_list varchar(10000),
 p_default_values_list varchar(10000), OUT r_fp_spec_id int(10) unsigned)
BEGIN 
	CALL SP__insert_formal_parmspec(p_num_parms, p_use_desc, r_fp_spec_id); 
    SET @fp_spec_id = r_fp_spec_id; 
    SET @s = 'INSERT INTO amp_core.formal_parm(fp_spec_id, order_num, parm_name, data_type_id, obj_actual_definition_id) VALUES'; 
    SET @loops = 1; 
    WHILE @loops < p_num_parms DO 
		BEGIN
			-- @data_type
				SET @data_type = TRIM(SUBSTRING_INDEX(p_data_types_list, ',', 1));
				SET p_data_types_list = REPLACE(p_data_types_list, CONCAT(@data_type, ','), ''); 
    
 			-- @parm_name
				SET @parm_name = TRIM(SUBSTRING_INDEX(p_parm_names_list, ',', 1)); 
				SET p_parm_names_list = REPLACE(p_parm_names_list, CONCAT(@parm_name, ','), '');
                
            -- @default_value
				SET @default_value = TRIM(SUBSTRING_INDEX(p_default_values_list, ',', 1));
                IF @default_value = 'NULL' THEN SET @default_value = null;
                ELSEIF @default_value = 'null' THEN SET @default_value = null;
				END IF;
                SET p_default_values_list = REPLACE(p_default_values_list, CONCAT(@default_value, ','), '');
            
				SET @s = CONCAT(@s, '(', @fp_spec_id, ',', @loops, ',', '"', @parm_name, '"', ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @default_value, '"', '),');
                SET @loops = @loops + 1; 
        END; 
    END WHILE; 
 
    -- @data_type
	SET @data_type = TRIM((SUBSTRING_INDEX(p_data_types_list, ',', 1)));
    
	-- @parm_name
	SET @parm_name = TRIM(SUBSTRING_INDEX(p_parm_names_list, ',', 1)); 
                
	-- @default_value
	IF @default_value = 'NULL' THEN SET @default_value = null;
                ELSEIF @default_value = 'null' THEN SET @default_value = null;
				END IF;
                SET p_default_values_list = REPLACE(p_default_values_list, CONCAT(@default_value, ','), '');

	SET @s = CONCAT(@s, '(', @fp_spec_id, ',', @loops, ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @parm_name, '"', ',', '"', @default_value, '"', ')');
	PREPARE stmt FROM @s; 
    EXECUTE stmt; 

END //
DELIMITER ;

