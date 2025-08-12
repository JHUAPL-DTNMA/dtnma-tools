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
-- Version 2.0 of the amp database
-- Changes:
-- reworking database to comply with new updates to DataModel and ARI structures 
-- SET GLOBAL sql_mode='NO_AUTO_VALUE_ON_ZERO';
-- SET SESSION sql_mode='NO_AUTO_VALUE_ON_ZERO';
-- SELECT @@GLOBAL .sql_mode;
-- SELECT @@SESSION .sql_mode;
-- CREATE DATABASE /*!32312 IF NOT EXISTS*/ /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

create or replace
function update_changetimestamp_column()
returns trigger as $$
begin
   NEW.changetimestamp = now();

return new;
end;

$$ language 'plpgsql';

create sequence if not exists  data_type_id_seq minvalue -2147483648;

create table if not exists data_type (
    data_type_id integer not null default nextval('data_type_id_seq'),
    type_name VARCHAR(50) not null unique,
    use_desc varchar not null,
    primary key (data_type_id)
);

alter sequence data_type_id_seq owned by data_type.data_type_id;

alter sequence data_type_id_seq restart with 255;

insert
	into
	data_type(type_name,
	use_desc)
values('LITERAL',
'A reserved type name for the union of all possible literal types.')
ON CONFLICT DO NOTHING;

alter sequence data_type_id_seq restart with 0;
-- Literal Types
insert
	into
	data_type(type_name,
	use_desc)
values('NULL',
'null')
ON CONFLICT DO NOTHING;
-- to start Auto increment at 0
insert
	into
	data_type(type_name,
	use_desc)
values('BOOL',
'Boolean')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('BYTE',
'8 bits, Standard Byte')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('INT',
'Signed 32 bit Integer')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('UINT',
' 32 bit Integer')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('VAST',
'Signed 64 bit Integer')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('UVAST',
' 64 bit Integer')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('REAL32',
'Single precision Floating Point')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('REAL64',
'double precision precision Floating Point')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('TEXTSTR',
'A text string composed of (unicode) characters.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('BYTESTR',
'A byte string composed of 8-bit values.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('TP',
'An absolute time point (TP).')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('TD',
'A relative time difference (TD) with a sign.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('LABEL',
'A text label of a parent object parameter. This is only valid in a nested parameterized ARI.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('CBOR',
'A byte string containing an encoded CBOR item. The structure is opaque to the Agent but guaranteed well-formed for the ADM using it.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('ARITYPE',
'An integer value representing one of the code points in this Literal Types table or the Object Types table.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('AC',
'An array containing an ordered list of ARIs.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('AM',
'A map containing keys of primitive ARIs and values of ARIs.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('TBL',
'A two-dimensional table containing cells of ARIs.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('EXECSET',
'A structure containing values to be executed by an Agent.')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('RPTSET',
'A structure containing reports of values sampled from an Agent.')
ON CONFLICT DO NOTHING;
--21
--  Managed Object Types
alter sequence data_type_id_seq restart with -256;

insert
	into
	data_type(type_name,
	use_desc)
values('OBJECT',
'A reserved type name for the union of all possible object types.')
ON CONFLICT DO NOTHING;

alter sequence data_type_id_seq increment by -1 restart with -1;

insert
	into
	data_type(type_name,
	use_desc)
values('IDENT',
'Identity')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('CONST',
'Constant')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('CTRL',
'Control')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('EDD',
'Externally Defined Data')
ON CONFLICT DO NOTHING;

alter sequence data_type_id_seq restart with -6;

insert
	into
	data_type(type_name,
	use_desc)
values('OPER',
'Operator')
ON CONFLICT DO NOTHING;

alter sequence data_type_id_seq restart with -8;

insert
	into
	data_type(type_name,
	use_desc)
values('SBR',
'State-Based Rule')
ON CONFLICT DO NOTHING;

alter sequence data_type_id_seq increment by -1 restart with -10;

insert
	into
	data_type(type_name,
	use_desc)
values('TBR',
'Time-Based Rule')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('VAR',
'Variable')
ON CONFLICT DO NOTHING;

insert
	into
	data_type(type_name,
	use_desc)
values('TYPEDEF',
'Named semantic type')
ON CONFLICT DO NOTHING;
-- -12




create table if not exists data_model (
    data_model_id serial ,
    namespace_type varchar not null,
    name varchar not null,
    enumeration int not null, 
    namespace varchar,
    version_name varchar default '0.0.0',
    use_desc varchar,
    primary key (data_model_id),
    unique(name, namespace ,version_name)
-- Below UNIQUE key not valid. Explicit check in SP instead instead.  TODO: Add appropriate indexes
-- UNIQUE (namespace_type, issuing_org , name_string , version_name)
);



CREATE TABLE if not exists registered_agents (
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

create table if not exists network_config (
    data_model_id INT not null,
    issuer_binary_string varchar not null,
    tag varchar default null,
    primary key (data_model_id),
    foreign key (data_model_id)
        references data_model (data_model_id)
);

create table if not exists obj_metadata (
    obj_metadata_id serial not null ,
    data_type_id INT not null,
    "name" varchar not null,
    data_model_id INT not null,
    object_enumeration int not null,
    "status" varchar ,
    reference varchar ,
    "description" varchar ,
    primary key (obj_metadata_id),
    foreign key (data_type_id)
        references data_type (data_type_id)
        on
delete
	cascade,
	foreign key (data_model_id)
        references data_model(data_model_id)
        on
	delete
		cascade,
		unique (data_type_id,
		data_model_id,
		object_enumeration)
);

create table if not exists obj_formal_definition (
    obj_formal_definition_id serial not null ,
    obj_metadata_id INT not null,
-- Nominally obj_enum, data_type_id, and adm_enum would make a UNIQUE KEY, but tables are too normalized to enforce this
use_desc varchar default null,
    primary key (obj_formal_definition_id),
    foreign key (obj_metadata_id)
        references obj_metadata (obj_metadata_id)
        on
delete
	cascade
);

create table if not exists obj_actual_definition (
    obj_actual_definition_id serial not null ,
    obj_metadata_id INT not null,
    use_desc varchar default ' ',
    primary key (obj_actual_definition_id),
    foreign key (obj_metadata_id)
        references obj_metadata (obj_metadata_id)
        on
delete
	cascade
	
);

/*CREATE TABLE IF NOT EXISTS object_ari (
    obj_metadata_id serial  NOT NULL ,
    object_ari_id serial  NOT NULL ,
    PRIMARY KEY (obj_metadata_id),
    FOREIGN KEY (obj_metadata_id)
        REFERENCES obj_metadata (obj_metadata_id)
        ON DELETE CASCADE,
    UNIQUE (obj_metadata_id , object_ari_id )
);*/
-- CREATE TABLE IF NOT EXISTS literal_ari (
--     obj_metadata_id serial  NOT NULL ,
--     literal_ari_id  serial  NOT NULL ,
--     literal value
--     PRIMARY KEY (obj_metadata_id),
--     FOREIGN KEY (obj_metadata_id)
--         REFERENCES obj_metadata (obj_metadata_id)
--         ON DELETE CASCADE,
--     UNIQUE (obj_metadata_id , literal_ari_id )
-- );
-- CREATE TABLE IF NOT EXISTS literal_ari_numeric (
--     literal_ari_id  serial  NOT NULL ,
--     numeric_value integer,
--     PRIMARY KEY (literal_ari_id  )
-- );
-- CREATE TABLE IF NOT EXISTS literal_ari_string (
--     literal_ari_id  serial  NOT NULL ,
--     string_value VARCHAR,
--     PRIMARY KEY (literal_ari_id ),
-- );


create table if not exists ari_collection (
    ac_id serial not null ,
    num_entries INT not null default 0,
    entries bytea,
    use_desc varchar,
    primary key (ac_id)
);

-- ari-map

create table if not exists ari_map (
    am_id serial not null ,
    num_entries INT not null default 0,
    entries bytea,
    use_desc varchar,
    primary key (am_id)
);

-- ari-tbl template 
create table if not exists ari_tblt (
    ari_tblt_id serial not null ,
    num_columns INT not null default 0,
    ac_id INT not null,
-- AC used as the row  templates 
use_desc varchar,
    primary key (ari_tblt_id),
    foreign key (ac_id)
        references ari_collection (ac_id)
);

create table if not exists ari_tbl (
    ari_tbl_id serial not null ,
    ari_tblt_id INT,
    num_entries INT not null default 0,
    table_entry varchar,
    table_entry_cbor bytea,
    agent_id varchar,
    primary key (ari_tbl_id),
    foreign key (ari_tblt_id)
        references ari_tblt (ari_tblt_id)
);


-- exec-set
create table if not exists execution_set(
    execution_set_id serial not null ,
    nonce_int BIGINT,
    nonce_bytes BYTEA,
    ac_id INT,
    use_desc varchar,
    agent_id varchar,
    primary key (execution_set_id),
    foreign key (ac_id)
        references ari_collection (ac_id),
    unique(nonce_int, nonce_bytes, ac_id, agent_id) --AC would be unique for entry and one nonce per agent
);

-- ari-tbl template 
create table if not exists ari_rptt (
    ari_rptt_id serial not null ,
    ac_id INT not null,
    -- AC used as the row  templates 
use_desc varchar,
    primary key (ari_rptt_id),
    foreign key (ac_id)
        references ari_collection (ac_id)
);

-- rpt-sets
create table if not exists ari_rptset (
    ari_rptset_id serial not null,
    nonce_int BIGINT,
    nonce_bytes BYTEA,
    reference_time varchar not null,
    report_list varchar,
    report_list_cbor bytea,
    agent_id varchar,
    primary key (ari_rptset_id)
);

create table if not exists formal_parmspec (
    fp_spec_id serial not null ,
    num_parms INT not null,
    use_desc varchar default null,
    parameters bytea,
    primary key (fp_spec_id)
);

create table if not exists actual_parmspec (
    ap_spec_id serial not null ,
    fp_spec_id INT not null,
    value_set bytea,
    use_desc varchar default null,
    primary key (ap_spec_id),
    foreign key (fp_spec_id)
        references formal_parmspec (fp_spec_id)
        on
delete
	cascade
);

create table if not exists edd_formal_definition (
    obj_formal_definition_id INT not null,
    fp_spec_id INT ,
    data_type varchar not null,
    primary key (obj_formal_definition_id),
    foreign key (obj_formal_definition_id)
        references obj_formal_definition (obj_formal_definition_id)
        on
delete
	cascade,
	foreign key (fp_spec_id)
        references formal_parmspec (fp_spec_id)
        on
	delete
		cascade,
			unique (fp_spec_id)
);

create table if not exists edd_actual_definition (
    obj_actual_definition_id INT not null,
    ap_spec_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (ap_spec_id)
        references actual_parmspec (ap_spec_id)
        on
	delete
		cascade,
		unique (ap_spec_id)
);

create table if not exists ident_formal_definition (
    obj_formal_definition_id INT not null,
    fp_spec_id INT ,
    primary key (obj_formal_definition_id),
    foreign key (obj_formal_definition_id)
        references obj_formal_definition (obj_formal_definition_id)
        on
delete
	cascade,
	foreign key (fp_spec_id)
        references formal_parmspec (fp_spec_id)
        on
	delete
		cascade,
		unique (fp_spec_id)
);

create table if not exists ident_actual_definition (
    obj_actual_definition_id INT not null,
    ap_spec_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (ap_spec_id)
        references actual_parmspec (ap_spec_id)
        on
	delete
		cascade,
		unique (ap_spec_id)
);


create table if not exists typedef_actual_definition (
    obj_actual_definition_id INT not null,
    data_type_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (data_type_id)
        references data_type (data_type_id)

);


create table if not exists const_actual_definition (
    obj_actual_definition_id INT not null,
    data_type varchar not null,
    data_value varchar not null,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade
);

create table if not exists control_formal_definition (
    obj_formal_definition_id INT not null,
    fp_spec_id INT ,
    result varchar,
    primary key (obj_formal_definition_id),
    foreign key (obj_formal_definition_id)
        references obj_formal_definition (obj_formal_definition_id)
        on
delete
	cascade,
	foreign key (fp_spec_id)
        references formal_parmspec (fp_spec_id)
        on
	delete
		cascade,
		unique (fp_spec_id)
);

create table if not exists control_actual_definition (
    obj_actual_definition_id INT not null,
    ap_spec_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (ap_spec_id)
        references actual_parmspec (ap_spec_id)
        on
	delete
		cascade,
		unique (ap_spec_id)
);

create table if not exists  macro_formal_definition (
    obj_formal_definition_id INT not null,
    fp_spec_id INT ,
    ac_id INT not null,
    max_call_depth INT default 4,
    primary key (obj_formal_definition_id),
    foreign key (obj_formal_definition_id)
        references obj_formal_definition (obj_formal_definition_id)
        on
delete
	cascade,
	foreign key (fp_spec_id)
        references formal_parmspec (fp_spec_id)
        on
	delete
		cascade,
		foreign key (ac_id)
        references ari_collection (ac_id)
        on
		delete
			cascade,
			unique (fp_spec_id)
);

create table if not exists  macro_actual_definition (
    obj_actual_definition_id INT not null,
    ap_spec_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (ap_spec_id)
        references actual_parmspec (ap_spec_id)
        on
	delete
		cascade,
		unique (ap_spec_id)
);

create table if not exists operator_formal_definition (
    obj_formal_definition_id INT not null,
    fp_spec_id INT ,
    num_operands INT not null,
    operands VARCHAR,
    result_name VARCHAR not null,
    result_type VARCHAR not null,
    primary key (obj_formal_definition_id),
    foreign key (obj_formal_definition_id)
        references obj_formal_definition (obj_formal_definition_id)
        on
delete
	cascade,
	foreign key (fp_spec_id) 
	references formal_parmspec (fp_spec_id)
		);

create table if not exists operator_actual_definition (
    obj_actual_definition_id INT not null,
    ap_spec_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (ap_spec_id)
        references actual_parmspec (ap_spec_id)
        on
	delete
		cascade,
		unique (ap_spec_id)
);
-- TODO: need to look at this ***
-- CREATE TABLE IF NOT EXISTS report_instance (
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

create table if not exists  expression (
    expression_id serial not null ,
    ac_id INT not null,
    primary key (expression_id),
	foreign key (ac_id)
        references ari_collection (ac_id)
        on
	delete
		cascade,
		unique (ac_id)
);

create table if not exists sbr_actual_definition (
    obj_actual_definition_id INT not null,
    action bytea not null,
    condition bytea not null,
    min_interval int,
    run_count BIGINT not null,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade
);

create table if not exists tbr_actual_definition (
    obj_actual_definition_id INT not null,
    wait_period TIME not null,
    run_count BIGINT not null,
    start_time TIME not null,
    action bytea not null,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade
);

create table if not exists variable_formal_definition (
    obj_formal_definition_id INT not null,
    fp_spec_id INT ,
    data_type_id INT not null,
    expression varchar ,
    init_value varchar ,
    primary key (obj_formal_definition_id),
    foreign key (obj_formal_definition_id)
        references obj_formal_definition (obj_formal_definition_id)
        on
delete
	cascade,
		foreign key (fp_spec_id) 
	references formal_parmspec (fp_spec_id),
	foreign key (data_type_id)
        references data_type (data_type_id)
        on
	delete
		cascade
);

create table if not exists variable_actual_definition (
    obj_actual_definition_id INT not null,
    ap_spec_id INT ,
    primary key (obj_actual_definition_id),
    foreign key (obj_actual_definition_id)
        references obj_actual_definition (obj_actual_definition_id)
        on
delete
	cascade,
	foreign key (ap_spec_id)
        references actual_parmspec (ap_spec_id)
        on
	delete
		cascade,
		unique (ap_spec_id)
		
	);
-- Ensure MySQL allows explicit INSERT of 0-values for initializing lookup tables
-- (Note: Other DB systems, such as PostgreSQL require different syntax for declaring auto-increment fields to begin with).
-- SET sql_mode = NO_AUTO_VALUE_ON_ZERO;

/*drop table if exists message_group_agents;

drop table if exists message_perform_control;

drop table if exists message_agents;

drop table if exists message_report_set_entry;

drop table if exists report_definition;

drop table if exists message_group_entry;

drop table if exists message_group;

drop table if exists enum_message_group_states;

drop table if exists enum_message_group_types;

drop table if exists DB_LOG_INFO;*/




create table if not exists enum_message_group_states (
  state_id serial not null ,
  name VARCHAR(45) null unique,
  description varchar null,
  primary key (state_id)
  );

comment on
table enum_message_group_states is 'Enumeration of state_id values in message_group'
  ;

insert
	into
	enum_message_group_states (state_id,
	name,
	description)
values
(0,
'initializing',
'Default value, but generally used solely by UI when creating new outgoing messages.'),
(1,
'ready',
'Indicates message is ready for processing, either for the manager to transmit (outgoing) or user to analyze (incoming)'),
(2,
'sent',
'For outgoing sets only, this indicates that the manager has parsed and sent this message group.'),
(3,
'error',
'Indicates that the manager failed to parse or transmit this group successfully.'),
(4,
'aborted',
'Indicates this outgoing message set has been aborted by the user before transmission.')
on conflict do nothing;

create table if not exists enum_message_group_types (
  type_id serial not null ,
  name VARCHAR(45) null unique,
  primary key (type_id)
  );

comment on
table enum_message_group_types is 'Enumeration of Message group opcode/type values'
  ;

insert
	into
	enum_message_group_types (type_id,
	name)
values
(0,
'REG_AGENT'),
(1,
'RPT_SET'),
(2,
'PERF_CTRL'),
(3,
'TBL_SET')
on conflict do nothing;

create table if not exists message_group (
  group_id serial not null ,
  created_ts TIMESTAMP not null default NOW(),
  modified_ts TIMESTAMP default CURRENT_TIMESTAMP,
  ts INT null,
  is_outgoing BOOL,
  state_id INT not null default 0,
  primary key (group_id),
  constraint message_group_state_id foreign key (state_id) references enum_message_group_states (state_id) on
delete
	restrict
  );

comment on
table message_group is 'Incoming and outgoing message sets';

comment on
column message_group.modified_ts is 'If NULL, this is an outgoing message.  Otherwise this is the raw AMP timestamp value of the received message group.  The function amp_ts_to_datetime() can convert this into a MySQL TIMESTAMP format.';

create or replace trigger update_mg_changetimestamp before
update
	on
	message_group for each row execute procedure
    update_changetimestamp_column();

create table if not exists message_group_agents (
  group_id INT not null,
  agent_id INT not null,
  primary key (group_id),
  constraint message_group_agents_group_id foreign key (group_id) references message_group (group_id) on
delete
	cascade,
	constraint message_group_agents_agent_id foreign key (agent_id) references registered_agents (registered_agents_id) on
	delete
		cascade
  );

comment on
table message_group_agents is 'For received groups, the agent that transmitted the group (singular entry).  For outgoing groups, a set of one or more destination agents for this message';

create table if not exists message_group_entry (
  group_id INT not null,
  message_id serial not null ,
  ack BOOL not null,
  nak BOOL not null,
  acl BOOL not null,
  order_num INT not null,
  type_id serial not null ,
  primary key (message_id),
  unique (group_id,
message_id,
order_num),
  constraint message_group_entry_group_id foreign key (group_id) references message_group (group_id) on
delete
	cascade,
	constraint message_group_entry_opcode foreign key (type_id) references enum_message_group_types (type_id) on
	delete
		cascade
  );

comment on
table message_group_entry is 'For received groups, the agent that transmitted the group (singular entry).  For outgoing groups, a set of one or more destination agents for this message';

comment on
column message_group_entry.type_id is 'Conforms to AMP Message Header Opcode value';

create table if not exists message_perform_control (
  message_id INT not null,
  tv INT null,
  ac_id INT not null,
  primary key (message_id),
  constraint message_perform_control_mid foreign key (message_id) references message_group_entry (message_id) on
delete
	cascade
  );

comment on
table message_perform_control is 'Control(s) to be sent to agent';

create table if not exists message_agents (
  message_id INT not null,
  agent_id INT null,
  primary key (message_id),
  constraint message_agents_mid foreign key (message_id) references message_group_entry (message_id) on
delete
	cascade,
	constraint message_agents_eid foreign key (agent_id) references registered_agents (registered_agents_id) on
	delete
		cascade
  );

comment on
table message_agents is 'For register_Agent messages, there will always be a single entry in this table. For Table and Msg sets this is the [optional] list of RX names for the set';
-- TODO TABLE message_table_set_entry


create table if not exists message_report_set_entry (
  message_id INT not null,
  ari_rptset_id INT null,
  order_num smallint ,
  unique (message_id,
order_num),
  constraint message_report_entry_mid foreign key (message_id) references message_group_entry (message_id) on
delete
	cascade,
	constraint message_report_entry_rid foreign key (ari_rptset_id) references ari_rptset (ari_rptset_id) on
	delete
		cascade
  );

comment on
table message_report_set_entry is 'A set of received reports'
  ;

create table if not exists DB_LOG_INFO (
  id SERIAL not null ,
  time TIMESTAMP not null default NOW(),
  msg varchar not null,
  level INT null,
  source VARCHAR(32) null,
  file VARCHAR(64) null,
  line INT null,
  primary key (id)
  );

comment on
table DB_LOG_INFO is 'logging table for  NM Manager Application. Primarily intended for debug purposes.'
;