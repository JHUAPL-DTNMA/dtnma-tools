<!--
Copyright (c) 2023 The Johns Hopkins University Applied Physics
Laboratory LLC.

This file is part of the Asynchronous Network Management System (ANMS).

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This work was performed for the Jet Propulsion Laboratory, California
Institute of Technology, sponsored by the United States Government under
the prime contract 80NM0018D0004 between the Caltech and NASA under
subcontract 1658085.
-->
# AMP Manager SQL Interface

This document describes a database for managing and storing AMP objects. It describes the design decisions and patterns used and gives justifications and examples. 
Where ever it was possible, generic SQL was used when creating the database. This was to make the database as reusable and protable as possible. This example of the AMP database was written in MySQL.
Uses entity relationship design pattern and has been normalized.


|Table Name|	Table Type|
|----------|--------------|
| actual_parmspec | Collection |
| ari_collection | Collection  |
| ari_map | Collection |
| ari_rptset |  Collection |
| ari_rptt |  Collection Template|
| ari_tbl |  Collection |
| ari_tblt | Collection Template |
| const_actual_definition | Actual Definition |
| control_actual_definition | Actual Definition |
| control_formal_definition | Formal Definition |
| data_model | Support |
| data_type |  Support |
| edd_actual_definition | Actual Definition |
| edd_formal_definition | Formal Definition |
| enum_message_group_states | Support |
| enum_message_group_types | Support |
| execution_set |  collection |
| expression | collection |
| formal_parmspec |Collection  |
| ident_actual_definition | Actual Definition |
| ident_formal_definition | Formal Definition |
| macro_actual_definition | Actual Definition |
| macro_formal_definition | Formal Definition |
| message_agents | Support  |
| message_group | Support |
| message_group_agents | Support |
| message_group_entry | Support |
| message_perform_control | Support |
| message_report_set_entry | Support |
| network_config | Support |
| nm_mgr_log | Support |
| obj_actual_definition | parent Actual Definition |
| obj_formal_definition | parent Formal Definition |
| obj_metadata | parent definition  |
| operator_actual_definition | Actual Definition |
| operator_formal_definition | Formal Definition |
| registered_agents | Support |
| sbr_actual_definition | Actual Definition |
| tbr_actual_definition | Actual Definition |
| typedef_actual_definition | Actual Definition |
| variable_actual_definition | Actual Definition|
| variable_formal_definition | Formal Definition|

	

# The AMP SQL MODEL
## The formal and actual definition paradigm 
Objects can be seen as being in three different states, a formal definition, an actual definition, and evaluated. 
A formal definition for an object contains all the information that defines that object including any formal parameters. For an EDD this would mean its formal definition is its data type, description, and the type, name, and default values for its formal parameters i.e. formal def for ED1 is that it is of type float, is a temperature reading at a given time and has one parameter that is of type time value. 
An actual definition contains all the information that defines an object alongside values for arguments. The actual definition for ED1 is that it is of type float and the value for its parameter is 2019-01-01T08:00:00Z, matching the datatype of the formal parameter that defines this object. 
Evaluated is when an actor in the system, like an agent, takes the information from an actual definition and evaluates it, generating a valid value. For ED1 this would mean an agent taking the information stored in the actual definition and generating a physical value, like a temperature reading at a certain time.   

The amp database uses this design pattern when modeling the different ADM objects in a system. The database also takes advantage of knowing some objects are not parameterizable, essentially meaning that their formal and actual definitions are the same, to limit the amount of redundant information. Knowing this, the database only generates formal definitions for objects that can contain parameters. All other objects just have actual definitions. 
For instances when a user is creating an object of a type that could be parametrized, but this instance of that type is not parameterized then you would still create a formal definition then a singleton value of that formal definition in the actual definition. This maintain that the database is holistic, and information is not missing or doesn’t require searching across several tables. For example, if a user request all the actual definitions for EDDs, they would only need to select from the Edd_actual_definition table instead of having to also select from Edd_actual_defintion for all the EDDs that do not have parameters. 
	 



## Parameter specifications

There are two types of Parameter specifications(parmspec), Formal which is used with formal object definitions to define formal parameters, object type, optional name, and optional default value, and actual parameters, object type, optional name, and actual value, which are used with actual object definitions to instantiate the objects, making them ready to be evaluated. Formal parameters can have a default value. This default value is a reference to an actual object definition.
	
Actual parameters can take 2 forms object parameters, and name parameters. Object parameters are parameters that are ADM objects. This is done by having the value of the actual_parm_object be a foreign key into the object_actual_definitions table. 

Name parameter are used when an actual parameter is not yet defined but instead is receiving its value from some other formal parameter. For example, if a report, R1, is defined as having one formal parameter, int A, and then its report entries are defined as ED1, ED2(1), ED2(A), where ED1 is singleton actual definition, ED2(1) is an actual definition with actual literal parameter int 1, and ED2(A) is an actual definition with actual name parameter A. This name parameter would reference the formal parameter A in R1 formal parameter spec. The name parameter value is a foreign key referencing a formal parameter. Then when ED2(A) need to be evaluated, another actual parameter that assigns a value to that formal parameter will be evaluated alongside it and that value can be assigned to A. 

Since actual parameter can references actual definitions and those actual definitions can have parameters that again reference other actual definitions, the database allows for recursive parameters. For example, a user can have an EDD, ed1, that takes an EDD as its only parameter, and another EDD, ed2, that takes an integer as a parameter. The database allows for a user to define an actual definition for ed1 that has the parameter ed1(ed1(ed2(3)).


## ARI
To normalize the database and avoid duplicative and complex data and
also allow for different binary encodings, ARI are not stored a single
field. Instead, all the different components of an ARI, a flag, an
optional nickname, object name, parameters, issuer, and tag, are
stored in various tables across the database. 

The flag byte can be generated after creating the other fields in the
ARI.

The optional nickname can be generated as defined in
https://tools.ietf.org/html/draft-birrane-dtn-amp-08#section-7.1 by
using the `namespace_id` stored in `obj_metadata` and selecting `adm_enum`
from the adm table. Then using the data_type_id from obj_metadata you
can select the ADM Object Type Enumeration from the
`ADM_Type_Enumeration` table. The `data_type_id` information is also used
in the flag. 

The Object name is either `obj_formal_definition_id` from
`obj_formal_definition` or it is `obj_actual_definition_id` from
`obj_actual_definition`. both are zero based indexes and unique. 

For the parameter fields, each object that can be parameterized has an
entry for a parameter spec in their tables, that point to the
appropriate parm spec table.

Issuer and tag information is stored in the `network_config` table which
can be retrieved by using the `namespace_id` stored in `object_metadata`.


	
## Object Metadata
Object metadata as the super table for objects. It stores information common to all objects in the amp schema and the other object definition tables, actual and formal have foreign keys into object metadata.

`object_metadata`
- `object_metadata_id` int(10)  
	unique id, primary key 
- `type_name` varchar  
	the type of the object, foreign key into `data_type(type_name)` 
- `obj_name` varchar  
	human readable name of the object 
- `namespace_id` int unsigned  
	id of the namespace this object belongs in, foreign key into `namespace(namespace_id)`
	
	
## The Object Formal Definition
Table that tracks all the formal definition. All formal definitions should include all information that will remain constant from actual definition to actual definition.

`object_formal_definition`
- `obj_formal_definition_id` int unsigned     
	unique id of this formal object definition
- `obj_metadata_id` int unsigned    
	id of the metadata for this definition, fk into `obj_metadata`
- `use_desc` varchar
	human readable use description. 
	
`edd_formal_definition`
- `obj_formal_definition_id` int unsigned       
	id of this formal definition, fk into `obj_formal_definition `
- `data_type_id` int unsigned   
	data type of this EDD fk into data_type
- `fp_spec_id` int unsigned  
	id of the formal parmspec for this EDD, fk into 
	
`control_formal_definition`
- `obj_formal_definition_id` int unsigned     
	id of this formal definition, fk into obj_formal_definition  
- `fp_spec_id` int unsigned 
	id of the formal parmspec for this CTRL, fk into formal_parmspec   
	
`macros_formal_definition `
- `obj_formal_definition_id` int unsigned     
	id of this formal definition, fk into obj_formal_definition  
- `fp_spec_id` int unsigned  
	id of the formal parmspec for this MAC, fk into formal_parmspec 
- `ac_id` int unsigned  
	the collection containing the controls and other macros this macro calls. fk into ari_collection,
- `max_call_depth` int unsigned  
	max call depth into other macro definitions, default 4 


`report_template_formal_definition`
- `obj_formal_definition_id` int unsigned     
	id of this formal definition, fk into `obj_formal_definition`
- `fp_spec_id` int unsigned
	id of the formal parmspec for this RPTT, fk into formal_parmspec 
- `ac_id` int unsigned   
	the id of the ari collection that defines the entries of the report. fk into `ari_collection`
	
## The Object Actual Definition
Table that tracks every actual definition defined in the database. Every actual definition will have an entry in this table and allows for one place that other tables need to use to access the actual definition. The actual definition that are under this table need to not store duplicative information that would also be stored in the formal definition i.e. the type information for an EDD would be defined in the formal definition so it should not also be defined in the actual definition.  

`object_actual_definition`
- `obj_actual_definition_id` int unsigned   
	id of this actual definition  
- `obj_metadata_id` int unsigned   
	  id of the metadata for this definition, fk into obj_metadata   
- `use_dec` varchar  
    human readable description. 


`EDD_actual_definition`
- `obj_actual_definition_id` int unsigned   
	id of this actual definition fk into obj_actual_definition
- `ap_spec_id` int unsigned  
    id of the actual parmspec for this EDD, fk into actual_parmspec  

`Controls_actual_defintion`
- `obj_actual_definition_id` int unsigned   
	id of this actual definition fk into obj_actual_definition
- `ap_spec_id` int unsigned  
    id of the actual parmspec for this Controls, fk into actual_parmspec  

`Macros_actual_definition`
- `obj_actual_definition_id` int unsigned   
	id of this actual definition fk into `obj_actual_definition`
- `ap_spec_id` int unsigned  
    id of the actual parmspec for this Macros, fk into `actual_parmspec`

`Operators_actual_definition` 
- `obj_actual_definition_id` int unsigned   
  id of this actual definition fk into `obj_actual_definition`
- `data_type_id` int unsigned  
  it is the return type of the operator, fk into data_type table
- `num_operands` int unsigned  
  the number of operands for the operator 
- `tnvc_id` int unsigned  
  it is the type collection that defines the intypes of the operators, fk into tnv_collection   

`Report_template_actual_definition`
- `obj_actual_definition_id` int unsigned   
	id of this actual definition fk into obj_actual_definition
- `ap_spec_id` int unsigned  
    id of the actual parmspec for this Report_template, fk into `actual_parmspec`
	 

`Sbr_actual_definition`
state based rules 
- `obj_actual_definition_id` int unsigned  
  id of this actual definition fk into obj_actual_definition  
- `expression_id` int unsigned  
  it is the expression that when evaluated to true starts the rule, fk into expression  
- `run_count` BIGINT the number of times the rule runs   
- `start_time` TIME
  when the expression should start to be evaluated       
- `ac_id` int unsigned  
  this ari defines the controls and macros that this rule preforms, fk into the ari_colleciton table.  



Tables_template_actual_definition
- `obj_actual_definition_id` int unsigned     
	id of this actual definition fk into obj_actual_definition  
- `tnvc_id` int unsigned
  it must be a type collection with optional names that defines the columns of the table fk into tnv_collection  


`tbr_actual_definition`
time based rule
- `obj_actual_definition_id` int unsigned   
  id of this actual definition fk into obj_actual_definition  
- `wait_period` TIME  
  the time to wait between running the action.  
- `run_count` BIGINT  
  the number of times that the action may be run.  
- `start_time` TIME  
  the time at which the TBR should start to be evalutated.  
- `ac_id` int unsigned   
  This ari defines the controls and macros that this rule preforms, fk int othe ari_colleciton table.  


`Variable_actual_definition`
- `obj_actual_definition_id` int unsigned   
  id of this actual definition fk into `obj_actual_definition`
- `data_type_id` int unsigned  
  the data type of the variable 
- `expression_id` int unisgned 
  id of the expression that defines this variable, fk into expression table 

## Auxiliary tables    
These tables are necessary to represent complex data types need for the running of the database, metadata to describe adm agents, and other information need to fully support adms. 
 

Data_type  
the data type table stores the enumeration and names for all the data types as defined in adm3 5.4  
- data_type_id  int unsigned 
  unique id of the data type   
- type_name  varchar  
  human readable name of the type  
- use_desc varchar  
  human readable descriptor of the data type  

ADM_Type_Enumeration  
Stores the enumerations AMP uses for nickname generation
- data_type_id int unsigned  
  id of the data type in the data_type table  
- Enumeration int unsigned  
  AMP enumeration

incoming_state    
    When reports are being received by a Manager from an Agent, they will be written into various Incoming Support table types. However, the application reviewing these incoming reports should not start to read them until the Manager has finished receiving and persisting them into the database. The incoming state table identifies three different wait states associated with receiving reports from Agents. The format of the table is defined as follows.   
- state_id  int unsigned   
  unique id of the state  
- state_name varchar(50)
  name of the state, Initializing, ready, processing, sent 
- use_desc varchar  
  human readable descriptor   
	
outgoing_state     
	When controls are being sent via a Manager to an Agent, they will be written into various Outgoing Support table types.  However, the Manager receiving these outgoing controls should not start to read them until the application has finished writing them into the database. 
    The outgoing_state table identifies four different wait states associated with sending controls to Agents. The format of the table is defined as follows.   
- state_id int unsigned     
    unique id of the state     
- state_name varchar(50)    
    name of the state, Initializing, ready, processed  
- use_desc varchar    
    human readable descriptor       

## ADM SUPPORT 
ADM  
- namespace_id int unsigned  
  id for the namespace this ADM is apart of foreign key into namespace table  
- adm_name varchar  
  human readable name of the ADM 
- adm_enum int unsigned   
  enumeration of the ADM used in nickname generation  
- adm_enum_label varchar  
  It is recommended that ADM enumerations should be labeled based on the number of bytes of the Nickname as a function of the size of the ADM enumeration. These labels are shown in Table 2 of Asynchronous Management Protocol draft-birrane-dtn-amp-08  
- use_desc varchar  
  human readable description of the ADM namespace  
- namespace_id int unsigned    
  unique id of the namespace   
- namespace_type varchar  
  The AMM defines three ways to identify namespaces for AMM object names: Moderated Namespaces, Anonymous Namespaces, and Issuer Namespaces  
- issuing_org varchar  
  the string that identifies the organization that is defining an AMM object 
- version_name varchar  
  string repesnetation of the version of this namespace, default 0.0.0  

Network_config  
- namespace_id int unsigned   
  the id of the namespace this network config belongs to, foreign key into namespace  
- issuer_binary_string varchar  
  any string that identifies the organization that is defining an AMM object
- tag varchar	 
  any string used to disambiguate AMM Objects for an Issuer  

## Collections

ARI Collections  
used with the ac_entries table to represent ari Collections. this table is referenced by ac_entries to create a collection  
- ac_id int unsigned   
  unique id, primary key    
- num_entries  int unsigned    
  number of entries in the collection    
- use_desc varchar    
  human readable descriptor of the spec. 	  


Formal_Parmspec  
the main collection for formal parms defined the number of parameters and is refenced by formal_parm   
- fp_spec_id int unsigned    
  unique id, primary key 
- num_parms int unsigned  
  number of parameters in the spec		 
- use_desc varchar  
  human readable descriptor of the spec. 	


actual_parmspec
Points to a formal parameter specification, actual parameters will point to an actual parmspec allowing for variable length parameters.  	
- ap_spec_id int unsigned  
  unique identifier primary key 
- fp_spec_id int unsigned  
  id of the formal parm spec, this actual parmspec uses, foreign key into formal_parmspec
- parm_count int unsigned  
  number of parameters.


## views
way to use join to put information in an easy way user can interact. Read-only but can I’ve information used on selects 
	for all the objects  
		join on metadata, the formal objects definition and formal parmspec info, and any other defining things type info table info etc. 

## stored procedures 
All the tables have their associated inserts and deletes wrapped around stored procedures to allow users to interact with the database in a control way, making sure that there are not any unintended or otherwise malicious effects of their actions. With deletion and removal of objects all of the lower tables and collection use cascade on deletion to help make sure nothing is left behind after the definition is deleted. This also means you only have to delete objects definition in obj_actual_definition/obj_formal_definition/obj_metadata to remove all references of it from the database. 


