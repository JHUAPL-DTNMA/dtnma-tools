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
This file documents the database representation for the encompassing message sets for incoming messages and outgoing controls.  These tables are modeled after the AMP Message Group data structure.

## Tables
###	Message_group
  - Group_id
  - Created_ts – Automatically set to CURRENT_TIMESTAMP on INSERT if not explicitly defined.
  - Modified_ts – Automatically set ON UPDATE.  For outgoing messages, this will ultimately be the sent time once the state transitions. 
  - received_ts - If NULL, this is an outgoing message.  Otherwise this is the AMP timestamp value of the received message group. 
  - State_id – Equivalent to existing, but a combined ENUM [or a lookup to a new message_states table with our stored procedures accepting the string name to simplify usage)
    - Initializing – Default value, but generally used solely by UI when creating new outgoing messages.  Application will insert incoming sets directly with Ready state, encompassing it’s activities in a transaction to prevent the record from being accessible until it’s finished.
    - Ready
      - For incoming sets, this means the message has been received and is ready for processing
      - For outgoing sets, this means it’s ready for the manager to retrieve and sent it.
    - Sent – For outgoing sets only, this indicates that the manager has parsed and sent this message group
    - Error – Used for incoming sets where the manager parsed it with errors, or for outgoing sets where the manager tried and failed to send the - sage
    - Aborted – Used for outgoing sets that have been cancelled from being sent by user command.
    - NOTE: If end-users or application need to mark message groups with other states, it shall be done in a new table.  More likely, users will want to comment on or categorize individual messages [same note applies], and not their containing group.  In either case, we want to enforce a separation between data state and future analysis information.

### Message_group_agents
    - Group_id – KEY, but not UNIQUE
    - Agent_id – The Agent this message was received from (incoming) or is destined for (outgoing)
    - Note
      - This field/table is normalized as this can be a many-to-one relationship for outgoing groups, but is always one-to-one for incoming
      - This is an informational/administrative field and does not correspond to any AMP data field.

### Message_group_entry
    - Group_id
    - Message_id
    - ACK – Bool
    - NAK – BOOL
    - ACL - Bool
    - Order_num
      - Note: As with other instances of order_num, this nominally represents the order of entries in the sent message.  In practice, this is used in an ‘ORDER BY’ clause on queries, and end-applications are otherwise indifferent to the actual value.  As an example, for outgoing messages, if order_num is equal for multiple entries, the manager will insert them in the order returned by the database. This is nominally the order of insertion, but is not guaranteed.
  - Type – Enum field.  Register Agent, Message Set, Table set, or Perform Control
    - This corresponds to the “op code” in the ADM specification
    - Technically a message group may contain any combination of these 4 types. In practice, it is likely to only contain one, and perform control should be the only valid value for outgoing sets.  We will consider this a convention, re-enforced by related table views, but won’t make it a DB requirement in order to keep things simple.
  - NOTE:
    - ACK, NAK, and ACL correspond to the related fields in the specification

### Message_perform_control
    - Message_id  (PRIMARY/UNIQUE key)
    - TV – INT UNSIGNED time value
      - This is the actual AMP Time value.
      - We can’t represent this as a timestamp, as that would not permit us to represent both relative and absolute times.
  - Ac_id
  - NOTE: This replaces outgoing_message_entry

### Message_agents
    - message_id – KEY, but not UNIQUE
    - Agent_id
    - Note:
      - For register agent, there will always be exactly one entry in this table (and that is the sole entry for this type)
      - For Perform Control, there may be one or more agents listed as destinations for this set.  This should nominally be the same as message_group_agents, but it is potentially valid for a user to decide for the actual send list and published send list to differ.
      - For Message and Table Sets, this is the list of RX_Names that this set was sent to. 

### Message_table_set_entry
    - Message_id
    - ? todo

### Message_report_set_entry
This table represents an entry in a report set. The database representation only defines report sets within the context of an encapsulating message group entry.

- Message_id
- Report_id
- order_num


### Report_definition
    - Report_id
    - Ari_id – ARI defining this report definition.  This is a FK to obj_actual_definition_id.
    - ts – Optional timestamp
    - Tnvc_id – Reference to the TNVC that defines the report contents

