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

-- object views 

create or replace
view vw_obj_metadata as
       select
	om.*,
	data_model."name" data_model_name,
	data_model.enumeration,
	data_model.namespace_type,
	data_model.use_desc
from
	obj_metadata om
left join data_model on
	data_model.data_model_id = om.data_model_id;

create or replace
view vw_obj_formal_def as
select
	VOM.*,
	DT.TYPE_NAME
from
	(
	select
		vom.*,
		ofd.obj_formal_definition_id,
		ofd.use_desc as "formal_desc"
	from
		obj_formal_definition ofd
	left join vw_obj_metadata vom on
		ofd.obj_metadata_id = vom.obj_metadata_id) VOM
left join DATA_TYPE DT on
	VOM.DATA_TYPE_ID = DT.DATA_TYPE_ID;

create or replace
view vw_obj_actual_def as
       select
	VOM.OBJ_METADATA_ID,
	DT.TYPE_NAME,
	VOM.name,
	VOM.data_model_id,
	VOM.data_model_name,
	VOM.enumeration,
	VOM.namespace_type,
	VOM.USE_DESC,
	VOM.OBJ_ACTUAL_DEFINITION_ID,
	VOM.ACTUAL_DESC
from
	(
	select
		VOM.OBJ_METADATA_ID,
			VOM.DATA_TYPE_ID,
			VOM.name,
			VOM.data_model_id,
			VOM.data_model_name,
			VOM.enumeration,
			VOM.namespace_type,
			VOM.USE_DESC,
			OAD.OBJ_ACTUAL_DEFINITION_ID,
			OAD.USE_DESC as ACTUAL_DESC
	from
		OBJ_ACTUAL_DEFINITION OAD
	left join VW_OBJ_METADATA VOM on
		OAD.OBJ_METADATA_ID = VOM.OBJ_METADATA_ID) VOM
left join DATA_TYPE DT on
	VOM.DATA_TYPE_ID = DT.DATA_TYPE_ID;


-- ctrl view 
create or replace
view vw_ctrl_definition as
	select
		join3.obj_metadata_id,
		join3.name,
		join3.data_model_id,
		join3.data_model_name,
		join3.obj_formal_definition_id,
		formal_parmspec.fp_spec_id,
		formal_parmspec.num_parms,
		formal_parmspec.parameters,
		join3.use_desc
from
		formal_parmspec
join (
	select
			obj_metadata.obj_metadata_id,
			obj_metadata.name,
			obj_metadata.data_model_id,
			obj_metadata.data_model_name,
			join2.obj_formal_definition_id,
			join2.fp_spec_id,
			join2.use_desc
	from
			vw_obj_metadata as obj_metadata
	join (
		select
				obj_formal_definition.obj_formal_definition_id,
				obj_formal_definition.obj_metadata_id,
				obj_formal_definition.use_desc,
				control_formal_definition.fp_spec_id
		from
				obj_formal_definition
		join control_formal_definition on
				control_formal_definition.obj_formal_definition_id = obj_formal_definition.obj_formal_definition_id) join2 on
			join2.obj_metadata_id = obj_metadata.obj_metadata_id) join3 on
		formal_parmspec.fp_spec_id = join3.fp_spec_id;

create or replace
view vw_ctrl_actual as
select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	obj_actual_definition_id,
	ap_spec_id,
	use_desc
from
	obj_metadata
join
    (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_formal_definition.obj_metadata_id,
		view1.use_desc,
		obj_actual_definition_id,
		ap_spec_id
	from
		obj_formal_definition
	join
         (
		select
			obj_actual_definition.obj_actual_definition_id,
			obj_actual_definition.obj_metadata_id,
			use_desc,
			ap_spec_id
		from
			obj_actual_definition
		join control_actual_definition on
			obj_actual_definition.obj_actual_definition_id = control_actual_definition.obj_actual_definition_id) as view1 on
		view1.obj_metadata_id = obj_formal_definition.obj_metadata_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;



-- edd view 
create or replace
view vw_edd_formal as
    select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	fp_spec_id,
	join2.data_type,
	use_desc
from
	obj_metadata
join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_metadata_id,
		use_desc,
		fp_spec_id,
		data_type
	from
		obj_formal_definition
	join edd_formal_definition on
		edd_formal_definition.obj_formal_definition_id = 
        obj_formal_definition.obj_formal_definition_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;

create or replace
view vw_edd_actual as
    select
	vw_edd_formal.obj_metadata_id,
	name,
	data_model_id,
	vw_edd_formal.obj_formal_definition_id,
	obj_actual_definition_id,
	vw_edd_formal.data_type,
	ap_spec_id,
	join2.use_desc
from
	vw_edd_formal
inner join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_formal_definition.obj_metadata_id,
		view1.use_desc,
		obj_actual_definition_id,
		ap_spec_id
	from
		obj_formal_definition
	join (
		select
			obj_actual_definition.obj_actual_definition_id,
			obj_actual_definition.obj_metadata_id,
			use_desc,
			ap_spec_id
		from
			obj_actual_definition
		join edd_actual_definition on
			obj_actual_definition.obj_actual_definition_id = edd_actual_definition.obj_actual_definition_id) as view1 on
		view1.obj_metadata_id = obj_formal_definition.obj_metadata_id) join2 on
	join2.obj_metadata_id = vw_edd_formal.obj_metadata_id;


-- mac views 
create or replace
view vw_mac_definition as
    select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	fp_spec_id,
	max_call_depth,
	use_desc
from
	obj_metadata
join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_metadata_id,
		use_desc,
		fp_spec_id,
		max_call_depth
	from
		obj_formal_definition
	join macro_formal_definition on
		macro_formal_definition.obj_formal_definition_id = obj_formal_definition.obj_formal_definition_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;

create or replace
view vw_mac_actual as
    select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	obj_actual_definition_id,
	ap_spec_id,
	use_desc
from
	obj_metadata
inner join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_formal_definition.obj_metadata_id,
		view1.use_desc,
		obj_actual_definition_id,
		ap_spec_id
	from
		obj_formal_definition
	join (
		select
			obj_actual_definition.obj_actual_definition_id,
			obj_actual_definition.obj_metadata_id,
			use_desc,
			ap_spec_id
		from
			obj_actual_definition
		join macro_actual_definition on
			obj_actual_definition.obj_actual_definition_id = macro_actual_definition.obj_actual_definition_id) as view1 on
		view1.obj_metadata_id = obj_formal_definition.obj_metadata_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;


-- oper view 
create or replace
view vw_oper_formal as
    select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	fp_spec_id,
	result_type,
	 result_name,
	num_operands,
	operands,
	use_desc
from
	obj_metadata
join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_metadata_id,
		fp_spec_id,
		use_desc,
		result_type,
		result_name, 
		num_operands,
		operands
	from
		obj_formal_definition
	join operator_formal_definition on
		operator_formal_definition.obj_formal_definition_id = obj_formal_definition.obj_formal_definition_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;

create or replace
view vw_oper_actual as
    select
	join2.obj_metadata_id,
	name,
	data_model_id,
	obj_actual_definition_id,
	obj_formal_definition_id,
	ap_spec_id,
	result_type,
	result_name,
	num_operands,
	operands,
	use_desc
from
	vw_oper_formal
join(
	select
		operator_actual_definition.obj_actual_definition_id,
		obj_metadata_id,
		ap_spec_id
	from
		obj_actual_definition
	join operator_actual_definition on
		operator_actual_definition.obj_actual_definition_id = obj_actual_definition.obj_actual_definition_id) join2 on
	join2.obj_metadata_id = vw_oper_formal.obj_metadata_id;


-- sbr view 
create or replace
view vw_sbr_actual as
    select
	obj_formal_definition.obj_metadata_id,
	name,
	data_model_id,
	obj_actual_definition_id,
	obj_formal_definition.obj_formal_definition_id,
	condition,
	run_count,
	min_interval,
	action,
	join3.use_desc
from 
		obj_formal_definition
join (
	select
		obj_metadata.obj_metadata_id,
		name,
		data_model_id,
		obj_actual_definition_id,
		condition,
		run_count,
		min_interval,
		action,
		use_desc
	from
		obj_metadata
	join
        (
		select
			obj_actual_definition.obj_actual_definition_id,
			obj_metadata_id,
			use_desc,
			condition,
			run_count,
			min_interval,
			action
		from
			obj_actual_definition
		join sbr_actual_definition on
			sbr_actual_definition.obj_actual_definition_id = obj_actual_definition.obj_actual_definition_id) join2 on
		join2.obj_metadata_id = obj_metadata.obj_metadata_id ) join3 on
	obj_formal_definition.obj_metadata_id = join3.obj_metadata_id;
        
-- tbr view 
create or replace
view vw_tbr_actual as
select
	obj_formal_definition.obj_metadata_id,
	name,
	data_model_id,
	obj_actual_definition_id,
	obj_formal_definition_id,
	wait_period,
	run_count,
	start_time,
	action,
	join3.use_desc
from 
obj_formal_definition
join (
select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_actual_definition_id,
	run_count,
	wait_period,
	start_time,
	action,
	use_desc
from
	obj_metadata
join
        (
	select
		obj_actual_definition.obj_actual_definition_id,
		obj_metadata_id,
		use_desc,
		run_count,
		wait_period,
		start_time,
		action
	from
		obj_actual_definition
	join tbr_actual_definition on
		tbr_actual_definition.obj_actual_definition_id = obj_actual_definition.obj_actual_definition_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id) join3 on
	obj_formal_definition.obj_metadata_id = join3.obj_metadata_id;


	-- var view 
create or replace
view vw_var_formal as
select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	fp_spec_id,
	expression,
	init_value,
	use_desc
from
	obj_metadata
inner join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_formal_definition.obj_metadata_id,
		fp_spec_id,
		expression,
		init_value,
		use_desc
	from
		obj_formal_definition
	join variable_formal_definition on
		obj_formal_definition.obj_formal_definition_id = variable_formal_definition.obj_formal_definition_id) as formal_join on
	formal_join.obj_metadata_id = obj_metadata.obj_metadata_id;

	
	create or replace
view vw_var_actual as
    select
	vw_var_formal.obj_metadata_id,
	name,
	data_model_id,
	vw_var_formal.obj_formal_definition_id,
	obj_actual_definition_id,
	ap_spec_id,
	expression,
	init_value,
	view1.use_desc
from
	vw_var_formal
join (
	select
		variable_actual_definition.obj_actual_definition_id,
        obj_metadata_id,
		use_desc,
		ap_spec_id
	from
			obj_actual_definition
	join variable_actual_definition on
			obj_actual_definition.obj_actual_definition_id = variable_actual_definition.obj_actual_definition_id) as view1 on
	vw_var_formal.obj_metadata_id = view1.obj_metadata_id;

	

-- ident view 
create or replace
view vw_ident_formal as
    select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	fp_spec_id,
	use_desc
from
	obj_metadata
join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_metadata_id,
		use_desc,
		fp_spec_id
	from
		obj_formal_definition
	join ident_formal_definition on
		ident_formal_definition.obj_formal_definition_id = 
        obj_formal_definition.obj_formal_definition_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;

create or replace
view vw_ident_actual as
    select
	obj_metadata.obj_metadata_id,
	name,
	data_model_id,
	obj_formal_definition_id,
	obj_actual_definition_id,
	ap_spec_id,
	use_desc
from
	obj_metadata
inner join
        (
	select
		obj_formal_definition.obj_formal_definition_id,
		obj_formal_definition.obj_metadata_id,
		view1.use_desc,
		obj_actual_definition_id,
		ap_spec_id
	from
		obj_formal_definition
	join (
		select
			obj_actual_definition.obj_actual_definition_id,
			obj_actual_definition.obj_metadata_id,
			use_desc,
			ap_spec_id
		from
			obj_actual_definition
		join ident_actual_definition on
			obj_actual_definition.obj_actual_definition_id = ident_actual_definition.obj_actual_definition_id) as view1 on
		view1.obj_metadata_id = obj_formal_definition.obj_metadata_id) join2 on
	join2.obj_metadata_id = obj_metadata.obj_metadata_id;
	



-- message view 
create or replace
view vw_ready_outgoing_message_groups as
select
	*
from
	message_group
where
	is_outgoing = true
	and state_id = 1;

create or replace
view vw_ready_incoming_message_groups as
select
	*
from
	message_group
where
	is_outgoing = false
	and state_id = 1;

create or replace
view vw_message_agents as
select
	ma.message_id,
	ra.*
from
	message_agents ma
left join registered_agents ra on
	ma.agent_id = ra.registered_agents_id;

create or replace
view vw_message_group_agents as
select
	ma.group_id,
	ra.*
from
	message_group_agents ma
left join registered_agents ra on
	ma.agent_id = ra.registered_agents_id;



-- 
-- view for table templates 

create or replace
view vw_ari_tblt as
    select
	ari_tblt_id ,
	num_columns,
	ari_tblt.ac_id,
	entries
from
	ari_tblt
inner join ari_collection 
    on
	ari_tblt.ac_id = ari_collection.ac_id;



/*
-- namespace view 
create or replace
view vw_namespace as 
select
	data_model.data_model_enum,
	data_model.name,
	namespace.name_string,
	namespace.version_name,
	data_model.use_desc
from
	data_model
join namespace 
   on
	data_model.data_model_id = namespace.data_model_id;	
*/
		
						
-- views for parameters

create or replace
	view VW_FORMAL_PARAMETERS as
select
	*
from
	FORMAL_PARMSPEC;


-- actual_parmspec view
create or replace
view VW_ACTUAL_PARAMETERS as
select
	formal_parmspec.fp_spec_id,
	formal_parmspec.num_parms,
	formal_parmspec.parameters as "parameters",
	actual_parmspec.ap_spec_id,
	actual_parmspec.value_set,
	actual_parmspec.use_desc
from
	actual_parmspec,
	formal_parmspec
where 
	actual_parmspec.fp_spec_id = formal_parmspec.fp_spec_id;


create or replace
view vw_rptt as 
select
	ari_rptt.*,
	ari_collection.num_entries,
	ari_collection.entries
from
	ari_collection,
	ari_rptt
where 
	ari_rptt.ac_id = ari_collection.ac_id;
	

create or replace
view vw_rpt_set as 
select
	rpt_join.*,
	vw_rptt.entries "template"  
from
	vw_rptt,
	(select
		ari_rptset.*,
		ari_collection.num_entries as ac_num_entries , 
		ari_collection.entries
	from
		ari_collection,
		ari_rptset
	where 
		ari_rptset.ac_id = ari_collection.ac_id ) as rpt_join
	where vw_rptt.ari_rptt_id = rpt_join.ari_rptt_id;

	
	



create or replace
	view vw_ari_union as 
SELECT union_join.*, obj_metadata.data_type_id
FROM 
(select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
obj_actual_definition_id,
use_desc from vw_ctrl_actual union all  
select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
obj_actual_definition_id,
use_desc from vw_edd_actual union all
select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
obj_actual_definition_id,
use_desc from vw_mac_actual union all
select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
obj_actual_definition_id,
use_desc from vw_oper_actual union all
select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
obj_actual_definition_id,
use_desc from vw_sbr_actual union all
select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
obj_actual_definition_id,
use_desc from vw_tbr_actual union all
select obj_metadata_id,
name,
data_model_id,
obj_formal_definition_id,
ap_spec_id,
use_desc from vw_ident_actual) as union_join join  obj_metadata on  
union_join.obj_metadata_id = obj_metadata.obj_metadata_id ;
	
	
/*
	-- ari view 
create or replace
	view vw_ari_union as 
select
	ACTUAL_ARI.OBJ_METADATA_ID,
	ACTUAL_ARI.OBJ_NAME,
	AD.name,
	AD.TYPE_NAME,
	ACTUAL_ARI.data_model_id,
	ACTUAL_ARI.OBJ_ACTUAL_DEFINITION_ID as obj_id,
	ACTUAL_ARI.AP_SPEC_ID as PARM_ID,
	true as ACTUAL
from
	(
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			AP_SPEC_ID
	from
		VW_EDD_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			AP_SPEC_ID
	from
		VW_MAC_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			null as OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			null as AP_SPEC_ID
	from
		VW_OPER_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			AP_SPEC_ID
	from
		VW_RPT_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			null as OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			null as AP_SPEC_ID
	from
		VW_SBR_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			null as OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			null as AP_SPEC_ID
	from
		VW_TBLT_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			null as OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			null as AP_SPEC_ID
	from
		VW_TBR_ACTUAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			null as OBJ_FORMAL_DEFINITION_ID,
			OBJ_ACTUAL_DEFINITION_ID,
			null as AP_SPEC_ID
	from
		VW_VAR_ACTUAL
		) as ACTUAL_ARI
inner join VW_OBJ_ACTUAL_DEF AD on
	AD.OBJ_ACTUAL_DEFINITION_ID = ACTUAL_ARI.OBJ_ACTUAL_DEFINITION_ID
union all
select
	FORMAL_ARI.OBJ_METADATA_ID,
	FORMAL_ARI.OBJ_NAME,
	FD.name,
	FD.TYPE_NAME,
	FORMAL_ARI.data_model_id,
	FORMAL_ARI.OBJ_FORMAL_DEFINITION_ID as obj_id,
	FORMAL_ARI.FP_SPEC_ID as PARM_ID,
	false as ACTUAL
from
	(
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
	from
		VW_CTRL_DEFINITION
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
	from
		VW_EDD_FORMAL
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
	from
		VW_MAC_DEFINITION
union all
	select
		OBJ_METADATA_ID,
			OBJ_NAME,
			data_model_id,
			OBJ_FORMAL_DEFINITION_ID,
			FP_SPEC_ID
	from
		VW_RPTT_FORMAL) FORMAL_ARI
inner join VW_OBJ_FORMAL_DEF FD on
	FD.OBJ_FORMAL_DEFINITION_ID = FORMAL_ARI.OBJ_FORMAL_DEFINITION_ID;
*/



/*
create or replace
view vw_ari as
select
	o.obj_actual_definition_id,
	o.obj_metadata_id,
	o.use_desc,
	-- vof.type_name, 
	vof.name,
	ns.*,
	vof.data_model_enum,
	vof.obj_enum,
	vof.use_desc as type_desc,
	vof.name as ns_desc,
	-- Note: desc are for debug only
	ate.Enumeration as data_model_type,
	coalesce(aps.tnvc_id,
	oad.tnvc_id) as tnvc_id,
	aps.fp_spec_id,
	coalesce(cad.ap_spec_id,
	ead.ap_spec_id,
	mad.ap_spec_id) as ap_spec_id,
	coalesce(lad.data_type_id,
	lcad.data_type_id,
	vof.data_type_id,
	oad.data_type_id,
	vad.data_type_id) as data_type_id,
	-- Data Type comes from exactly on eof these (VERIFY)
  coalesce(lcad.data_value,
	lad.data_value) as data_value,
	oad.num_operands,
	-- TODO: This field may be redundant vs tnvc_id length
	vad.expression_id
from
	obj_actual_definition o
left join vw_obj_formal_def vof on
	o.obj_metadata_id = vof.obj_metadata_id
	--    LEFT JOIN obj_metadata om ON o.obj_metadata_id=om.obj_metadata_id
	--    LEFT JOIN data_type dt ON om.data_type_id=dt.data_type_id
	--    LEFT JOIN data_model ON data_model.data_model_id = om.data_model_id
left join namespace ns on
	ns.data_model_id = vof.data_model_id
left join data_model_Type_Enumeration ate on
	ate.data_type_id = vof.data_type_id
	-- Types specifying actual_parmspec
left join control_actual_definition cad on
	cad.obj_actual_definition_id = o.obj_actual_definition_id
left join edd_actual_definition ead on
	ead.obj_actual_definition_id = o.obj_actual_definition_id
left join macro_actual_definition mad on
	mad.obj_actual_definition_id = o.obj_actual_definition_id
left join actual_parmspec aps on
	aps.ap_spec_id = coalesce(cad.ap_spec_id,
	ead.ap_spec_id,
	mad.ap_spec_id)
left join literal_actual_definition lad on
	lad.obj_actual_definition_id = o.obj_actual_definition_id
left join const_actual_definition lcad on
	lcad.obj_actual_definition_id = o.obj_actual_definition_id
left join operator_actual_definition oad on
	oad.obj_actual_definition_id = o.obj_actual_definition_id
left join variable_actual_definition vad on
	vad.obj_actual_definition_id = o.obj_actual_definition_id
;
*/
