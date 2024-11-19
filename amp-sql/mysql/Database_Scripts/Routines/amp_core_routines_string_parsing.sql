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

-- STORED PROCEDURE(S)


-- ==
-- function for string processing used by other sp
-- help make inserting large sets of data more user friendly 
-- ==
DROP PROCEDURE IF EXISTS SP__get_delimiter_position; 
DELIMITER //
CREATE PROCEDURE SP__get_delimiter_position(IN p_str varchar(255), p_delimiter varchar(255), p_start_position int unsigned, OUT r_found_position int unsigned)
BEGIN
	IF (p_str IS NOT NULL) && (p_start_position IS NOT NULL)THEN 
		BEGIN
			SET @length = CHAR_LENGTH(p_str); 
			SET @position_index = p_start_position; 
            SET @iterator_index = p_start_position; 
            SET @char_value = ''; 
            SET @delimiter_found = FALSE; 
            SET r_found_position = NULL; 
			WHILE (@position_index != @length) && (@delimiter_found != TRUE) DO 
				BEGIN
					SET @char_value = SUBSTRING(p_str, @iterator_index, 1);
                    IF @char_value LIKE p_delimiter THEN 
						SET @position_index = @iterator_index;
                        SET r_found_position = @position_index; 
                        SET @delimiter_found = TRUE; 
                    END IF; 
                    SET @iterator_index = @iterator_index + 1; 
				END;
			END WHILE; 
		END; 
    ELSE 
		BEGIN
			SET r_found_position = NULL; 
        END; 
    END IF; 
END //
DELIMITER ;

DROP PROCEDURE IF EXISTS SP__null_string_check; 
DELIMITER //
CREATE PROCEDURE SP__null_string_check(INOUT p_string varchar(255))
BEGIN
	IF (p_string LIKE 'null') || (p_string LIKE 'NULL') || (p_string LIKE 'Null') THEN
		BEGIN
			SET p_string = NULL; 
        END; 
    END IF; 
END //
DELIMITER ;