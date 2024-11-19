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

-- NOTE: These Views assumes that lookup table for state_id is static.  state_id can be alternately represented as an enum in compatible databases

CREATE OR REPLACE VIEW vw_ready_outgoing_message_groups AS SELECT * FROM message_group WHERE is_outgoing = TRUE AND state_id=1;
CREATE OR REPLACE VIEW vw_ready_incoming_message_groups AS SELECT * FROM message_group WHERE is_outgoing = FALSE AND state_id=1;
CREATE OR REPLACE VIEW vw_message_agents AS SELECT ma.message_id, ra.* FROM message_agents ma LEFT JOIN registered_agents ra ON ma.agent_id=ra.registered_agents_id;
CREATE OR REPLACE VIEW vw_message_group_agents AS SELECT ma.group_id, ra.* FROM message_group_agents ma LEFT JOIN registered_agents ra ON ma.agent_id=ra.registered_agents_id;
