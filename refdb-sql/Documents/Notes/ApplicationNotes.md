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

## Manager Logging
The table "DB_LOG_INFO" is provided for logging of debug messages normally output to stderr by the ION NM Manager application.  The types of messages generated are determined by the AMP_DEBUG_LVL that the application was configured with.

This table is not a required part of the AMP database, but is an example of additional implementation-specific information that may be logged with it.  Future implementations may choose to add other companion tables.

### Mgr_log
    - Id - Primary auto-increment key
    - Timestamp
    - Msg varchar
    - Level INT – Nominally matches the value of AMP_DEBUG_LVL
    - Source  – Function or other descriptive name for the source of the error.
    - File - Filename that this message originated from
    - Line - Source line that this message originated from

