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

-- create messages 
use amp_core;

-- ==================================================================
-- SP__insert_outgoing_message_set
-- IN 
-- 		p_created_ts int(10) unsigned - created timestamp
-- 		p_modified_ts int(10) unsigned - modified timestamp
-- 		p_state int(10) unsigned - state of the message set 
-- 		p_agent_id int(10) unsigned - agent of this message 
-- OUT 
-- 		r_set_id int(10) unsigned - id of the set
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_outgoing_message_set; 
DELIMITER //
CREATE PROCEDURE SP__insert_outgoing_message_set(IN p_created_ts DATETIME,
p_modified_ts DATETIME, p_state int(10) unsigned, p_agent_id int(10) unsigned, OUT r_set_id int(10) unsigned )
BEGIN
INSERT INTO amp_core.outgoing_message_set (created_ts, modified_ts, state, agent_id)
VALUES (p_created_ts, p_modified_ts, p_state, p_agent_id);

SET r_set_id = LAST_INSERT_ID();
END//
DELIMITER ;


-- ==================================================================
-- SP__insert_outgoing_message_entry
-- IN 
-- 		p_set_id int(10) unsigned - id of the outgoing message set this entry belongs toß
--      p_message_order int(10) unsigned - order of this message in the set
--      p_start_ts int(10) unsigned - start time 
--      p_ac_id int(10) unsigned - ac of this message 
-- OUT 
--      r_message_id int(10) unsigned - id of this message
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_outgoing_message_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_outgoing_message_entry(IN p_set_id int(10) unsigned, p_message_order int(10) unsigned, p_start_ts datetime, p_ac_id int(10) unsigned, OUT r_message_id int(10) unsigned)
BEGIN
INSERT INTO amp_core.outgoing_message_entry (set_id, message_order, start_ts, ac_id)
VALUES (p_set_id, p_message_order, p_start_ts, p_ac_id);
SET r_message_id = last_insert_id();
END//
DELIMITER ;


-- ==================================================================
-- SP__update_outgoing_message_set
-- IN 
-- 		p_set_id int(10) unsigned - id of the set to update 
-- 		p_state int(10) unsigned - state of the message set 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__update_outgoing_message_set; 
DELIMITER //
CREATE PROCEDURE SP__update_outgoing_message_set(IN p_set_id int(10) unsigned, p_state int(10) unsigned)
BEGIN
	
	UPDATE amp_core.outgoing_message_set
	SET
	modified_ts = NOW(),
	state = p_state
	WHERE set_id = p_set_id;
END//
DELIMITER ;


-- ==================================================================
-- SP__insert_incoming_message_set
-- IN 
-- 		p_created_ts int(10) unsigned - created timestamp
-- 		p_modified_ts int(10) unsigned - modified timestamp
-- 		p_state int(10) unsigned - state of the message set 
-- 		p_agent_id int(10) unsigned - agent of this message 
-- OUT 
-- 		r_set_id int(10) unsigned - id of the set
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_incoming_message_set; 
DELIMITER //
CREATE PROCEDURE SP__insert_incoming_message_set(IN p_created_ts datetime,
p_modified_ts datetime, p_state int(10) unsigned, p_agent_id int(10) unsigned, OUT r_set_id int(10) unsigned)
BEGIN
INSERT INTO amp_core.incoming_message_set (created_ts, modified_ts, state, agent_id)
VALUES (p_created_ts, p_modified_ts, p_state, p_agent_id);

SET r_set_id = LAST_INSERT_ID();
END//
DELIMITER ;

-- ==================================================================
-- SP__insert_incoming_message_entry
-- IN 
-- 		p_set_id int(10) unsigned - id of the outgoing message set this entry belongs toß
--      p_message_order int(10) unsigned - order of this message in the set
--      p_start_ts int(10) unsigned - start time 
--      p_ac_id int(10) unsigned - ac of this message 
-- OUT 
--      r_message_id int(10) unsigned - id of this message
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_incoming_message_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_incoming_message_entry(IN p_set_id int(10) unsigned, p_message_order int(10) unsigned, p_start_ts datetime, p_ac_id int(10) unsigned, OUT r_message_id int(10) unsigned)
BEGIN
INSERT INTO amp_core.incoming_message_entry (set_id, message_order, start_ts, ac_id)
VALUES (p_set_id, p_message_order, p_start_ts, p_ac_id);
SET r_message_id = last_insert_id();
END//
DELIMITER ;


-- ==================================================================
-- SP__update_incoming_message_set
-- IN 
-- 		p_set_id int(10) unsigned - id of the set to update 
-- 		p_state int(10) unsigned - state of the message set 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__update_incoming_message_set; 
DELIMITER //
CREATE PROCEDURE SP__update_incoming_message_set(IN p_set_id int(10) unsigned, p_state int(10) unsigned)
BEGIN
	
	UPDATE amp_core.incoming_message_set
	SET
	modified_ts = NOW(),
	state = p_state
	WHERE set_id = p_set_id;
END//
DELIMITER ;


-- SP__insert_message_report_entry(message_id, order_num, ari_id, tnvc_id, ts)
DROP PROCEDURE IF EXISTS SP__insert_message_report_entry; 
DELIMITER //
CREATE PROCEDURE SP__insert_message_report_entry(IN
       p_msg_id int unsigned,
       p_order_num INT UNSIGNED,
       p_ari_id INT UNSIGNED,
       p_tnvc_id INT UNSIGNED,
       p_ts INT UNSIGNED,
       OUT r_obj_id int unsigned
       )
BEGIN

    INSERT INTO report_definition (ari_id, ts, tnvc_id) VALUES (p_ari_id, p_ts, p_tnvc_id);
    SET r_obj_id = LAST_INSERT_ID();

    IF p_order_num IS NULL THEN
       SELECT order_num+1 INTO p_order_num FROM message_report_set_entry WHERE message_id=p_msg_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_order_num IS NULL THEN
       SET p_order_num = 0;
    END IF; 

    INSERT INTO message_report_set_entry (message_id, report_id, order_num) VALUES (p_msg_id, r_obj_id, p_order_num);
    
END //
DELIMITER ;

-- SP__insert_message_group_agent_id(group_id, agent_id )
DROP PROCEDURE IF EXISTS SP__insert_message_group_agent_id; 
DELIMITER //
CREATE PROCEDURE SP__insert_message_group_agent_id(IN
       p_group_id int unsigned,
       p_agent INT UNSIGNED,
       OUT r_obj_id int unsigned
       )
BEGIN
        INSERT INTO message_group_agents (group_id, agent_id) VALUES (p_group_id, p_agent);
        SET r_obj_id = LAST_INSERT_ID();
END //
DELIMITER ;

-- SP__insert_message_group_agent_name(group_id, agent_name)
DROP PROCEDURE IF EXISTS SP__insert_message_group_agent_name; 
DELIMITER //
CREATE PROCEDURE SP__insert_message_group_agent_name(IN
       p_group_id int unsigned,
       p_agent VARCHAR(128),
       OUT r_obj_id int unsigned
       )
BEGIN
        DECLARE eid INT;  
        CALL SP__insert_agent(p_agent, eid); -- Select or Insert Agent ID
        INSERT INTO message_group_agents (group_id, agent_id) VALUES (p_group_id, eid);
        SET r_obj_id = LAST_INSERT_ID();

END //
DELIMITER ;

-- SP__insert_message_entry_agent( message_id, agent_name )
DROP PROCEDURE IF EXISTS SP__insert_message_entry_agent; 
DELIMITER //
CREATE PROCEDURE SP__insert_message_entry_agent(IN
       p_mid int unsigned,
       p_agent VARCHAR(128)
       )
BEGIN
        DECLARE eid INT;  
        CALL SP__insert_agent(p_agent, eid); -- Select or Insert Agent ID
        INSERT INTO message_agents (message_id, agent_id) VALUES (p_mid, eid);

END //
DELIMITER ;


