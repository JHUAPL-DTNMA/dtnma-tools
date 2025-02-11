<!--
Copyright (c) 2025 The Johns Hopkins University Applied Physics
Laboratory LLC.

This file is part of the Delay-Tolerant Networking Management
Architecture (DTNMA) Tools package.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

-->
# refdb-sql
 JHUAPL    

             _________
            |^|     | |
            | |_____| |
            |  _____  |
            | |     | |
            | |_____| |
            |_|_____|_|

A MySQL/PostgreSQL database that can be used to support management of the Asynchronous Management Protocol.  

The MySQL/PostgreSQL database was created using as generic SQL as possible allowing for easy porting to the user preferred flavor of SQL. 

This is version 1.0 of the new AMP database designed at JHUAPL, using lessons learned from previous iterations of an AMP database. This database confirms to Asynchronous Management Protocol draft-birrane-dtn-amp-08 the most recent version of AMP as of release. 
   
See [Here](Documents/Notes/AMP_Manager_SQL_Interface.md). This document defines the database and explains design decisions.  

## Setup
An example docker compose configuration script is provided as a convenient option for getting started.  See the comments at the top of docker-compose.yml for details.

Alternatively, a setup script (setup.mysql) is provided to conveniently source all required SQL files from the command line.  This would be the recommended approach for instantiating production systems.  See the file comments for details.


## Files

Below shows the file tree of this repo.
```bash
    .
    ├── Documents               # Stores all the supporting documentation for the db
    │   ├── Entity_Relationship_Diagrams # generated database ER diagrams that show the different interactions of the database
    │   │                                # good place to see how the tables interact with each other and how the foreign key 
    │   │                                # relationships are setup. Also shows how the actual formal definition design is put 
    │   │                                # into effect.
    │   │
    │   └── Notes               # Notes on the database design. AMP_Manager_SQL_Interface.md is the main document that 
    │                           # describes the database. It will explain design decisions and describe the various tables with 
    │                           # some examples of how different amp objects will look like in the database. SP.txt is for the stored procedures
    │                           # definitions, detailing the inputs and outputs of each SP and. packaging.txt details packaging exampls of the databae  
    │
    |── README.md               # This Document 
    └── mysql/ or postgres/
        │  amp_core.sql             # a dump of the MySQL database. A good starting point to load the database. ß
        ├── Agent_scripts           # SQL scripts to populate the DB with agents. Used for testing completeness of the database
        │                           # good place to see examples of the storeprocedures being used and how to interact with the database.
        │
        └── Database_Scripts        # Stores the SQL files to generate the DB
             │
             ├── Routines            # Stores all SQL code that creates the stored procurers to interact with the database
             │
             ├── Tables              # The SQL scripts that create the database, delete the database, and also removes all entries   
             │                       # in the tables.
             │
             └── Views               # Contains the SQL code for generating table views of the AMP objects and collections
```
