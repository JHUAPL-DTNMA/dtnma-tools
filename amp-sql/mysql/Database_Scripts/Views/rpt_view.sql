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
use amp_core;
CREATE OR REPLACE VIEW `vw_rptt_formal` AS
    SELECT 
        obj_metadata.obj_metadata_id,
        obj_name,
        namespace_id,
        obj_formal_definition_id,
        fp_spec_id,
        use_desc
    FROM
        obj_metadata
            JOIN
        (SELECT 
            obj_formal_definition.obj_formal_definition_id,
                obj_metadata_id,
                use_desc,
                fp_spec_id
        FROM
            obj_formal_definition
        JOIN report_template_formal_definition ON report_template_formal_definition.obj_formal_definition_id = obj_formal_definition.obj_formal_definition_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id;


CREATE OR REPLACE VIEW `vw_rpt_actual` AS
    SELECT 
        obj_metadata.obj_metadata_id,
        obj_name,
        namespace_id,
        obj_formal_definition_id,
        obj_actual_definition_id,
        ap_spec_id,
        use_desc
    FROM
        obj_metadata
            INNER JOIN
        (SELECT 
            obj_formal_definition.obj_formal_definition_id,
                obj_formal_definition.obj_metadata_id,
                view1.use_desc,
                obj_actual_definition_id,
                ap_spec_id
        FROM
            obj_formal_definition
        JOIN (SELECT 
            obj_actual_definition.obj_actual_definition_id,
               obj_actual_definition.obj_metadata_id,
                use_desc,
                ap_spec_id
        FROM
            amp_core.obj_actual_definition
        JOIN amp_core.report_template_actual_definition ON obj_actual_definition.obj_actual_definition_id = report_template_actual_definition.obj_actual_definition_id) AS view1 ON view1.obj_metadata_id = amp_core.obj_formal_definition.obj_metadata_id) join2 ON join2.obj_metadata_id = obj_metadata.obj_metadata_id;

CREATE OR REPLACE VIEW `vw_rpt_entries` AS
SELECT 
        time,
        agent_id_string as 'Agent ID',
        obj_name as 'Report Name',
        adm_name as 'ADM',
        report_id as 'Report ID',
        str_values as 'String Values',
        uint_values as 'UINT Values',
        int_values as 'INT Values',
        real32_values as 'REAL32 Values',
        real64_values as 'REAL64 Values',
        uvast_values as 'UVAST Values',
        vast_values as 'VAST Values',
        obj_values as 'Object ID Values',
        ac_values as 'AC ID Values',
        tnvc_values as 'TNVC ID Values'
    FROM
        (SELECT 
            time,
                obj_name,
                adm_name,
                report_id,
                str_values,
                uint_values,
                int_values,
                obj_values,
                ac_values,
                tnvc_values,
                real32_values,
                real64_values,
                uvast_values,
                vast_values,
                group_id
        FROM
            ((SELECT 
            time,
                obj_name,
                adm_name,
                message_report_set_entry.report_id,
                str_values,
                uint_values,
                int_values,
                obj_values,
                ac_values,
                tnvc_values,
                real32_values,
                real64_values,
                uvast_values,
                vast_values,
                message_id
        FROM
            (SELECT 
            ts AS time,
                obj_name,
                adm_name,
                report_id,
                -- compiling all the separate variables in the TNVC into one column              
                GROUP_CONCAT(str_value) AS str_values,
                GROUP_CONCAT(uint_value) AS uint_values,
                GROUP_CONCAT(int_value) AS int_values,
                GROUP_CONCAT(obj_value) AS obj_values,
                GROUP_CONCAT(ac_value) AS ac_values,
                GROUP_CONCAT(tnvc_value) AS tnvc_values,
                GROUP_CONCAT(real32_value) AS real32_values,
                GROUP_CONCAT(real64_value) AS real64_values,
                GROUP_CONCAT(uvast_value) AS uvast_values,
                GROUP_CONCAT(vast_value) AS vast_values
        FROM
        -- compiling all the reports together         
            (((SELECT 
            ts,
                report_id,
                str_value,
                uint_value,
                int_value,
                obj_value,
                ac_value,
                tnvc_value,
                real32_value,
                real64_value,
                uvast_value,
                vast_value,
                order_num,
                ari_id
        FROM
            report_definition
        INNER JOIN vw_tnvc_entries ON report_definition.tnvc_id = vw_tnvc_entries.tnvc_id order by report_id , order_num) AS sel1
        INNER JOIN obj_actual_definition ON sel1.ari_id = obj_actual_definition.obj_actual_definition_id)
        INNER JOIN vw_obj_metadata ON vw_obj_metadata.obj_metadata_id = obj_actual_definition.obj_metadata_id)
        GROUP BY report_id) AS reports
        INNER JOIN message_report_set_entry ON message_report_set_entry.report_id = reports.report_id) AS message_report
        INNER JOIN message_group_entry ON message_report.message_id = message_group_entry.message_id)) AS agent_message
            INNER JOIN
        vw_message_group_agents ON agent_message.group_id = vw_message_group_agents.group_id;
