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
|actual_parmspec|	Collection|
|actual_parm|	Collection entry|
|actual_parm_name|	Collection entry|
|actual_parm_object|	Collection entry|
|actual_parm_ac|	Collection entry|
|actual_parm_tnvc|	Collection entry|
|adm|	support|
|ADM_Type_Enumeration|	Supporting Info|
|ari_collection|	Collection|
|ari_collection_formal_entry|	Collection entry|
|ari_collection_actual_entry|	Collection entry|
|const_actual_definition|	Actual definition|
|control_actual_definition|	Actual definition|
|control_formal_definition|	Formal definition|
|data_type|	Supporting info|
|edd_actual_definition|	Actual definition|
|edd_formal_definition|	Formal definition|
|expression|	collection|
|formal_parm|	Collection entry |
|formal_parmspec|	Collection|
|incoming_message_entry|	Support|
|incoming_message_set|	Support |
|incoming_state|	Support|
|literal_actual_definition|	Actual definition|
|macro_actual_definition|	Actual definition|
|macro_formal_definition|	Formal definition|
|namespace|	Support |
|network_config|	Support|
|obj_actual_definition|	Parent actual definition|
|obj_formal_definition|	parent formal definition|
|obj_metadata|	parent definition |
|operator_actual_definition|	Actual definition|
|outgoing_message_entry|	Support|
|outgoing_message_set|	support|
|outgoing_state|	Support|
|registered_agents|	Support|
|report_instance|	Evaluated|
|report_template_actual_definition|	Actual definition|
|report_template_formal_definition|	Formal definition|
|sbr_actual_definition|	Actual definition|
|table_instance|	Evaluated|
|table_template_actual_definition|	Actual definition|
|tbr_actual_definition|	Actual definition|
|type_name_value_collection|	collection|
|type_name_value_entry|	Collection entry|
|type_name_value_obj_entry| Collection entry|
|type_name_value_int_entry| Collection entry|
|type_name_value_uint_entry| Collection entry|
|type_name_value_vast_entry| Collection entry|
|type_name_value_uvast_entry| Collection entry|
|type_name_value_real32_entry| Collection entry|
|type_name_value_real64_entry| Collection entry|
|type_name_value_string_entry| Collection entry|
|type_name_value_bool_entry| Collection entry|
|type_name_value_byte_entry| Collection entry|
|variable_actual_definition|	Actual definition|

	

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
	the collection containg the controls and other macros this macro calls. fk into ari_collection,
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

		 
		
type_name_value_collection  
 the main table of the type name value collection stores the number entries and a human readable description. the type_name_value reference this table and store the entries for this collection 
- tnvc_id int unsigned  
  the primary key of the tnv collection  
- num_entries int unsigned   
  number of the entries in the collection  
- use_desc  varchar  
  human readable descriptor of the spec  

type_name_value_entry  
a type name value collection entry. Has its own unique id that is reference by the lower tnv entries tables that actually store the values. For efficiency each primitive type has its own table so that we can store them as their actual value instead of a string representation. 
- tnv_id int unsigned 
  unique id  
- tnvc_id int unsigned   
  points to the collection it belongs to, foreign key into tnv_collection(tnvc_id)  
- order_num int unsigned  
  the order this entry is in the collection  
- date_type_id int unsigned    
  type of the data object foreign key into data_type(enum_id)  
- date_name varchar   
  human readable name for the object   

These tables can be used to store the values for the type name value entries. Either they are an actual object so store the id of that actual object, or a primitive type.
type_name_value_obj_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- obj_actual_definition_id int unsigned  
  id of this actual object referenes obj_actual_definition   
type_name_value_int_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value int   
  value of this int_entry  
type_name_value_uint_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value  int unsigned
  value of this uint_entry  
type_name_value_vast_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value BIGINT  
  value of this vast_entry  
type_name_value_uvast_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value BIGINT unsigned  
  value of this uvast_entry  
type_name_value_real32_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value FLOAT
  value of this real32_entry  
type_name_value_real64_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value DOUBLE  
  value of this real64_entry  
type_name_value_string_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value  varchar  
  value of this string_entry  
type_name_value_bool_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value BOOL   
  value of this bool_entry  
type_name_value_byte_entry  
- tnv_id int unsigned 
  id of the this tnv entry references type_name_value_entry  
- entry_value  TINYINT    
  value of this byte_entry  

		
ARI Collections  
used with the ac_entries table to represent ari Collections. this table is referenced by ac_entries to create a collection  
- ac_id int unsigned   
  unique id, primary key    
- num_entries  int unsigned    
  number of entries in the collection    
- use_desc varchar    
  human readable descriptor of the spec. 	  

ARI_collection_entry   
entries into an ARI collection    
- ac_entry_id int unsigned  
  unique id, primary key 
- ac_id int unsigned  
  the identifier of the ARI collection, foreign key into ari_collection(ac_id)  

ARI_collection_actual_entry  
    table to store the actual definition entries in the ari_collection  
- ac_entry_id int unsigned  
    unique id, primary key , forign key into ARI_collection_entry  
- obj_actual_definition_id  int unsigned  
  id of the actual definition for this entry, FK into obj_actual_definition   

ARI_collection_formal_entry  
  table to store the formal definition entries in the ari_collection  
- ac_entry_id   
  unique id, primary key , forign key into ARI_collection_entry  
- obj_formal_definition_id  
  id of the formal definition for this entry, FK into obj_actual_definition  
 	
Formal_Parmspec  
the main collection for formal parms defined the number of parameters and is refenced by formal_parm   
- fp_spec_id int unsigned    
  unique id, primary key 
- num_parms int unsigned  
  number of parameters in the spec		 
- use_desc varchar  
  human readable descriptor of the spec. 	

Formal_Parm  
formal paramaters that populate a formal parmspec. They have their own unique key alongside the fp_spec_id of the formal_parmspec collection they belong too. They can have an optional default value. They are also referenced by actual_parm_name when that actual parameter value is what the formal parameters value will be.  
- fp_id int unsigned  
        unique id, primary key 
- fp_spec_id int unsigned  
        identifies the parmspec it is a part of, foreign key into formal_parmspec id 
- order_num int unsigned  
        order of the this parm
- data_type_id int unsigned  
        type of the data object foreign key into data_type  
- parm_name varchar  
  name of this parameter 
- default_value int unsigned  
  fk into obj_actual_definition, optional default value for this parameter. 

actual_parmspec
Points to a formal parameter specification, actual parameters will point to an actual parmspec allowing for variable length parameters.  	
- ap_spec_id int unsigned  
  unique identifier primary key 
- fp_spec_id int unsigned  
  id of the formal parm spec, this actual parmspec uses, foreign key into formal_parmspec
- parm_count int unsigned  
  number of parameters.

actual_parm
an actual parameter that populates an actual parameter specification. need to agree with a formal parameter. 
- ap_id int unsigned  
	primary id 
- ap_spec_id int unsigned  
  fk into actual parmspec
- order_num int unsigned  
  the order the comes in	
		 
actual_parm_name  
parm by refrence   
- ap_id int unsigned  
  id of the actual parm
- data_type_id int unsigned   
  data type enumeration
- fp_id int unsigned  
  formal parameter this parameter gets its value from  
	
actual_parm_object  
  parm for objects   
-  ap_id int unsigned  
  id of the actual parm  
- data_type_id  int unsigned  
  what type of data this object is   
- actual_obj_id  int unsigned    
  id of the acutal object this parameter references  
	
actual_parm_ac  
  parameters that are ari collections  
- ap_id int unsigned   
  id of the actual parameter
- ac_id int unsigned   
  id of the ari collection this parameter references  

actual_parm_tnvc  
  parameters that are type name value collection  
- ap_id  int unsigned  
  id of the actual parm  
- tnvc_id int unsigned  
  id of the type name value collection this parameter references  




## views
way to use join to put information in an easy way user can interact. Read-only but can I’ve information used on selects 
	for all the objects  
		join on metadata, the formal objects definition and formal parmspec info, and any other defining things type info table info etc. 

## stored procedures 
All the tables have their associated inserts and deletes wrapped around stored procedures to allow users to interact with the database in a control way, making sure that there are not any unintended or otherwise malicious effects of their actions. With deletion and removal of objects all of the lower tables and collection use cascade on deletion to help make sure nothing is left behind after the definition is deleted. This also means you only have to delete objects definition in obj_actual_definition/obj_formal_definition/obj_metadata to remove all references of it from the database. 


