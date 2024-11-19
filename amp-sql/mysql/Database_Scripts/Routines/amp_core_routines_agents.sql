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
use amp_core;

-- inserting reg agents into the db

-- ==================================================================
-- SP__insert_agent
--  inserting a new agent into the systemß
-- IN 
-- 		p_agent_id_string- name of the agent to insert 
-- OUT
-- 		r_registered_agents_id - teh id of the agent in the db
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_agent; 
DELIMITER //
CREATE PROCEDURE SP__insert_agent(IN p_agent_id_string VARCHAR(255), OUT r_registered_agents_id INT unsigned)
BEGIN 
	SET @cur_time = NOW(); 
    SET @lower_name = LOWER(p_agent_id_string);
    SET @eid = NULL;
    SELECT registered_agents_id INTO @eid FROM registered_agents WHERE @lower_name = agent_id_string;
    
	IF (@eid IS NOT NULL) THEN 
	BEGIN 
		UPDATE registered_agents SET last_registered = @cur_time WHERE registered_agents_id=@eid;
        SET r_registered_agents_id = @eid;
    END;
    ELSE BEGIN
    INSERT INTO registered_agents (agent_id_string,first_registered, last_registered)
		VALUES (@lower_name, @cur_time, @cur_time);
		SET r_registered_agents_id = LAST_INSERT_ID();
    END;
    END IF;
END //
DELIMITER ;

