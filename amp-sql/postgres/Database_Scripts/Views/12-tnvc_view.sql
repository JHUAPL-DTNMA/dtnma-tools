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

-- view for type name value collections

CREATE OR REPLACE VIEW vw_tnvc AS
SELECT type_name_value_entry.tnvc_id,
       tnv_id,
       order_num,
       data_type_id,
       data_name,
       use_desc
FROM type_name_value_entry,
     type_name_value_collection
WHERE type_name_value_collection.tnvc_id = type_name_value_entry.tnvc_id;


CREATE OR REPLACE VIEW vw_tnvc_entries AS
SELECT e.*,
       eint.entry_value AS int_value,
       euint.entry_value AS uint_value,
       eobj.obj_actual_definition_id AS obj_value,
       estr.entry_value as str_value,
       eac.ac_id AS ac_value,
       etnvc.tnvc_id AS tnvc_value,
       ereal32.entry_value as real32_value,
       ereal64.entry_value as real64_value,
       euvast.entry_value as uvast_value,
       evast.entry_value as vast_value
FROM type_name_value_entry e
LEFT JOIN type_name_value_uint_entry euint ON euint.tnv_id=e.tnv_id
LEFT JOIN type_name_value_int_entry eint ON eint.tnv_id=e.tnv_id
LEFT JOIN type_name_value_obj_entry eobj ON eobj.tnv_id=e.tnv_id
LEFT JOIN type_name_value_string_entry estr ON estr.tnv_id=e.tnv_id
LEFT JOIN type_name_value_ac_entry eac ON eac.tnv_id=e.tnv_id
LEFT JOIN type_name_value_tnvc_entry etnvc ON etnvc.tnv_id=e.tnv_id
LEFT JOIN type_name_value_real32_entry ereal32 ON ereal32.tnv_id=e.tnv_id
LEFT JOIN type_name_value_real64_entry ereal64 ON ereal64.tnv_id=e.tnv_id
LEFT JOIN type_name_value_uvast_entry euvast ON euvast.tnv_id=e.tnv_id
LEFT JOIN type_name_value_vast_entry evast ON evast.tnv_id=e.tnv_id
ORDER BY order_num ASC;

