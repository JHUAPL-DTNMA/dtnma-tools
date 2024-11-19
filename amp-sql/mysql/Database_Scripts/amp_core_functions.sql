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

-- SQL Wrapper Functions for simplified client usage of Stored Procedures via SELECT when returning single values

DELIMITER $
DROP FUNCTION IF EXISTS register_agent$
CREATE FUNCTION register_agent(
       agent_eid VARCHAR(255)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE eidx INT;
    CALL SP__insert_agent(agent_eid, eidx);
    RETURN eidx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS create_tnvc$
CREATE FUNCTION create_tnvc(
       use_desc varchar(255)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_collection(use_desc, idx);
    RETURN idx;
END$

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_int_entry$
CREATE FUNCTION insert_tnvc_int_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_int_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_uint_entry$
CREATE FUNCTION insert_tnvc_uint_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT unsigned
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_uint_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_uvast_entry$
CREATE FUNCTION insert_tnvc_uvast_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val BIGINT unsigned
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_uvast_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_vast_entry$
CREATE FUNCTION insert_tnvc_vast_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val BIGINT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_vast_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;


DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_real32_entry$
CREATE FUNCTION insert_tnvc_real32_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val float
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_real32_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_real64_entry$
CREATE FUNCTION insert_tnvc_real64_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val float
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_real64_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;


DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_str_entry$
CREATE FUNCTION insert_tnvc_str_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val varchar(255)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_str_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_bool_entry$
CREATE FUNCTION insert_tnvc_bool_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val bool
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_bool_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_byte_entry$
CREATE FUNCTION insert_tnvc_byte_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val tinyint
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_byte_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_tv_entry$
CREATE FUNCTION insert_tnvc_tv_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_tv_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_ts_entry$
CREATE FUNCTION insert_tnvc_ts_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_ts_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_ac_entry$
CREATE FUNCTION insert_tnvc_ac_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_ac_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;


DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_obj_entry$
CREATE FUNCTION insert_tnvc_obj_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_obj_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_tnvc_tnvc_entry$
CREATE FUNCTION insert_tnvc_tnvc_entry(
       tnvc_id INT unsigned,
       order_num INT unsigned,
       data_name varchar(255),
       val INT
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_tnvc_tnvc_entry(tnvc_id, order_num, data_name, val, idx);
    RETURN idx;
END$
DELIMITER ;


DELIMITER $
DROP FUNCTION IF EXISTS insert_ac$
CREATE FUNCTION insert_ac(
       num_entries INT unsigned,
       use_desc varchar(255)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_ac_id(num_entries, use_desc, idx);
    RETURN idx;
END$
DELIMITER ;

DELIMITER $
DROP FUNCTION IF EXISTS insert_ac_actual_entry$
CREATE FUNCTION insert_ac_actual_entry(
       ac_id INT unsigned,
       obj_def_id INT unsigned, 
       order_num INT unsigned
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_ac_actual_entry(ac_id, obj_def_id, order_num, idx);
    RETURN idx;
END$
DELIMITER ;

-- function insert_ari_ctrl(obj_metadata_id, actual_parmspec_id, description)
DELIMITER $
DROP FUNCTION IF EXISTS insert_ari_ctrl$
CREATE FUNCTION insert_ari_ctrl(
       metadata_id INT unsigned,
       ap_id INT unsigned,    -- actual_parmspec_id
       use_desc varchar(255)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_control_actual_definition(metadata_id, ap_id, use_desc, idx);
    RETURN idx;
END$
DELIMITER ;

-- create_actual_parmspec(formal_def_id, length, description)
DELIMITER $
DROP FUNCTION IF EXISTS create_actual_parmspec$
CREATE FUNCTION create_actual_parmspec(
       formal_def_id INT unsigned,
       len INT unsigned,
       use_desc varchar(255) -- optional
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_actual_parmspec(formal_def_id, len, use_desc, idx);
    RETURN idx;
END$
DELIMITER ;
DELIMITER $
DROP FUNCTION IF EXISTS create_actual_parmspec_tnvc$
CREATE FUNCTION create_actual_parmspec_tnvc(
       formal_def_id INT unsigned,
       p_tnvc_id INT unsigned,
       use_desc varchar(255) -- optional
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_actual_parmspec_tnvc(formal_def_id, p_tnvc_id, use_desc, idx);
    RETURN idx;
END$
DELIMITER ;

-- create_ac(length, description)
DELIMITER $
DROP FUNCTION IF EXISTS create_ac$
CREATE FUNCTION create_ac(
       len INT unsigned,
       use_desc varchar(255) -- optional
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_ac_id(len, use_desc, idx);
    RETURN idx;
END$
DELIMITER ;


DELIMITER $
DROP FUNCTION IF EXISTS add_message_report_set$
CREATE FUNCTION add_message_report_set(
       p_group_id INT UNSIGNED,
       p_ack BOOL,
       p_nak BOOL,
       p_acl BOOL,
       p_idx INT UNSIGNED
       ) RETURNS INT
       DETERMINISTIC
BEGIN

    IF p_idx IS NULL THEN
       SELECT order_num+1 INTO p_idx FROM message_group_entry WHERE group_id=p_group_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_idx IS NULL THEN
       SET p_idx = 0;
    END IF; 

    INSERT INTO message_group_entry (group_id, ack, nak, acl, order_num, type_id) VALUES (p_group_id, p_ack, p_nak, p_acl, p_idx, 1);
    SET @last_id = LAST_INSERT_ID();
    
    RETURN @last_id;
END$
DELIMITER ;

-- add_message_table_set(group_id, ack, nak, acl, idx)
DELIMITER $
DROP FUNCTION IF EXISTS add_message_table_set$
CREATE FUNCTION add_message_table_set(
       p_group_id INT UNSIGNED,
       p_ack BOOL,
       p_nak BOOL,
       p_acl BOOL,
       p_idx INT UNSIGNED
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    IF p_idx IS NULL THEN
       SELECT order_num+1 INTO p_idx FROM message_group_entry WHERE group_id=p_group_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_idx IS NULL THEN
       SET p_idx = 0;
    END IF; 
    
    INSERT INTO message_group_entry (group_id, ack, nak, acl, order_num, type_id) VALUES (p_group_id, p_ack, p_nak, p_acl, p_idx, 2);
    SET @last_id = LAST_INSERT_ID();

    RETURN @last_id;
END$
DELIMITER ;

-- add_message_ctrl_entry(group_id, ack, nak, acl, idx, timevalue or NULL, ac_id)
DELIMITER $
DROP FUNCTION IF EXISTS add_message_ctrl_entry$
CREATE FUNCTION add_message_ctrl_entry(
       p_group_id INT UNSIGNED,
       p_ack BOOL,
       p_nak BOOL,
       p_acl BOOL,
       p_idx INT UNSIGNED,
       p_tv INT UNSIGNED,
       p_ac_id INT UNSIGNED
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    INSERT INTO message_group_entry (group_id, ack, nak, acl, order_num, type_id) VALUES (p_group_id, p_ack, p_nak, p_acl, p_idx, 2);
    SET @last_id = LAST_INSERT_ID();

    INSERT INTO message_perform_control(message_id, tv, ac_id) VALUES (@last_id, p_tv, p_ac_id);

    RETURN @last_id;
END$
DELIMITER ;

-- add_message_register_entry(group_id, ack, nak, acl, idx, agent_name)
DELIMITER $
DROP FUNCTION IF EXISTS add_message_register_entry$
CREATE FUNCTION add_message_register_entry(
       p_group_id INT UNSIGNED,
       p_ack BOOL,
       p_nak BOOL,
       p_acl BOOL,
       p_idx INT UNSIGNED,
       agent_id_string VARCHAR(128)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE eid INT;
    
	IF p_idx IS NULL THEN
       SELECT order_num+1 INTO p_idx FROM message_group_entry WHERE group_id=p_group_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_idx IS NULL THEN
       SET p_idx = 0;
    END IF; 
    
    INSERT INTO message_group_entry (group_id, ack, nak, acl, order_num, type_id) VALUES (p_group_id, p_ack, p_nak, p_acl, p_idx, 0);
    SET @last_id = LAST_INSERT_ID();
    CALL SP__insert_agent(agent_id_string, eid); -- Select or Insert Agent ID
    INSERT INTO message_agents (message_id, agent_id) VALUES (@last_id, eid);
    RETURN @last_id;
END$
DELIMITER ;

-- create_incoming_meessage_group(timestamp, From Agent name [ie: ipn:2.1] )
DELIMITER $
DROP FUNCTION IF EXISTS create_incoming_message_group$
CREATE FUNCTION create_incoming_message_group(
       ts INT UNSIGNED,
       agent_id_string VARCHAR(128)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE eid INT;
    
    INSERT INTO message_group (ts, state_id, is_outgoing) VALUES (ts, 1, FALSE);
    SET @last_id = LAST_INSERT_ID();

    CALL SP__insert_agent(agent_id_string, eid); -- Select or Insert Agent ID
    INSERT INTO message_group_agents (group_id, agent_id) VALUES (@last_id, eid);
    RETURN @last_id;
END$
DELIMITER ;

-- add_message_table_set(group_id, ack, nak, acl, idx)
DELIMITER $
DROP FUNCTION IF EXISTS insert_message_report_entry$
CREATE FUNCTION insert_message_report_entry(
       p_msg_id int unsigned,
       p_order_num INT UNSIGNED,
       p_ari_id INT UNSIGNED,
       p_tnvc_id INT UNSIGNED,
       p_ts INT UNSIGNED
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_message_report_entry(p_msg_id, p_order_num, p_ari_id, p_tnvc_id, p_ts, idx);
    RETURN idx;
END$
DELIMITER ;

-- insert_message_group_agent_name(group_id, agent_name)
DELIMITER $
DROP FUNCTION IF EXISTS insert_message_group_agent_name$
CREATE FUNCTION insert_message_group_agent_name(
       group_id INT unsigned,
       agent_name varchar(255)
       ) RETURNS INT
       DETERMINISTIC
BEGIN
    DECLARE idx INT;
    CALL SP__insert_message_group_agent_name(group_id, agent_name, idx);
    RETURN idx;
END$
DELIMITER ;
