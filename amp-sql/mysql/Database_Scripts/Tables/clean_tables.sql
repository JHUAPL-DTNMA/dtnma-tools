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

-- cleaning out the tables 

use amp_core;

DELETE FROM adm;
DELETE FROM amp_core.namespace;

DELETE FROM amp_core.edd_formal_definition 
WHERE
    TRUE;

DELETE FROM amp_core.adm 
WHERE
    TRUE;

DELETE FROM amp_core.ari_collection 
WHERE
    TRUE;

DELETE FROM amp_core.ari_collection_entry 
WHERE
    TRUE;

DELETE FROM amp_core.expression 
WHERE
    TRUE;

DELETE FROM amp_core.obj_formal_definition 
WHERE
    TRUE;

DELETE FROM amp_core.obj_metadata
WHERE
    TRUE;

DELETE FROM amp_core.obj_actual_definition 
WHERE
    TRUE;

DELETE FROM amp_core.operator_actual_definition 
WHERE
    TRUE;

DELETE FROM amp_core.variable_actual_definition 
WHERE
    TRUE;

DELETE FROM amp_core.control_formal_definition;

DELETE FROM amp_core.control_actual_definition;

DELETE FROM `amp_core`.`outgoing_message_set`
WHERE TRUE;

DELETE FROM `amp_core`.`incoming_message_set`
WHERE TRUE;

DELETE FROM `amp_core`.`formal_parmspec`
WHERE TRUE;

DELETE FROM amp_core.registered_agents
Where TRUE;







