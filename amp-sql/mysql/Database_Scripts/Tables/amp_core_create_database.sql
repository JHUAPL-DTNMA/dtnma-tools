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

-- A DB for storing and using the ADMs and ADM objects
-- 
-- Version 1.0 of the amp database
-- Changes:
-- All sp_ are changed to SP__ to follow good sql practices.
-- 
-- Updated the constraints of Constants included derived types. 
-- 
-- Reworking SBR and TBR instance and definitons. All the base info will be in definition and Instances will be used for holding parameter info 
-- and when to run.
-- can store what the previous events and every so often remove old instances or move them to long term storage. 
--
-- Updated obj_defintion to cascade on delete 
--
-- updated some naming conventions, making sure fk agree with pk, and pk follow table name. obj_identifer to obj_metadata. 
--  


SET GLOBAL sql_mode='NO_AUTO_VALUE_ON_ZERO';
SET SESSION sql_mode='NO_AUTO_VALUE_ON_ZERO';
SELECT @@GLOBAL .sql_mode;
SELECT @@SESSION .sql_mode;
 
CREATE DATABASE /*!32312 IF NOT EXISTS*/ amp_core /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

USE amp_core;

CREATE TABLE data_type (
    data_type_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    type_name VARCHAR(50) NOT NULL UNIQUE,
    use_desc VARCHAR(255) NOT NULL,
    PRIMARY KEY (data_type_id)
);

-- AMM Object Types
INSERT INTO data_type VALUES(0,'CONST','Constant'); -- to start Auto increment at 0 
INSERT INTO data_type(type_name, use_desc) VALUES('CTRL','Control');
INSERT INTO data_type(type_name, use_desc) VALUES('EDD','Externally Defined Data');
INSERT INTO data_type(type_name, use_desc) VALUES('LIT','Literal');
INSERT INTO data_type(type_name, use_desc) VALUES('MAC','Macro');
INSERT INTO data_type(type_name, use_desc) VALUES('OPER','Operator');
INSERT INTO data_type(type_name, use_desc) VALUES('RPT','Report');
INSERT INTO data_type(type_name, use_desc) VALUES('RPTT','Report Template');
INSERT INTO data_type(type_name, use_desc) VALUES('SBR','State Based Rules');
INSERT INTO data_type(type_name, use_desc) VALUES('TBL','Table');
INSERT INTO data_type(type_name, use_desc) VALUES('TBLT','Table Template');
INSERT INTO data_type(type_name, use_desc) VALUES('TBR','Time Based Rules');
INSERT INTO data_type(type_name, use_desc) VALUES('VAR','Variable');
INSERT INTO data_type(type_name, use_desc) VALUES('MDAT','Metadata'); 

-- Primative Types
INSERT INTO data_type VALUES(16,'BOOL','Boolean'); -- to start Auto increment at 16
INSERT INTO data_type(type_name, use_desc) VALUES('BYTE','8 bits, Standard Byte');
INSERT INTO data_type(type_name, use_desc) VALUES('STR','Character String');
INSERT INTO data_type(type_name, use_desc) VALUES('INT','Signed 32 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('UINT','Unsigned 32 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('VAST','Signed 64 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('UVAST','Unsigned 64 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('REAL32','Single precision Floating Point');
INSERT INTO data_type(type_name, use_desc) VALUES('REAL64','Double precision Floating Point');

-- Compound/Derived Types
INSERT INTO data_type VALUES(32,'TV','Time Value'); -- to start Auto increment at 32 
INSERT INTO data_type(type_name, use_desc) VALUES('TS','Timestamp');
INSERT INTO data_type(type_name, use_desc) VALUES('TNV','Type-Name-Value');
INSERT INTO data_type(type_name, use_desc) VALUES('TNVC','Type-Name-Value Collection');
INSERT INTO data_type(type_name, use_desc) VALUES('ARI','AMM Resource Identifier');
INSERT INTO data_type(type_name, use_desc) VALUES('AC','ARI Collection');
INSERT INTO data_type(type_name, use_desc) VALUES('EXPR','Expression');
INSERT INTO data_type(type_name, use_desc) VALUES('BYTESTR','Bytestring');
INSERT INTO data_type(type_name, use_desc) VALUES ('PARMNAME','parameter name');
INSERT INTO data_type(type_name, use_desc) VALUES('FP','formal parameter');
INSERT INTO data_type(type_name, use_desc) VALUES ('APV','actual parameter-by-value');  
INSERT INTO data_type(type_name, use_desc) VALUES('APN', 'actual parameter-by-name');


CREATE TABLE IF NOT EXISTS amp_core.ADM_Type_Enumeration (
    data_type_id INT UNSIGNED NOT NULL,
    Enumeration INT UNSIGNED NOT NULL,
    PRIMARY KEY (data_type_id),
    CONSTRAINT fk1 FOREIGN KEY (data_type_id)
        REFERENCES amp_core.data_type (data_type_id)
        ON DELETE CASCADE ON UPDATE NO ACTION
);
    
--   Table 1: ADM Type Enumerations
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (0, 0);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (1, 1);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (2, 2);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (4, 3);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (5, 4);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (7, 5);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (8, 6);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (10, 7);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (11, 8);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (12, 9);

    

CREATE TABLE registered_agents (
    registered_agents_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    agent_id_string VARCHAR(128) NOT NULL DEFAULT 'ipn:0.0',
    first_registered DATETIME NOT NULL DEFAULT NOW(),
    last_registered DATETIME NOT NULL DEFAULT NOW(),
    PRIMARY KEY (registered_agents_id),
    UNIQUE (agent_id_string)
);

CREATE TABLE namespace (
    namespace_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    namespace_type VARCHAR(255) NOT NULL,
    issuing_org VARCHAR(255) NOT NULL,
    name_string VARCHAR(255) NOT NULL,
    version_name VARCHAR(255) NOT NULL DEFAULT '0.0.0',
    PRIMARY KEY (namespace_id)
    -- Below UNIQUE key not valid. Explicit check in SP instead instead.  TODO: Add appropriate indexes
    -- UNIQUE (namespace_type, issuing_org , name_string , version_name)
);


CREATE TABLE adm (
    namespace_id INT UNSIGNED NOT NULL,
    adm_name VARCHAR(255),
    adm_enum INT UNSIGNED,
    adm_enum_label VARCHAR(255),
    use_desc VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (namespace_id),
    FOREIGN KEY (namespace_id)
        REFERENCES namespace (namespace_id),
    UNIQUE (adm_enum)
);

CREATE TABLE network_config (
    namespace_id INT UNSIGNED NOT NULL,
    issuer_binary_string VARCHAR(255) NOT NULL,
    tag VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (namespace_id),
    FOREIGN KEY (namespace_id)
        REFERENCES namespace (namespace_id)
);


CREATE TABLE obj_metadata (
    obj_metadata_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    data_type_id INT UNSIGNED NOT NULL,
    obj_name VARCHAR(255) NOT NULL,
    namespace_id INT UNSIGNED,
    PRIMARY KEY (obj_metadata_id),
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    FOREIGN KEY (namespace_id)
        REFERENCES namespace (namespace_id)
        ON DELETE CASCADE,
    UNIQUE (data_type_id , obj_name , namespace_id)
);


CREATE TABLE obj_formal_definition (
    obj_formal_definition_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    obj_metadata_id INT UNSIGNED NOT NULL,
    obj_enum INT UNSIGNED DEFAULT NULL, -- Nominally obj_enum, data_type_id, and adm_enum would make a UNIQUE KEY, but tables are too normalized to enforce this
    use_desc VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (obj_formal_definition_id),
    FOREIGN KEY (obj_metadata_id)
        REFERENCES obj_metadata (obj_metadata_id)
        ON DELETE CASCADE
);

CREATE TABLE obj_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    obj_metadata_id INT UNSIGNED NOT NULL,
    use_desc VARCHAR(255) DEFAULT ' ',
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_metadata_id)
        REFERENCES obj_metadata (obj_metadata_id)
        ON DELETE CASCADE
);

CREATE TABLE formal_parmspec (
    fp_spec_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    num_parms INT UNSIGNED NOT NULL,
    use_desc VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (fp_spec_id)
);


CREATE TABLE IF NOT EXISTS formal_parm (
    fp_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    fp_spec_id INT UNSIGNED NOT NULL,
    order_num INT UNSIGNED NOT NULL,
    parm_name VARCHAR(255) DEFAULT NULL,
    data_type_id INT UNSIGNED NOT NULL,
    obj_actual_definition_id INT UNSIGNED NULL DEFAULT NULL,
    PRIMARY KEY (fp_id),
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE ON UPDATE NO ACTION,
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE SET NULL,
    UNIQUE (fp_id , order_num)
);

CREATE TABLE ari_collection (
    ac_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    num_entries INT UNSIGNED NOT NULL DEFAULT 0,
    use_desc VARCHAR(255),
    PRIMARY KEY (ac_id)
);


CREATE TABLE type_name_value_collection (
    tnvc_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    use_desc VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (tnvc_id)
);

CREATE TABLE type_name_value_entry (
    tnv_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    tnvc_id INT UNSIGNED NOT NULL,
    order_num INT UNSIGNED DEFAULT 0,
    data_type_id INT UNSIGNED,
    data_name VARCHAR(255) DEFAULT NULL,
    fp_id INT UNSIGNED DEFAULT NULL,
    PRIMARY KEY (tnv_id),
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    FOREIGN KEY (tnvc_id)
        REFERENCES type_name_value_collection (tnvc_id)
        ON DELETE CASCADE,
    FOREIGN KEY (fp_id) REFERENCES formal_parm (fp_id),
UNIQUE (tnvc_id , order_num)
);

CREATE TABLE type_name_value_obj_entry ( 
tnv_id INT UNSIGNED NOT NULL,
obj_actual_definition_id INT UNSIGNED DEFAULT NULL,
FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE SET NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_int_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value INT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_ac_entry ( 
tnv_id INT UNSIGNED NOT NULL,
ac_id INT UNSIGNED DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE,
FOREIGN KEY (ac_id)
	REFERENCES  ari_collection (ac_id)
);

CREATE TABLE type_name_value_tnvc_entry ( 
tnv_id INT UNSIGNED NOT NULL,
tnvc_id INT UNSIGNED DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE,
FOREIGN KEY (tnvc_id)
	REFERENCES  type_name_value_collection (tnvc_id)
);


CREATE TABLE type_name_value_uint_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value INT UNSIGNED DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_vast_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value BIGINT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_uvast_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value BIGINT UNSIGNED DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_real32_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value FLOAT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);


CREATE TABLE type_name_value_real64_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value DOUBLE DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_string_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value VARCHAR(255) DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_bool_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value BOOL DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_byte_entry ( 
tnv_id INT UNSIGNED NOT NULL,
entry_value TINYINT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE ari_collection_entry (
    ac_entry_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    ac_id INT UNSIGNED NOT NULL,
    order_num INT UNSIGNED NOT NULL,
    PRIMARY KEY (ac_entry_id),
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (ac_id , order_num)
);

CREATE TABLE ari_collection_formal_entry (
    ac_entry_id INT UNSIGNED NOT NULL,
    obj_formal_definition_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (ac_entry_id),
    FOREIGN KEY (ac_entry_id)
        REFERENCES ari_collection_entry (ac_entry_id)
        ON DELETE CASCADE,
    FOREIGN KEY (obj_formal_definition_id)
        REFERENCES obj_formal_definition (obj_formal_definition_id)
        ON DELETE CASCADE
);


CREATE TABLE ari_collection_actual_entry (
    ac_entry_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (ac_entry_id),
    FOREIGN KEY (ac_entry_id)
        REFERENCES ari_collection_entry (ac_entry_id)
        ON DELETE CASCADE,
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE
);



 
CREATE TABLE actual_parmspec (
    ap_spec_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    fp_spec_id INT UNSIGNED NOT NULL,
    tnvc_id INT UNSIGNED NOT NULL,
    use_desc VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (ap_spec_id),
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE,
    FOREIGN KEY (tnvc_id)
        REFERENCES type_name_value_collection (tnvc_id)
        ON DELETE CASCADE

);



CREATE TABLE edd_formal_definition (
    obj_formal_definition_id INT UNSIGNED NOT NULL,
    fp_spec_id INT UNSIGNED,
    data_type_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_formal_definition_id),
    FOREIGN KEY (obj_formal_definition_id)
        REFERENCES obj_formal_definition (obj_formal_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE,
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    UNIQUE (fp_spec_id)
);

CREATE TABLE edd_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    ap_spec_id INT UNSIGNED,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ap_spec_id)
        REFERENCES actual_parmspec (ap_spec_id)
        ON DELETE CASCADE,
    UNIQUE (ap_spec_id)
);


CREATE TABLE const_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    data_type_id INT UNSIGNED NOT NULL,
    data_value VARCHAR(255) NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
--    UNIQUE (data_type_id , data_value), -- TODO: Update SP to auto-select existing, joining on metadata_id to enforce constraint. The columns in this table alone are insufficient
    CHECK (data_type_id > 15 AND data_type_id < 40)
);

CREATE TABLE control_formal_definition (
    obj_formal_definition_id INT UNSIGNED NOT NULL,
    fp_spec_id INT UNSIGNED,
    PRIMARY KEY (obj_formal_definition_id),
    FOREIGN KEY (obj_formal_definition_id)
        REFERENCES obj_formal_definition (obj_formal_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE,
    UNIQUE (fp_spec_id)
);

CREATE TABLE control_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    ap_spec_id INT UNSIGNED,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ap_spec_id)
        REFERENCES actual_parmspec (ap_spec_id)
        ON DELETE CASCADE,
    UNIQUE (ap_spec_id)
);

CREATE TABLE macro_formal_definition (
    obj_formal_definition_id INT UNSIGNED NOT NULL,
    fp_spec_id INT UNSIGNED,
    ac_id INT UNSIGNED NOT NULL,
    max_call_depth INT UNSIGNED DEFAULT 4,
    PRIMARY KEY (obj_formal_definition_id),
    FOREIGN KEY (obj_formal_definition_id)
        REFERENCES obj_formal_definition (obj_formal_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (fp_spec_id)
);


CREATE TABLE macro_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    ap_spec_id INT UNSIGNED,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ap_spec_id)
        REFERENCES actual_parmspec (ap_spec_id)
        ON DELETE CASCADE,
    UNIQUE (ap_spec_id)
);

CREATE TABLE operator_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    data_type_id INT UNSIGNED NOT NULL,
    num_operands INT UNSIGNED NOT NULL,
    tnvc_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    FOREIGN KEY (tnvc_id)
        REFERENCES type_name_value_collection (tnvc_id)
        ON DELETE CASCADE
);

CREATE TABLE literal_actual_definition (
    obj_actual_definition_id INT(10) UNSIGNED NOT NULL,
    data_type_id INT(10) UNSIGNED NOT NULL,
    data_value VARCHAR(255) NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    CHECK (data_type_id > 16)
    -- UNIQUE (data_value) -- TODO: Unique constraint must include metadata_id, so we will have to enforce via SP later
);


CREATE TABLE expression (
    expression_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    data_type_id INT UNSIGNED NOT NULL,
    ac_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (expression_id),
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (data_type_id , ac_id)
);

CREATE TABLE report_template_formal_definition (
    obj_formal_definition_id INT UNSIGNED NOT NULL,
    fp_spec_id INT UNSIGNED,
    ac_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_formal_definition_id),
    FOREIGN KEY (obj_formal_definition_id)
        REFERENCES obj_formal_definition (obj_formal_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (fp_spec_id)
);

CREATE TABLE report_template_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    ap_spec_id INT UNSIGNED,
    ts TIMESTAMP NULL COMMENT "TS value embedded in raw report. This field may be NULL if embedded in a set of reports with a common timestamp (per specification).  Calling application is responsible for converting AMP Timestamp specification into a SQL-compatible format.",
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ap_spec_id)
        REFERENCES actual_parmspec (ap_spec_id)
        ON DELETE CASCADE,
    UNIQUE (ap_spec_id)
);

-- TODO: need to look at this ***
-- CREATE TABLE report_instance (
--     obj_actual_definition_id INT UNSIGNED NOT NULL,
--     generation_time TIME NOT NULL,
--     tnvc_id INT UNSIGNED NOT NULL,
--     PRIMARY KEY (obj_actual_definition_id),
--     FOREIGN KEY (obj_actual_definition_id)
--         REFERENCES obj_actual_definition (obj_actual_definition_id)
--         ON DELETE CASCADE,
--     FOREIGN KEY (tnvc_id)
--         REFERENCES type_name_value_collection (tnvc_id)
-- );

CREATE TABLE sbr_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    expression_id INT UNSIGNED NOT NULL,
    run_count BIGINT UNSIGNED NOT NULL,
    start_time TIME NOT NULL,
    ac_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (expression_id)
        REFERENCES expression (expression_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (expression_id , run_count , start_time , ac_id)
);
 

CREATE TABLE tbr_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    wait_period TIME NOT NULL,
    run_count BIGINT UNSIGNED NOT NULL,
    start_time TIME NOT NULL,
    ac_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (wait_period , run_count , start_time , ac_id)
);

CREATE TABLE table_template_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    tnvc_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (tnvc_id)
        REFERENCES type_name_value_collection (tnvc_id)
        ON DELETE CASCADE,
    UNIQUE (tnvc_id)
);

-- need to work on storing rows 
-- CREATE TABLE table_instance (
--     obj_actual_definition_id INT UNSIGNED NOT NULL,
--     time_stamp TIME NOT NULL,
--     num_rows INT UNSIGNED NOT NULL DEFAULT 0,
--     tnvc_id INT UNSIGNED,
--     PRIMARY KEY (obj_actual_definition_id),
--     FOREIGN KEY (obj_actual_definition_id)
--         REFERENCES obj_actual_definition (obj_actual_definition_id)
--         ON DELETE CASCADE,
--     FOREIGN KEY (tnvc_id)
--         REFERENCES type_name_value_collection (tnvc_id)
--         ON DELETE CASCADE
-- ); 

CREATE TABLE variable_actual_definition (
    obj_actual_definition_id INT UNSIGNED NOT NULL,
    data_type_id INT UNSIGNED NOT NULL,
    expression_id INT UNSIGNED NOT NULL,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE,
    FOREIGN KEY (expression_id)
        REFERENCES expression (expression_id)
        ON DELETE CASCADE,
    UNIQUE (data_type_id , expression_id)
);



-- Ensure MySQL allows explicit INSERT of 0-values for initializing lookup tables
-- (Note: Other DB systems, such as PostgreSQL require different syntax for declaring auto-increment fields to begin with).
SET sql_mode = NO_AUTO_VALUE_ON_ZERO;

DROP TABLE IF EXISTS amp_core.message_group_agents;
DROP TABLE IF EXISTS amp_core.message_perform_control;
DROP TABLE IF EXISTS amp_core.message_agents;
DROP TABLE IF EXISTS amp_core.message_report_set_entry;
DROP TABLE IF EXISTS amp_core.report_definition;
DROP TABLE IF EXISTS amp_core.message_group_entry;
DROP TABLE IF EXISTS amp_core.message_group;
DROP TABLE IF EXISTS amp_core.enum_message_group_states;
DROP TABLE IF EXISTS amp_core.enum_message_group_types;

DROP TABLE IF EXISTS amp_core.nm_mgr_log;

CREATE TABLE `amp_core`.`enum_message_group_states` (
  `state_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NULL,
  `description` VARCHAR(255) NULL,
  PRIMARY KEY (`state_id`),
  UNIQUE INDEX `name_UNIQUE` (`name` ASC) VISIBLE
  )
  COMMENT = 'Enumeration of state_id values in message_group'
  ;
INSERT INTO `enum_message_group_states` (`state_id`, `name`, `description`) VALUES
(0,	'initializing',	'Default value, but generally used solely by UI when creating new outgoing messages.'),
(1,	'ready',	'Indicates message is ready for processing, either for the manager to transmit (outgoing) or user to analyze (incoming)'),
(2,	'sent',	'For outgoing sets only, this indicates that the manager has parsed and sent this message group.'),
(3,	'error',	'Indicates that the manager failed to parse or transmit this group successfully.'),
(4,	'aborted',	'Indicates this outgoing message set has been aborted by the user before transmission.');


CREATE TABLE `amp_core`.`enum_message_group_types` (
  `type_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NULL,
  PRIMARY KEY (`type_id`),
  UNIQUE INDEX `name_UNIQUE` (`name` ASC) VISIBLE
  )
  COMMENT = 'Enumeration of Message group opcode/type values'
  ;
INSERT INTO `enum_message_group_types` (`type_id`, `name`) VALUES
(0,	'REG_AGENT'),
(1,	'RPT_SET'),
(2,	'PERF_CTRL'),
(3,	'TBL_SET');


CREATE TABLE `amp_core`.`message_group` (
  `group_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `created_ts` DATETIME NOT NULL DEFAULT NOW(),
  `modified_ts` DATETIME NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'If NULL, this is an outgoing message.  Otherwise this is the raw AMP timestamp value of the received message group.  The function amp_ts_to_datetime() can convert this into a MySQL DateTime format.',
  `ts` INT UNSIGNED NULL,
  `is_outgoing` BOOL,
  `state_id` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`group_id`),
  CONSTRAINT `message_group_state_id` FOREIGN KEY (`state_id`) REFERENCES `enum_message_group_states` (`state_id`) ON DELETE RESTRICT
  )
COMMENT = 'Incoming and outgoing message sets';

CREATE TABLE `amp_core`.`message_group_agents` (
  `group_id` INT UNSIGNED NOT NULL,
  `agent_id` INT UNSIGNED NOT NULL,
  KEY (`group_id`),
  CONSTRAINT message_group_agents_group_id FOREIGN KEY (group_id) REFERENCES message_group (group_id) ON DELETE CASCADE,
  CONSTRAINT `message_group_agents_agent_id` FOREIGN KEY (`agent_id`) REFERENCES `registered_agents` (`registered_agents_id`) ON DELETE CASCADE
  )
  COMMENT = 'For received groups, the agent that transmitted the group (singular entry).  For outgoing groups, a set of one or more destination agents for this message'
  ;

CREATE TABLE `amp_core`.`message_group_entry` (
  `group_id` INT UNSIGNED NOT NULL,
  `message_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `ack` BOOL NOT NULL,
  `nak` BOOL NOT NULL,
  `acl` BOOL NOT NULL,
  `order_num` INT UNSIGNED NOT NULL,
  `type_id` INT UNSIGNED NOT NULL COMMENT 'Conforms to AMP Message Header Opcode value',
  PRIMARY KEY (`message_id`),
  UNIQUE KEY (group_id, message_id, order_num),
  CONSTRAINT message_group_entry_group_id FOREIGN KEY (group_id) REFERENCES message_group (group_id) ON DELETE CASCADE,
  CONSTRAINT message_group_entry_opcode FOREIGN KEY (type_id) REFERENCES enum_message_group_types (type_id) ON DELETE CASCADE
  )
  COMMENT = 'For received groups, the agent that transmitted the group (singular entry).  For outgoing groups, a set of one or more destination agents for this message'
  ;

CREATE TABLE `amp_core`.`message_perform_control` (
  `message_id` INT UNSIGNED NOT NULL,
  `tv` INT UNSIGNED NULL,
  `ac_id` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`message_id`),
  CONSTRAINT message_perform_control_mid FOREIGN KEY (message_id) REFERENCES message_group_entry (message_id) ON DELETE CASCADE
  )
  COMMENT = 'Control(s) to be sent to agent'
  ;

CREATE TABLE `amp_core`.`message_agents` (
  `message_id` INT UNSIGNED NOT NULL,
  `agent_id` INT UNSIGNED NULL,
  KEY (`message_id`),
  CONSTRAINT message_agents_mid FOREIGN KEY (message_id) REFERENCES message_group_entry (message_id) ON DELETE CASCADE,
  CONSTRAINT message_agents_eid FOREIGN KEY (agent_id) REFERENCES registered_agents (registered_agents_id) ON DELETE CASCADE
  )
  COMMENT = 'For register_Agent messages, there will always be a single entry in this table. For Table and Msg sets this is the [optional] list of RX names for the set'
  ;

-- TODO TABLE message_table_set_entry

CREATE TABLE `amp_core`.`report_definition` (
  `report_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `ari_id` INT UNSIGNED NOT NULL,
  ts INT UNSIGNED NULL COMMENT 'Optional AMP Timestamp as received',
 `tnvc_id` INT UNSIGNED NULL,
  PRIMARY KEY (`report_id`),
  CONSTRAINT report_definition_ari FOREIGN KEY (ari_id) REFERENCES obj_actual_definition (obj_actual_definition_id) ON DELETE CASCADE,
  CONSTRAINT report_definition_tnvc FOREIGN KEY (tnvc_id) REFERENCES type_name_value_collection (tnvc_id) ON DELETE CASCADE
  )
  COMMENT = 'An instance of a receipt message report'
  ;

CREATE TABLE `amp_core`.`message_report_set_entry` (
  `message_id` INT UNSIGNED NOT NULL,
  `report_id` INT UNSIGNED NULL,
  `order_num` TINYINT UNSIGNED,
  UNIQUE KEY (`message_id`, `order_num`),
  CONSTRAINT message_report_entry_mid FOREIGN KEY (message_id) REFERENCES message_group_entry (message_id) ON DELETE CASCADE,
  CONSTRAINT message_report_entry_rid FOREIGN KEY (report_id) REFERENCES report_definition (report_id) ON DELETE CASCADE
  )
  COMMENT = 'A set of received reports'
  ;

CREATE TABLE `amp_core`.`nm_mgr_log` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `time` TIMESTAMP NOT NULL DEFAULT NOW(),
  `msg` VARCHAR(255) NOT NULL,
  `details` TEXT NULL,
  `level` INT UNSIGNED NULL,
  `source` VARCHAR(32) NULL,
  `file` VARCHAR(64) NULL,
  `line` INT UNSIGNED NULL,
  PRIMARY KEY (`id`)
  )
  COMMENT = 'AMP_DEBUG logging table for ION NM Manager Application. Primarily intended for debug purposes.'
  ;


