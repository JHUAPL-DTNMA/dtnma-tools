Copyright (c) ${years} The Johns Hopkins University Applied Physics
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


#  /usr/local/mysql/bin/mysql -uroot amp_core

#get the database password with default
read -p "Password [dbpass]: " pass


#execute sql commands with the user
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Tables/amp_core_create_database.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/ctrl_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/edd_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/mac_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/oper_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/rpt_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/tblt_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/sbr_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/tbr_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Views/tnvc_view.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_obj.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_var.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_ac.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_actual_parmspec.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_agents.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_const.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_ctrl.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_data_values.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_edd.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_expr.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_formal_parmspec.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_lit.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_macro.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_messages.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_namespace.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_operator.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_report.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_sbr.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_table.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_tbr.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Database_Scripts/Routines/amp_core_routines_tnvc.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Agent_Scripts/adm_bp_agent.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Agent_Scripts/adm_amp_agent.sql
 /usr/local/mysql/bin/mysql -uroot --password=$pass < mysql/Agent_Scripts/nm_example.sql