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

-- STORED PROCEDURE(S) for creating expresions that are used in rules and variables 

-- DROP PROCEDURE IF EXISTS sp_insert_postfix_operations; 
-- DELIMITER //
-- CREATE PROCEDURE sp_insert_postfix_operations(IN p_num_operators int(10) unsigned, p_operator_ids_list varchar(1000), p_operands_values_list varchar(10000), OUT r_postfix_operations int(10) unsigned)
-- BEGIN -- we get a list of operators and a list of operands need to create an ari collection for both 
--    -- need to finish
--    -- generate an ari collection
--    -- add in the airs
--    -- call sp_insert_ari_collection(IN p_num_entries int(10) unsigned, p_definition_ids_list varchar(10000), p_instance_ids_list varchar(10000), p_use_desc varchar(255), OUT r_ac_id int(10) unsigned)
--    
-- END //
-- DELIMITER ;


-- ==================================================================
-- SP__insert_expression 
-- adds an expression to the database
-- Parameters:
-- in 
-- 		p_out_type int(10) unsigned - data type id for the return type of the expression  
-- 		p_num_operators int(10) unsigned - number of operators   
-- 		p_postfix_ids_list varchar(1000) - id of the ac that lists the equation in postfix notation
-- OUT 
-- 		r_expr_id int(10) unsigned - id of the expr in the database
-- =================================================================
DROP PROCEDURE IF EXISTS SP__insert_expression; 
DELIMITER //
CREATE PROCEDURE SP__insert_expression(IN p_out_type INT(10) UNSIGNED,   
p_postfix_operations INT(10) UNSIGNED, OUT r_expr_id INT(10) UNSIGNED)
BEGIN 
	-- SELECT p_out_type;
	-- IF p_out_type = 2 or p_out_type = 12 or p_out_type = 3 or p_out_type = 0 THEN
	-- 	BEGIN
			INSERT INTO amp_core.expression(data_type_id, ac_id) VALUES(p_out_type, p_postfix_operations); 
			SET r_expr_id = LAST_INSERT_ID();
	-- END;
    -- END IF;
END //
DELIMITER ;


-- ==================================================================
-- SP__delete_expression 
-- Delete an expression from the database
-- Parameters:
-- in 
-- 		p_expr_id int(10) unsigned - id of the expr in the database to delete
-- ==================================================================

DROP PROCEDURE IF EXISTS SP__delete_expression; 
DELIMITER //
CREATE PROCEDURE SP__delete_expression(IN p_expr_id INT(10) UNSIGNED)
BEGIN 
	DELETE from  amp_core.expression WHERE (expression_id = p_expr_id);
END //
DELIMITER ;
