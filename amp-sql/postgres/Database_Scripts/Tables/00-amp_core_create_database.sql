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


-- SET GLOBAL sql_mode='NO_AUTO_VALUE_ON_ZERO';
-- SET SESSION sql_mode='NO_AUTO_VALUE_ON_ZERO';
-- SELECT @@GLOBAL .sql_mode;
-- SELECT @@SESSION .sql_mode;

-- CREATE DATABASE /*!32312 IF NOT EXISTS*/ /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

CREATE OR REPLACE FUNCTION update_changetimestamp_column()
RETURNS TRIGGER AS $$
BEGIN
   NEW.changetimestamp = now();
   RETURN NEW;
END;
$$ language 'plpgsql';

CREATE SEQUENCE data_type_id_seq minvalue 0;
CREATE TABLE data_type (
    data_type_id integer NOT NULL DEFAULT nextval('data_type_id_seq'),
    type_name VARCHAR(50) NOT NULL UNIQUE,
    use_desc varchar NOT NULL,
    PRIMARY KEY (data_type_id)
);
ALTER SEQUENCE data_type_id_seq OWNED BY data_type.data_type_id;
ALTER SEQUENCE data_type_id_seq RESTART WITH 0;
-- AMM Object Types
INSERT INTO data_type(type_name, use_desc) VALUES('CONST','Constant'); -- to start Auto increment at 0
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

ALTER SEQUENCE data_type_id_seq RESTART WITH 16;
-- Primative Types
INSERT INTO data_type(type_name, use_desc) VALUES('BOOL','Boolean'); -- to start Auto increment at 16
INSERT INTO data_type(type_name, use_desc) VALUES('BYTE','8 bits, Standard Byte');
INSERT INTO data_type(type_name, use_desc) VALUES('STR','Character String');
INSERT INTO data_type(type_name, use_desc) VALUES('INT','Signed 32 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('UINT',' 32 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('VAST','Signed 64 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('UVAST',' 64 bit Integer');
INSERT INTO data_type(type_name, use_desc) VALUES('REAL32','Single precision Floating Point');
INSERT INTO data_type(type_name, use_desc) VALUES('REAL64','double precision precision Floating Point');

ALTER SEQUENCE data_type_id_seq RESTART WITH 32;
-- Compound/Derived Types
INSERT INTO data_type(type_name, use_desc) VALUES('TV','Time Value'); -- to start Auto increment at 32
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


CREATE TABLE IF NOT EXISTS ADM_Type_Enumeration (
    data_type_id INT  NOT NULL,
    Enumeration INT  NOT NULL,
    PRIMARY KEY (data_type_id),
    CONSTRAINT fk1 FOREIGN KEY (data_type_id)
        REFERENCES data_type (data_type_id)
        ON DELETE CASCADE ON UPDATE NO ACTION
);

--   Table 1: ADM Type Enumerations
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (0, 0);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (1, 1);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (2, 2);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (3, 3);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (5, 4);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (7, 5);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (8, 6);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (10, 7);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (11, 8);
INSERT INTO ADM_Type_Enumeration (data_type_id, Enumeration) VALUES (12, 9);



CREATE TABLE registered_agents (
    registered_agents_id serial  NOT NULL ,
    agent_id_string VARCHAR(128) NOT NULL DEFAULT 'ipn:0.0',
    first_registered TIMESTAMP NOT NULL DEFAULT NOW(),
    last_registered TIMESTAMP NOT NULL DEFAULT NOW(),
    historical_data json[],
    received_reports json[],
    supported_a_d_ms character varying(255)[],  
    PRIMARY KEY (registered_agents_id),
    UNIQUE (agent_id_string)
);

CREATE TABLE agent_parameter(
    agent_parameter_id serial NOT NULL, 
    command_name VARCHAR NOT NULL,
    command_parameters VARCHAR,
    PRIMARY KEY (agent_parameter_id),
    UNIQUE (command_name,command_parameters)
);

CREATE TABLE agent_parameter_received(
    ts TIMESTAMP default current_timestamp,
    agent_parameter_received_id serial NOT NULL,
    manager_id INT,
    registered_agents_id INT NOT NULL,
    agent_parameter_id INT NOT NULL,
    command_parameters VARCHAR,
    PRIMARY KEY (agent_parameter_received_id),
    FOREIGN KEY (registered_agents_id)
        REFERENCES registered_agents (registered_agents_id),
    FOREIGN KEY (agent_parameter_id)
        REFERENCES agent_parameter (agent_parameter_id)
);



CREATE TABLE namespace (
    namespace_id serial  NOT NULL ,
    namespace_type varchar NOT NULL,
    issuing_org varchar NOT NULL,
    name_string varchar NOT NULL,
    version_name varchar NOT NULL DEFAULT '0.0.0',
    PRIMARY KEY (namespace_id)
    -- Below UNIQUE key not valid. Explicit check in SP instead instead.  TODO: Add appropriate indexes
    -- UNIQUE (namespace_type, issuing_org , name_string , version_name)
);


CREATE TABLE adm (
    namespace_id INT  NOT NULL,
    adm_name varchar,
    adm_enum INT ,
    adm_enum_label varchar,
    use_desc varchar DEFAULT NULL,
    PRIMARY KEY (namespace_id),
    FOREIGN KEY (namespace_id)
        REFERENCES namespace (namespace_id),
    UNIQUE (adm_enum)
);

CREATE TABLE network_config (
    namespace_id INT  NOT NULL,
    issuer_binary_string varchar NOT NULL,
    tag varchar DEFAULT NULL,
    PRIMARY KEY (namespace_id),
    FOREIGN KEY (namespace_id)
        REFERENCES namespace (namespace_id)
);


CREATE TABLE obj_metadata (
    obj_metadata_id serial  NOT NULL ,
    data_type_id INT  NOT NULL,
    obj_name varchar NOT NULL,
    namespace_id INT ,
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
    obj_formal_definition_id serial  NOT NULL ,
    obj_metadata_id INT  NOT NULL,
    obj_enum INT  DEFAULT NULL, -- Nominally obj_enum, data_type_id, and adm_enum would make a UNIQUE KEY, but tables are too normalized to enforce this
    use_desc varchar DEFAULT NULL,
    PRIMARY KEY (obj_formal_definition_id),
    FOREIGN KEY (obj_metadata_id)
        REFERENCES obj_metadata (obj_metadata_id)
        ON DELETE CASCADE
);

CREATE TABLE obj_actual_definition (
    obj_actual_definition_id serial  NOT NULL ,
    obj_metadata_id INT  NOT NULL,
    use_desc varchar DEFAULT ' ',
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_metadata_id)
        REFERENCES obj_metadata (obj_metadata_id)
        ON DELETE CASCADE
);

CREATE TABLE formal_parmspec (
    fp_spec_id serial  NOT NULL ,
    num_parms INT  NOT NULL,
    use_desc varchar DEFAULT NULL,
    PRIMARY KEY (fp_spec_id)
);


CREATE TABLE IF NOT EXISTS formal_parm (
    fp_id serial  NOT NULL ,
    fp_spec_id INT  NOT NULL,
    order_num INT  NOT NULL,
    parm_name varchar DEFAULT NULL,
    data_type_id INT  NOT NULL,
    obj_actual_definition_id INT  NULL DEFAULT NULL,
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
    ac_id serial  NOT NULL ,
    num_entries INT  NOT NULL DEFAULT 0,
    use_desc varchar,
    PRIMARY KEY (ac_id)
);


CREATE TABLE type_name_value_collection (
    tnvc_id serial  NOT NULL ,
    use_desc varchar DEFAULT NULL,
    PRIMARY KEY (tnvc_id)
);

CREATE TABLE type_name_value_entry (
    tnv_id serial  NOT NULL ,
    tnvc_id INT  NOT NULL,
    order_num INT  DEFAULT 0,
    data_type_id INT ,
    data_name varchar DEFAULT NULL,
    fp_id INT  DEFAULT NULL,
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
tnv_id INT  NOT NULL,
obj_actual_definition_id INT  DEFAULT NULL,
FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE SET NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_int_entry (
tnv_id INT  NOT NULL,
entry_value INT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_ac_entry (
tnv_id INT  NOT NULL,
ac_id INT  DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE,
FOREIGN KEY (ac_id)
	REFERENCES  ari_collection (ac_id)
);

CREATE TABLE type_name_value_tnvc_entry (
tnv_id INT  NOT NULL,
tnvc_id INT  DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE,
FOREIGN KEY (tnvc_id)
	REFERENCES  type_name_value_collection (tnvc_id)
);


CREATE TABLE type_name_value_uint_entry (
tnv_id INT  NOT NULL,
entry_value INT  DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_vast_entry (
tnv_id INT  NOT NULL,
entry_value BIGINT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_uvast_entry (
tnv_id INT  NOT NULL,
entry_value BIGINT  DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_real32_entry (
tnv_id INT  NOT NULL,
entry_value FLOAT DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);


CREATE TABLE type_name_value_real64_entry (
tnv_id INT  NOT NULL,
entry_value double precision DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_string_entry (
tnv_id INT  NOT NULL,
entry_value varchar DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_bool_entry (
tnv_id INT  NOT NULL,
entry_value BOOL DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE type_name_value_byte_entry (
tnv_id INT  NOT NULL,
entry_value smallint DEFAULT NULL,
FOREIGN KEY (tnv_id)
	REFERENCES  type_name_value_entry (tnv_id)
    ON DELETE CASCADE
);

CREATE TABLE ari_collection_entry (
    ac_entry_id serial  NOT NULL ,
    ac_id INT  NOT NULL,
    order_num INT  NOT NULL,
    PRIMARY KEY (ac_entry_id),
    FOREIGN KEY (ac_id)
        REFERENCES ari_collection (ac_id)
        ON DELETE CASCADE,
    UNIQUE (ac_id , order_num)
);

CREATE TABLE ari_collection_formal_entry (
    ac_entry_id INT  NOT NULL,
    obj_formal_definition_id INT  NOT NULL,
    PRIMARY KEY (ac_entry_id),
    FOREIGN KEY (ac_entry_id)
        REFERENCES ari_collection_entry (ac_entry_id)
        ON DELETE CASCADE,
    FOREIGN KEY (obj_formal_definition_id)
        REFERENCES obj_formal_definition (obj_formal_definition_id)
        ON DELETE CASCADE
);


CREATE TABLE ari_collection_actual_entry (
    ac_entry_id serial  NOT NULL ,
    obj_actual_definition_id INT  NOT NULL,
    PRIMARY KEY (ac_entry_id),
    FOREIGN KEY (ac_entry_id)
        REFERENCES ari_collection_entry (ac_entry_id)
        ON DELETE CASCADE,
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE
);




CREATE TABLE actual_parmspec (
    ap_spec_id serial  NOT NULL ,
    fp_spec_id INT  NOT NULL,
    tnvc_id INT  NOT NULL,
    use_desc varchar DEFAULT NULL,
    PRIMARY KEY (ap_spec_id),
    FOREIGN KEY (fp_spec_id)
        REFERENCES formal_parmspec (fp_spec_id)
        ON DELETE CASCADE,
    FOREIGN KEY (tnvc_id)
        REFERENCES type_name_value_collection (tnvc_id)
        ON DELETE CASCADE

);



CREATE TABLE edd_formal_definition (
    obj_formal_definition_id INT  NOT NULL,
    fp_spec_id INT ,
    data_type_id INT  NOT NULL,
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
    obj_actual_definition_id INT  NOT NULL,
    ap_spec_id INT ,
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
    obj_actual_definition_id INT  NOT NULL,
    data_type_id INT  NOT NULL,
    data_value varchar NOT NULL,
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
    obj_formal_definition_id INT  NOT NULL,
    fp_spec_id INT ,
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
    obj_actual_definition_id INT  NOT NULL,
    ap_spec_id INT ,
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
    obj_formal_definition_id INT  NOT NULL,
    fp_spec_id INT ,
    ac_id INT  NOT NULL,
    max_call_depth INT  DEFAULT 4,
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
    obj_actual_definition_id INT  NOT NULL,
    ap_spec_id INT ,
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
    obj_actual_definition_id INT  NOT NULL,
    data_type_id INT  NOT NULL,
    num_operands INT  NOT NULL,
    tnvc_id INT  NOT NULL,
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
    obj_actual_definition_id INT  NOT NULL,
    data_type_id INT  NOT NULL,
    data_value varchar NOT NULL,
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
    expression_id serial  NOT NULL ,
    data_type_id INT  NOT NULL,
    ac_id INT  NOT NULL,
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
    obj_formal_definition_id INT  NOT NULL,
    fp_spec_id INT ,
    ac_id INT  NOT NULL,
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
    obj_actual_definition_id INT  NOT NULL,
    ap_spec_id INT ,
    ts TIMESTAMP NULL,

    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (ap_spec_id)
        REFERENCES actual_parmspec (ap_spec_id)
        ON DELETE CASCADE,
    UNIQUE (ap_spec_id)
);
COMMENT ON COLUMN report_template_actual_definition.ts IS 'TS value embedded in raw report. This field may be NULL if embedded in a set of reports with a common timestamp (per specification).  Calling application is responsible for converting AMP Timestamp specification into a SQL-compatible format.';

-- TODO: need to look at this ***
-- CREATE TABLE report_instance (
--     obj_actual_definition_id INT  NOT NULL,
--     generation_time TIME NOT NULL,
--     tnvc_id INT  NOT NULL,
--     PRIMARY KEY (obj_actual_definition_id),
--     FOREIGN KEY (obj_actual_definition_id)
--         REFERENCES obj_actual_definition (obj_actual_definition_id)
--         ON DELETE CASCADE,
--     FOREIGN KEY (tnvc_id)
--         REFERENCES type_name_value_collection (tnvc_id)
-- );

CREATE TABLE sbr_actual_definition (
    obj_actual_definition_id INT  NOT NULL,
    expression_id INT  NOT NULL,
    run_count BIGINT  NOT NULL,
    start_time TIME NOT NULL,
    ac_id INT  NOT NULL,
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
    obj_actual_definition_id INT  NOT NULL,
    wait_period TIME NOT NULL,
    run_count BIGINT  NOT NULL,
    start_time TIME NOT NULL,
    ac_id INT  NOT NULL,
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
    obj_actual_definition_id INT  NOT NULL,
    tnvc_id INT  NOT NULL,
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
CREATE TABLE table_instance (
    obj_actual_definition_id INT  NOT NULL,
    time_stamp TIME NOT NULL,
    num_rows INT  NOT NULL DEFAULT 0,
    tnvc_id INT ,
    PRIMARY KEY (obj_actual_definition_id),
    FOREIGN KEY (obj_actual_definition_id)
        REFERENCES obj_actual_definition (obj_actual_definition_id)
        ON DELETE CASCADE,
    FOREIGN KEY (tnvc_id)
        REFERENCES type_name_value_collection (tnvc_id)
        ON DELETE CASCADE
);

CREATE TABLE variable_actual_definition (
    obj_actual_definition_id INT  NOT NULL,
    data_type_id INT  NOT NULL,
    expression_id INT  NOT NULL,
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
-- SET sql_mode = NO_AUTO_VALUE_ON_ZERO;

DROP TABLE IF EXISTS message_group_agents;
DROP TABLE IF EXISTS message_perform_control;
DROP TABLE IF EXISTS message_agents;
DROP TABLE IF EXISTS message_report_set_entry;
DROP TABLE IF EXISTS report_definition;
DROP TABLE IF EXISTS message_group_entry;
DROP TABLE IF EXISTS message_group;
DROP TABLE IF EXISTS enum_message_group_states;
DROP TABLE IF EXISTS enum_message_group_types;

DROP TABLE IF EXISTS nm_mgr_log;

CREATE TABLE enum_message_group_states (
  state_id serial  NOT NULL ,
  name VARCHAR(45) NULL UNIQUE,
  description varchar NULL,
  PRIMARY KEY (state_id)
  );
  COMMENT ON TABLE enum_message_group_states is 'Enumeration of state_id values in message_group'
  ;
INSERT INTO enum_message_group_states (state_id, name, description) VALUES
(0,	'initializing',	'Default value, but generally used solely by UI when creating new outgoing messages.'),
(1,	'ready',	'Indicates message is ready for processing, either for the manager to transmit (outgoing) or user to analyze (incoming)'),
(2,	'sent',	'For outgoing sets only, this indicates that the manager has parsed and sent this message group.'),
(3,	'error',	'Indicates that the manager failed to parse or transmit this group successfully.'),
(4,	'aborted',	'Indicates this outgoing message set has been aborted by the user before transmission.');


CREATE TABLE enum_message_group_types (
  type_id serial  NOT NULL ,
  name VARCHAR(45) NULL UNIQUE,
  PRIMARY KEY (type_id)
  );
  COMMENT ON TABLE enum_message_group_types is 'Enumeration of Message group opcode/type values'
  ;
INSERT INTO enum_message_group_types (type_id, name) VALUES
(0,	'REG_AGENT'),
(1,	'RPT_SET'),
(2,	'PERF_CTRL'),
(3,	'TBL_SET');


CREATE TABLE message_group (
  group_id serial  NOT NULL ,
  created_ts TIMESTAMP NOT NULL DEFAULT NOW(),
  modified_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  ts INT  NULL,
  is_outgoing BOOL,
  state_id INT  NOT NULL DEFAULT 0,
  PRIMARY KEY (group_id),
  CONSTRAINT message_group_state_id FOREIGN KEY (state_id) REFERENCES enum_message_group_states (state_id) ON DELETE RESTRICT
  );
COMMENT on TABLE message_group IS 'Incoming and outgoing message sets';
COMMENT on COLUMN message_group.modified_ts is 'If NULL, this is an outgoing message.  Otherwise this is the raw AMP timestamp value of the received message group.  The function amp_ts_to_datetime() can convert this into a MySQL TIMESTAMP format.';
 CREATE TRIGGER update_mg_changetimestamp BEFORE UPDATE
    ON message_group FOR EACH ROW EXECUTE PROCEDURE
    update_changetimestamp_column();

CREATE TABLE message_group_agents (
  group_id INT  NOT NULL,
  agent_id INT  NOT NULL,
  PRIMARY KEY (group_id),
  CONSTRAINT message_group_agents_group_id FOREIGN KEY (group_id) REFERENCES message_group (group_id) ON DELETE CASCADE,
  CONSTRAINT message_group_agents_agent_id FOREIGN KEY (agent_id) REFERENCES registered_agents (registered_agents_id) ON DELETE CASCADE
  );
  COMMENT ON TABLE message_group_agents IS 'For received groups, the agent that transmitted the group (singular entry).  For outgoing groups, a set of one or more destination agents for this message';

CREATE TABLE message_group_entry (
  group_id INT  NOT NULL,
  message_id serial  NOT NULL ,
  ack BOOL NOT NULL,
  nak BOOL NOT NULL,
  acl BOOL NOT NULL,
  order_num INT  NOT NULL,
  type_id serial  NOT NULL ,
  PRIMARY KEY (message_id),
  UNIQUE (group_id, message_id, order_num),
  CONSTRAINT message_group_entry_group_id FOREIGN KEY (group_id) REFERENCES message_group (group_id) ON DELETE CASCADE,
  CONSTRAINT message_group_entry_opcode FOREIGN KEY (type_id) REFERENCES enum_message_group_types (type_id) ON DELETE CASCADE
  );
  COMMENT ON TABLE message_group_entry is 'For received groups, the agent that transmitted the group (singular entry).  For outgoing groups, a set of one or more destination agents for this message';
COMMENT ON COLUMN message_group_entry.type_id IS 'Conforms to AMP Message Header Opcode value';


CREATE TABLE message_perform_control (
  message_id INT  NOT NULL,
  tv INT  NULL,
  ac_id INT  NOT NULL,
  PRIMARY KEY (message_id),
  CONSTRAINT message_perform_control_mid FOREIGN KEY (message_id) REFERENCES message_group_entry (message_id) ON DELETE CASCADE
  );
   COMMENT ON TABLE message_perform_control IS 'Control(s) to be sent to agent';

CREATE TABLE message_agents (
  message_id INT  NOT NULL,
  agent_id INT  NULL,
  PRIMARY KEY (message_id),
  CONSTRAINT message_agents_mid FOREIGN KEY (message_id) REFERENCES message_group_entry (message_id) ON DELETE CASCADE,
  CONSTRAINT message_agents_eid FOREIGN KEY (agent_id) REFERENCES registered_agents (registered_agents_id) ON DELETE CASCADE
  );
  COMMENT ON TABLE message_agents IS 'For register_Agent messages, there will always be a single entry in this table. For Table and Msg sets this is the [optional] list of RX names for the set';

-- TODO TABLE message_table_set_entry

CREATE TABLE report_definition (
  report_id serial  NOT NULL ,
  ari_id INT  NOT NULL,
  ts INT  NULL ,
 tnvc_id INT  NULL,
  PRIMARY KEY (report_id),
  CONSTRAINT report_definition_ari FOREIGN KEY (ari_id) REFERENCES obj_actual_definition (obj_actual_definition_id) ON DELETE CASCADE,
  CONSTRAINT report_definition_tnvc FOREIGN KEY (tnvc_id) REFERENCES type_name_value_collection (tnvc_id) ON DELETE CASCADE
  );
  COMMENT ON TABLE report_definition IS 'An instance of a receipt message report'  ;
COMMENT ON COLUMN report_definition.ts IS 'Optional AMP Timestamp as received';

CREATE TABLE message_report_set_entry (
  message_id INT  NOT NULL,
  report_id INT  NULL,
  order_num smallint ,
  UNIQUE  (message_id, order_num),
  CONSTRAINT message_report_entry_mid FOREIGN KEY (message_id) REFERENCES message_group_entry (message_id) ON DELETE CASCADE,
  CONSTRAINT message_report_entry_rid FOREIGN KEY (report_id) REFERENCES report_definition (report_id) ON DELETE CASCADE
  );
  COMMENT ON TABLE message_report_set_entry is 'A set of received reports'
  ;

CREATE TABLE nm_mgr_log (
  id SERIAL  NOT NULL ,
  time TIMESTAMP NOT NULL DEFAULT NOW(),
  msg varchar NOT NULL,
  details TEXT NULL,
  level INT  NULL,
  source VARCHAR(32) NULL,
  file VARCHAR(64) NULL,
  line INT  NULL,
  PRIMARY KEY (id)
  );
  COMMENT ON TABLE nm_mgr_log is  'AMP_DEBUG logging table for ION NM Manager Application. Primarily intended for debug purposes.'
;
-- user for man user and accessing in the ui
-- create an admin and a normal user
-- can create super set of this for things not supported ect

CREATE SEQUENCE user_id_seq minvalue 0;
CREATE TABLE "user" (
                      id integer NOT NULL DEFAULT nextval('user_id_seq'),
                      username character varying(255) NOT NULL UNIQUE,
                      email character varying(255) UNIQUE,
                      first_name character varying(255),
                      last_name character varying(255),
                      password character varying(255),
                      is_enabled boolean DEFAULT true,
                      is_mfa_enabled boolean DEFAULT false,
                      is_ldap_user boolean DEFAULT false,
                      last_login_at timestamp with time zone,
                      last_login_ip character varying(255),
                      current_login_at timestamp with time zone,
                      current_login_ip character varying(255),
                      login_count integer DEFAULT 0,
                      token character varying(255) UNIQUE,
                      token_secret character varying(255),
                      otp_secret character varying(64),
                      otp_secret_registered boolean DEFAULT false,
                      otp_secret_forgot character varying(64),
                      password_token_reset character varying(255),
                      roles integer[],
                      permissions integer[],
                      details jsonb,
                      created_at TIMESTAMP NOT NULL DEFAULT NOW(),
                      updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                      PRIMARY KEY (id)
);
ALTER SEQUENCE user_id_seq OWNED BY "user".id;
ALTER SEQUENCE user_id_seq RESTART WITH 0;


-- whenyou send invites to user
-- admin send invites get token for setup time out
CREATE SEQUENCE invite_id_seq minvalue 0;
CREATE TABLE invite (
                        id integer NOT NULL DEFAULT nextval('invite_id_seq'),
                        email character varying(255) NOT NULL UNIQUE,
                        token character varying(255),
                        token_secret character varying(255),
                        token_expire timestamp with time zone,
                        is_enabled boolean DEFAULT true,
                        created_at TIMESTAMP NOT NULL DEFAULT NOW(),
                        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                        PRIMARY KEY (id)
);
ALTER SEQUENCE invite_id_seq OWNED BY invite.id;
ALTER SEQUENCE invite_id_seq RESTART WITH 0;


--things wanted to see for agents
-- redundant from registered agent
-- CREATE SEQUENCE agent_id_seq minvalue 0;
-- CREATE TABLE agent (
--                        id integer NOT NULL DEFAULT nextval('agent_id_seq'),
--                        name character varying(255) NOT NULL UNIQUE,
--                        status integer,
--                        historical_data json[],
--                        received_reports json[],
--                        supported_a_d_ms character varying(255)[],
--                        created_at TIMESTAMP NOT NULL DEFAULT NOW(),
--                        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
--                        PRIMARY KEY (id)
-- );
-- ALTER SEQUENCE agent_id_seq OWNED BY agent.id;
-- ALTER SEQUENCE agent_id_seq RESTART WITH 0;

-- temp things that might be ready to be removed
CREATE SEQUENCE alert_id_seq minvalue 0;
CREATE TABLE alert (
                       id integer NOT NULL DEFAULT nextval('alert_id_seq'),
                       title text,
                       status integer NOT NULL,
                       message text,
                       created_at TIMESTAMP NOT NULL DEFAULT NOW(),
                       updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                       PRIMARY KEY (id)
);
ALTER SEQUENCE alert_id_seq OWNED BY alert.id;
ALTER SEQUENCE alert_id_seq RESTART WITH 0;

CREATE SEQUENCE network_status_id_seq minvalue 0;
CREATE TABLE networkStatus (
                               id integer NOT NULL DEFAULT nextval('network_status_id_seq'),
                               name text,
                               status integer DEFAULT 0,
                               historical_data json[],
                               received_reports json[],
                               supported_a_d_ms text[],
                               "to" integer,
                               "from" integer,
                               created_at TIMESTAMP NOT NULL DEFAULT NOW(),
                               updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                               PRIMARY KEY (id)
);
ALTER SEQUENCE network_status_id_seq OWNED BY networkStatus.id;
ALTER SEQUENCE network_status_id_seq RESTART WITH 0;

-- create a new user for grafana
-- CREATE USER
CREATE ROLE grafana WITH
	NOLOGIN
	NOSUPERUSER
	NOCREATEDB
	NOCREATEROLE
	INHERIT
	NOREPLICATION
	CONNECTION LIMIT -1
	PASSWORD 'grafana';

GRANT pg_read_all_data TO grafana;
ALTER ROLE grafana
	LOGIN;
