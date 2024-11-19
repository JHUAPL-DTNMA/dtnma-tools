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

-- creating examples 


use amp_core;

--  test case for adding some sample report requests and returns 
-- creating a new report that takes 

-- ac of the report
-- locate the edds to included 
SET @edd1 = (SELECT obj_actual_definition_id from vw_edd_actual where obj_name =  'cur_time');
SET @edd2 = (SELECT obj_actual_definition_id from vw_edd_actual where obj_name =  'num_pend_fwd');
SET @edd3 = (SELECT obj_actual_definition_id from vw_edd_actual where obj_name =  'num_pend_dis');
SET @edd4 = (SELECT obj_actual_definition_id from vw_edd_actual where obj_name =  'num_sbr');
SET @edd5 = (SELECT obj_actual_definition_id from vw_edd_actual where obj_name =  'num_tbr');

-- create the ac set 
CALL SP__insert_ac_id(5, "ac fpr test report", @r_rpt_ac );
CALL SP__insert_ac_actual_entry(@r_rpt_ac , @edd1, 1, @r_ac_entry_id);
CALL SP__insert_ac_actual_entry(@r_rpt_ac , @edd2, 2, @r_ac_entry_id);
CALL SP__insert_ac_actual_entry(@r_rpt_ac , @edd3, 3, @r_ac_entry_id);
CALL SP__insert_ac_actual_entry(@r_rpt_ac , @edd4, 4, @r_ac_entry_id);
CALL SP__insert_ac_actual_entry(@r_rpt_ac , @edd5, 5, @r_ac_entry_id);

CALL SP__insert_obj_metadata(7, 'test_rpt', null, @rptt_obj_id_1);
CALL SP__insert_report_template_formal_definition(@rptt_obj_id_1, 'This is a test report.', null, @r_rpt_ac , @rptt_definition_id_1);
-- singleton value of the report since it doesnt have any parameters 
CALL SP__insert_report_actual_definition(@rptt_obj_id_1, null, 'this is the singelton of the test report' , @rptt_actual_id_1);
 

-- Example ARI command(s) to place in the outgoing message queue, with the intended ARI CBOR or URI equivalents to validate. We have plenty of 
-- examples in the nm unit-tests that we can start from.
-- ION Inserts into register_agents table all known agents, and agents discovered from received registration messages.
CALL SP__insert_agent("test:test", @r_registered_agents_id);
CALL SP__insert_agent("test:test2", @r_registered_agents_id);
-- Currently implemented and working as a simple insert.  See my prior note on future enhancements here.

-- Tester Inserts into DB a command to generate a report.
-- For example, ari:/IANA:amp.agent/Ctrl.gen_rpts([ari:/IANA:amp.agent/Rptt.full_report()],[])

-- long way of getting state_id could just do 0 i think 
CALL SP__insert_outgoing_message_set( DATE('1997-05-07 08:00:00'), null, (SELECT state_id FROM outgoing_state WHERE state_name = 'Initializing'), @r_registered_agents_id, @r_set_id);



-- CREATE the ac for the message set ALTER
-- Ctrl.gen_rpts([ari:/IANA:amp.agent/Rptt.full_report()],[])
-- SP__insert_control_actual_definition(IN p_obj_definition_id int(10) unsigned, p_ap_spec_id int(10) unsigned, p_use_desc varchar(255), OUT r_instance_id int(10) unsigned

CALL SP__insert_ac_id(1, "ac for gen report", @r_rpt_ac );
CALL SP__insert_ac_actual_entry(@r_rpt_ac , (SELECT obj_actual_definition_id FROM vw_rpt_actual WHERE obj_name = 'full_report'), 1, @r_ac_entry_id);

CALL SP__insert_tnvc_collection( 'tnvc for gen report', @r_tnvc_id );


CALL SP__insert_actual_parmspec((SELECT  fp_spec_id FROM control_formal_definition WHERE obj_formal_definition_id = (SELECT obj_formal_definition_id FROM vw_ctrl_definition WHERE obj_name = 'gen_rpts')), 2, 'actual parms for test gen_rpts', @r_ap_spec_id);
CALL SP__insert_actual_parms_ac(@r_ap_spec_id, 1, @r_rpt_ac);
CALL SP__insert_actual_parms_tnvc(@r_ap_spec_id , 2, @r_tnvc_id );

CALL SP__insert_control_actual_definition((SELECT obj_metadata_id FROM vw_ctrl_definition WHERE obj_name = 'gen_rpts' ), @r_ap_spec_id, 'Ctrl.gen_rpts([ari:/IANA:amp.agent/Rptt.full_report()],[])', @r_act_ctrl_id );

CALL SP__insert_ac_id(1, "ac for message 1", @r_mes_ac );
CALL SP__insert_ac_actual_entry(@r_mes_ac , @r_act_ctrl_id, 1, @r_ac_entry_id);

CALL SP__insert_outgoing_message_entry(@r_set_id,1, DATE('2020-06-24 08:00:00'), @r_mes_ac, @r_message_id);

SET @p_state = (SELECT state_id
    FROM outgoing_state
    WHERE state_name = 'ready');
CALL SP__update_outgoing_message_set(@r_set_id, @p_state); 


 --  testing adding the same agent  
CALL SP__insert_agent("test:test", @r_registered_agents_id);





-- Application SELECTS pending (outgoing) messages and finds the newly inserted record
-- This query will be made in a loop for mySQL.  If we ever create a Postgresql port, we could use LISTEN/NOTIFY instead.
-- Application SELECTS component pieces of the command to build up the appropriate ARI definition and encapsulating message definition, then serializes into CBOR for transmission.
-- Application receives a response and INSERTs the received report into the database
-- As noted; We can insert the full definition of the received message set, but all we really need are the individual reports (with timestamps).  I can grab the CBOR strings for a sample report, or more usefully, the fully decomposed definition as output by my amp.me script to serve as an example of what we need to insert into the database.
-- Future: UI or other processing application accesses received reports as needed. 




