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
USE amp_core;

-- STORED PROCEDURE(S) For creating namespaces and adms

-- ==================================================================
-- SP__insert_namespace
--    insert a new namespace into the db
-- Parameters:
-- in 
--      p_namespace_type varchar(255) - type of the namespace
-- 		p_issuing_org varchar(255) - name of the issuing organization for this ADM.
-- 		p_name_string varchar(255)- his is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_version varchar(255) -This is a string representation of the version of the ADM.
--           ADM version representations are formated at the discretion of
--           the publishing organization.
-- out 
-- 		r_namespace_id int(10) unsigned - id of the namespace in the database 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_namespace; 
DELIMITER //
CREATE PROCEDURE SP__insert_namespace(IN p_namespace_type varchar(255), p_issuing_org varchar(255), p_name_string 
varchar(255), p_version varchar(255), OUT r_namespace_id int(10) unsigned)
BEGIN

    SELECT namespace_id INTO r_namespace_id
           FROM amp_core.namespace
           WHERE namespace_type=p_namespace_type AND issuing_org=p_issuing_org AND name_string=p_name_string AND version_name=p_version;
    IF (r_namespace_id IS NULL) THEN
    	INSERT INTO amp_core.namespace(namespace_type, issuing_org, name_string, version_name) VALUES(p_namespace_type, p_issuing_org, p_name_string, p_version); 
        SET r_namespace_id = LAST_INSERT_ID();
    END IF;
    
END //
DELIMITER ;


-- ==================================================================
-- SP__insert_network_defined_namespace
--    insert a new network defined adm into the db
-- Parameters:
-- in 
-- 		p_issuing_org varchar(255) - name of the issuing organization for this ADM.
-- 		p_name_string varchar(255)- this is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_version varchar(255) -This is a string representation of the version of the ADM.
--           ADM version representations are formated at the discretion of
--           the publishing organization.
-- 		p_issuer_binary_string varchar(255) - any string that identifies the organization that is
-- 			  defining an AMM object
-- 	    p_tag varchar(255) - any string used to disambiguate AMM Objects for an Issuer
-- out 
-- 		r_namespace_id int(10) unsigned - id of the namespace in the database 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_network_defined_namespace; 
DELIMITER //
CREATE PROCEDURE SP__insert_network_defined_namespace(IN p_issuing_org varchar(255), p_name_string varchar(255), 
p_version varchar(255), p_issuer_binary_string varchar(255), p_tag varchar(255), OUT r_namespace_id int(10) unsigned)
BEGIN
	CALL SP__insert_namespace('NETWORK_CONFIG', p_issuing_org, p_name_string, p_version, r_namespace_id); 
    INSERT INTO amp_core.network_config(namespace_id, issuer_binary_string, tag) VALUES(r_namespace_id, p_issuer_binary_string, p_tag); 
END //
DELIMITER ;


-- ==================================================================
-- SP__insert_network_defined_namespace
--    insert a new moderated adm into the db
-- Parameters:
-- in 
-- 		p_issuing_org varchar(255) - name of the issuing organization for this ADM.
-- 		p_name_string varchar(255)- this is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_version varchar(255) - This is a string representation of the version of the ADM.
--           ADM version representations are formated at the discretion of
--           the publishing organization.
-- 		p_adm_name_string varchar(255) -  this is the human-readable name of the ADM that should appear
--           in message logs, user-interfaces, and other human-facing
--           applications
-- 		p_adm_enum int(10) unsigned - an unsigned integer in the range of 0 to
-- 			 (2^64)/20
-- 		p_adm_enum_label varchar(255) - labeled based on the number of bytes
-- 			of the Nickname as a function of the size of the ADM enumeration
-- 		p_use_desc varchar(255) - human readable use description
-- out
-- 		r_namespace_id int(10) unsigned - id of the namespace in the database 
-- ==================================================================
DROP PROCEDURE IF EXISTS SP__insert_adm_defined_namespace;
DELIMITER //
CREATE PROCEDURE SP__insert_adm_defined_namespace(IN p_issuing_org varchar(255), p_namespace_string varchar(255), 
p_version varchar(255), p_adm_name_string varchar(255), p_adm_enum int(10) unsigned, p_adm_enum_label varchar(255), 
p_use_desc varchar(255), OUT r_namespace_id int(10) unsigned) 
BEGIN
	CALL SP__insert_namespace('MODERATED', p_issuing_org, p_namespace_string, p_version, r_namespace_id);
    INSERT INTO amp_core.adm(namespace_id, adm_name, adm_enum, adm_enum_label, use_desc) VALUES(r_namespace_id, 
    p_adm_name_string, p_adm_enum, p_adm_enum_label, p_use_desc); 
END // 
DELIMITER ;


