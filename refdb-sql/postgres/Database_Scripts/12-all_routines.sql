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

-- ==================================================================
-- Author: David Linko	
-- 
-- Description:  inserting, updating and removing object formal and actual defintion. The backbone for the DB and all the objects that make it up. 
-- since deletes concasde if you remove the obj instance or definiton you remove that entity from the DB 
-- ==================================================================




create or replace procedure SP__insert_data_model(in p_namespace_type varchar, p_name varchar, p_enumeration integer, p_namespace varchar, p_version_name varchar, p_use_desc varchar, out r_data_model_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	INSERT INTO data_model
	(namespace_type, "name", enumeration, namespace, version_name, use_desc)
	VALUES(p_namespace_type, p_name, p_enumeration, p_namespace, p_version_name, p_use_desc) RETURNING data_model_id into r_data_model_id;
end$$;



-- ==================================================================
-- Parameters:
-- in 
-- 		p_obj_type varchar - object of the type
--      p_obj_name varchar -  human readable name of the new object
--      p_date_model_id integer - date_model this object belongs to
--      p_object_enumeration - object enumeration in data model
-- 		p_status
-- 		p_reference
--      p_description
-- out 
-- 		r_obj_id integer - id of the new object in the database
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_obj_metadata(IN p_obj_type_id integer, p_obj_name varchar, p_data_model_id integer,  p_object_enumeration integer, p_status varchar, p_reference varchar,  p_description varchar,INOUT r_obj_id integer)
LANGUAGE plpgsql
AS $$ BEGIN

   SELECT obj_metadata_id INTO r_obj_id
              FROM obj_metadata
              WHERE data_type_id=p_obj_type_id AND name=p_obj_name AND data_model_id=p_data_model_id and object_enumeration = p_object_enumeration;

    IF (r_obj_id IS NULL) THEN
    	INSERT INTO obj_metadata(data_type_id, name, data_model_id, object_enumeration, status, reference, description) VALUES(p_obj_type_id, p_obj_name, p_data_model_id, p_object_enumeration, p_status, p_reference, p_description) RETURNING obj_metadata_id into r_obj_id;      
    END IF;

end$$;


-- ==================================================================
-- Parameters:
-- in 
-- 		p_obj_id integer - 
-- 
-- ==================================================================

CREATE OR REPLACE PROCEDURE SP__delete_obj_metadata(IN p_obj_id integer)
LANGUAGE plpgsql
AS $$ BEGIN 
	DELETE FROM obj_metadata
WHERE obj_metadata_id = p_obj_id;

end$$;




-- ==================================================================
-- Parameters:
-- in  
-- 		p_obj_metadata_id integer - 
-- 		p_use_desc varchar - 
-- out 
-- 		r_formal_id integer - 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_obj_formal_definition(IN p_obj_metadata_id integer, p_use_desc varchar, INOUT r_formal_id integer)
LANGUAGE plpgsql

AS $$ 
/*
DECLARE
 data_type integer;
 enumeration_this INTEGER;
 obj_enum integer;

BEGIN

    -- Get the next available ID for automatic enumeration of new formal definitions
    SELECT data_type_id, vw_obj_metadata.enumeration INTO data_type,enumeration_this FROM vw_obj_metadata WHERE obj_metadata_id=p_obj_metadata_id;
    SELECT COALESCE(MAX(vof.obj_enum)+1,0) INTO obj_enum FROM vw_obj_formal_def vof WHERE vof.data_type_id=data_type AND vof.enumeration=enumeration_this;      
*/
BEGIN
	INSERT INTO obj_formal_definition(obj_metadata_id, use_desc) VALUES(p_obj_metadata_id, p_use_desc) RETURNING obj_formal_definition_id INTO r_formal_id;
end$$;




-- ==================================================================
-- Parameters:
-- in IN p_obj_metadata_id integer, p_use_desc varchar, INOUT r_actual_id integer)
--
-- out 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_obj_actual_definition(IN p_obj_metadata_id integer, p_use_desc varchar, INOUT r_actual_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	INSERT INTO obj_actual_definition(obj_metadata_id, use_desc) VALUES(p_obj_metadata_id, p_use_desc) RETURNING  obj_actual_definition_id INTO r_actual_id;
end$$;


-- ==================================================================
-- can use name or id for to delete 
-- Parameters:
-- in 
-- 		p_obj_id integer - Id of the specific obj to delete 
-- 		p_obj_name varchar - name of the obj to delete 
-- ================================================================== 
CREATE OR REPLACE PROCEDURE SP__delete_obj_formal_definition(p_obj_id integer, p_obj_name varchar )
LANGUAGE plpgsql

AS $$ 
<<obj_delete>>
BEGIN
	IF( p_obj_id is NULL AND p_obj_name is not NULL) THEN
		select obj_id FROM obj_metadata WHERE obj_name = p_obj_name into p_obj_id;
    ELSE 
		exit obj_delete;
    END IF;
    
	DELETE FROM obj_metadata WHERE obj_id = p_obj_id;
END$$;

 

-- ==================================================================
-- Parameters:
-- in 
-- 		p_act_id integer
--  	p_obj_name varchar
-- 		p_date_model_id integer 
--
-- ==================================================================
-- for just removing the obj instance 
-- two options for removal if you want to remove a specific instance you can specify the instance id, 
-- or if you want to remove all the instances of a specific definition you can supply the definition name
-- TODO: can add another option of adding a number and only removes up to that number of entries.
-- maybe too overloaded
CREATE OR REPLACE PROCEDURE SP__delete_obj_actual_definition(p_act_id integer, p_obj_name varchar ) 
LANGUAGE plpgsql
AS $$ 
<<obj_inst_del>>
BEGIN 

	-- if its just removing one instance 
	IF( p_inst_id is not null) THEN
		DELETE FROM obj_actual_definition WHERE actual_definition_id = p_act_id;
    -- if its removing all instances with this defeintion
	ELSE -- removing all instances with this name 
		IF( p_obj_name is null or p_date_model_id) then -- nothing to do 
			exit obj_inst_del;
		END IF;
		DELETE FROM obj_actual_definition where obj_metadata_id =(select obj_metadata_id from obj_metadat where obj_name =  p_obj_name ); 
	END IF;
END$$;



-- STORED PROCEDURE(S) for creating an ari collection and entries

-- ==================================================================
-- SP__insert_ac_id 
-- adds an ari collection to the database
-- Parameters:
-- in 
-- 		p_num_entries integer - number of entries in the ac
-- 		p_use_desc varchar - human readable description
-- OUT 
-- 		r_ac_id integer- id of the ac
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_ac_id(IN p_num_entries integer, p_entries bytea, p_use_desc varchar,  INOUT r_ac_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	INSERT INTO ari_collection(num_entries, entries, use_desc) VALUES(p_num_entries, p_entries,p_use_desc) RETURNING  ac_id INTO r_ac_id;
end$$;






-- STORED PROCEDURE(S) for the actaul parameters specefications and actual parameter sets. has real data 


-- ==================================================================
-- SP__insert_actual_parmspec
--  inserting an actual parmspec into db
-- IN 
-- 		p_fp_spec_id integer - the id of the formal parm spec for this actual parmspec
-- 		p_num_parms integer - number of parms 
-- 		p_use_desc varchar - human readable describtion
-- OUT 
-- 		r_ap_spec_id integer - id of the parmspec in the db 
-- ==================================================================
-- TODO: p_num_parms argument is deprecated and will be removed

CREATE OR REPLACE PROCEDURE SP__insert_actual_parmspec(IN p_fp_spec_id  integer, p_value_set bytea, p_use_desc varchar, INOUT r_ap_spec_id integer)
LANGUAGE plpgsql
AS $$
 BEGIN
    INSERT INTO actual_parmspec
(ap_spec_id, fp_spec_id, value_set, use_desc)
VALUES(nextval('actual_parmspec_ap_spec_id_seq'::regclass), p_fp_spec_id, p_value_set, p_use_desc) RETURNING ap_spec_id into r_ap_spec_id;
end$$;





-- 


-- inserting reg agents into the db

-- ==================================================================
-- SP__insert_agent
--  inserting a new agent into the systemß
-- IN 
-- 		p_agent_endpoint_uri- name of the agent to insert 
-- OUT
-- 		r_row_id - teh id of the agent in the db
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_agent(IN p_agent_endpoint_uri TEXT, INOUT r_row_id integer)
LANGUAGE plpgsql

AS $$
DECLARE
 cur_time TIMESTAMP;
 lower_uri TEXT;
 row_id INTEGER;
BEGIN
    cur_time := NOW(); 
    lower_uri := LOWER(p_agent_endpoint_uri);
    row_id := NULL;
    SELECT registered_agents_id INTO row_id
        FROM registered_agents WHERE lower_uri = agent_endpoint_uri;

    IF (row_id IS NOT NULL) THEN
    BEGIN 
        UPDATE registered_agents
            SET last_registered = cur_time 
            WHERE registered_agents_id=row_id;
        r_row_id := row_id;
    END;
    ELSE
    BEGIN
        INSERT INTO registered_agents (agent_endpoint_uri, first_registered, last_registered)
            VALUES (lower_uri, cur_time, cur_time)
            RETURNING registered_agents_id INTO r_row_id;
    END;
    END IF;
end$$;



-- ==================================================================
-- Author: David Linko	
-- 
-- Description:  inserting, updating and removing constant formal and actual definitions
-- using the obj routines
-- 
-- ==================================================================

-- ==================================================================
-- SP__insert_const_actual_definition;
-- Parameters:
-- in 
--     p_obj_id integer - id for the object metadata
--      p_use_desc varchar - humanreadable description of the constant 
-- 		p_data_type varchar -  name of the data type for the constant 
--  	p_data_value_string varchar - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id integer id of the actual defintion entry 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_const_actual_definition(IN p_obj_id integer, p_use_desc varchar, p_data_type varchar, p_data_value_string varchar, INOUT r_actual_definition_id integer)
LANGUAGE plpgsql

AS $$
DECLARE data_id integer;
 BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_actual_definition_id); 
    INSERT INTO const_actual_definition(obj_actual_definition_id, data_type, data_value) VALUES(r_actual_definition_id, p_data_type, p_data_value_string); 
end$$;



-- ==================================================================
-- SP__delete_const_actual_definition
-- cna us either hte name or the id of the constant to delete 
-- Parameters:
-- in 
--  	p_obj_id integer -  id of the constan to delete 
-- 		p_obj_name varchar -   name of the constant to delete
-- ==================================================================

CREATE OR REPLACE PROCEDURE SP__delete_const_actual_definition(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql

AS $$ 
DECLARE
metadata_id INTEGER;
BEGIN
	IF (p_obj_id != null) THEN
		metadata_id = (SELECT obj_metadata_id FROM obj_actual_definition where obj_actual_definition_id = p_obj_id );
	ELSE
		IF (p_obj_name != NULL) THEN 
			metadata_id = (SELECT obj_metadata_id FROM obj_metadata where obj_name = p_obj_name); 
        END IF;
	END IF;
    CALL SP__delete_obj_metadata(metadata_id);
    
END$$;


-- ==================================================================
-- Author: David Linko	
-- 
-- Description:  inserting, updating and removing control formal and actual definitions
-- using the obj routines
-- 
-- ==================================================================


-- ==================================================================
-- SP__insert_control_formal_definition;
-- Parameters:
-- in 
--     p_obj_id integer - id for the object metadata
--      p_use_desc varchar - humanreadable description of the constant 
-- 		p_data_type varchar -  name of the data type for the constant 
--  	p_data_value_string varchar - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id integer id of the actual defintion entry 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_control_formal_definition(IN p_obj_id integer, p_use_desc varchar, p_fp_spec_id integer, p_result varchar, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO control_formal_definition(obj_formal_definition_id, fp_spec_id, result) VALUES(r_definition_id, p_fp_spec_id, p_result);
end$$;


-- ==================================================================
-- SP__delete_control_formal_definition;
-- Parameters:
-- in 
--     p_obj_id  - id of the control to delete
--     p_obj_name - name of the control to delete 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__delete_control_formal_definition(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__delete_obj_formal_definition(p_obj_id, p_obj_name);
end$$;



-- ==================================================================
-- SP__insert_control_actual_definition;
-- Parameters:
-- in 
--     p_obj_id integer - id for the object metadata
--      p_use_desc varchar - humanreadable description of the constant 
-- 		p_data_type varchar -  name of the data type for the constant 
--  	p_data_value_string varchar - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id integer id of the actual defintion entry 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_control_actual_definition(IN p_obj_definition_id integer, p_ap_spec_id integer, p_use_desc varchar, INOUT r_instance_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_instance_id);

    -- TODO: Skip insert if it has no parameters
    INSERT INTO control_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_instance_id, p_ap_spec_id);  
end$$; 
 


-- ==================================================================
-- SP__delete_control_actual_definition;
-- Parameters:
-- in 
--     p_obj_id integer - actual_definiton id of the control that is to be deleted
--     p_obj_name varchar- name of the control to delete, this allows to delete all the actual definitons 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__delete_control_actual_definition(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql

AS $$ <<ctrl_actual_definition_del>> 
	
	DECLARE done INT DEFAULT FALSE;
    DECLARE actual_definition_id_hold integer; 
	DECLARE ap_spec_id_hold integer;
    DECLARE actual_definition_cursor CURSOR
			FOR SELECT actual_definition_id, ap_spec_id FROM vw_ctrl_actual_definition WHERE obj_name = p_obj_name;
	-- DECLARE CONTINUE HANDLER FOR NOT FOUND SET done := TRUE;	
    DECLARE ap_id integer; 
	BEGIN
	
    -- only one thing to remove 
	If( p_inst_id is not null) then 
		ap_id = (SELECT ap_spec_id from ctrl_actual_definition where actual_definition_id = p_inst_id );
		IF( ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_id;
		END IF;
		DELETE FROM ctrl_actual_definition 
WHERE
    intance_id = p_actual_definition_id;
		CALL SP__delete_obj_actual_definition(p_actual_definition_id, p_obj_name);
	ELSE -- removing all instances with this name 
		IF( p_obj_name is null) then -- nothing to do 
			exit ctrl_actual_definition_del;
		END IF;
		OPEN actual_definition_cursor;
        <<read_loop>> LOOP
			FETCH NEXT FROM actual_definition_cursor INTO 
				actual_definition_id_hold, ap_spec_id_hold;
			IF done THEN
				exit read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;

			DELETE FROM ctrl_actual_definition
WHERE
    ctrl_actual_definition.actual_definition_id = actual_definition_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_definition_cursor;
END$$;




-- =================
-- for adding idnet into the database 
-- SP__insert_ident_formal_definition 
-- IN 
-- 		p_obj_id integer - metadata id of this ident
-- 		p_use_desc varchar - human readable description
-- 		p_data_type_id - data type for the idnet 
-- OUT 
-- 		r_definition_id integer - id of the start 
-- ====================================

CREATE OR REPLACE PROCEDURE SP__insert_ident_formal_definition(IN p_obj_id integer, p_use_desc varchar,  p_fp_spec_id integer, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO ident_formal_definition(obj_formal_definition_id, fp_spec_id) VALUES(r_definition_id, p_fp_spec_id);
end$$;


-- =================
-- for adding idnet into the database 
-- SP__insert_ident_actual_definition 
-- IN 
-- 		p_obj_id integer - metadata id of this ident
-- 		p_use_desc varchar - human readable description
-- 		p_data_type_id - data type for the idnet 
-- OUT 
-- 		r_definition_id integer - id of the start 
-- ====================================

CREATE OR REPLACE PROCEDURE SP__insert_ident_actual_definition(IN p_obj_id integer, p_use_desc varchar,  p_ap_spec_id integer, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO ident_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_definition_id, p_ap_spec_id);
end$$;



-- =================
-- for adding typdef into the database 
-- SP__insert_typdef_actual_definition 
-- IN 
-- 		p_obj_id integer - metadata id of this typdef
-- 		p_use_desc varchar - human readable description
-- 		p_data_type_id - data type for the typdef 
-- OUT 
-- 		r_definition_id integer - id of the typdef 
-- ====================================

CREATE OR REPLACE PROCEDURE SP__insert_typdef_actual_definition(IN p_obj_id integer, p_use_desc varchar,  p_data_type_id integer, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO typedef_actual_definition(obj_actual_definition_id, data_type_id) VALUES(r_definition_id, p_data_type_id);
end$$;


-- ==================================================================
-- SP__insert_edd_formal_definition;
-- Parameters:
-- in 
--     p_obj_id integer - id for the object metadata
--      p_use_desc varchar - humanreadable description of the edd 
-- 		p_data_type varchar -  name of the data type for the edd 
--  	p_data_value_string varchar - blob conating the encoded value of the edd
-- out
-- 		r_actual_definition_id integer id of the actual defintion entry 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_edd_formal_definition(IN p_obj_id integer, p_use_desc varchar, p_fp_spec_id integer, p_external_data_type varchar, INOUT r_formal_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_formal_definition_id); 
    INSERT INTO edd_formal_definition(obj_formal_definition_id, fp_spec_id, data_type) VALUES(r_formal_definition_id, p_fp_spec_id, p_external_data_type); 
end$$;




-- ==================================================================
-- SP__delete_edd_actual_definition;
-- Parameters:
-- in 
--      p_obj_id integer - id for the edd to delete 
-- 		p_obj_name varchar -  name of the edd  to delete 
--
-- ==================================================================
 
CREATE OR REPLACE PROCEDURE SP__delete_edd_formal_defintion(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql

AS $$ 
<<edd_def_del>>
DECLARE def_id integer;
DECLARE fp_id integer;
BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		select obj_id FROM obj_metadata WHERE obj_name = p_obj_name into p_obj_id;
    ELSE 
		exit edd_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		select obj_name from vw_edd_formal where obj_id = p_obj_id into p_obj_name;
    END IF ;
    
	-- deleting all instances 
	CALL SP__delete_edd_actual_definition(null, p_obj_name);
    
	def_id = (select obj_formal_definition_id from obj_formal_definition where obj_metadata_id = p_obj_id);
	fp_id = (SELECT fp_spec_id from edd_formal_definition where obj_formal_definition_id = def_id );
	DELETE FROM formal_parmspec WHERE fp_spec_id = fp_id;
    
	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
end$$; 



-- ==================================================================
-- SP__insert_edd_actual_definition;
-- Parameters:
-- in 
--     p_obj_id integer - id for the object metadata
--      p_use_desc varchar - humanreadable description of the constant 
-- 		p_data_type varchar -  name of the data type for the constant 
--  	p_data_value_string varchar - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id integer id of the actual defintion entry 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_edd_actual_definition(IN p_obj_definition_id integer, p_use_desc varchar, p_ap_spec_id integer,  INOUT r_actual_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_definition_id); 
    INSERT INTO edd_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_actual_definition_id, p_ap_spec_id);  
end$$; 
 


-- ==================================================================
-- SP__delete_edd_actual_definition;
-- Parameters:
-- in 
--     p_obj_id integer - id for the object metadata
--      p_use_desc varchar - humanreadable description of the constant 
-- 		p_data_type varchar -  name of the data type for the constant 
--  	p_data_value_string varchar - blob conating the encoded value of the constant 
-- out
-- 		r_actual_definition_id integer id of the actual defintion entry 
-- ==================================================================
-- for instance can supply the definiton name to remove all the instances of that definition or can remove 

create or replace
procedure SP__delete_edd_actual_definition(in p_actual_definition_id integer, p_obj_name varchar)
language plpgsql
as $$ 
declare
    ap_id integer;

begin 
	if( p_actual_definition_id is not null) then 
		ap_id = (
select
	ap_spec_id
from
	edd_actual_definition
where
	actual_definition_id = p_actual_definition_id );
end if;

if( ap_id is not null) then
			delete
from
	actual_parmspec
where
	ap_spec_id = ap_id;
end if;

delete
from
	edd_actual_definition
where
	p_actual_definition_id = p_actual_definition_id;

call SP__delete_obj_actual_definition(p_actual_definition_id,
p_obj_name);

end$$;




-- ==================================================================
-- SP__insert_expression 
-- adds an expression to the database
-- Parameters:
-- in 
-- 		p_out_type integer - data type id for the return type of the expression  
-- 		p_num_operators integer - number of operators   
-- 		p_postfix_ids_list varchar(1000) - id of the ac that lists the equation in postfix notation
-- OUT 
-- 		r_expr_id integer - id of the expr in the database
-- =================================================================
CREATE OR REPLACE PROCEDURE SP__insert_expression(IN p_out_type integer,   
p_postfix_operations integer, INOUT r_expr_id integer)
LANGUAGE plpgsql
AS $$ BEGIN 
	-- SELECT p_out_type;
	-- IF p_out_type = 2 or p_out_type = 12 or p_out_type = 3 or p_out_type = 0 THEN BEGIN
			INSERT INTO expression(data_type_id, ac_id) VALUES(p_out_type, p_postfix_operations) RETURNING  expression_id INTO r_expr_id;
	-- END;
    -- END IF;
end$$;



-- ==================================================================
-- SP__delete_expression 
-- Delete an expression from the database
-- Parameters:
-- in 
-- 		p_expr_id integer - id of the expr in the database to delete
-- ==================================================================

CREATE OR REPLACE PROCEDURE SP__delete_expression(IN p_expr_id integer)
LANGUAGE plpgsql
AS $$ BEGIN 
	DELETE from  expression WHERE (expression_id = p_expr_id);
end$$;


-- STORED PROCEDURE(S) for the formal parameters specefications. Stores type name information. 


-- ==================================================================
-- SP__insert_formal_parmspec
-- inserts a new formal parmspec in the db
-- IN 
-- 		p_num_parms integer - number if parms in the spec
-- 		p_use_desc varchar - human readable describtion
--      p_parameters bytea - blob containg the parameters 
-- OUT 
-- 		r_fp_spec_id integer -  the id of the spec 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_formal_parmspec(IN p_num_parms integer, p_use_desc varchar, p_parameters bytea, INOUT r_fp_spec_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	INSERT INTO formal_parmspec(num_parms, use_desc ,parameters) VALUES(p_num_parms, p_use_desc, p_parameters) RETURNING  fp_spec_id INTO r_fp_spec_id;
end$$;





-- ==================================================================
-- SP__insert_macro_formal_definition
-- IN
-- 		p_obj_id integer - id of the metadata info 
-- 		p_use_desc varchar - human readable describtion
--      p_fp_spec_id integer - formal parmspec of the macro
--      p_max_call_depth integer - max call depth of the macro
--      p_definition_ac integer - ari collection definining the macro
-- OUT
-- 		r_definition_id integer - id of the new formal definition	 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_macro_formal_definition(IN p_obj_id integer, p_use_desc varchar, p_fp_spec_id integer, p_max_call_depth integer, p_definition_ac integer, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO macro_formal_definition(obj_formal_definition_id, fp_spec_id, ac_id, max_call_depth) VALUES(r_definition_id, p_fp_spec_id, p_definition_ac, p_max_call_depth); 
end$$;



-- ==================================================================
-- SP__delete_mac_formal_defintion
-- IN 
--      p_obj_id integer - id of the macro to be deleted
-- 	 	p_obj_name varchar - name of the macro to delete
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__delete_macro_formal_defintion(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$ 

<<mac_def_del>>
DEClare 
def_id int;
fp_id int;
ac_id int;
BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		select obj_id FROM obj_metadata WHERE obj_name = p_obj_name into p_obj_id;
    ELSE 
		EXIT mac_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		select obj_name from vw_mac_formal where obj_id = p_obj_id into p_obj_name;
    END IF ;
    
	-- deleting all actuals 
	CALL SP__delete_macro_actual(null, p_obj_name);
	
	def_id = (select definition_id from obj_formal_definition where obj_id = p_obj_id);
	fp_id = (SELECT fp_spec_id from macro_formal_definition where definition_id = def_id );
    ac_id = (SELECT ac_id FROM macro_formal_definition where definition_id = def_id );
    
	DELETE FROM formal_parmspec WHERE fp_spec_id = fp_id;
	DELETE FROM ari_collection WHERE ac_id = ac_id;

	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
end$$; 



-- ==================================================================
-- SP__insert_macro_actual_definition
-- -- IN
-- 		p_obj_id integer - id of the metadata info 
--      p_ap_spec_id integer - actual parmspec id
--      p_actual_ac integer - ari collection containg all actual ARI for this macro
--      p_use_desc varchar - human readable description
-- OUT
-- 		r_actual_id integer - id of the new actual definition	
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_macro_actual_definition(IN p_obj_definition_id integer, p_ap_spec_id integer, p_actual_ac integer, p_use_desc varchar, INOUT r_actual_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_id); 
    INSERT INTO macro_actual(obj_actual_definition_id, ap_spec_id, ac_id) VALUES(r_actual_id, p_ap_spec_id, p_actual_ac);  
end$$; 
 
 
-- ==================================================================
-- SP__delete_mac_actual_definition
-- IN 
--      p_obj_id integer - id of the macro to be deleted
-- 	 	p_obj_name varchar - name of the macro to delete
-- ==================================================================
-- for actual can supply the definiton name to remove all the actuals of that definition or can remove 

CREATE OR REPLACE PROCEDURE SP__delete_mac_actual_definition(IN p_inst_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$ 
<<mac_inst_del>>
DECLARE 
ap_id int;
done INT DEFAULT FALSE;
 actual_id_hold int; 
ap_spec_id_hold int;
ac_id_hold int;
actual_cursor CURSOR
			FOR SELECT actual_id, ap_spec_id, ac_id  WHERE obj_name = p_obj_name;
	-- DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;	

BEGIN
	
	
    -- only one thing to remove 
	If( p_inst_id is not null) then 
		ap_id = (SELECT ap_spec_id from macro_actual_definition where actual_id = p_inst_id );
		IF( ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_id;
		END IF;
		DELETE FROM macro_actual 
WHERE
    intance_id = p_inst_id;
		CALL SP__delete_obj_actual(p_inst_id, p_obj_name);
	ELSE -- removing all actuals with this name 
		IF( p_obj_name is null) then -- nothing to do 
			EXIT mac_inst_del ;
		END IF;
		OPEN actual_cursor;
        <<read_loop>> LOOP
			FETCH NEXT FROM actual_cursor INTO 
				actual_id_hold, ap_spec_id_hold, ac_id_hold;
			IF done THEN
				EXIT read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;
			DELETE FROM ari_collection WHERE ac_id = ac_id_hold;

			DELETE FROM macro_actual_definition
WHERE
    macro_actual.actual_id = actual_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_cursor;
end$$; 

-- create messages 

-- ==================================================================
-- SP__insert_outgoing_message_set
-- IN 
-- 		p_created_ts integer - created timestamp
-- 		p_modified_ts integer - modified timestamp
-- 		p_state integer - state of the message set 
-- 		p_agent_id integer - agent of this message 
-- OUT 
-- 		r_set_id integer - id of the set
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_outgoing_message_set(IN p_created_ts timestamp,
p_modified_ts timestamp, p_state integer, p_agent_id integer, INOUT r_set_id integer )
LANGUAGE plpgsql
AS $$ BEGIN
INSERT INTO outgoing_message_set (created_ts, modified_ts, state, agent_id)
VALUES (p_created_ts, p_modified_ts, p_state, p_agent_id) RETURNING eated_ts_id INTO r_set_id;
END$$;



-- ==================================================================
-- SP__insert_outgoing_message_entry
-- IN 
-- 		p_set_id integer - id of the outgoing message set this entry belongs toß
--      p_message_order integer - order of this message in the set
--      p_start_ts integer - start time 
--      p_ac_id integer - ac of this message 
-- OUT 
--      r_message_id integer - id of this message
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_outgoing_message_entry(IN p_set_id integer, p_message_order integer, p_start_ts timestamp, p_ac_id integer, INOUT r_message_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
INSERT INTO outgoing_message_entry (set_id, message_order, start_ts, ac_id)
VALUES (p_set_id, p_message_order, p_start_ts, p_ac_id) RETURNING outgoing_message_entry_id INTO r_message_id;
END$$;



-- ==================================================================
-- SP__update_outgoing_message_set
-- IN 
-- 		p_set_id integer - id of the set to update 
-- 		p_state integer - state of the message set 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__update_outgoing_message_set(IN p_set_id integer, p_state integer)
LANGUAGE plpgsql
AS $$ BEGIN
	
	UPDATE outgoing_message_set
	SET
	modified_ts = NOW(),
	state = p_state
	WHERE set_id = p_set_id;
END$$;



-- ==================================================================
-- SP__insert_incoming_message_set
-- IN 
-- 		p_created_ts integer - created timestamp
-- 		p_modified_ts integer - modified timestamp
-- 		p_state integer - state of the message set 
-- 		p_agent_id integer - agent of this message 
-- OUT 
-- 		r_set_id integer - id of the set
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_incoming_message_set(IN p_created_ts timestamp,
p_modified_ts timestamp, p_state integer, p_agent_id integer, INOUT r_set_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
INSERT INTO incoming_message_set (created_ts, modified_ts, state, agent_id)
VALUES (p_created_ts, p_modified_ts, p_state, p_agent_id) RETURNING eated_ts_id INTO r_set_id;
END$$;


-- ==================================================================
-- SP__insert_incoming_message_entry
-- IN 
-- 		p_set_id integer - id of the outgoing message set this entry belongs toß
--      p_message_order integer - order of this message in the set
--      p_start_ts integer - start time 
--      p_ac_id integer - ac of this message 
-- OUT 
--      r_message_id integer - id of this message
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_incoming_message_entry(IN p_set_id integer, p_message_order integer, p_start_ts timestamp, p_ac_id integer, INOUT r_message_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
INSERT INTO incoming_message_entry (set_id, message_order, start_ts, ac_id)
VALUES (p_set_id, p_message_order, p_start_ts, p_ac_id) RETURNING incoming_message_entry_id INTO r_message_id;
END$$;



-- ==================================================================
-- SP__update_incoming_message_set
-- IN 
-- 		p_set_id integer - id of the set to update 
-- 		p_state integer - state of the message set 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__update_incoming_message_set(IN p_set_id integer, p_state integer)
LANGUAGE plpgsql
AS $$ BEGIN
	
	UPDATE incoming_message_set
	SET
	modified_ts = NOW(),
	state = p_state
	WHERE set_id = p_set_id;
END$$;



-- SP__insert_message_report_entry(message_id, order_num, ari_id, tnvc_id, ts)


CREATE OR REPLACE PROCEDURE SP__insert_message_report_entry(IN
       p_msg_id integer,
       p_order_num integer,
       p_ari_id integer,
       p_tnvc_id integer,
       p_ts integer,
       INOUT r_obj_id integer
       )
LANGUAGE plpgsql
AS $$ BEGIN

    INSERT INTO report_definition (ari_id, ts, tnvc_id) VALUES (p_ari_id, p_ts, p_tnvc_id) RETURNING report_id INTO r_obj_id;

    IF p_order_num IS NULL THEN
       SELECT order_num+1 INTO p_order_num FROM message_report_set_entry WHERE message_id=p_msg_id ORDER BY order_num DESC LIMIT 1;
    END IF;
    IF p_order_num IS NULL THEN
       p_order_num = 0;
    END IF; 

    INSERT INTO message_report_set_entry (message_id, report_id, order_num) VALUES (p_msg_id, r_obj_id, p_order_num);
    
end$$;


-- SP__insert_message_group_agent_id(group_id, agent_id )


CREATE OR REPLACE PROCEDURE SP__insert_message_group_agent_id(IN
       p_group_id integer,
       p_agent integer,
       INOUT r_obj_id integer
       )
LANGUAGE plpgsql
AS $$ BEGIN
        INSERT INTO message_group_agents (group_id, agent_id) VALUES (p_group_id, p_agent) RETURNING message_group_agents_id INTO r_obj_id;
end$$;


-- SP__insert_message_group_agent_name(group_id, agent_name)


CREATE OR REPLACE PROCEDURE SP__insert_message_group_agent_name(IN
       p_group_id integer,
       p_agent VARCHAR(128),
       INOUT r_obj_id integer
       )
LANGUAGE plpgsql
AS $$ DECLARE eid INT;   BEGIN
        
        CALL SP__insert_agent(p_agent, eid); -- Select or Insert Agent ID
        INSERT INTO message_group_agents (group_id, agent_id) VALUES (p_group_id, eid) RETURNING message_group_agents_id INTO r_obj_id;

end$$;


-- SP__insert_message_entry_agent( message_id, agent_name )


CREATE OR REPLACE PROCEDURE SP__insert_message_entry_agent(IN
       p_mid integer,
       p_agent VARCHAR(128)
       )
LANGUAGE plpgsql
AS $$ DECLARE eid INT;  BEGIN
        
        CALL SP__insert_agent(p_agent, eid); -- Select or Insert Agent ID
        INSERT INTO message_agents (message_id, agent_id) VALUES (p_mid, eid);

end$$;




-- STORED PROCEDURE(S) For creating date_models and adms

-- ==================================================================
-- SP__insert_date_model
--    insert a new date_model into the db
-- Parameters:
-- in 
--      p_date_model_type varchar - type of the date_model
-- 		p_issuing_org varchar - name of the issuing organization for this ADM.
-- 		p_name_string varchar- his is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_version varchar -This is a string representation of the version of the ADM.
--           ADM version representations are formated at the discretion of
--           the publishing organization.
-- out 
-- 		r_date_model_id integer - id of the date_model in the database 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_date_model(IN p_date_model_type varchar, p_issuing_org varchar, p_name_string 
varchar, p_version varchar, INOUT r_date_model_id integer)
LANGUAGE plpgsql
AS $$ BEGIN

    SELECT date_model_id INTO r_date_model_id
           FROM date_model
           WHERE date_model_type=p_date_model_type AND issuing_org=p_issuing_org AND name_string=p_name_string AND version_name=p_version;
    IF (r_date_model_id IS NULL) THEN
    	INSERT INTO date_model(date_model_type, issuing_org, name_string, version_name) VALUES(p_date_model_type, p_issuing_org, p_name_string, p_version) RETURNING date_model_id INTo r_date_model_id; 
    END IF;
    
end$$;



-- ==================================================================
-- SP__insert_network_defined_date_model
--    insert a new network defined adm into the db
-- Parameters:
-- in 
-- 		p_issuing_org varchar - name of the issuing organization for this ADM.
-- 		p_name_string varchar- this is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_version varchar -This is a string representation of the version of the ADM.
--           ADM version representations are formated at the discretion of
--           the publishing organization.
-- 		p_issuer_binary_string varchar - any string that identifies the organization that is
-- 			  defining an AMM object
-- 	    p_tag varchar - any string used to disambiguate AMM Objects for an Issuer
-- out 
-- 		r_date_model_id integer - id of the date_model in the database 
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_network_defined_date_model(IN p_issuing_org varchar, p_name_string varchar, 
p_version varchar, p_issuer_binary_string varchar, p_tag varchar, INOUT r_date_model_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_date_model('NETWORK_CONFIG', p_issuing_org, p_name_string, p_version, r_date_model_id); 
    INSERT INTO network_config(date_model_id, issuer_binary_string, tag) VALUES(r_date_model_id, p_issuer_binary_string, p_tag); 
end$$;



-- ==================================================================
-- SP__insert_network_defined_date_model
--    insert a new moderated adm into the db
-- Parameters:
-- in 
-- 		p_issuing_org varchar - name of the issuing organization for this ADM.
-- 		p_name_string varchar- this is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_version varchar - This is a string representation of the version of the ADM.
--           ADM version representations are formated at the discretion of
--           the publishing organization.
-- 		p_adm_name_string varchar -  this is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_enumeration integer - an unsigned integer in the range of 0 to
-- 			 (2^64)/20
-- 		p_enumeration_label varchar - labeled based on the number of bytes
-- 			of the Nickname as a function of the size of the ADM enumeration
-- 		p_use_desc varchar - human readable use description
-- out
-- 		r_date_model_id integer - id of the date_model in the database 
-- ==================================================================

CREATE OR REPLACE PROCEDURE SP__insert_adm_defined_date_model(IN p_issuing_org varchar, p_date_model_string varchar, 
p_version varchar, p_adm_name_string varchar, p_enumeration integer, p_enumeration_label varchar, 
p_use_desc varchar, INOUT r_date_model_id integer) 
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_date_model('MODERATED', p_issuing_org, p_date_model_string, p_version, r_date_model_id);
    INSERT INTO adm(date_model_id, adm_name, enumeration, enumeration_label, use_desc) VALUES(r_date_model_id, 
    p_adm_name_string, p_enumeration, p_enumeration_label, p_use_desc); 
end$$; 







-- STORED PROCEDURE(S) for creating updating an deleting operator definitions 


-- ==================================================================
-- SP__insert_operator_formal_definition
-- IN 
-- 		p_obj_id integer - metadata id for this report
-- 		p_use_desc varchar - human readable use description
--      fp_spec_id integer, 
--      p_num_operands integer, 
-- 		p_operands varchar, 
--		p_result_name varchar, 
--		p_result_type varchar,    
-- OUT 
-- 		r_definition_id integer - actual id of this operator
-- ==================================================================
--add_oid, 'Add two uvast values.The operands are cast to the least compatible numeric typebefore the arithmetic.', null, 2, null, '.left, .right', 'result', null, add_fid);
CREATE OR REPLACE PROCEDURE SP__insert_operator_formal_definition(IN p_obj_id integer, p_use_desc varchar, 
fp_spec_id integer, p_num_operands integer, p_operands varchar, p_result_name varchar, p_result_type varchar,  INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO operator_formal_definition(obj_formal_definition_id, fp_spec_id, num_operands, operands, result_name, result_type)
    VALUES(r_definition_id, fp_spec_id, p_num_operands, p_operands, p_result_name, p_result_type); 
end$$;


-- ==================================================================
-- SP__insert_operator_actual_definition
-- IN 
-- 		p_obj_id integer - metadata id for this report
-- 		p_use_desc varchar - human readable use description
--      ap_spec_id integer,     
-- OUT 
-- 		r_definition_id integer - actual id of this operator
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_operator_actual_definition(IN p_obj_definition_id integer, p_use_desc varchar, p_ap_spec_id integer,  INOUT r_actual_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_definition_id); 
    INSERT INTO operator_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_actual_definition_id, p_ap_spec_id);  
end$$; 

-- ==================================================================
-- SP__delete_operator_formal_defintion
-- IN 
-- 		p_obj_id integer - id of this op to delete,  
-- 		p_obj_name varchar - name of this op to delete 
-- ==================================================================
 
CREATE OR REPLACE PROCEDURE SP__delete_operator_formal_defintion(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$ 
<<oper_def_del>>
DECLARE 
def_id int; 
fp_id int;
BEGIN
    def_id = (SELECT obj_formal_definition_id from obj_formal_definition where obj_metadata_id = p_obj_id );
	fp_id = (SELECT fp_spec_id from operator_formal_definition where obj_formal_definition_id = def_id );
 
	DELETE FROM formal_parmspec WHERE fp_spec_id = fp_id;

	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
	CALL SP__delete_obj_metadata(p_obj_id, p_obj_name);
end$$; 






-- STORED PROCEDURE(S) for adding updating and deleting time base rule defintions and instances 

-- =================
-- for adding state based rules into the database 
-- SP__insert_sbr_actual_definition 
-- IN 
-- 		p_obj_id integer - metadata id of this SBR
-- 		p_use_desc varchar- human readable description
-- 		p_expr_id integer, - id of the expresion for this rule
-- 		p_ac_id integer, - id of the ari collection that defines the action of this rule 
-- 		p_start_time time - whem this rule starts 
-- OUT 
-- 		r_definition_id integer - id of the start 
-- ====================================


CREATE OR REPLACE PROCEDURE SP__insert_sbr_actual_definition(IN p_obj_id integer, p_use_desc varchar,  p_condition bytea, p_action bytea, p_min_interval integer, p_max_count integer, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO sbr_actual_definition(obj_actual_definition_id, condition, action, min_interval, max_count) VALUES(r_definition_id, p_condition, p_action, p_min_interval, p_max_count);
end$$;



-- ==================================================================
-- SP__delete_sbr_actual_definition;
-- Parameters:
-- in 
--      p_obj_id integer - id for the sbr to delete 
-- 		p_obj_name varchar -  name of the sbr to delete --
-- ==================================================================
 
CREATE OR REPLACE PROCEDURE SP__delete_sbr_actual_definition(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$ 
<<sbr_def_del>>
DECLARE
exp_id int;
ac_id int;
BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		select obj_id FROM obj_metadata WHERE obj_name = p_obj_name into p_obj_id;
    ELSE 
		EXIT sbr_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		select obj_name from vw_sbr_formal where obj_id = p_obj_id into p_obj_name;
    END IF ;
    
	
    CALL SP__delete_obj_atual_defintion(p_obj_id, p_obj_name);
end$$;


-- STORED PROCEDURE(S)


-- ==
-- function for string processing used by other sp
-- help make inserting large sets of data more user friendly 
-- ==


CREATE OR REPLACE PROCEDURE SP__get_delimiter_position(IN p_str varchar, p_delimiter varchar, p_start_position integer, INOUT r_found_position integer)
LANGUAGE plpgsql
AS $$ 
DECLARE
length int;
position_index int;
iterator_index int;
char_value varchar;
delimiter_found BOOLEAN;
BEGIN
	IF (p_str IS NOT NULL) && (p_start_position IS NOT NULL)THEN 
		 BEGIN
			length := CHAR_LENGTH(p_str); 
			position_index := p_start_position; 
            iterator_index := p_start_position; 
            char_value := ''; 
            delimiter_found := FALSE; 
            r_found_position := NULL; 
			WHILE (position_index != length) && (delimiter_found != TRUE) loop 
				 
					char_value := SUBSTRING(p_str, iterator_index, 1);
                    IF char_value LIKE p_delimiter THEN 
						position_index := iterator_index;
                        r_found_position := position_index; 
                        delimiter_found := TRUE; 
                    END IF; 
                    iterator_index := iterator_index + 1; 
				
			END LOOP; 
		END; 
    ELSE 
		 BEGIN
			r_found_position := NULL; 
        END; 
    END IF; 
end$$;




CREATE OR REPLACE PROCEDURE SP__null_string_check(INOUT p_string varchar)
LANGUAGE plpgsql
AS $$ BEGIN
	IF (p_string LIKE 'null') || (p_string LIKE 'NULL') || (p_string LIKE 'Null') THEN BEGIN
			 p_string := NULL; 
        END; 
    END IF; 
end$$;


-- 
-- 
-- -- table_definition_id,  use_desc, 
-- CREATE OR REPLACE PROCEDURE SP__insert_table_instance(IN p_obj_definition_id integer,  p_use_desc varchar, p_row_values_list varchar(10000), INOUT r_instance_id integer)

-- 	
--     -- have to visit how to store multiple rows is is just one long value collection splitting at every num_colmns?
-- 	CALL SP__insert_obj_instance(p_obj_definition_id, p_use_desc, r_instance_id); 
--     @n_rows = (select num_entries from type_name_value_collection where tnvc_id =(select columns_list from table_template_definition where definition_id = p_obj_definition_id));
--     CALL  SP__insert_tnv_collection(@num_rows, null, null, p_row_values_list , @tnvc_id);  
--     INSERT INTO table_instance(instance_id, ap_spec_id) VALUES(r_instance_id, p_ap_spec_id);   
-- end$$;
-- 

-- STORED PROCEDURE(S) for adding updating and deleting time base rule defintions and instances 
-- need type checking for actions, they need to be Macros or controls


-- ===============================================================
-- SP__insert_tbr_actual_definition 
-- IN
-- 		p_obj_id integer - metadata id
-- 		p_use_desc varchar - human readable descriptionb
-- 		p_wait_per time  - how long to wait before starting 
-- 		p_run_count bigint - number of times to run 
-- 		p_start_time time - when to start 
-- 		p_ac_id integer - ac of the actions for this rules 
-- OUT 
-- 		r_definition_id integer - id of this tbr
-- ===============================================================


CREATE OR REPLACE PROCEDURE SP__insert_tbr_actual_definition(IN p_obj_id integer, p_use_desc varchar, 
p_wait_per time, p_run_count bigint, p_start_time time, p_action bytea, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO tbr_actual_definition(obj_actual_definition_id, wait_period, run_count, start_time, action) VALUES(r_definition_id, p_wait_per, p_run_count, p_start_time, p_action);
end$$;




-- ==================================================================
-- SP__delete_tbr_actual_definition;
-- Parameters:
-- in 
--      p_obj_id integer - id for the tbr to delete 
-- 		p_obj_name varchar -  name of the tbr to delete --
-- ==================================================================
 
CREATE OR REPLACE PROCEDURE SP__delete_tbr_actual_definition(IN p_obj_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$ 
<<tbr_def_del>>
DECLARE ac_id int;
BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		select obj_id FROM obj_metadata WHERE obj_name = p_obj_name into p_obj_id;
    ELSE 
		EXIT tbr_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		select obj_name from vw_tbr_formal where obj_id = p_obj_id into p_obj_name;
    END IF ;
    
	
    CALL SP__delete_obj_atual_defintion(p_obj_id, p_obj_name);
end$$;



-- STORED PROCEDURE(S) for adding updating and removing variables 
-- ==================================================================
-- SP__insert_variable_definition 
-- inserting a new variable 
-- IN 
-- 		p_obj_id integer - metadata id of the variable
-- 		p_use_desc varchar - human readable description 
-- 		p_out_type integer - out type of the variable
-- 		p_num_operators integer - number of operators 
-- 		p_operator_ids_list varchar - 
-- OUT 
-- 		r_definition_id integer - definition id of the variable
--
-- ==================================================================

CREATE OR REPLACE PROCEDURE SP__insert_variable_formal_definition(IN p_obj_id integer, p_use_desc varchar, p_fp_spec_id integer,
p_out_type varchar,  p_expression varchar, p_init_value varchar, INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ 
DECLARE 
r_type_id int;
BEGIN 
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id);
	r_type_id = (SELECT data_type_id from data_type where type_name = UPPER(p_out_type));
	INSERT INTO variable_formal_definition(obj_formal_definition_id, fp_spec_id, data_type_id, expression, init_value) VALUES(r_definition_id, p_fp_spec_id, r_type_id, p_expression, p_init_value); 
end$$;


-- ==================================================================
-- SP__insert_variable_definition 
-- inserting a new variable 
-- IN 
-- 		p_obj_id integer - metadata id of the variable
-- 		p_use_desc varchar - human readable description 
-- 		p_out_type integer - out type of the variable
-- 		p_num_operators integer - number of operators 
-- 		p_operator_ids_list varchar - 
-- OUT 
-- 		r_definition_id integer - definition id of the variable
--
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__insert_variable_actual_definition(IN p_obj_id integer, p_use_desc varchar, p_ap_spec_id integer,
INOUT r_definition_id integer)
LANGUAGE plpgsql
AS $$ 
DECLARE 
r_expr_id int;
BEGIN 
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
	INSERT INTO variable_actual_definition(obj_actual_definition_id,ap_spec_id) VALUES(r_definition_id, p_ap_spec_id); 

end$$;



-- ==================================================================
-- SP__delete_var_actual_definition;
-- Parameters:
-- in 
--      p_obj_id integer - id for the var to delete 
-- 		p_obj_name varchar -  name of the var to delete --
-- ==================================================================
CREATE OR REPLACE PROCEDURE SP__delete_variable_definition(IN p_definition_id integer, p_obj_name varchar)
LANGUAGE plpgsql
AS $$
DECLARE
exp_id int;
 BEGIN
	-- delete the expresion first
    exp_id = (SELECT expression_id from variable_actual_definition where p_definition_id = obj_actual_definition);
    CALL SP__delete_expression(exp_id);
    CALL SP__delete_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
	
end$$;


CREATE OR REPLACE PROCEDURE SP__add_agent_parameter_received(IN p_manager_id INTEGER, p_registered_agents_id INTEGER, p_agent_parameter_id int, p_command_parameters VARCHAR )
LANGUAGE plpgsql
as $$ BEGIN
    INSERT INTO agent_parameter_received(manager_id, registered_agents_id, agent_parameter_id, command_parameters) VALUES(p_manager_id, p_registered_agents_id, p_agent_parameter_id, p_command_parameters);
END$$;


CREATE OR REPLACE PROCEDURE SP__add_agent_parameter(IN  p_command_name VARCHAR, p_command_parameters VARCHAR )
LANGUAGE plpgsql
as $$ BEGIN
    INSERT INTO agent_parameter(command_name, command_parameters) VALUES(p_command_name, p_command_parameters);
END$$;


create or replace procedure SP__insert_rptset(in p_nonce_cbor BYTEA, p_reference_time TIMESTAMP, p_report_list TEXT, p_report_list_cbor BYTEA, p_agent_endpoint_uri TEXT)
language plpgsql
as $$
DECLARE
    agent_row_id INTEGER;
BEGIN
    CALL SP__insert_agent(p_agent_endpoint_uri, agent_row_id);
    INSERT INTO ari_rptset(nonce_cbor, reference_time, report_list, report_list_cbor, agent_id)
        VALUES(p_nonce_cbor, p_reference_time, p_report_list, p_report_list_cbor, agent_row_id);
End$$;

create or replace procedure SP__insert_execset(in p_nonce_cbor BYTEA, p_use_desc varchar, p_agent_id varchar, p_exec_set bytea, p_num_entries INT)
language plpgsql
	as $$ 
	DECLARE
	r_ac_id INTEGER;
	BEGIN 
	SELECT ac_id INTO r_ac_id
		FROM  ari_collection where num_entries = p_num_entries and entries = p_exec_set;
		
	IF (r_ac_id IS NULL) THEN
		insert INTO ari_collection(num_entries,entries) 
		VALUES(p_num_entries,p_exec_set) RETURNING ac_id into r_ac_id;
    END IF ;
		INSERT INTO execution_set(nonce_cbor, ac_id , use_desc, agent_id)
	VALUES(p_nonce_cbor, r_ac_id, p_use_desc, p_agent_id);
	End$$;
