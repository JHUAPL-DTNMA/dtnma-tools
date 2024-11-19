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

-- MySQL dump 10.13  Distrib 8.0.19, for macos10.15 (x86_64)
--
-- Host: localhost    Database: amp_core
-- ------------------------------------------------------
-- Server version	8.0.19

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `amp_core`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `amp_core` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

USE `amp_core`;

--
-- Table structure for table `actual_parm`
--

DROP TABLE IF EXISTS `actual_parm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actual_parm` (
  `ap_id` int unsigned NOT NULL AUTO_INCREMENT,
  `ap_spec_id` int unsigned NOT NULL,
  `order_num` int unsigned NOT NULL,
  PRIMARY KEY (`ap_id`),
  KEY `actual_parm_fk1` (`ap_spec_id`),
  CONSTRAINT `actual_parm_fk1` FOREIGN KEY (`ap_spec_id`) REFERENCES `actual_parmspec` (`ap_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=441 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actual_parm`
--

LOCK TABLES `actual_parm` WRITE;
/*!40000 ALTER TABLE `actual_parm` DISABLE KEYS */;
/*!40000 ALTER TABLE `actual_parm` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `actual_parm_ac`
--

DROP TABLE IF EXISTS `actual_parm_ac`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actual_parm_ac` (
  `ap_id` int unsigned NOT NULL AUTO_INCREMENT,
  `ac_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`ap_id`),
  KEY `actual_parm_object_ibfk_30_idx` (`ac_id`),
  CONSTRAINT `actual_parm_object_ibfk_10` FOREIGN KEY (`ap_id`) REFERENCES `actual_parm` (`ap_id`) ON DELETE CASCADE,
  CONSTRAINT `actual_parm_object_ibfk_30` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=440 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actual_parm_ac`
--

LOCK TABLES `actual_parm_ac` WRITE;
/*!40000 ALTER TABLE `actual_parm_ac` DISABLE KEYS */;
/*!40000 ALTER TABLE `actual_parm_ac` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `actual_parm_name`
--

DROP TABLE IF EXISTS `actual_parm_name`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actual_parm_name` (
  `ap_id` int unsigned NOT NULL AUTO_INCREMENT,
  `data_type_id` int unsigned NOT NULL,
  `fp_id` int unsigned NOT NULL,
  PRIMARY KEY (`ap_id`),
  KEY `actual_parm_name_ibfk_2` (`data_type_id`),
  KEY `actual_parm_name_ibfk_3` (`fp_id`),
  CONSTRAINT `actual_parm_name_ibfh_1` FOREIGN KEY (`ap_id`) REFERENCES `actual_parm` (`ap_id`) ON DELETE CASCADE,
  CONSTRAINT `actual_parm_name_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`),
  CONSTRAINT `actual_parm_name_ibfk_3` FOREIGN KEY (`fp_id`) REFERENCES `formal_parm` (`fp_id`)
) ENGINE=InnoDB AUTO_INCREMENT=439 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actual_parm_name`
--

LOCK TABLES `actual_parm_name` WRITE;
/*!40000 ALTER TABLE `actual_parm_name` DISABLE KEYS */;
/*!40000 ALTER TABLE `actual_parm_name` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `actual_parm_object`
--

DROP TABLE IF EXISTS `actual_parm_object`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actual_parm_object` (
  `ap_id` int unsigned NOT NULL AUTO_INCREMENT,
  `data_type_id` int unsigned NOT NULL,
  `obj_actual_definition_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`ap_id`),
  KEY `actual_parm_object_ibfk_2` (`data_type_id`),
  KEY `actual_parm_object_ibfk_3` (`obj_actual_definition_id`),
  CONSTRAINT `actual_parm_object_ibfk_1` FOREIGN KEY (`ap_id`) REFERENCES `actual_parm` (`ap_id`) ON DELETE CASCADE,
  CONSTRAINT `actual_parm_object_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`),
  CONSTRAINT `actual_parm_object_ibfk_3` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=436 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actual_parm_object`
--

LOCK TABLES `actual_parm_object` WRITE;
/*!40000 ALTER TABLE `actual_parm_object` DISABLE KEYS */;
/*!40000 ALTER TABLE `actual_parm_object` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `actual_parm_tnvc`
--

DROP TABLE IF EXISTS `actual_parm_tnvc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actual_parm_tnvc` (
  `ap_id` int unsigned NOT NULL AUTO_INCREMENT,
  `tnvc_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`ap_id`),
  KEY `actual_parm_object_ibfk_300_idx` (`tnvc_id`),
  CONSTRAINT `actual_parm_object_ibfk_100` FOREIGN KEY (`ap_id`) REFERENCES `actual_parm` (`ap_id`) ON DELETE CASCADE,
  CONSTRAINT `actual_parm_object_ibfk_300` FOREIGN KEY (`tnvc_id`) REFERENCES `type_name_value_collection` (`tnvc_id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=441 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actual_parm_tnvc`
--

LOCK TABLES `actual_parm_tnvc` WRITE;
/*!40000 ALTER TABLE `actual_parm_tnvc` DISABLE KEYS */;
/*!40000 ALTER TABLE `actual_parm_tnvc` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `actual_parmspec`
--

DROP TABLE IF EXISTS `actual_parmspec`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actual_parmspec` (
  `ap_spec_id` int unsigned NOT NULL AUTO_INCREMENT,
  `fp_spec_id` int unsigned NOT NULL,
  `num_parm` int unsigned NOT NULL DEFAULT '0',
  `use_desc` varchar DEFAULT NULL,
  PRIMARY KEY (`ap_spec_id`),
  KEY `actual_parmspec_ibfk_1` (`fp_spec_id`),
  CONSTRAINT `actual_parmspec_ibfk_1` FOREIGN KEY (`fp_spec_id`) REFERENCES `formal_parmspec` (`fp_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=428 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actual_parmspec`
--

LOCK TABLES `actual_parmspec` WRITE;
/*!40000 ALTER TABLE `actual_parmspec` DISABLE KEYS */;
/*!40000 ALTER TABLE `actual_parmspec` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `adm`
--

DROP TABLE IF EXISTS `adm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `adm` (
  `namespace_id` int unsigned NOT NULL,
  `adm_name` varchar DEFAULT NULL,
  `adm_enum` int unsigned DEFAULT NULL,
  `adm_enum_label` varchar DEFAULT NULL,
  `use_desc` varchar DEFAULT NULL,
  PRIMARY KEY (`namespace_id`),
  UNIQUE KEY `adm_enum` (`adm_enum`),
  CONSTRAINT `adm_ibfk_1` FOREIGN KEY (`namespace_id`) REFERENCES `namespace` (`namespace_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `adm`
--

LOCK TABLES `adm` WRITE;
/*!40000 ALTER TABLE `adm` DISABLE KEYS */;
/*!40000 ALTER TABLE `adm` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ADM_Type_Enumeration`
--

DROP TABLE IF EXISTS `ADM_Type_Enumeration`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ADM_Type_Enumeration` (
  `data_type_id` int unsigned NOT NULL,
  `Enumeration` int unsigned NOT NULL,
  PRIMARY KEY (`data_type_id`),
  CONSTRAINT `fk1` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ADM_Type_Enumeration`
--

LOCK TABLES `ADM_Type_Enumeration` WRITE;
/*!40000 ALTER TABLE `ADM_Type_Enumeration` DISABLE KEYS */;
INSERT INTO `ADM_Type_Enumeration` VALUES (0,0),(1,1),(2,2),(4,3),(5,4),(7,5),(8,6),(10,7),(11,8),(12,9);
/*!40000 ALTER TABLE `ADM_Type_Enumeration` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ari_collection`
--

DROP TABLE IF EXISTS `ari_collection`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ari_collection` (
  `ac_id` int unsigned NOT NULL AUTO_INCREMENT,
  `num_entries` int unsigned NOT NULL DEFAULT '0',
  `use_desc` varchar DEFAULT NULL,
  PRIMARY KEY (`ac_id`)
) ENGINE=InnoDB AUTO_INCREMENT=132 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ari_collection`
--

LOCK TABLES `ari_collection` WRITE;
/*!40000 ALTER TABLE `ari_collection` DISABLE KEYS */;
/*!40000 ALTER TABLE `ari_collection` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ari_collection_actual_entry`
--

DROP TABLE IF EXISTS `ari_collection_actual_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ari_collection_actual_entry` (
  `ac_entry_id` int unsigned NOT NULL AUTO_INCREMENT,
  `obj_actual_definition_id` int unsigned NOT NULL,
  PRIMARY KEY (`ac_entry_id`),
  KEY `obj_actual_definition_id` (`obj_actual_definition_id`),
  CONSTRAINT `ari_collection_actual_entry_ibfk_1` FOREIGN KEY (`ac_entry_id`) REFERENCES `ari_collection_entry` (`ac_entry_id`) ON DELETE CASCADE,
  CONSTRAINT `ari_collection_actual_entry_ibfk_2` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=446 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ari_collection_actual_entry`
--

LOCK TABLES `ari_collection_actual_entry` WRITE;
/*!40000 ALTER TABLE `ari_collection_actual_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `ari_collection_actual_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ari_collection_entry`
--

DROP TABLE IF EXISTS `ari_collection_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ari_collection_entry` (
  `ac_entry_id` int unsigned NOT NULL AUTO_INCREMENT,
  `ac_id` int unsigned NOT NULL,
  `order_num` int unsigned NOT NULL,
  PRIMARY KEY (`ac_entry_id`),
  UNIQUE KEY `ac_id` (`ac_id`,`order_num`),
  CONSTRAINT `ari_collection_entry_ibfk_1` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=448 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ari_collection_entry`
--

LOCK TABLES `ari_collection_entry` WRITE;
/*!40000 ALTER TABLE `ari_collection_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `ari_collection_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ari_collection_formal_entry`
--

DROP TABLE IF EXISTS `ari_collection_formal_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ari_collection_formal_entry` (
  `ac_entry_id` int unsigned NOT NULL,
  `obj_formal_definition_id` int unsigned NOT NULL,
  PRIMARY KEY (`ac_entry_id`),
  KEY `obj_formal_definition_id` (`obj_formal_definition_id`),
  CONSTRAINT `ari_collection_formal_entry_ibfk_1` FOREIGN KEY (`ac_entry_id`) REFERENCES `ari_collection_entry` (`ac_entry_id`) ON DELETE CASCADE,
  CONSTRAINT `ari_collection_formal_entry_ibfk_2` FOREIGN KEY (`obj_formal_definition_id`) REFERENCES `obj_formal_definition` (`obj_formal_definition_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ari_collection_formal_entry`
--

LOCK TABLES `ari_collection_formal_entry` WRITE;
/*!40000 ALTER TABLE `ari_collection_formal_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `ari_collection_formal_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `const_actual_definition`
--

DROP TABLE IF EXISTS `const_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `const_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `data_type_id` int unsigned NOT NULL,
  `data_value` varchar NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `data_type_id` (`data_type_id`,`data_value`),
  CONSTRAINT `const_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `const_actual_definition_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `const_actual_definition_chk_1` CHECK (((`data_type_id` > 15) and (`data_type_id` < 40)))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `const_actual_definition`
--

LOCK TABLES `const_actual_definition` WRITE;
/*!40000 ALTER TABLE `const_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `const_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `control_actual_definition`
--

DROP TABLE IF EXISTS `control_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `control_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `ap_spec_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `ap_spec_id` (`ap_spec_id`),
  CONSTRAINT `control_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `control_actual_definition_ibfk_2` FOREIGN KEY (`ap_spec_id`) REFERENCES `actual_parmspec` (`ap_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `control_actual_definition`
--

LOCK TABLES `control_actual_definition` WRITE;
/*!40000 ALTER TABLE `control_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `control_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `control_formal_definition`
--

DROP TABLE IF EXISTS `control_formal_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `control_formal_definition` (
  `obj_formal_definition_id` int unsigned NOT NULL,
  `fp_spec_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`obj_formal_definition_id`),
  UNIQUE KEY `fp_spec_id` (`fp_spec_id`),
  CONSTRAINT `control_formal_definition_ibfk_1` FOREIGN KEY (`obj_formal_definition_id`) REFERENCES `obj_formal_definition` (`obj_formal_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `control_formal_definition_ibfk_2` FOREIGN KEY (`fp_spec_id`) REFERENCES `formal_parmspec` (`fp_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `control_formal_definition`
--

LOCK TABLES `control_formal_definition` WRITE;
/*!40000 ALTER TABLE `control_formal_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `control_formal_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `data_type`
--

DROP TABLE IF EXISTS `data_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_type` (
  `data_type_id` int unsigned NOT NULL AUTO_INCREMENT,
  `type_name` varchar(50) NOT NULL,
  `use_desc` varchar NOT NULL,
  PRIMARY KEY (`data_type_id`),
  UNIQUE KEY `type_name` (`type_name`)
) ENGINE=InnoDB AUTO_INCREMENT=44 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `data_type`
--

LOCK TABLES `data_type` WRITE;
/*!40000 ALTER TABLE `data_type` DISABLE KEYS */;
INSERT INTO `data_type` VALUES (0,'CONST','Constant'),(1,'CTRL','Control'),(2,'EDD','Externally Defined Data'),(3,'LIT','Literal'),(4,'MAC','Macro'),(5,'OPER','Operator'),(6,'RPT','Report'),(7,'RPTT','Report Template'),(8,'SBR','State Based Rules'),(9,'TBL','Table'),(10,'TBLT','Table Template'),(11,'TBR','Time Based Rules'),(12,'VAR','Variable'),(13,'MDAT','Metadata'),(16,'BOOL','Boolean'),(17,'BYTE','8 bits, Standard Byte'),(18,'STR','Character String'),(19,'INT','Signed 32 bit Integer'),(20,'UINT','Unsigned 32 bit Integer'),(21,'VAST','Signed 64 bit Integer'),(22,'UVAST','Unsigned 64 bit Integer'),(23,'REAL32','Single precision Floating Point'),(24,'REAL64','Double precision Floating Point'),(32,'TV','Time Value'),(33,'TS','Timestamp'),(34,'TNV','Type-Name-Value'),(35,'TNVC','Type-Name-Value Collection'),(36,'ARI','AMM Resource Identifier'),(37,'AC','ARI Collection'),(38,'EXPR','Expression'),(39,'BYTESTR','Bytestring'),(40,'PARMNAME','parameter name'),(41,'FP','formal parameter'),(42,'APV','actual parameter-by-value'),(43,'APN','actual parameter-by-name');
/*!40000 ALTER TABLE `data_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `edd_actual_definition`
--

DROP TABLE IF EXISTS `edd_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `edd_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `ap_spec_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `ap_spec_id` (`ap_spec_id`),
  CONSTRAINT `edd_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `edd_actual_definition_ibfk_2` FOREIGN KEY (`ap_spec_id`) REFERENCES `actual_parmspec` (`ap_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `edd_actual_definition`
--

LOCK TABLES `edd_actual_definition` WRITE;
/*!40000 ALTER TABLE `edd_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `edd_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `edd_formal_definition`
--

DROP TABLE IF EXISTS `edd_formal_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `edd_formal_definition` (
  `obj_formal_definition_id` int unsigned NOT NULL,
  `fp_spec_id` int unsigned DEFAULT NULL,
  `data_type_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_formal_definition_id`),
  UNIQUE KEY `fp_spec_id` (`fp_spec_id`),
  KEY `data_type_id` (`data_type_id`),
  CONSTRAINT `edd_formal_definition_ibfk_1` FOREIGN KEY (`obj_formal_definition_id`) REFERENCES `obj_formal_definition` (`obj_formal_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `edd_formal_definition_ibfk_2` FOREIGN KEY (`fp_spec_id`) REFERENCES `formal_parmspec` (`fp_spec_id`) ON DELETE CASCADE,
  CONSTRAINT `edd_formal_definition_ibfk_3` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `edd_formal_definition`
--

LOCK TABLES `edd_formal_definition` WRITE;
/*!40000 ALTER TABLE `edd_formal_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `edd_formal_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `expression`
--

DROP TABLE IF EXISTS `expression`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `expression` (
  `expression_id` int unsigned NOT NULL AUTO_INCREMENT,
  `data_type_id` int unsigned NOT NULL,
  `ac_id` int unsigned NOT NULL,
  PRIMARY KEY (`expression_id`),
  UNIQUE KEY `data_type_id` (`data_type_id`,`ac_id`),
  KEY `ac_id` (`ac_id`),
  CONSTRAINT `expression_ibfk_1` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `expression_ibfk_2` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=29 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `expression`
--

LOCK TABLES `expression` WRITE;
/*!40000 ALTER TABLE `expression` DISABLE KEYS */;
/*!40000 ALTER TABLE `expression` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `formal_parm`
--

DROP TABLE IF EXISTS `formal_parm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `formal_parm` (
  `fp_id` int unsigned NOT NULL AUTO_INCREMENT,
  `fp_spec_id` int unsigned NOT NULL,
  `order_num` int unsigned NOT NULL,
  `parm_name` varchar DEFAULT NULL,
  `data_type_id` int unsigned NOT NULL,
  `obj_actual_definition_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`fp_id`),
  UNIQUE KEY `fp_id` (`fp_id`,`order_num`),
  KEY `fp_spec_id` (`fp_spec_id`),
  KEY `data_type_id` (`data_type_id`),
  KEY `obj_actual_definition_id` (`obj_actual_definition_id`),
  CONSTRAINT `formal_parm_ibfk_1` FOREIGN KEY (`fp_spec_id`) REFERENCES `formal_parmspec` (`fp_spec_id`) ON DELETE CASCADE,
  CONSTRAINT `formal_parm_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `formal_parm_ibfk_3` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=935 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `formal_parm`
--

LOCK TABLES `formal_parm` WRITE;
/*!40000 ALTER TABLE `formal_parm` DISABLE KEYS */;
/*!40000 ALTER TABLE `formal_parm` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `formal_parmspec`
--

DROP TABLE IF EXISTS `formal_parmspec`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `formal_parmspec` (
  `fp_spec_id` int unsigned NOT NULL AUTO_INCREMENT,
  `num_parms` int unsigned NOT NULL,
  `use_desc` varchar DEFAULT NULL,
  PRIMARY KEY (`fp_spec_id`)
) ENGINE=InnoDB AUTO_INCREMENT=505 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `formal_parmspec`
--

LOCK TABLES `formal_parmspec` WRITE;
/*!40000 ALTER TABLE `formal_parmspec` DISABLE KEYS */;
/*!40000 ALTER TABLE `formal_parmspec` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `incoming_message_entry`
--

DROP TABLE IF EXISTS `incoming_message_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `incoming_message_entry` (
  `message_id` int unsigned NOT NULL AUTO_INCREMENT,
  `set_id` int unsigned NOT NULL,
  `message_order` int unsigned NOT NULL,
  `ac_id` int unsigned NOT NULL,
  PRIMARY KEY (`message_id`),
  KEY `ac_id` (`ac_id`),
  KEY `set_id` (`set_id`),
  CONSTRAINT `incoming_message_entry_ibfk_1` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE,
  CONSTRAINT `incoming_message_entry_ibfk_2` FOREIGN KEY (`set_id`) REFERENCES `incoming_message_set` (`set_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `incoming_message_entry`
--

LOCK TABLES `incoming_message_entry` WRITE;
/*!40000 ALTER TABLE `incoming_message_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `incoming_message_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `incoming_message_set`
--

DROP TABLE IF EXISTS `incoming_message_set`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `incoming_message_set` (
  `set_id` int unsigned NOT NULL AUTO_INCREMENT,
  `received_ts` datetime DEFAULT NULL,
  `generated_ts` datetime DEFAULT NULL,
  `state` tinyint unsigned NOT NULL,
  `agent_id` int unsigned NOT NULL,
  PRIMARY KEY (`set_id`),
  KEY `state` (`state`),
  KEY `agent_id` (`agent_id`),
  CONSTRAINT `incoming_message_set_ibfk_1` FOREIGN KEY (`state`) REFERENCES `incoming_state` (`state_id`) ON DELETE CASCADE,
  CONSTRAINT `incoming_message_set_ibfk_2` FOREIGN KEY (`agent_id`) REFERENCES `registered_agents` (`registered_agents_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `incoming_message_set`
--

LOCK TABLES `incoming_message_set` WRITE;
/*!40000 ALTER TABLE `incoming_message_set` DISABLE KEYS */;
/*!40000 ALTER TABLE `incoming_message_set` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `incoming_state`
--

DROP TABLE IF EXISTS `incoming_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `incoming_state` (
  `state_id` tinyint unsigned NOT NULL AUTO_INCREMENT,
  `state_name` varchar(50) NOT NULL,
  `use_desc` varchar NOT NULL,
  PRIMARY KEY (`state_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `incoming_state`
--

LOCK TABLES `incoming_state` WRITE;
/*!40000 ALTER TABLE `incoming_state` DISABLE KEYS */;
INSERT INTO `incoming_state` VALUES (3,'Initializing','Manager is recieving reports'),(4,'Ready','Manager has completed reception'),(5,'Processed','Application is done processing reports');
/*!40000 ALTER TABLE `incoming_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `literal_actual_definition`
--

DROP TABLE IF EXISTS `literal_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `literal_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `data_type_id` int unsigned NOT NULL,
  `data_value` varchar NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `data_value` (`data_value`),
  KEY `data_type_id` (`data_type_id`),
  CONSTRAINT `literal_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `literal_actual_definition_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `literal_actual_definition_chk_1` CHECK ((`data_type_id` > 16))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `literal_actual_definition`
--

LOCK TABLES `literal_actual_definition` WRITE;
/*!40000 ALTER TABLE `literal_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `literal_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `macro_actual_definition`
--

DROP TABLE IF EXISTS `macro_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `macro_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `ap_spec_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `ap_spec_id` (`ap_spec_id`),
  CONSTRAINT `macro_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `macro_actual_definition_ibfk_2` FOREIGN KEY (`ap_spec_id`) REFERENCES `actual_parmspec` (`ap_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `macro_actual_definition`
--

LOCK TABLES `macro_actual_definition` WRITE;
/*!40000 ALTER TABLE `macro_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `macro_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `macro_formal_definition`
--

DROP TABLE IF EXISTS `macro_formal_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `macro_formal_definition` (
  `obj_formal_definition_id` int unsigned NOT NULL,
  `fp_spec_id` int unsigned DEFAULT NULL,
  `ac_id` int unsigned NOT NULL,
  `max_call_depth` int unsigned DEFAULT '4',
  PRIMARY KEY (`obj_formal_definition_id`),
  UNIQUE KEY `fp_spec_id` (`fp_spec_id`),
  KEY `ac_id` (`ac_id`),
  CONSTRAINT `macro_formal_definition_ibfk_1` FOREIGN KEY (`obj_formal_definition_id`) REFERENCES `obj_formal_definition` (`obj_formal_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `macro_formal_definition_ibfk_2` FOREIGN KEY (`fp_spec_id`) REFERENCES `formal_parmspec` (`fp_spec_id`) ON DELETE CASCADE,
  CONSTRAINT `macro_formal_definition_ibfk_3` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `macro_formal_definition`
--

LOCK TABLES `macro_formal_definition` WRITE;
/*!40000 ALTER TABLE `macro_formal_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `macro_formal_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `namespace`
--

DROP TABLE IF EXISTS `namespace`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `namespace` (
  `namespace_id` int unsigned NOT NULL AUTO_INCREMENT,
  `namespace_type` varchar NOT NULL,
  `issuing_org` varchar NOT NULL,
  `name_string` varchar NOT NULL,
  `version_name` varchar NOT NULL DEFAULT '0.0.0',
  PRIMARY KEY (`namespace_id`),
  UNIQUE KEY `namespace_type` (`namespace_type`,`issuing_org`),
  UNIQUE KEY `issuing_org` (`issuing_org`,`name_string`,`version_name`)
) ENGINE=InnoDB AUTO_INCREMENT=66 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `namespace`
--

LOCK TABLES `namespace` WRITE;
/*!40000 ALTER TABLE `namespace` DISABLE KEYS */;
/*!40000 ALTER TABLE `namespace` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `network_config`
--

DROP TABLE IF EXISTS `network_config`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `network_config` (
  `namespace_id` int unsigned NOT NULL,
  `issuer_binary_string` varchar NOT NULL,
  `tag` varchar DEFAULT NULL,
  PRIMARY KEY (`namespace_id`),
  CONSTRAINT `network_config_ibfk_1` FOREIGN KEY (`namespace_id`) REFERENCES `namespace` (`namespace_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `network_config`
--

LOCK TABLES `network_config` WRITE;
/*!40000 ALTER TABLE `network_config` DISABLE KEYS */;
/*!40000 ALTER TABLE `network_config` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `obj_actual_definition`
--

DROP TABLE IF EXISTS `obj_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `obj_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL AUTO_INCREMENT,
  `obj_metadata_id` int unsigned NOT NULL,
  `use_desc` varchar DEFAULT ' ',
  PRIMARY KEY (`obj_actual_definition_id`),
  KEY `obj_metadata_id` (`obj_metadata_id`),
  CONSTRAINT `obj_actual_definition_ibfk_1` FOREIGN KEY (`obj_metadata_id`) REFERENCES `obj_metadata` (`obj_metadata_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3265 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `obj_actual_definition`
--

LOCK TABLES `obj_actual_definition` WRITE;
/*!40000 ALTER TABLE `obj_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `obj_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `obj_formal_definition`
--

DROP TABLE IF EXISTS `obj_formal_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `obj_formal_definition` (
  `obj_formal_definition_id` int unsigned NOT NULL AUTO_INCREMENT,
  `obj_metadata_id` int unsigned NOT NULL,
  `use_desc` varchar DEFAULT NULL,
  PRIMARY KEY (`obj_formal_definition_id`),
  KEY `obj_metadata_id` (`obj_metadata_id`),
  CONSTRAINT `obj_formal_definition_ibfk_1` FOREIGN KEY (`obj_metadata_id`) REFERENCES `obj_metadata` (`obj_metadata_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1592 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `obj_formal_definition`
--

LOCK TABLES `obj_formal_definition` WRITE;
/*!40000 ALTER TABLE `obj_formal_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `obj_formal_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `obj_metadata`
--

DROP TABLE IF EXISTS `obj_metadata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `obj_metadata` (
  `obj_metadata_id` int unsigned NOT NULL AUTO_INCREMENT,
  `data_type_id` int unsigned NOT NULL,
  `obj_name` varchar NOT NULL,
  `namespace_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`obj_metadata_id`),
  UNIQUE KEY `type_name` (`data_type_id`,`obj_name`,`namespace_id`),
  KEY `namespace_id` (`namespace_id`),
  CONSTRAINT `obj_metadata_ibfk_1` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `obj_metadata_ibfk_2` FOREIGN KEY (`namespace_id`) REFERENCES `namespace` (`namespace_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3707 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `obj_metadata`
--

LOCK TABLES `obj_metadata` WRITE;
/*!40000 ALTER TABLE `obj_metadata` DISABLE KEYS */;
/*!40000 ALTER TABLE `obj_metadata` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `operator_actual_definition`
--

DROP TABLE IF EXISTS `operator_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `operator_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `data_type_id` int unsigned NOT NULL,
  `num_operands` int unsigned NOT NULL,
  `tnvc_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  KEY `data_type_id` (`data_type_id`),
  KEY `tnvc_id` (`tnvc_id`),
  CONSTRAINT `operator_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `operator_actual_definition_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `operator_actual_definition_ibfk_3` FOREIGN KEY (`tnvc_id`) REFERENCES `type_name_value_collection` (`tnvc_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `operator_actual_definition`
--

LOCK TABLES `operator_actual_definition` WRITE;
/*!40000 ALTER TABLE `operator_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `operator_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `outgoing_message_entry`
--

DROP TABLE IF EXISTS `outgoing_message_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `outgoing_message_entry` (
  `message_id` int unsigned NOT NULL AUTO_INCREMENT,
  `set_id` int unsigned NOT NULL,
  `message_order` int unsigned NOT NULL,
  `start_ts` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `ac_id` int unsigned NOT NULL,
  PRIMARY KEY (`message_id`),
  KEY `ac_id` (`ac_id`),
  KEY `set_id` (`set_id`),
  CONSTRAINT `outgoing_message_entry_ibfk_1` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE,
  CONSTRAINT `outgoing_message_entry_ibfk_2` FOREIGN KEY (`set_id`) REFERENCES `outgoing_message_set` (`set_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `outgoing_message_entry`
--

LOCK TABLES `outgoing_message_entry` WRITE;
/*!40000 ALTER TABLE `outgoing_message_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `outgoing_message_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `outgoing_message_set`
--

DROP TABLE IF EXISTS `outgoing_message_set`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `outgoing_message_set` (
  `set_id` int unsigned NOT NULL AUTO_INCREMENT,
  `created_ts` datetime DEFAULT NULL,
  `modified_ts` datetime DEFAULT NULL,
  `state` tinyint NOT NULL,
  `agent_id` int unsigned NOT NULL,
  PRIMARY KEY (`set_id`),
  KEY `state` (`state`),
  KEY `agent_id` (`agent_id`),
  CONSTRAINT `outgoing_message_set_ibfk_1` FOREIGN KEY (`state`) REFERENCES `outgoing_state` (`state_id`) ON DELETE CASCADE,
  CONSTRAINT `outgoing_message_set_ibfk_2` FOREIGN KEY (`agent_id`) REFERENCES `registered_agents` (`registered_agents_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `outgoing_message_set`
--

LOCK TABLES `outgoing_message_set` WRITE;
/*!40000 ALTER TABLE `outgoing_message_set` DISABLE KEYS */;
/*!40000 ALTER TABLE `outgoing_message_set` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `outgoing_state`
--

DROP TABLE IF EXISTS `outgoing_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `outgoing_state` (
  `state_id` tinyint NOT NULL AUTO_INCREMENT,
  `state_name` varchar(50) NOT NULL,
  `use_desc` varchar NOT NULL,
  PRIMARY KEY (`state_id`),
  UNIQUE KEY `state_name` (`state_name`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `outgoing_state`
--

LOCK TABLES `outgoing_state` WRITE;
/*!40000 ALTER TABLE `outgoing_state` DISABLE KEYS */;
INSERT INTO `outgoing_state` VALUES (7,'Initializing','Application writing controls'),(8,'Ready','Ready for Sending to Agent'),(9,'Sent','Manager send completed'),(10,'Aborted',' Used if a message group is cancelled/aborted without being sent');
/*!40000 ALTER TABLE `outgoing_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `registered_agents`
--

DROP TABLE IF EXISTS `registered_agents`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `registered_agents` (
  `registered_agents_id` int unsigned NOT NULL AUTO_INCREMENT,
  `agent_id_string` varchar(128) NOT NULL DEFAULT 'ipn:0.0',
  `first_registered` datetime NOT NULL,
  `last_registered` datetime NOT NULL,
  PRIMARY KEY (`registered_agents_id`),
  UNIQUE KEY `agent_id_string_UNIQUE` (`agent_id_string`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `registered_agents`
--

LOCK TABLES `registered_agents` WRITE;
/*!40000 ALTER TABLE `registered_agents` DISABLE KEYS */;
/*!40000 ALTER TABLE `registered_agents` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `report_template_actual_definition`
--

DROP TABLE IF EXISTS `report_template_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `report_template_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `ap_spec_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `ap_spec_id` (`ap_spec_id`),
  CONSTRAINT `report_template_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `report_template_actual_definition_ibfk_2` FOREIGN KEY (`ap_spec_id`) REFERENCES `actual_parmspec` (`ap_spec_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `report_template_actual_definition`
--

LOCK TABLES `report_template_actual_definition` WRITE;
/*!40000 ALTER TABLE `report_template_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `report_template_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `report_template_formal_definition`
--

DROP TABLE IF EXISTS `report_template_formal_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `report_template_formal_definition` (
  `obj_formal_definition_id` int unsigned NOT NULL,
  `fp_spec_id` int unsigned DEFAULT NULL,
  `ac_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_formal_definition_id`),
  UNIQUE KEY `fp_spec_id` (`fp_spec_id`),
  KEY `ac_id` (`ac_id`),
  CONSTRAINT `report_template_formal_definition_ibfk_1` FOREIGN KEY (`obj_formal_definition_id`) REFERENCES `obj_formal_definition` (`obj_formal_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `report_template_formal_definition_ibfk_2` FOREIGN KEY (`fp_spec_id`) REFERENCES `formal_parmspec` (`fp_spec_id`) ON DELETE CASCADE,
  CONSTRAINT `report_template_formal_definition_ibfk_3` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `report_template_formal_definition`
--

LOCK TABLES `report_template_formal_definition` WRITE;
/*!40000 ALTER TABLE `report_template_formal_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `report_template_formal_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sbr_actual_definition`
--

DROP TABLE IF EXISTS `sbr_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sbr_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `expression_id` int unsigned NOT NULL,
  `run_count` bigint unsigned NOT NULL,
  `start_time` time NOT NULL,
  `ac_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `expression_id` (`expression_id`,`run_count`,`start_time`,`ac_id`),
  KEY `ac_id` (`ac_id`),
  CONSTRAINT `sbr_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `sbr_actual_definition_ibfk_2` FOREIGN KEY (`expression_id`) REFERENCES `expression` (`expression_id`) ON DELETE CASCADE,
  CONSTRAINT `sbr_actual_definition_ibfk_3` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sbr_actual_definition`
--

LOCK TABLES `sbr_actual_definition` WRITE;
/*!40000 ALTER TABLE `sbr_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `sbr_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `table_template_actual_definition`
--

DROP TABLE IF EXISTS `table_template_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `table_template_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `tnvc_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `tnvc_id` (`tnvc_id`),
  CONSTRAINT `table_template_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `table_template_actual_definition_ibfk_2` FOREIGN KEY (`tnvc_id`) REFERENCES `type_name_value_collection` (`tnvc_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `table_template_actual_definition`
--

LOCK TABLES `table_template_actual_definition` WRITE;
/*!40000 ALTER TABLE `table_template_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `table_template_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tbr_actual_definition`
--

DROP TABLE IF EXISTS `tbr_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `tbr_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `wait_period` time NOT NULL,
  `run_count` bigint unsigned NOT NULL,
  `start_time` time NOT NULL,
  `ac_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `wait_period` (`wait_period`,`run_count`,`start_time`,`ac_id`),
  KEY `ac_id` (`ac_id`),
  CONSTRAINT `tbr_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `tbr_actual_definition_ibfk_2` FOREIGN KEY (`ac_id`) REFERENCES `ari_collection` (`ac_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tbr_actual_definition`
--

LOCK TABLES `tbr_actual_definition` WRITE;
/*!40000 ALTER TABLE `tbr_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `tbr_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_bool_entry`
--

DROP TABLE IF EXISTS `type_name_value_bool_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_bool_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` tinyint(1) DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_bool_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_bool_entry`
--

LOCK TABLES `type_name_value_bool_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_bool_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_bool_entry` VALUES (2802,NULL),(2803,NULL),(2804,NULL),(2805,NULL),(2806,NULL),(2887,NULL),(2888,NULL),(2889,NULL),(2890,NULL),(2891,NULL),(2973,NULL),(2974,NULL),(2975,NULL),(2976,NULL),(2977,NULL),(3059,NULL),(3060,NULL),(3061,NULL),(3062,NULL),(3063,NULL),(3160,NULL),(3161,NULL),(3162,NULL),(3163,NULL),(3164,NULL),(3261,NULL),(3262,NULL),(3263,NULL),(3264,NULL),(3265,NULL),(3362,NULL),(3363,NULL),(3364,NULL),(3365,NULL),(3366,NULL),(3464,NULL),(3465,NULL),(3466,NULL),(3467,NULL),(3468,NULL),(3571,NULL),(3572,NULL),(3573,NULL),(3574,NULL),(3575,NULL);
/*!40000 ALTER TABLE `type_name_value_bool_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_byte_entry`
--

DROP TABLE IF EXISTS `type_name_value_byte_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_byte_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` tinyint DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_byte_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_byte_entry`
--

LOCK TABLES `type_name_value_byte_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_byte_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `type_name_value_byte_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_collection`
--

DROP TABLE IF EXISTS `type_name_value_collection`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_collection` (
  `tnvc_id` int unsigned NOT NULL AUTO_INCREMENT,
  `use_desc` varchar DEFAULT NULL,
  PRIMARY KEY (`tnvc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1951 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_collection`
--

LOCK TABLES `type_name_value_collection` WRITE;
/*!40000 ALTER TABLE `type_name_value_collection` DISABLE KEYS */;
INSERT INTO `type_name_value_collection` VALUES (1,NULL),(2,NULL),(3,NULL),(4,NULL),(5,NULL),(6,NULL),(7,NULL),(8,NULL),(9,NULL),(10,NULL),(11,NULL),(12,NULL),(13,NULL),(14,NULL),(15,NULL),(16,NULL),(17,NULL),(18,NULL),(19,NULL),(20,NULL),(21,NULL),(22,NULL),(23,NULL),(24,NULL),(25,NULL),(26,NULL),(27,NULL),(28,NULL),(29,NULL),(30,NULL),(31,NULL),(32,NULL),(33,NULL),(34,NULL),(35,NULL),(36,NULL),(37,NULL),(38,NULL),(39,NULL),(40,NULL),(41,NULL),(42,NULL),(43,NULL),(44,NULL),(45,NULL),(46,NULL),(47,NULL),(48,NULL),(49,NULL),(50,NULL),(51,NULL),(52,NULL),(53,NULL),(54,NULL),(55,NULL),(56,NULL),(57,NULL),(58,NULL),(59,NULL),(60,NULL),(61,NULL),(62,NULL),(63,NULL),(64,NULL),(65,NULL),(66,NULL),(67,NULL),(68,NULL),(69,NULL),(70,NULL),(71,NULL),(72,NULL),(73,NULL),(74,NULL),(75,NULL),(76,NULL),(77,NULL),(78,NULL),(79,NULL),(80,NULL),(81,NULL),(82,NULL),(83,NULL),(84,NULL),(85,NULL),(86,NULL),(87,NULL),(88,NULL),(89,NULL),(90,NULL),(91,NULL),(92,NULL),(93,NULL),(94,NULL),(95,NULL),(96,NULL),(97,NULL),(98,NULL),(99,NULL),(100,NULL),(101,NULL),(102,NULL),(103,NULL),(104,NULL),(105,NULL),(106,NULL),(107,NULL),(108,NULL),(109,NULL),(110,NULL),(111,NULL),(112,NULL),(113,NULL),(114,NULL),(115,NULL),(116,NULL),(117,NULL),(118,NULL),(119,NULL),(120,NULL),(121,NULL),(122,NULL),(123,NULL),(124,NULL),(125,NULL),(126,NULL),(127,NULL),(128,NULL),(129,NULL),(130,NULL),(131,NULL),(132,NULL),(133,NULL),(134,NULL),(135,NULL),(136,NULL),(137,NULL),(138,NULL),(139,NULL),(140,NULL),(141,NULL),(142,NULL),(143,NULL),(144,NULL),(145,NULL),(146,NULL),(147,NULL),(148,NULL),(149,NULL),(150,NULL),(151,NULL),(152,NULL),(153,NULL),(154,NULL),(155,NULL),(156,NULL),(157,NULL),(158,NULL),(159,NULL),(160,NULL),(161,NULL),(162,NULL),(163,NULL),(164,NULL),(165,NULL),(166,NULL),(167,NULL),(168,NULL),(169,NULL),(170,NULL),(171,NULL),(172,NULL),(173,NULL),(174,NULL),(175,NULL),(176,NULL),(177,NULL),(178,NULL),(179,NULL),(180,NULL),(181,NULL),(182,NULL),(183,NULL),(184,NULL),(185,NULL),(186,NULL),(187,NULL),(188,NULL),(189,NULL),(190,NULL),(191,NULL),(192,NULL),(193,NULL),(194,NULL),(195,NULL),(196,NULL),(197,NULL),(198,NULL),(199,NULL),(200,NULL),(201,NULL),(202,NULL),(203,NULL),(204,NULL),(205,NULL),(206,NULL),(207,NULL),(208,NULL),(209,NULL),(210,NULL),(211,NULL),(212,NULL),(213,NULL),(214,NULL),(215,NULL),(216,NULL),(217,NULL),(218,NULL),(219,NULL),(220,NULL),(221,NULL),(222,NULL),(223,NULL),(224,NULL),(225,NULL),(226,NULL),(227,NULL),(228,NULL),(229,NULL),(230,NULL),(231,NULL),(232,NULL),(233,NULL),(234,NULL),(235,NULL),(236,NULL),(237,NULL),(238,NULL),(239,NULL),(240,NULL),(241,NULL),(242,NULL),(243,NULL),(244,NULL),(245,NULL),(246,NULL),(247,NULL),(248,NULL),(249,NULL),(250,NULL),(251,NULL),(252,NULL),(253,NULL),(254,NULL),(255,NULL),(256,NULL),(257,NULL),(258,NULL),(259,NULL),(260,NULL),(261,NULL),(262,NULL),(263,NULL),(264,NULL),(265,NULL),(266,NULL),(267,NULL),(268,NULL),(269,NULL),(270,NULL),(271,NULL),(272,NULL),(273,NULL),(274,NULL),(275,NULL),(276,NULL),(277,NULL),(278,NULL),(279,NULL),(280,NULL),(281,NULL),(282,NULL),(283,NULL),(284,NULL),(285,NULL),(286,NULL),(287,NULL),(288,NULL),(289,NULL),(290,NULL),(291,NULL),(292,NULL),(293,NULL),(294,NULL),(295,NULL),(296,NULL),(297,NULL),(298,NULL),(299,NULL),(300,NULL),(301,NULL),(302,NULL),(303,NULL),(304,NULL),(305,NULL),(306,NULL),(307,NULL),(308,NULL),(309,NULL),(310,NULL),(311,NULL),(312,NULL),(313,NULL),(314,NULL),(315,NULL),(316,NULL),(317,NULL),(318,NULL),(319,NULL),(320,NULL),(321,NULL),(322,NULL),(323,NULL),(324,NULL),(325,NULL),(326,NULL),(327,NULL),(328,NULL),(329,NULL),(330,NULL),(331,NULL),(332,NULL),(333,NULL),(334,NULL),(335,NULL),(336,NULL),(337,NULL),(338,NULL),(339,NULL),(340,NULL),(341,NULL),(342,NULL),(343,NULL),(344,NULL),(345,NULL),(346,NULL),(347,NULL),(348,NULL),(349,NULL),(350,NULL),(351,NULL),(352,NULL),(353,NULL),(354,NULL),(355,NULL),(356,NULL),(357,NULL),(358,NULL),(359,NULL),(360,NULL),(361,NULL),(362,NULL),(363,NULL),(364,NULL),(365,NULL),(366,NULL),(367,NULL),(368,NULL),(369,NULL),(370,NULL),(371,NULL),(372,NULL),(373,NULL),(374,NULL),(375,NULL),(376,NULL),(377,NULL),(378,NULL),(379,NULL),(380,NULL),(381,NULL),(382,NULL),(383,NULL),(384,NULL),(385,NULL),(386,NULL),(387,NULL),(388,NULL),(389,NULL),(390,NULL),(391,NULL),(392,NULL),(393,NULL),(394,NULL),(395,NULL),(396,NULL),(397,NULL),(398,NULL),(399,NULL),(400,NULL),(401,NULL),(402,NULL),(403,NULL),(404,NULL),(405,NULL),(406,NULL),(407,NULL),(408,NULL),(409,NULL),(410,NULL),(411,NULL),(412,NULL),(413,NULL),(414,NULL),(415,NULL),(416,NULL),(417,NULL),(418,NULL),(419,NULL),(420,NULL),(421,NULL),(422,NULL),(423,NULL),(424,NULL),(425,NULL),(426,NULL),(427,NULL),(428,NULL),(429,NULL),(430,NULL),(431,NULL),(432,NULL),(433,NULL),(434,NULL),(435,NULL),(436,NULL),(437,NULL),(438,NULL),(439,NULL),(440,NULL),(441,NULL),(442,NULL),(443,NULL),(444,NULL),(445,NULL),(446,NULL),(447,NULL),(448,NULL),(449,NULL),(450,NULL),(451,NULL),(452,NULL),(453,NULL),(454,NULL),(455,NULL),(456,NULL),(457,NULL),(458,NULL),(459,NULL),(460,NULL),(461,NULL),(462,NULL),(463,NULL),(464,NULL),(465,NULL),(466,NULL),(467,NULL),(468,NULL),(469,NULL),(470,NULL),(471,NULL),(472,NULL),(473,NULL),(474,NULL),(475,NULL),(476,NULL),(477,NULL),(478,NULL),(479,NULL),(480,NULL),(481,NULL),(482,NULL),(483,NULL),(484,NULL),(485,NULL),(486,NULL),(487,NULL),(488,NULL),(489,NULL),(490,NULL),(491,NULL),(492,NULL),(493,NULL),(494,NULL),(495,NULL),(496,NULL),(497,NULL),(498,NULL),(499,NULL),(500,NULL),(501,NULL),(502,NULL),(503,NULL),(504,NULL),(505,NULL),(506,NULL),(507,NULL),(508,NULL),(509,NULL),(510,NULL),(511,NULL),(512,NULL),(513,NULL),(514,NULL),(515,NULL),(516,NULL),(517,NULL),(518,NULL),(519,NULL),(520,NULL),(521,NULL),(522,NULL),(523,NULL),(524,NULL),(525,NULL),(526,NULL),(527,NULL),(528,NULL),(529,NULL),(530,NULL),(531,NULL),(532,NULL),(533,NULL),(534,NULL),(535,NULL),(536,NULL),(537,NULL),(538,NULL),(539,NULL),(540,NULL),(541,NULL),(542,NULL),(543,NULL),(544,NULL),(545,NULL),(546,NULL),(547,NULL),(548,NULL),(549,NULL),(550,NULL),(551,NULL),(552,NULL),(553,NULL),(554,NULL),(555,NULL),(556,NULL),(557,NULL),(558,NULL),(559,NULL),(560,NULL),(561,NULL),(562,NULL),(563,NULL),(564,NULL),(565,NULL),(566,NULL),(567,NULL),(568,NULL),(569,NULL),(570,NULL),(571,NULL),(572,NULL),(573,NULL),(574,NULL),(575,NULL),(576,NULL),(577,NULL),(578,NULL),(579,NULL),(580,NULL),(581,NULL),(582,NULL),(583,NULL),(584,NULL),(585,NULL),(586,NULL),(587,NULL),(588,NULL),(589,NULL),(590,NULL),(591,NULL),(592,NULL),(593,NULL),(594,NULL),(595,NULL),(596,NULL),(597,NULL),(598,NULL),(599,NULL),(600,NULL),(601,NULL),(602,NULL),(603,NULL),(604,NULL),(605,NULL),(606,NULL),(607,NULL),(608,NULL),(609,NULL),(610,NULL),(611,NULL),(612,NULL),(613,NULL),(614,NULL),(615,NULL),(616,NULL),(617,NULL),(618,NULL),(619,NULL),(620,NULL),(621,NULL),(622,NULL),(623,NULL),(624,NULL),(625,NULL),(626,NULL),(627,NULL),(628,NULL),(629,NULL),(630,NULL),(631,NULL),(632,NULL),(633,NULL),(634,NULL),(635,NULL),(636,NULL),(637,NULL),(638,NULL),(639,NULL),(640,NULL),(641,NULL),(642,NULL),(643,NULL),(644,NULL),(645,NULL),(646,NULL),(647,NULL),(648,NULL),(649,NULL),(650,NULL),(651,NULL),(652,NULL),(653,NULL),(654,NULL),(655,NULL),(656,NULL),(657,NULL),(658,NULL),(659,NULL),(660,NULL),(661,NULL),(662,NULL),(663,NULL),(664,NULL),(665,NULL),(666,NULL),(667,NULL),(668,NULL),(669,NULL),(670,NULL),(671,NULL),(672,NULL),(673,NULL),(674,NULL),(675,NULL),(676,NULL),(677,NULL),(678,NULL),(679,NULL),(680,NULL),(681,NULL),(682,NULL),(683,NULL),(684,NULL),(685,NULL),(686,NULL),(687,NULL),(688,NULL),(689,NULL),(690,NULL),(691,NULL),(692,NULL),(693,NULL),(694,NULL),(695,NULL),(696,NULL),(697,NULL),(698,NULL),(699,NULL),(700,NULL),(701,NULL),(702,NULL),(703,NULL),(704,NULL),(705,NULL),(706,NULL),(707,NULL),(708,NULL),(709,NULL),(710,NULL),(711,NULL),(712,NULL),(713,NULL),(714,NULL),(715,NULL),(716,NULL),(717,NULL),(718,NULL),(719,NULL),(720,NULL),(721,NULL),(722,NULL),(723,NULL),(724,NULL),(725,NULL),(726,NULL),(727,NULL),(728,NULL),(729,NULL),(730,NULL),(731,NULL),(732,NULL),(733,NULL),(734,NULL),(735,NULL),(736,NULL),(737,NULL),(738,NULL),(739,NULL),(740,NULL),(741,NULL),(742,NULL),(743,NULL),(744,NULL),(745,NULL),(746,NULL),(747,NULL),(748,NULL),(749,NULL),(750,NULL),(751,NULL),(752,NULL),(753,NULL),(754,NULL),(755,NULL),(756,NULL),(757,NULL),(758,NULL),(759,NULL),(760,NULL),(761,NULL),(762,NULL),(763,NULL),(764,NULL),(765,NULL),(766,NULL),(767,NULL),(768,NULL),(769,NULL),(770,NULL),(771,NULL),(772,NULL),(773,NULL),(774,NULL),(775,NULL),(776,NULL),(777,NULL),(778,NULL),(779,NULL),(780,NULL),(781,NULL),(782,NULL),(783,NULL),(784,NULL),(785,NULL),(786,NULL),(787,NULL),(788,NULL),(789,NULL),(790,NULL),(791,NULL),(792,NULL),(793,NULL),(794,NULL),(795,NULL),(796,NULL),(797,NULL),(798,NULL),(799,NULL),(800,NULL),(801,NULL),(802,NULL),(803,NULL),(804,NULL),(805,NULL),(806,NULL),(807,NULL),(808,NULL),(809,NULL),(810,NULL),(811,NULL),(812,NULL),(813,NULL),(814,NULL),(815,NULL),(816,NULL),(817,NULL),(818,NULL),(819,NULL),(820,NULL),(821,NULL),(822,NULL),(823,NULL),(824,NULL),(825,NULL),(826,NULL),(827,NULL),(828,NULL),(829,NULL),(830,NULL),(831,NULL),(832,NULL),(833,NULL),(834,NULL),(835,NULL),(836,NULL),(837,NULL),(838,NULL),(839,NULL),(840,NULL),(841,NULL),(842,NULL),(843,NULL),(844,NULL),(845,NULL),(846,NULL),(847,NULL),(848,NULL),(849,NULL),(850,NULL),(851,NULL),(852,NULL),(853,NULL),(854,NULL),(855,NULL),(856,NULL),(857,NULL),(858,NULL),(859,NULL),(860,NULL),(861,NULL),(862,NULL),(863,NULL),(864,NULL),(865,NULL),(866,NULL),(867,NULL),(868,NULL),(869,NULL),(870,NULL),(871,NULL),(872,NULL),(873,NULL),(874,NULL),(875,NULL),(876,NULL),(877,NULL),(878,NULL),(879,NULL),(880,NULL),(881,NULL),(882,NULL),(883,NULL),(884,NULL),(885,NULL),(886,NULL),(887,NULL),(888,NULL),(889,NULL),(890,NULL),(891,NULL),(892,NULL),(893,NULL),(894,NULL),(895,NULL),(896,NULL),(897,NULL),(898,NULL),(899,NULL),(900,NULL),(901,NULL),(902,NULL),(903,NULL),(904,NULL),(905,NULL),(906,NULL),(907,NULL),(908,NULL),(909,NULL),(910,NULL),(911,NULL),(912,NULL),(913,NULL),(914,NULL),(915,NULL),(916,NULL),(917,NULL),(918,NULL),(919,NULL),(920,NULL),(921,NULL),(922,NULL),(923,NULL),(924,NULL),(925,NULL),(926,NULL),(927,NULL),(928,NULL),(929,NULL),(930,NULL),(931,NULL),(932,NULL),(933,NULL),(934,NULL),(935,NULL),(936,NULL),(937,NULL),(938,NULL),(939,NULL),(940,NULL),(941,NULL),(942,NULL),(943,NULL),(944,NULL),(945,NULL),(946,NULL),(947,NULL),(948,NULL),(949,NULL),(950,NULL),(951,NULL),(952,NULL),(953,NULL),(954,NULL),(955,NULL),(956,NULL),(957,NULL),(958,NULL),(959,NULL),(960,NULL),(961,NULL),(962,NULL),(963,NULL),(964,NULL),(965,NULL),(966,NULL),(967,NULL),(968,NULL),(969,NULL),(970,NULL),(971,NULL),(972,NULL),(973,NULL),(974,NULL),(975,NULL),(976,NULL),(977,NULL),(978,NULL),(979,NULL),(980,NULL),(981,NULL),(982,NULL),(983,NULL),(984,NULL),(985,NULL),(986,NULL),(987,NULL),(988,NULL),(989,NULL),(990,NULL),(991,NULL),(992,NULL),(993,NULL),(994,NULL),(995,NULL),(996,NULL),(997,NULL),(998,NULL),(999,NULL),(1000,NULL),(1001,NULL),(1002,NULL),(1003,NULL),(1004,NULL),(1005,NULL),(1006,NULL),(1007,NULL),(1008,NULL),(1009,NULL),(1010,NULL),(1011,NULL),(1012,NULL),(1013,NULL),(1014,NULL),(1015,NULL),(1016,NULL),(1017,NULL),(1018,NULL),(1019,NULL),(1020,NULL),(1021,NULL),(1022,NULL),(1023,NULL),(1024,NULL),(1025,NULL),(1026,NULL),(1027,NULL),(1028,NULL),(1029,NULL),(1030,NULL),(1031,NULL),(1032,NULL),(1033,NULL),(1034,NULL),(1035,NULL),(1036,NULL),(1037,NULL),(1038,NULL),(1039,NULL),(1040,NULL),(1041,NULL),(1042,NULL),(1043,NULL),(1044,NULL),(1045,NULL),(1046,NULL),(1047,NULL),(1048,NULL),(1049,NULL),(1050,NULL),(1051,NULL),(1052,NULL),(1053,NULL),(1054,NULL),(1055,NULL),(1056,NULL),(1057,NULL),(1058,NULL),(1059,NULL),(1060,NULL),(1061,NULL),(1062,NULL),(1063,NULL),(1064,NULL),(1065,NULL),(1066,NULL),(1067,NULL),(1068,NULL),(1069,NULL),(1070,NULL),(1071,NULL),(1072,NULL),(1073,NULL),(1074,NULL),(1075,NULL),(1076,NULL),(1077,NULL),(1078,NULL),(1079,NULL),(1080,NULL),(1081,NULL),(1082,NULL),(1083,NULL),(1084,NULL),(1085,NULL),(1086,NULL),(1087,NULL),(1088,NULL),(1089,NULL),(1090,NULL),(1091,NULL),(1092,NULL),(1093,NULL),(1094,NULL),(1095,NULL),(1096,NULL),(1097,NULL),(1098,NULL),(1099,NULL),(1100,NULL),(1101,NULL),(1102,NULL),(1103,NULL),(1104,NULL),(1105,NULL),(1106,NULL),(1107,NULL),(1108,NULL),(1109,NULL),(1110,NULL),(1111,NULL),(1112,NULL),(1113,NULL),(1114,NULL),(1115,NULL),(1116,NULL),(1117,NULL),(1118,NULL),(1119,NULL),(1120,NULL),(1121,NULL),(1122,NULL),(1123,NULL),(1124,NULL),(1125,NULL),(1126,NULL),(1127,NULL),(1128,NULL),(1129,NULL),(1130,NULL),(1131,NULL),(1132,NULL),(1133,NULL),(1134,NULL),(1135,NULL),(1136,NULL),(1137,NULL),(1138,NULL),(1139,NULL),(1140,NULL),(1141,NULL),(1142,NULL),(1143,NULL),(1144,NULL),(1145,NULL),(1146,NULL),(1147,NULL),(1148,NULL),(1149,NULL),(1150,NULL),(1151,NULL),(1152,NULL),(1153,NULL),(1154,NULL),(1155,NULL),(1156,NULL),(1157,NULL),(1158,NULL),(1159,NULL),(1160,NULL),(1161,NULL),(1162,NULL),(1163,NULL),(1164,NULL),(1165,NULL),(1166,NULL),(1167,NULL),(1168,NULL),(1169,NULL),(1170,NULL),(1171,NULL),(1172,NULL),(1173,NULL),(1174,NULL),(1175,NULL),(1176,NULL),(1177,NULL),(1178,NULL),(1179,NULL),(1180,NULL),(1181,NULL),(1182,NULL),(1183,NULL),(1184,NULL),(1185,NULL),(1186,NULL),(1187,NULL),(1188,NULL),(1189,NULL),(1190,NULL),(1191,NULL),(1192,NULL),(1193,NULL),(1194,NULL),(1195,NULL),(1196,NULL),(1197,NULL),(1198,NULL),(1199,NULL),(1200,NULL),(1201,NULL),(1202,NULL),(1203,NULL),(1204,NULL),(1205,NULL),(1206,NULL),(1207,NULL),(1208,NULL),(1209,NULL),(1210,NULL),(1211,NULL),(1212,NULL),(1213,NULL),(1214,NULL),(1215,NULL),(1216,NULL),(1217,NULL),(1218,NULL),(1219,NULL),(1220,NULL),(1221,NULL),(1222,NULL),(1223,NULL),(1224,NULL),(1225,NULL),(1226,NULL),(1227,NULL),(1228,NULL),(1229,NULL),(1230,NULL),(1231,NULL),(1232,NULL),(1233,NULL),(1234,NULL),(1235,NULL),(1236,NULL),(1237,NULL),(1238,NULL),(1239,NULL),(1240,NULL),(1241,NULL),(1242,NULL),(1243,NULL),(1244,NULL),(1245,NULL),(1246,NULL),(1247,NULL),(1248,NULL),(1249,NULL),(1250,NULL),(1251,NULL),(1252,NULL),(1253,NULL),(1254,NULL),(1255,NULL),(1256,NULL),(1257,NULL),(1258,NULL),(1259,NULL),(1260,NULL),(1261,NULL),(1262,NULL),(1263,NULL),(1264,NULL),(1265,NULL),(1266,NULL),(1267,NULL),(1268,NULL),(1269,NULL),(1270,NULL),(1271,NULL),(1272,NULL),(1273,NULL),(1274,NULL),(1275,NULL),(1276,NULL),(1277,NULL),(1278,NULL),(1279,NULL),(1280,NULL),(1281,NULL),(1282,NULL),(1283,NULL),(1284,NULL),(1285,NULL),(1286,NULL),(1287,NULL),(1288,NULL),(1289,NULL),(1290,NULL),(1291,NULL),(1292,NULL),(1293,NULL),(1294,NULL),(1295,NULL),(1296,NULL),(1297,NULL),(1298,NULL),(1299,NULL),(1300,NULL),(1301,NULL),(1302,NULL),(1303,NULL),(1304,NULL),(1305,NULL),(1306,NULL),(1307,NULL),(1308,NULL),(1309,NULL),(1310,NULL),(1311,NULL),(1312,NULL),(1313,NULL),(1314,NULL),(1315,NULL),(1316,NULL),(1317,NULL),(1318,NULL),(1319,NULL),(1320,NULL),(1321,NULL),(1322,NULL),(1323,NULL),(1324,NULL),(1325,NULL),(1326,NULL),(1327,NULL),(1328,NULL),(1329,NULL),(1330,NULL),(1331,NULL),(1332,NULL),(1333,NULL),(1334,NULL),(1335,NULL),(1336,NULL),(1337,NULL),(1338,NULL),(1339,NULL),(1340,NULL),(1341,NULL),(1342,NULL),(1343,NULL),(1344,NULL),(1345,NULL),(1346,NULL),(1347,NULL),(1348,NULL),(1349,NULL),(1350,NULL),(1351,NULL),(1352,NULL),(1353,NULL),(1354,NULL),(1355,NULL),(1356,NULL),(1357,NULL),(1358,NULL),(1359,NULL),(1360,NULL),(1361,NULL),(1362,NULL),(1363,NULL),(1364,NULL),(1365,NULL),(1366,NULL),(1367,NULL),(1368,NULL),(1369,NULL),(1370,NULL),(1371,NULL),(1372,NULL),(1373,NULL),(1374,NULL),(1375,NULL),(1376,NULL),(1377,NULL),(1378,NULL),(1379,NULL),(1380,NULL),(1381,NULL),(1382,NULL),(1383,NULL),(1384,NULL),(1385,NULL),(1386,NULL),(1387,NULL),(1388,NULL),(1389,NULL),(1390,NULL),(1391,NULL),(1392,NULL),(1393,NULL),(1394,NULL),(1395,NULL),(1396,NULL),(1397,NULL),(1398,NULL),(1399,NULL),(1400,NULL),(1401,NULL),(1402,NULL),(1403,NULL),(1404,NULL),(1405,NULL),(1406,NULL),(1407,NULL),(1408,NULL),(1409,NULL),(1410,NULL),(1411,NULL),(1412,NULL),(1413,NULL),(1414,NULL),(1415,NULL),(1416,NULL),(1417,NULL),(1418,NULL),(1419,NULL),(1420,NULL),(1421,NULL),(1422,NULL),(1423,NULL),(1424,NULL),(1425,NULL),(1426,NULL),(1427,NULL),(1428,NULL),(1429,NULL),(1430,NULL),(1431,NULL),(1432,NULL),(1433,NULL),(1434,NULL),(1435,NULL),(1436,NULL),(1437,NULL),(1438,NULL),(1439,NULL),(1440,'operands fo plusINT'),(1441,'operands for plusINT'),(1442,'operands for plusINT'),(1443,'operands for plusINT'),(1444,'operands for plusINT'),(1445,'operands for plusINT'),(1446,'operands for plusINT'),(1447,'operands for plusINT'),(1448,'operands for plusINT'),(1449,'operands for plusUINT'),(1450,'operands for plusVAST'),(1451,'operands for plusUVAST'),(1452,'operands for plusREAL32'),(1453,'operands for plusREAL64'),(1454,'operands for minusINT'),(1455,'operands for minusUINT'),(1456,'operands for minusVAST'),(1457,'operands for minusUVAST'),(1458,'operands for minusREAL32'),(1459,'operands for minusREAL64'),(1460,'operands for multINT'),(1461,'operands for multUINT'),(1462,'operands for multVAST'),(1463,'operands for multUVAST'),(1464,'operands for multREAL32'),(1465,'operands for multREAL64'),(1466,'operands for divINT'),(1467,'operands for divUINT'),(1468,'operands for divVAST'),(1469,'operands for divUVAST'),(1470,'operands for divREAL32'),(1471,'operands for divREAL64'),(1472,'operands for modINT'),(1473,'operands for modUINT'),(1474,'operands for modVAST'),(1475,'operands for modUVAST'),(1476,'operands for modREAL32'),(1477,'operands for modREAL64'),(1478,'operands for expINT'),(1479,'operands for expUINT'),(1480,'operands for expVAST'),(1481,'operands for expUVAST'),(1482,'operands for expREAL32'),(1483,'operands for expREAL64'),(1484,'operands for bitAND'),(1485,'operands bitO for'),(1486,'operands for bitXOR'),(1487,'operands for bitNOT'),(1488,'operands for plusINT'),(1489,'operands for plusUINT'),(1490,'operands for plusVAST'),(1491,'operands for plusUVAST'),(1492,'operands for plusREAL32'),(1493,'operands for plusREAL64'),(1494,'operands for minusINT'),(1495,'operands for minusUINT'),(1496,'operands for minusVAST'),(1497,'operands for minusUVAST'),(1498,'operands for minusREAL32'),(1499,'operands for minusREAL64'),(1500,'operands for multINT'),(1501,'operands for multUINT'),(1502,'operands for multVAST'),(1503,'operands for multUVAST'),(1504,'operands for multREAL32'),(1505,'operands for multREAL64'),(1506,'operands for divINT'),(1507,'operands for divUINT'),(1508,'operands for divVAST'),(1509,'operands for divUVAST'),(1510,'operands for divREAL32'),(1511,'operands for divREAL64'),(1512,'operands for modINT'),(1513,'operands for modUINT'),(1514,'operands for modVAST'),(1515,'operands for modUVAST'),(1516,'operands for modREAL32'),(1517,'operands for modREAL64'),(1518,'operands for expINT'),(1519,'operands for expUINT'),(1520,'operands for expVAST'),(1521,'operands for expUVAST'),(1522,'operands for expREAL32'),(1523,'operands for expREAL64'),(1524,'operands for bitAND'),(1525,'operands bitO for'),(1526,'operands for bitXOR'),(1527,'operands for bitNOT'),(1528,'operands for logAND'),(1529,'operands logO for'),(1530,'operands for logNOT'),(1531,'operands for abs'),(1532,'operands for plusINT'),(1533,'operands for plusUINT'),(1534,'operands for plusVAST'),(1535,'operands for plusUVAST'),(1536,'operands for plusREAL32'),(1537,'operands for plusREAL64'),(1538,'operands for minusINT'),(1539,'operands for minusUINT'),(1540,'operands for minusVAST'),(1541,'operands for minusUVAST'),(1542,'operands for minusREAL32'),(1543,'operands for minusREAL64'),(1544,'operands for multINT'),(1545,'operands for multUINT'),(1546,'operands for multVAST'),(1547,'operands for multUVAST'),(1548,'operands for multREAL32'),(1549,'operands for multREAL64'),(1550,'operands for divINT'),(1551,'operands for divUINT'),(1552,'operands for divVAST'),(1553,'operands for divUVAST'),(1554,'operands for divREAL32'),(1555,'operands for divREAL64'),(1556,'operands for modINT'),(1557,'operands for modUINT'),(1558,'operands for modVAST'),(1559,'operands for modUVAST'),(1560,'operands for modREAL32'),(1561,'operands for modREAL64'),(1562,'operands for expINT'),(1563,'operands for expUINT'),(1564,'operands for expVAST'),(1565,'operands for expUVAST'),(1566,'operands for expREAL32'),(1567,'operands for expREAL64'),(1568,'operands for bitAND'),(1569,'operands bitO for'),(1570,'operands for bitXOR'),(1571,'operands for bitNOT'),(1572,'operands for logAND'),(1573,'operands logO for'),(1574,'operands for logNOT'),(1575,'operands for abs'),(1576,'operands  for'),(1577,'operands for plusINT'),(1578,'operands for plusUINT'),(1579,'operands for plusVAST'),(1580,'operands for plusUVAST'),(1581,'operands for plusREAL32'),(1582,'operands for plusREAL64'),(1583,'operands for minusINT'),(1584,'operands for minusUINT'),(1585,'operands for minusVAST'),(1586,'operands for minusUVAST'),(1587,'operands for minusREAL32'),(1588,'operands for minusREAL64'),(1589,'operands for multINT'),(1590,'operands for multUINT'),(1591,'operands for multVAST'),(1592,'operands for multUVAST'),(1593,'operands for multREAL32'),(1594,'operands for multREAL64'),(1595,'operands for divINT'),(1596,'operands for divUINT'),(1597,'operands for divVAST'),(1598,'operands for divUVAST'),(1599,'operands for divREAL32'),(1600,'operands for divREAL64'),(1601,'operands for modINT'),(1602,'operands for modUINT'),(1603,'operands for modVAST'),(1604,'operands for modUVAST'),(1605,'operands for modREAL32'),(1606,'operands for modREAL64'),(1607,'operands for expINT'),(1608,'operands for expUINT'),(1609,'operands for expVAST'),(1610,'operands for expUVAST'),(1611,'operands for expREAL32'),(1612,'operands for expREAL64'),(1613,'operands for bitAND'),(1614,'operands bitO for'),(1615,'operands for bitXOR'),(1616,'operands for bitNOT'),(1617,'operands for logAND'),(1618,'operands logO for'),(1619,'operands for logNOT'),(1620,'operands for abs'),(1621,'operands  for'),(1622,'operands for plusINT'),(1623,'operands for plusUINT'),(1624,'operands for plusVAST'),(1625,'operands for plusUVAST'),(1626,'operands for plusREAL32'),(1627,'operands for plusREAL64'),(1628,'operands for minusINT'),(1629,'operands for minusUINT'),(1630,'operands for minusVAST'),(1631,'operands for minusUVAST'),(1632,'operands for minusREAL32'),(1633,'operands for minusREAL64'),(1634,'operands for multINT'),(1635,'operands for multUINT'),(1636,'operands for multVAST'),(1637,'operands for multUVAST'),(1638,'operands for multREAL32'),(1639,'operands for multREAL64'),(1640,'operands for divINT'),(1641,'operands for divUINT'),(1642,'operands for divVAST'),(1643,'operands for divUVAST'),(1644,'operands for divREAL32'),(1645,'operands for divREAL64'),(1646,'operands for modINT'),(1647,'operands for modUINT'),(1648,'operands for modVAST'),(1649,'operands for modUVAST'),(1650,'operands for modREAL32'),(1651,'operands for modREAL64'),(1652,'operands for expINT'),(1653,'operands for expUINT'),(1654,'operands for expVAST'),(1655,'operands for expUVAST'),(1656,'operands for expREAL32'),(1657,'operands for expREAL64'),(1658,'operands for bitAND'),(1659,'operands bitO for'),(1660,'operands for bitXOR'),(1661,'operands for bitNOT'),(1662,'operands for logAND'),(1663,'operands logO for'),(1664,'operands for logNOT'),(1665,'operands for abs'),(1666,'operands  for'),(1667,'operands  for'),(1668,'operands  for'),(1669,'operands  for'),(1670,'operands  for'),(1671,'operands  for'),(1672,'operands  for'),(1673,'operands  for'),(1674,'columns for the adms table'),(1675,'operands for plusINT'),(1676,'operands for plusUINT'),(1677,'operands for plusVAST'),(1678,'operands for plusUVAST'),(1679,'operands for plusREAL32'),(1680,'operands for plusREAL64'),(1681,'operands for minusINT'),(1682,'operands for minusUINT'),(1683,'operands for minusVAST'),(1684,'operands for minusUVAST'),(1685,'operands for minusREAL32'),(1686,'operands for minusREAL64'),(1687,'operands for multINT'),(1688,'operands for multUINT'),(1689,'operands for multVAST'),(1690,'operands for multUVAST'),(1691,'operands for multREAL32'),(1692,'operands for multREAL64'),(1693,'operands for divINT'),(1694,'operands for divUINT'),(1695,'operands for divVAST'),(1696,'operands for divUVAST'),(1697,'operands for divREAL32'),(1698,'operands for divREAL64'),(1699,'operands for modINT'),(1700,'operands for modUINT'),(1701,'operands for modVAST'),(1702,'operands for modUVAST'),(1703,'operands for modREAL32'),(1704,'operands for modREAL64'),(1705,'operands for expINT'),(1706,'operands for expUINT'),(1707,'operands for expVAST'),(1708,'operands for expUVAST'),(1709,'operands for expREAL32'),(1710,'operands for expREAL64'),(1711,'operands for bitAND'),(1712,'operands bitO for'),(1713,'operands for bitXOR'),(1714,'operands for bitNOT'),(1715,'operands for logAND'),(1716,'operands logO for'),(1717,'operands for logNOT'),(1718,'operands for abs'),(1719,'operands  for'),(1720,'operands  for'),(1721,'operands  for'),(1722,'operands  for'),(1723,'operands  for'),(1724,'operands  for'),(1725,'operands  for'),(1726,'operands  for'),(1727,'columns for the adms table'),(1728,'operands for plusINT'),(1729,'operands for plusUINT'),(1730,'operands for plusVAST'),(1731,'operands for plusUVAST'),(1732,'operands for plusREAL32'),(1733,'operands for plusREAL64'),(1734,'operands for minusINT'),(1735,'operands for minusUINT'),(1736,'operands for minusVAST'),(1737,'operands for minusUVAST'),(1738,'operands for minusREAL32'),(1739,'operands for minusREAL64'),(1740,'operands for multINT'),(1741,'operands for multUINT'),(1742,'operands for multVAST'),(1743,'operands for multUVAST'),(1744,'operands for multREAL32'),(1745,'operands for multREAL64'),(1746,'operands for divINT'),(1747,'operands for divUINT'),(1748,'operands for divVAST'),(1749,'operands for divUVAST'),(1750,'operands for divREAL32'),(1751,'operands for divREAL64'),(1752,'operands for modINT'),(1753,'operands for modUINT'),(1754,'operands for modVAST'),(1755,'operands for modUVAST'),(1756,'operands for modREAL32'),(1757,'operands for modREAL64'),(1758,'operands for expINT'),(1759,'operands for expUINT'),(1760,'operands for expVAST'),(1761,'operands for expUVAST'),(1762,'operands for expREAL32'),(1763,'operands for expREAL64'),(1764,'operands for bitAND'),(1765,'operands bitO for'),(1766,'operands for bitXOR'),(1767,'operands for bitNOT'),(1768,'operands for logAND'),(1769,'operands logO for'),(1770,'operands for logNOT'),(1771,'operands for abs'),(1772,'operands  for'),(1773,'operands  for'),(1774,'operands  for'),(1775,'operands  for'),(1776,'operands  for'),(1777,'operands  for'),(1778,'operands  for'),(1779,'operands  for'),(1780,'columns for the adms table'),(1781,'operands for plusINT'),(1782,'operands for plusUINT'),(1783,'operands for plusVAST'),(1784,'operands for plusUVAST'),(1785,'operands for plusREAL32'),(1786,'operands for plusREAL64'),(1787,'operands for minusINT'),(1788,'operands for minusUINT'),(1789,'operands for minusVAST'),(1790,'operands for minusUVAST'),(1791,'operands for minusREAL32'),(1792,'operands for minusREAL64'),(1793,'operands for multINT'),(1794,'operands for multUINT'),(1795,'operands for multVAST'),(1796,'operands for multUVAST'),(1797,'operands for multREAL32'),(1798,'operands for multREAL64'),(1799,'operands for divINT'),(1800,'operands for divUINT'),(1801,'operands for divVAST'),(1802,'operands for divUVAST'),(1803,'operands for divREAL32'),(1804,'operands for divREAL64'),(1805,'operands for modINT'),(1806,'operands for modUINT'),(1807,'operands for modVAST'),(1808,'operands for modUVAST'),(1809,'operands for modREAL32'),(1810,'operands for modREAL64'),(1811,'operands for expINT'),(1812,'operands for expUINT'),(1813,'operands for expVAST'),(1814,'operands for expUVAST'),(1815,'operands for expREAL32'),(1816,'operands for expREAL64'),(1817,'operands for bitAND'),(1818,'operands bitO for'),(1819,'operands for bitXOR'),(1820,'operands for bitNOT'),(1821,'operands for logAND'),(1822,'operands logO for'),(1823,'operands for logNOT'),(1824,'operands for abs'),(1825,'operands  for'),(1826,'operands  for'),(1827,'operands  for'),(1828,'operands  for'),(1829,'operands  for'),(1830,'operands  for'),(1831,'operands  for'),(1832,'operands  for'),(1833,'columns for the adms table'),(1834,'columns for the variables table'),(1835,'operands for plusINT'),(1836,'operands for plusUINT'),(1837,'operands for plusVAST'),(1838,'operands for plusUVAST'),(1839,'operands for plusREAL32'),(1840,'operands for plusREAL64'),(1841,'operands for minusINT'),(1842,'operands for minusUINT'),(1843,'operands for minusVAST'),(1844,'operands for minusUVAST'),(1845,'operands for minusREAL32'),(1846,'operands for minusREAL64'),(1847,'operands for multINT'),(1848,'operands for multUINT'),(1849,'operands for multVAST'),(1850,'operands for multUVAST'),(1851,'operands for multREAL32'),(1852,'operands for multREAL64'),(1853,'operands for divINT'),(1854,'operands for divUINT'),(1855,'operands for divVAST'),(1856,'operands for divUVAST'),(1857,'operands for divREAL32'),(1858,'operands for divREAL64'),(1859,'operands for modINT'),(1860,'operands for modUINT'),(1861,'operands for modVAST'),(1862,'operands for modUVAST'),(1863,'operands for modREAL32'),(1864,'operands for modREAL64'),(1865,'operands for expINT'),(1866,'operands for expUINT'),(1867,'operands for expVAST'),(1868,'operands for expUVAST'),(1869,'operands for expREAL32'),(1870,'operands for expREAL64'),(1871,'operands for bitAND'),(1872,'operands bitO for'),(1873,'operands for bitXOR'),(1874,'operands for bitNOT'),(1875,'operands for logAND'),(1876,'operands logO for'),(1877,'operands for logNOT'),(1878,'operands for abs'),(1879,'operands  for'),(1880,'operands  for'),(1881,'operands  for'),(1882,'operands  for'),(1883,'operands  for'),(1884,'operands  for'),(1885,'operands  for'),(1886,'operands  for'),(1887,'columns for the adms table'),(1888,'columns for the variables table'),(1889,'columns for the rptts table'),(1890,'columns for the macros table'),(1891,'columns for the rules table'),(1892,'columns for the tblts table'),(1893,'operands for plusINT'),(1894,'operands for plusUINT'),(1895,'operands for plusVAST'),(1896,'operands for plusUVAST'),(1897,'operands for plusREAL32'),(1898,'operands for plusREAL64'),(1899,'operands for minusINT'),(1900,'operands for minusUINT'),(1901,'operands for minusVAST'),(1902,'operands for minusUVAST'),(1903,'operands for minusREAL32'),(1904,'operands for minusREAL64'),(1905,'operands for multINT'),(1906,'operands for multUINT'),(1907,'operands for multVAST'),(1908,'operands for multUVAST'),(1909,'operands for multREAL32'),(1910,'operands for multREAL64'),(1911,'operands for divINT'),(1912,'operands for divUINT'),(1913,'operands for divVAST'),(1914,'operands for divUVAST'),(1915,'operands for divREAL32'),(1916,'operands for divREAL64'),(1917,'operands for modINT'),(1918,'operands for modUINT'),(1919,'operands for modVAST'),(1920,'operands for modUVAST'),(1921,'operands for modREAL32'),(1922,'operands for modREAL64'),(1923,'operands for expINT'),(1924,'operands for expUINT'),(1925,'operands for expVAST'),(1926,'operands for expUVAST'),(1927,'operands for expREAL32'),(1928,'operands for expREAL64'),(1929,'operands for bitAND'),(1930,'operands bitO for'),(1931,'operands for bitXOR'),(1932,'operands for bitNOT'),(1933,'operands for logAND'),(1934,'operands logO for'),(1935,'operands for logNOT'),(1936,'operands for abs'),(1937,'operands  for'),(1938,'operands  for'),(1939,'operands  for'),(1940,'operands  for'),(1941,'operands  for'),(1942,'operands  for'),(1943,'operands  for'),(1944,'operands  for'),(1945,'columns for the adms table'),(1946,'columns for the variables table'),(1947,'columns for the rptts table'),(1948,'columns for the macros table'),(1949,'columns for the rules table'),(1950,'columns for the tblts table');
/*!40000 ALTER TABLE `type_name_value_collection` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_entry`
--

DROP TABLE IF EXISTS `type_name_value_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_entry` (
  `tnv_id` int unsigned NOT NULL AUTO_INCREMENT,
  `tnvc_id` int unsigned DEFAULT NULL,
  `order_num` int unsigned DEFAULT '0',
  `data_type_id` int unsigned DEFAULT NULL,
  `data_name` varchar DEFAULT NULL,
  PRIMARY KEY (`tnv_id`),
  UNIQUE KEY `tnvc_id` (`tnvc_id`,`order_num`),
  KEY `data_type_id` (`data_type_id`),
  CONSTRAINT `type_name_value_entry_ibfk_1` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `type_name_value_entry_ibfk_2` FOREIGN KEY (`tnvc_id`) REFERENCES `type_name_value_collection` (`tnvc_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3599 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_entry`
--

LOCK TABLES `type_name_value_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_entry` VALUES (1,1,1,19,NULL),(2,1,2,19,NULL),(3,2,1,20,NULL),(4,2,2,20,NULL),(5,3,1,21,NULL),(6,3,2,21,NULL),(7,4,1,22,NULL),(8,4,2,22,NULL),(9,5,1,23,NULL),(10,5,2,23,NULL),(11,6,1,24,NULL),(12,6,2,24,NULL),(13,7,1,19,NULL),(14,7,2,19,NULL),(15,8,1,20,NULL),(16,8,2,20,NULL),(17,9,1,21,NULL),(18,9,2,21,NULL),(19,10,1,22,NULL),(20,10,2,22,NULL),(21,11,1,23,NULL),(22,11,2,23,NULL),(23,12,1,24,NULL),(24,12,2,24,NULL),(25,13,1,19,NULL),(26,13,2,19,NULL),(27,14,1,20,NULL),(28,14,2,20,NULL),(29,15,1,21,NULL),(30,15,2,21,NULL),(31,16,1,22,NULL),(32,16,2,22,NULL),(33,17,1,23,NULL),(34,17,2,23,NULL),(35,18,1,24,NULL),(36,18,2,24,NULL),(37,19,1,19,NULL),(38,19,2,19,NULL),(39,20,1,20,NULL),(40,20,2,20,NULL),(41,21,1,21,NULL),(42,21,2,21,NULL),(43,22,1,22,NULL),(44,22,2,22,NULL),(45,23,1,23,NULL),(46,23,2,23,NULL),(47,24,1,24,NULL),(48,24,2,24,NULL),(49,25,1,19,NULL),(50,25,2,19,NULL),(51,26,1,20,NULL),(52,26,2,20,NULL),(53,27,1,21,NULL),(54,27,2,21,NULL),(55,28,1,22,NULL),(56,28,2,22,NULL),(57,29,1,23,NULL),(58,29,2,23,NULL),(59,30,1,24,NULL),(60,30,2,24,NULL),(61,31,1,19,NULL),(62,31,2,19,NULL),(63,32,1,20,NULL),(64,32,2,20,NULL),(65,33,1,21,NULL),(66,33,2,21,NULL),(67,34,1,22,NULL),(68,34,2,22,NULL),(69,35,1,23,NULL),(70,35,2,23,NULL),(71,36,1,24,NULL),(72,36,2,24,NULL),(73,37,1,22,NULL),(74,37,2,22,NULL),(75,38,1,22,NULL),(76,38,2,22,NULL),(77,39,1,22,NULL),(78,39,2,22,NULL),(79,40,1,22,NULL),(80,41,1,16,NULL),(81,41,2,16,NULL),(82,42,1,16,NULL),(83,42,2,16,NULL),(84,43,1,16,NULL),(85,44,1,22,NULL),(86,45,1,NULL,NULL),(87,45,2,NULL,NULL),(88,46,1,NULL,NULL),(89,46,2,NULL,NULL),(90,47,1,NULL,NULL),(91,47,2,NULL,NULL),(92,48,1,NULL,NULL),(93,48,2,NULL,NULL),(94,49,1,NULL,NULL),(95,49,2,NULL,NULL),(96,50,1,NULL,NULL),(97,50,2,NULL,NULL),(98,51,1,22,NULL),(99,51,2,22,NULL),(100,52,1,22,NULL),(101,52,2,22,NULL),(102,53,1,18,'adm_name'),(103,54,1,36,'ids'),(104,55,1,36,'ids'),(105,56,1,36,'ids'),(106,57,1,36,'ids'),(107,58,1,36,'ids'),(108,59,1,19,NULL),(109,59,2,19,NULL),(110,60,1,20,NULL),(111,60,2,20,NULL),(112,61,1,21,NULL),(113,61,2,21,NULL),(114,62,1,22,NULL),(115,62,2,22,NULL),(116,63,1,23,NULL),(117,63,2,23,NULL),(118,64,1,24,NULL),(119,64,2,24,NULL),(120,65,1,19,NULL),(121,65,2,19,NULL),(122,66,1,20,NULL),(123,66,2,20,NULL),(124,67,1,21,NULL),(125,67,2,21,NULL),(126,68,1,22,NULL),(127,68,2,22,NULL),(128,69,1,23,NULL),(129,69,2,23,NULL),(130,70,1,24,NULL),(131,70,2,24,NULL),(132,71,1,19,NULL),(133,71,2,19,NULL),(134,72,1,20,NULL),(135,72,2,20,NULL),(136,73,1,21,NULL),(137,73,2,21,NULL),(138,74,1,22,NULL),(139,74,2,22,NULL),(140,75,1,23,NULL),(141,75,2,23,NULL),(142,76,1,24,NULL),(143,76,2,24,NULL),(144,77,1,19,NULL),(145,77,2,19,NULL),(146,78,1,20,NULL),(147,78,2,20,NULL),(148,79,1,21,NULL),(149,79,2,21,NULL),(150,80,1,22,NULL),(151,80,2,22,NULL),(152,81,1,23,NULL),(153,81,2,23,NULL),(154,82,1,24,NULL),(155,82,2,24,NULL),(156,83,1,19,NULL),(157,83,2,19,NULL),(158,84,1,20,NULL),(159,84,2,20,NULL),(160,85,1,21,NULL),(161,85,2,21,NULL),(162,86,1,22,NULL),(163,86,2,22,NULL),(164,87,1,23,NULL),(165,87,2,23,NULL),(166,88,1,24,NULL),(167,88,2,24,NULL),(168,89,1,19,NULL),(169,89,2,19,NULL),(170,90,1,20,NULL),(171,90,2,20,NULL),(172,91,1,21,NULL),(173,91,2,21,NULL),(174,92,1,22,NULL),(175,92,2,22,NULL),(176,93,1,23,NULL),(177,93,2,23,NULL),(178,94,1,24,NULL),(179,94,2,24,NULL),(180,95,1,22,NULL),(181,95,2,22,NULL),(182,96,1,22,NULL),(183,96,2,22,NULL),(184,97,1,22,NULL),(185,97,2,22,NULL),(186,98,1,22,NULL),(187,99,1,16,NULL),(188,99,2,16,NULL),(189,100,1,16,NULL),(190,100,2,16,NULL),(191,101,1,16,NULL),(192,102,1,22,NULL),(193,103,1,NULL,NULL),(194,103,2,NULL,NULL),(195,104,1,NULL,NULL),(196,104,2,NULL,NULL),(197,105,1,NULL,NULL),(198,105,2,NULL,NULL),(199,106,1,NULL,NULL),(200,106,2,NULL,NULL),(201,107,1,NULL,NULL),(202,107,2,NULL,NULL),(203,108,1,NULL,NULL),(204,108,2,NULL,NULL),(205,109,1,22,NULL),(206,109,2,22,NULL),(207,110,1,22,NULL),(208,110,2,22,NULL),(209,111,1,18,'adm_name'),(210,112,1,36,'ids'),(211,113,1,36,'ids'),(212,114,1,36,'ids'),(213,115,1,36,'ids'),(214,116,1,36,'ids'),(215,117,1,19,NULL),(216,117,2,19,NULL),(217,118,1,20,NULL),(218,118,2,20,NULL),(219,119,1,21,NULL),(220,119,2,21,NULL),(221,120,1,22,NULL),(222,120,2,22,NULL),(223,121,1,23,NULL),(224,121,2,23,NULL),(225,122,1,24,NULL),(226,122,2,24,NULL),(227,123,1,19,NULL),(228,123,2,19,NULL),(229,124,1,20,NULL),(230,124,2,20,NULL),(231,125,1,21,NULL),(232,125,2,21,NULL),(233,126,1,22,NULL),(234,126,2,22,NULL),(235,127,1,23,NULL),(236,127,2,23,NULL),(237,128,1,24,NULL),(238,128,2,24,NULL),(239,129,1,19,NULL),(240,129,2,19,NULL),(241,130,1,20,NULL),(242,130,2,20,NULL),(243,131,1,21,NULL),(244,131,2,21,NULL),(245,132,1,22,NULL),(246,132,2,22,NULL),(247,133,1,23,NULL),(248,133,2,23,NULL),(249,134,1,24,NULL),(250,134,2,24,NULL),(251,135,1,19,NULL),(252,135,2,19,NULL),(253,136,1,20,NULL),(254,136,2,20,NULL),(255,137,1,21,NULL),(256,137,2,21,NULL),(257,138,1,22,NULL),(258,138,2,22,NULL),(259,139,1,23,NULL),(260,139,2,23,NULL),(261,140,1,24,NULL),(262,140,2,24,NULL),(263,141,1,19,NULL),(264,141,2,19,NULL),(265,142,1,20,NULL),(266,142,2,20,NULL),(267,143,1,21,NULL),(268,143,2,21,NULL),(269,144,1,22,NULL),(270,144,2,22,NULL),(271,145,1,23,NULL),(272,145,2,23,NULL),(273,146,1,24,NULL),(274,146,2,24,NULL),(275,147,1,19,NULL),(276,147,2,19,NULL),(277,148,1,20,NULL),(278,148,2,20,NULL),(279,149,1,21,NULL),(280,149,2,21,NULL),(281,150,1,22,NULL),(282,150,2,22,NULL),(283,151,1,23,NULL),(284,151,2,23,NULL),(285,152,1,24,NULL),(286,152,2,24,NULL),(287,153,1,22,NULL),(288,153,2,22,NULL),(289,154,1,22,NULL),(290,154,2,22,NULL),(291,155,1,22,NULL),(292,155,2,22,NULL),(293,156,1,22,NULL),(294,157,1,16,NULL),(295,157,2,16,NULL),(296,158,1,16,NULL),(297,158,2,16,NULL),(298,159,1,16,NULL),(299,160,1,22,NULL),(300,161,1,NULL,NULL),(301,161,2,NULL,NULL),(302,162,1,NULL,NULL),(303,162,2,NULL,NULL),(304,163,1,NULL,NULL),(305,163,2,NULL,NULL),(306,164,1,NULL,NULL),(307,164,2,NULL,NULL),(308,165,1,NULL,NULL),(309,165,2,NULL,NULL),(310,166,1,NULL,NULL),(311,166,2,NULL,NULL),(312,167,1,22,NULL),(313,167,2,22,NULL),(314,168,1,22,NULL),(315,168,2,22,NULL),(316,169,1,18,'adm_name'),(317,170,1,36,'ids'),(318,171,1,36,'ids'),(319,172,1,36,'ids'),(320,173,1,36,'ids'),(321,174,1,36,'ids'),(322,176,1,19,NULL),(323,176,2,19,NULL),(324,177,1,20,NULL),(325,177,2,20,NULL),(326,178,1,21,NULL),(327,178,2,21,NULL),(328,179,1,22,NULL),(329,179,2,22,NULL),(330,180,1,23,NULL),(331,180,2,23,NULL),(332,181,1,24,NULL),(333,181,2,24,NULL),(334,182,1,19,NULL),(335,182,2,19,NULL),(336,183,1,20,NULL),(337,183,2,20,NULL),(338,184,1,21,NULL),(339,184,2,21,NULL),(340,185,1,22,NULL),(341,185,2,22,NULL),(342,186,1,23,NULL),(343,186,2,23,NULL),(344,187,1,24,NULL),(345,187,2,24,NULL),(346,188,1,19,NULL),(347,188,2,19,NULL),(348,189,1,20,NULL),(349,189,2,20,NULL),(350,190,1,21,NULL),(351,190,2,21,NULL),(352,191,1,22,NULL),(353,191,2,22,NULL),(354,192,1,23,NULL),(355,192,2,23,NULL),(356,193,1,24,NULL),(357,193,2,24,NULL),(358,194,1,19,NULL),(359,194,2,19,NULL),(360,195,1,20,NULL),(361,195,2,20,NULL),(362,196,1,21,NULL),(363,196,2,21,NULL),(364,197,1,22,NULL),(365,197,2,22,NULL),(366,198,1,23,NULL),(367,198,2,23,NULL),(368,199,1,24,NULL),(369,199,2,24,NULL),(370,200,1,19,NULL),(371,200,2,19,NULL),(372,201,1,20,NULL),(373,201,2,20,NULL),(374,202,1,21,NULL),(375,202,2,21,NULL),(376,203,1,22,NULL),(377,203,2,22,NULL),(378,204,1,23,NULL),(379,204,2,23,NULL),(380,205,1,24,NULL),(381,205,2,24,NULL),(382,206,1,19,NULL),(383,206,2,19,NULL),(384,207,1,20,NULL),(385,207,2,20,NULL),(386,208,1,21,NULL),(387,208,2,21,NULL),(388,209,1,22,NULL),(389,209,2,22,NULL),(390,210,1,23,NULL),(391,210,2,23,NULL),(392,211,1,24,NULL),(393,211,2,24,NULL),(394,212,1,22,NULL),(395,212,2,22,NULL),(396,213,1,22,NULL),(397,213,2,22,NULL),(398,214,1,22,NULL),(399,214,2,22,NULL),(400,215,1,22,NULL),(401,216,1,16,NULL),(402,216,2,16,NULL),(403,217,1,16,NULL),(404,217,2,16,NULL),(405,218,1,16,NULL),(406,219,1,22,NULL),(407,220,1,NULL,NULL),(408,220,2,NULL,NULL),(409,221,1,NULL,NULL),(410,221,2,NULL,NULL),(411,222,1,NULL,NULL),(412,222,2,NULL,NULL),(413,223,1,NULL,NULL),(414,223,2,NULL,NULL),(415,224,1,NULL,NULL),(416,224,2,NULL,NULL),(417,225,1,NULL,NULL),(418,225,2,NULL,NULL),(419,226,1,22,NULL),(420,226,2,22,NULL),(421,227,1,22,NULL),(422,227,2,22,NULL),(423,228,1,18,'adm_name'),(424,229,1,36,'ids'),(425,230,1,36,'ids'),(426,231,1,36,'ids'),(427,232,1,36,'ids'),(428,233,1,36,'ids'),(429,234,1,19,NULL),(430,234,2,19,NULL),(431,235,1,20,NULL),(432,235,2,20,NULL),(433,236,1,21,NULL),(434,236,2,21,NULL),(435,237,1,22,NULL),(436,237,2,22,NULL),(437,238,1,23,NULL),(438,238,2,23,NULL),(439,239,1,24,NULL),(440,239,2,24,NULL),(441,240,1,19,NULL),(442,240,2,19,NULL),(443,241,1,20,NULL),(444,241,2,20,NULL),(445,242,1,21,NULL),(446,242,2,21,NULL),(447,243,1,22,NULL),(448,243,2,22,NULL),(449,244,1,23,NULL),(450,244,2,23,NULL),(451,245,1,24,NULL),(452,245,2,24,NULL),(453,246,1,19,NULL),(454,246,2,19,NULL),(455,247,1,20,NULL),(456,247,2,20,NULL),(457,248,1,21,NULL),(458,248,2,21,NULL),(459,249,1,22,NULL),(460,249,2,22,NULL),(461,250,1,23,NULL),(462,250,2,23,NULL),(463,251,1,24,NULL),(464,251,2,24,NULL),(465,252,1,19,NULL),(466,252,2,19,NULL),(467,253,1,20,NULL),(468,253,2,20,NULL),(469,254,1,21,NULL),(470,254,2,21,NULL),(471,255,1,22,NULL),(472,255,2,22,NULL),(473,256,1,23,NULL),(474,256,2,23,NULL),(475,257,1,24,NULL),(476,257,2,24,NULL),(477,258,1,19,NULL),(478,258,2,19,NULL),(479,259,1,20,NULL),(480,259,2,20,NULL),(481,260,1,21,NULL),(482,260,2,21,NULL),(483,261,1,22,NULL),(484,261,2,22,NULL),(485,262,1,23,NULL),(486,262,2,23,NULL),(487,263,1,24,NULL),(488,263,2,24,NULL),(489,264,1,19,NULL),(490,264,2,19,NULL),(491,265,1,20,NULL),(492,265,2,20,NULL),(493,266,1,21,NULL),(494,266,2,21,NULL),(495,267,1,22,NULL),(496,267,2,22,NULL),(497,268,1,23,NULL),(498,268,2,23,NULL),(499,269,1,24,NULL),(500,269,2,24,NULL),(501,270,1,22,NULL),(502,270,2,22,NULL),(503,271,1,22,NULL),(504,271,2,22,NULL),(505,272,1,22,NULL),(506,272,2,22,NULL),(507,273,1,22,NULL),(508,274,1,16,NULL),(509,274,2,16,NULL),(510,275,1,16,NULL),(511,275,2,16,NULL),(512,276,1,16,NULL),(513,277,1,22,NULL),(514,278,1,NULL,NULL),(515,278,2,NULL,NULL),(516,279,1,NULL,NULL),(517,279,2,NULL,NULL),(518,280,1,NULL,NULL),(519,280,2,NULL,NULL),(520,281,1,NULL,NULL),(521,281,2,NULL,NULL),(522,282,1,NULL,NULL),(523,282,2,NULL,NULL),(524,283,1,NULL,NULL),(525,283,2,NULL,NULL),(526,284,1,22,NULL),(527,284,2,22,NULL),(528,285,1,22,NULL),(529,285,2,22,NULL),(530,286,1,18,'adm_name'),(531,287,1,36,'ids'),(532,288,1,36,'ids'),(533,289,1,36,'ids'),(534,290,1,36,'ids'),(535,291,1,36,'ids'),(536,293,1,19,NULL),(537,293,2,19,NULL),(538,294,1,20,NULL),(539,294,2,20,NULL),(540,295,1,21,NULL),(541,295,2,21,NULL),(542,296,1,22,NULL),(543,296,2,22,NULL),(544,297,1,23,NULL),(545,297,2,23,NULL),(546,298,1,24,NULL),(547,298,2,24,NULL),(548,299,1,19,NULL),(549,299,2,19,NULL),(550,300,1,20,NULL),(551,300,2,20,NULL),(552,301,1,21,NULL),(553,301,2,21,NULL),(554,302,1,22,NULL),(555,302,2,22,NULL),(556,303,1,23,NULL),(557,303,2,23,NULL),(558,304,1,24,NULL),(559,304,2,24,NULL),(560,305,1,19,NULL),(561,305,2,19,NULL),(562,306,1,20,NULL),(563,306,2,20,NULL),(564,307,1,21,NULL),(565,307,2,21,NULL),(566,308,1,22,NULL),(567,308,2,22,NULL),(568,309,1,23,NULL),(569,309,2,23,NULL),(570,310,1,24,NULL),(571,310,2,24,NULL),(572,311,1,19,NULL),(573,311,2,19,NULL),(574,312,1,20,NULL),(575,312,2,20,NULL),(576,313,1,21,NULL),(577,313,2,21,NULL),(578,314,1,22,NULL),(579,314,2,22,NULL),(580,315,1,23,NULL),(581,315,2,23,NULL),(582,316,1,24,NULL),(583,316,2,24,NULL),(584,317,1,19,NULL),(585,317,2,19,NULL),(586,318,1,20,NULL),(587,318,2,20,NULL),(588,319,1,21,NULL),(589,319,2,21,NULL),(590,320,1,22,NULL),(591,320,2,22,NULL),(592,321,1,23,NULL),(593,321,2,23,NULL),(594,322,1,24,NULL),(595,322,2,24,NULL),(596,323,1,19,NULL),(597,323,2,19,NULL),(598,324,1,20,NULL),(599,324,2,20,NULL),(600,325,1,21,NULL),(601,325,2,21,NULL),(602,326,1,22,NULL),(603,326,2,22,NULL),(604,327,1,23,NULL),(605,327,2,23,NULL),(606,328,1,24,NULL),(607,328,2,24,NULL),(608,329,1,22,NULL),(609,329,2,22,NULL),(610,330,1,22,NULL),(611,330,2,22,NULL),(612,331,1,22,NULL),(613,331,2,22,NULL),(614,332,1,22,NULL),(615,333,1,16,NULL),(616,333,2,16,NULL),(617,334,1,16,NULL),(618,334,2,16,NULL),(619,335,1,16,NULL),(620,336,1,22,NULL),(621,337,1,NULL,NULL),(622,337,2,NULL,NULL),(623,338,1,NULL,NULL),(624,338,2,NULL,NULL),(625,339,1,NULL,NULL),(626,339,2,NULL,NULL),(627,340,1,NULL,NULL),(628,340,2,NULL,NULL),(629,341,1,NULL,NULL),(630,341,2,NULL,NULL),(631,342,1,NULL,NULL),(632,342,2,NULL,NULL),(633,343,1,22,NULL),(634,343,2,22,NULL),(635,344,1,22,NULL),(636,344,2,22,NULL),(637,345,1,18,'adm_name'),(638,346,1,36,'ids'),(639,347,1,36,'ids'),(640,348,1,36,'ids'),(641,349,1,36,'ids'),(642,350,1,36,'ids'),(643,352,1,19,NULL),(644,352,2,19,NULL),(645,353,1,20,NULL),(646,353,2,20,NULL),(647,354,1,21,NULL),(648,354,2,21,NULL),(649,355,1,22,NULL),(650,355,2,22,NULL),(651,356,1,23,NULL),(652,356,2,23,NULL),(653,357,1,24,NULL),(654,357,2,24,NULL),(655,358,1,19,NULL),(656,358,2,19,NULL),(657,359,1,20,NULL),(658,359,2,20,NULL),(659,360,1,21,NULL),(660,360,2,21,NULL),(661,361,1,22,NULL),(662,361,2,22,NULL),(663,362,1,23,NULL),(664,362,2,23,NULL),(665,363,1,24,NULL),(666,363,2,24,NULL),(667,364,1,19,NULL),(668,364,2,19,NULL),(669,365,1,20,NULL),(670,365,2,20,NULL),(671,366,1,21,NULL),(672,366,2,21,NULL),(673,367,1,22,NULL),(674,367,2,22,NULL),(675,368,1,23,NULL),(676,368,2,23,NULL),(677,369,1,24,NULL),(678,369,2,24,NULL),(679,370,1,19,NULL),(680,370,2,19,NULL),(681,371,1,20,NULL),(682,371,2,20,NULL),(683,372,1,21,NULL),(684,372,2,21,NULL),(685,373,1,22,NULL),(686,373,2,22,NULL),(687,374,1,23,NULL),(688,374,2,23,NULL),(689,375,1,24,NULL),(690,375,2,24,NULL),(691,376,1,19,NULL),(692,376,2,19,NULL),(693,377,1,20,NULL),(694,377,2,20,NULL),(695,378,1,21,NULL),(696,378,2,21,NULL),(697,379,1,22,NULL),(698,379,2,22,NULL),(699,380,1,23,NULL),(700,380,2,23,NULL),(701,381,1,24,NULL),(702,381,2,24,NULL),(703,382,1,19,NULL),(704,382,2,19,NULL),(705,383,1,20,NULL),(706,383,2,20,NULL),(707,384,1,21,NULL),(708,384,2,21,NULL),(709,385,1,22,NULL),(710,385,2,22,NULL),(711,386,1,23,NULL),(712,386,2,23,NULL),(713,387,1,24,NULL),(714,387,2,24,NULL),(715,388,1,22,NULL),(716,388,2,22,NULL),(717,389,1,22,NULL),(718,389,2,22,NULL),(719,390,1,22,NULL),(720,390,2,22,NULL),(721,391,1,22,NULL),(722,392,1,16,NULL),(723,392,2,16,NULL),(724,393,1,16,NULL),(725,393,2,16,NULL),(726,394,1,16,NULL),(727,395,1,22,NULL),(728,396,1,NULL,NULL),(729,396,2,NULL,NULL),(730,397,1,NULL,NULL),(731,397,2,NULL,NULL),(732,398,1,NULL,NULL),(733,398,2,NULL,NULL),(734,399,1,NULL,NULL),(735,399,2,NULL,NULL),(736,400,1,NULL,NULL),(737,400,2,NULL,NULL),(738,401,1,NULL,NULL),(739,401,2,NULL,NULL),(740,402,1,22,NULL),(741,402,2,22,NULL),(742,403,1,22,NULL),(743,403,2,22,NULL),(744,404,1,19,NULL),(745,404,2,19,NULL),(746,405,1,20,NULL),(747,405,2,20,NULL),(748,406,1,21,NULL),(749,406,2,21,NULL),(750,407,1,22,NULL),(751,407,2,22,NULL),(752,408,1,23,NULL),(753,408,2,23,NULL),(754,409,1,24,NULL),(755,409,2,24,NULL),(756,410,1,19,NULL),(757,410,2,19,NULL),(758,411,1,20,NULL),(759,411,2,20,NULL),(760,412,1,21,NULL),(761,412,2,21,NULL),(762,413,1,22,NULL),(763,413,2,22,NULL),(764,414,1,23,NULL),(765,414,2,23,NULL),(766,415,1,24,NULL),(767,415,2,24,NULL),(768,416,1,19,NULL),(769,416,2,19,NULL),(770,417,1,20,NULL),(771,417,2,20,NULL),(772,418,1,21,NULL),(773,418,2,21,NULL),(774,419,1,22,NULL),(775,419,2,22,NULL),(776,420,1,23,NULL),(777,420,2,23,NULL),(778,421,1,24,NULL),(779,421,2,24,NULL),(780,422,1,19,NULL),(781,422,2,19,NULL),(782,423,1,20,NULL),(783,423,2,20,NULL),(784,424,1,21,NULL),(785,424,2,21,NULL),(786,425,1,22,NULL),(787,425,2,22,NULL),(788,426,1,23,NULL),(789,426,2,23,NULL),(790,427,1,24,NULL),(791,427,2,24,NULL),(792,428,1,19,NULL),(793,428,2,19,NULL),(794,429,1,20,NULL),(795,429,2,20,NULL),(796,430,1,21,NULL),(797,430,2,21,NULL),(798,431,1,22,NULL),(799,431,2,22,NULL),(800,432,1,23,NULL),(801,432,2,23,NULL),(802,433,1,24,NULL),(803,433,2,24,NULL),(804,434,1,19,NULL),(805,434,2,19,NULL),(806,435,1,20,NULL),(807,435,2,20,NULL),(808,436,1,21,NULL),(809,436,2,21,NULL),(810,437,1,22,NULL),(811,437,2,22,NULL),(812,438,1,23,NULL),(813,438,2,23,NULL),(814,439,1,24,NULL),(815,439,2,24,NULL),(816,440,1,22,NULL),(817,440,2,22,NULL),(818,441,1,22,NULL),(819,441,2,22,NULL),(820,442,1,22,NULL),(821,442,2,22,NULL),(822,443,1,22,NULL),(823,444,1,16,NULL),(824,444,2,16,NULL),(825,445,1,16,NULL),(826,445,2,16,NULL),(827,446,1,16,NULL),(828,447,1,22,NULL),(829,448,1,NULL,NULL),(830,448,2,NULL,NULL),(831,449,1,NULL,NULL),(832,449,2,NULL,NULL),(833,450,1,NULL,NULL),(834,450,2,NULL,NULL),(835,451,1,NULL,NULL),(836,451,2,NULL,NULL),(837,452,1,NULL,NULL),(838,452,2,NULL,NULL),(839,453,1,NULL,NULL),(840,453,2,NULL,NULL),(841,454,1,22,NULL),(842,454,2,22,NULL),(843,455,1,22,NULL),(844,455,2,22,NULL),(845,456,1,19,NULL),(846,456,2,19,NULL),(847,457,1,20,NULL),(848,457,2,20,NULL),(849,458,1,21,NULL),(850,458,2,21,NULL),(851,459,1,22,NULL),(852,459,2,22,NULL),(853,460,1,23,NULL),(854,460,2,23,NULL),(855,461,1,24,NULL),(856,461,2,24,NULL),(857,462,1,19,NULL),(858,462,2,19,NULL),(859,463,1,20,NULL),(860,463,2,20,NULL),(861,464,1,21,NULL),(862,464,2,21,NULL),(863,465,1,22,NULL),(864,465,2,22,NULL),(865,466,1,23,NULL),(866,466,2,23,NULL),(867,467,1,24,NULL),(868,467,2,24,NULL),(869,468,1,19,NULL),(870,468,2,19,NULL),(871,469,1,20,NULL),(872,469,2,20,NULL),(873,470,1,21,NULL),(874,470,2,21,NULL),(875,471,1,22,NULL),(876,471,2,22,NULL),(877,472,1,23,NULL),(878,472,2,23,NULL),(879,473,1,24,NULL),(880,473,2,24,NULL),(881,474,1,19,NULL),(882,474,2,19,NULL),(883,475,1,20,NULL),(884,475,2,20,NULL),(885,476,1,21,NULL),(886,476,2,21,NULL),(887,477,1,22,NULL),(888,477,2,22,NULL),(889,478,1,23,NULL),(890,478,2,23,NULL),(891,479,1,24,NULL),(892,479,2,24,NULL),(893,480,1,19,NULL),(894,480,2,19,NULL),(895,481,1,20,NULL),(896,481,2,20,NULL),(897,482,1,21,NULL),(898,482,2,21,NULL),(899,483,1,22,NULL),(900,483,2,22,NULL),(901,484,1,23,NULL),(902,484,2,23,NULL),(903,485,1,24,NULL),(904,485,2,24,NULL),(905,486,1,19,NULL),(906,486,2,19,NULL),(907,487,1,20,NULL),(908,487,2,20,NULL),(909,488,1,21,NULL),(910,488,2,21,NULL),(911,489,1,22,NULL),(912,489,2,22,NULL),(913,490,1,23,NULL),(914,490,2,23,NULL),(915,491,1,24,NULL),(916,491,2,24,NULL),(917,492,1,22,NULL),(918,492,2,22,NULL),(919,493,1,22,NULL),(920,493,2,22,NULL),(921,494,1,22,NULL),(922,494,2,22,NULL),(923,495,1,22,NULL),(924,496,1,16,NULL),(925,496,2,16,NULL),(926,497,1,16,NULL),(927,497,2,16,NULL),(928,498,1,16,NULL),(929,499,1,22,NULL),(930,500,1,NULL,NULL),(931,500,2,NULL,NULL),(932,501,1,NULL,NULL),(933,501,2,NULL,NULL),(934,502,1,NULL,NULL),(935,502,2,NULL,NULL),(936,503,1,NULL,NULL),(937,503,2,NULL,NULL),(938,504,1,NULL,NULL),(939,504,2,NULL,NULL),(940,505,1,NULL,NULL),(941,505,2,NULL,NULL),(942,506,1,22,NULL),(943,506,2,22,NULL),(944,507,1,22,NULL),(945,507,2,22,NULL),(946,508,1,18,'adm_name'),(947,509,1,36,'ids'),(948,510,1,36,'ids'),(949,511,1,36,'ids'),(950,512,1,36,'ids'),(951,513,1,36,'ids'),(952,514,1,19,NULL),(953,514,2,19,NULL),(954,515,1,20,NULL),(955,515,2,20,NULL),(956,516,1,21,NULL),(957,516,2,21,NULL),(958,517,1,22,NULL),(959,517,2,22,NULL),(960,518,1,23,NULL),(961,518,2,23,NULL),(962,519,1,24,NULL),(963,519,2,24,NULL),(964,520,1,19,NULL),(965,520,2,19,NULL),(966,521,1,20,NULL),(967,521,2,20,NULL),(968,522,1,21,NULL),(969,522,2,21,NULL),(970,523,1,22,NULL),(971,523,2,22,NULL),(972,524,1,23,NULL),(973,524,2,23,NULL),(974,525,1,24,NULL),(975,525,2,24,NULL),(976,526,1,19,NULL),(977,526,2,19,NULL),(978,527,1,20,NULL),(979,527,2,20,NULL),(980,528,1,21,NULL),(981,528,2,21,NULL),(982,529,1,22,NULL),(983,529,2,22,NULL),(984,530,1,23,NULL),(985,530,2,23,NULL),(986,531,1,24,NULL),(987,531,2,24,NULL),(988,532,1,19,NULL),(989,532,2,19,NULL),(990,533,1,20,NULL),(991,533,2,20,NULL),(992,534,1,21,NULL),(993,534,2,21,NULL),(994,535,1,22,NULL),(995,535,2,22,NULL),(996,536,1,23,NULL),(997,536,2,23,NULL),(998,537,1,24,NULL),(999,537,2,24,NULL),(1000,538,1,19,NULL),(1001,538,2,19,NULL),(1002,539,1,20,NULL),(1003,539,2,20,NULL),(1004,540,1,21,NULL),(1005,540,2,21,NULL),(1006,541,1,22,NULL),(1007,541,2,22,NULL),(1008,542,1,23,NULL),(1009,542,2,23,NULL),(1010,543,1,24,NULL),(1011,543,2,24,NULL),(1012,544,1,19,NULL),(1013,544,2,19,NULL),(1014,545,1,20,NULL),(1015,545,2,20,NULL),(1016,546,1,21,NULL),(1017,546,2,21,NULL),(1018,547,1,22,NULL),(1019,547,2,22,NULL),(1020,548,1,23,NULL),(1021,548,2,23,NULL),(1022,549,1,24,NULL),(1023,549,2,24,NULL),(1024,550,1,22,NULL),(1025,550,2,22,NULL),(1026,551,1,22,NULL),(1027,551,2,22,NULL),(1028,552,1,22,NULL),(1029,552,2,22,NULL),(1030,553,1,22,NULL),(1031,554,1,16,NULL),(1032,554,2,16,NULL),(1033,555,1,16,NULL),(1034,555,2,16,NULL),(1035,556,1,16,NULL),(1036,557,1,22,NULL),(1037,558,1,NULL,NULL),(1038,558,2,NULL,NULL),(1039,559,1,NULL,NULL),(1040,559,2,NULL,NULL),(1041,560,1,NULL,NULL),(1042,560,2,NULL,NULL),(1043,561,1,NULL,NULL),(1044,561,2,NULL,NULL),(1045,562,1,NULL,NULL),(1046,562,2,NULL,NULL),(1047,563,1,NULL,NULL),(1048,563,2,NULL,NULL),(1049,564,1,22,NULL),(1050,564,2,22,NULL),(1051,565,1,22,NULL),(1052,565,2,22,NULL),(1053,566,1,18,'adm_name'),(1054,567,1,36,'ids'),(1055,568,1,36,'ids'),(1056,569,1,36,'ids'),(1057,570,1,36,'ids'),(1058,571,1,36,'ids'),(1059,572,1,19,NULL),(1060,572,2,19,NULL),(1061,573,1,20,NULL),(1062,573,2,20,NULL),(1063,574,1,21,NULL),(1064,574,2,21,NULL),(1065,575,1,22,NULL),(1066,575,2,22,NULL),(1067,576,1,23,NULL),(1068,576,2,23,NULL),(1069,577,1,24,NULL),(1070,577,2,24,NULL),(1071,578,1,19,NULL),(1072,578,2,19,NULL),(1073,579,1,20,NULL),(1074,579,2,20,NULL),(1075,580,1,21,NULL),(1076,580,2,21,NULL),(1077,581,1,22,NULL),(1078,581,2,22,NULL),(1079,582,1,23,NULL),(1080,582,2,23,NULL),(1081,583,1,24,NULL),(1082,583,2,24,NULL),(1083,584,1,19,NULL),(1084,584,2,19,NULL),(1085,585,1,20,NULL),(1086,585,2,20,NULL),(1087,586,1,21,NULL),(1088,586,2,21,NULL),(1089,587,1,22,NULL),(1090,587,2,22,NULL),(1091,588,1,23,NULL),(1092,588,2,23,NULL),(1093,589,1,24,NULL),(1094,589,2,24,NULL),(1095,590,1,19,NULL),(1096,590,2,19,NULL),(1097,591,1,20,NULL),(1098,591,2,20,NULL),(1099,592,1,21,NULL),(1100,592,2,21,NULL),(1101,593,1,22,NULL),(1102,593,2,22,NULL),(1103,594,1,23,NULL),(1104,594,2,23,NULL),(1105,595,1,24,NULL),(1106,595,2,24,NULL),(1107,596,1,19,NULL),(1108,596,2,19,NULL),(1109,597,1,20,NULL),(1110,597,2,20,NULL),(1111,598,1,21,NULL),(1112,598,2,21,NULL),(1113,599,1,22,NULL),(1114,599,2,22,NULL),(1115,600,1,23,NULL),(1116,600,2,23,NULL),(1117,601,1,24,NULL),(1118,601,2,24,NULL),(1119,602,1,19,NULL),(1120,602,2,19,NULL),(1121,603,1,20,NULL),(1122,603,2,20,NULL),(1123,604,1,21,NULL),(1124,604,2,21,NULL),(1125,605,1,22,NULL),(1126,605,2,22,NULL),(1127,606,1,23,NULL),(1128,606,2,23,NULL),(1129,607,1,24,NULL),(1130,607,2,24,NULL),(1131,608,1,22,NULL),(1132,608,2,22,NULL),(1133,609,1,22,NULL),(1134,609,2,22,NULL),(1135,610,1,22,NULL),(1136,610,2,22,NULL),(1137,611,1,22,NULL),(1138,612,1,16,NULL),(1139,612,2,16,NULL),(1140,613,1,16,NULL),(1141,613,2,16,NULL),(1142,614,1,16,NULL),(1143,615,1,22,NULL),(1144,616,1,NULL,NULL),(1145,616,2,NULL,NULL),(1146,617,1,NULL,NULL),(1147,617,2,NULL,NULL),(1148,618,1,NULL,NULL),(1149,618,2,NULL,NULL),(1150,619,1,NULL,NULL),(1151,619,2,NULL,NULL),(1152,620,1,NULL,NULL),(1153,620,2,NULL,NULL),(1154,621,1,NULL,NULL),(1155,621,2,NULL,NULL),(1156,622,1,22,NULL),(1157,622,2,22,NULL),(1158,623,1,22,NULL),(1159,623,2,22,NULL),(1160,624,1,18,'adm_name'),(1161,625,1,36,'ids'),(1162,626,1,36,'ids'),(1163,627,1,36,'ids'),(1164,628,1,36,'ids'),(1165,629,1,36,'ids'),(1166,630,1,19,NULL),(1167,630,2,19,NULL),(1168,631,1,20,NULL),(1169,631,2,20,NULL),(1170,632,1,21,NULL),(1171,632,2,21,NULL),(1172,633,1,22,NULL),(1173,633,2,22,NULL),(1174,634,1,23,NULL),(1175,634,2,23,NULL),(1176,635,1,24,NULL),(1177,635,2,24,NULL),(1178,636,1,19,NULL),(1179,636,2,19,NULL),(1180,637,1,20,NULL),(1181,637,2,20,NULL),(1182,638,1,21,NULL),(1183,638,2,21,NULL),(1184,639,1,22,NULL),(1185,639,2,22,NULL),(1186,640,1,23,NULL),(1187,640,2,23,NULL),(1188,641,1,24,NULL),(1189,641,2,24,NULL),(1190,642,1,19,NULL),(1191,642,2,19,NULL),(1192,643,1,20,NULL),(1193,643,2,20,NULL),(1194,644,1,21,NULL),(1195,644,2,21,NULL),(1196,645,1,22,NULL),(1197,645,2,22,NULL),(1198,646,1,23,NULL),(1199,646,2,23,NULL),(1200,647,1,24,NULL),(1201,647,2,24,NULL),(1202,648,1,19,NULL),(1203,648,2,19,NULL),(1204,649,1,20,NULL),(1205,649,2,20,NULL),(1206,650,1,21,NULL),(1207,650,2,21,NULL),(1208,651,1,22,NULL),(1209,651,2,22,NULL),(1210,652,1,23,NULL),(1211,652,2,23,NULL),(1212,653,1,24,NULL),(1213,653,2,24,NULL),(1214,654,1,19,NULL),(1215,654,2,19,NULL),(1216,655,1,20,NULL),(1217,655,2,20,NULL),(1218,656,1,21,NULL),(1219,656,2,21,NULL),(1220,657,1,22,NULL),(1221,657,2,22,NULL),(1222,658,1,23,NULL),(1223,658,2,23,NULL),(1224,659,1,24,NULL),(1225,659,2,24,NULL),(1226,660,1,19,NULL),(1227,660,2,19,NULL),(1228,661,1,20,NULL),(1229,661,2,20,NULL),(1230,662,1,21,NULL),(1231,662,2,21,NULL),(1232,663,1,22,NULL),(1233,663,2,22,NULL),(1234,664,1,23,NULL),(1235,664,2,23,NULL),(1236,665,1,24,NULL),(1237,665,2,24,NULL),(1238,666,1,22,NULL),(1239,666,2,22,NULL),(1240,667,1,22,NULL),(1241,667,2,22,NULL),(1242,668,1,22,NULL),(1243,668,2,22,NULL),(1244,669,1,22,NULL),(1245,670,1,16,NULL),(1246,670,2,16,NULL),(1247,671,1,16,NULL),(1248,671,2,16,NULL),(1249,672,1,16,NULL),(1250,673,1,22,NULL),(1251,675,1,19,NULL),(1252,675,2,19,NULL),(1253,676,1,20,NULL),(1254,676,2,20,NULL),(1255,677,1,21,NULL),(1256,677,2,21,NULL),(1257,678,1,22,NULL),(1258,678,2,22,NULL),(1259,679,1,23,NULL),(1260,679,2,23,NULL),(1261,680,1,24,NULL),(1262,680,2,24,NULL),(1263,681,1,19,NULL),(1264,681,2,19,NULL),(1265,682,1,20,NULL),(1266,682,2,20,NULL),(1267,683,1,21,NULL),(1268,683,2,21,NULL),(1269,684,1,22,NULL),(1270,684,2,22,NULL),(1271,685,1,23,NULL),(1272,685,2,23,NULL),(1273,686,1,24,NULL),(1274,686,2,24,NULL),(1275,687,1,19,NULL),(1276,687,2,19,NULL),(1277,688,1,20,NULL),(1278,688,2,20,NULL),(1279,689,1,21,NULL),(1280,689,2,21,NULL),(1281,690,1,22,NULL),(1282,690,2,22,NULL),(1283,691,1,23,NULL),(1284,691,2,23,NULL),(1285,692,1,24,NULL),(1286,692,2,24,NULL),(1287,693,1,19,NULL),(1288,693,2,19,NULL),(1289,694,1,20,NULL),(1290,694,2,20,NULL),(1291,695,1,21,NULL),(1292,695,2,21,NULL),(1293,696,1,22,NULL),(1294,696,2,22,NULL),(1295,697,1,23,NULL),(1296,697,2,23,NULL),(1297,698,1,24,NULL),(1298,698,2,24,NULL),(1299,699,1,19,NULL),(1300,699,2,19,NULL),(1301,700,1,20,NULL),(1302,700,2,20,NULL),(1303,701,1,21,NULL),(1304,701,2,21,NULL),(1305,702,1,22,NULL),(1306,702,2,22,NULL),(1307,703,1,23,NULL),(1308,703,2,23,NULL),(1309,704,1,24,NULL),(1310,704,2,24,NULL),(1311,705,1,19,NULL),(1312,705,2,19,NULL),(1313,706,1,20,NULL),(1314,706,2,20,NULL),(1315,707,1,21,NULL),(1316,707,2,21,NULL),(1317,708,1,22,NULL),(1318,708,2,22,NULL),(1319,709,1,23,NULL),(1320,709,2,23,NULL),(1321,710,1,24,NULL),(1322,710,2,24,NULL),(1323,711,1,22,NULL),(1324,711,2,22,NULL),(1325,712,1,22,NULL),(1326,712,2,22,NULL),(1327,713,1,22,NULL),(1328,713,2,22,NULL),(1329,714,1,22,NULL),(1330,715,1,16,NULL),(1331,715,2,16,NULL),(1332,716,1,16,NULL),(1333,716,2,16,NULL),(1334,717,1,16,NULL),(1335,718,1,22,NULL),(1336,719,1,NULL,NULL),(1337,719,2,NULL,NULL),(1338,720,1,NULL,NULL),(1339,720,2,NULL,NULL),(1340,721,1,NULL,NULL),(1341,721,2,NULL,NULL),(1342,722,1,NULL,NULL),(1343,722,2,NULL,NULL),(1344,723,1,NULL,NULL),(1345,723,2,NULL,NULL),(1346,724,1,NULL,NULL),(1347,724,2,NULL,NULL),(1348,725,1,22,NULL),(1349,725,2,22,NULL),(1350,726,1,22,NULL),(1351,726,2,22,NULL),(1352,727,1,18,'adm_name'),(1353,728,1,36,'ids'),(1354,729,1,36,'ids'),(1355,730,1,36,'ids'),(1356,731,1,36,'ids'),(1357,732,1,36,'ids'),(1358,734,1,19,NULL),(1359,734,2,19,NULL),(1360,735,1,20,NULL),(1361,735,2,20,NULL),(1362,736,1,21,NULL),(1363,736,2,21,NULL),(1364,737,1,22,NULL),(1365,737,2,22,NULL),(1366,738,1,23,NULL),(1367,738,2,23,NULL),(1368,739,1,24,NULL),(1369,739,2,24,NULL),(1370,740,1,19,NULL),(1371,740,2,19,NULL),(1372,741,1,20,NULL),(1373,741,2,20,NULL),(1374,742,1,21,NULL),(1375,742,2,21,NULL),(1376,743,1,22,NULL),(1377,743,2,22,NULL),(1378,744,1,23,NULL),(1379,744,2,23,NULL),(1380,745,1,24,NULL),(1381,745,2,24,NULL),(1382,746,1,19,NULL),(1383,746,2,19,NULL),(1384,747,1,20,NULL),(1385,747,2,20,NULL),(1386,748,1,21,NULL),(1387,748,2,21,NULL),(1388,749,1,22,NULL),(1389,749,2,22,NULL),(1390,750,1,23,NULL),(1391,750,2,23,NULL),(1392,751,1,24,NULL),(1393,751,2,24,NULL),(1394,752,1,19,NULL),(1395,752,2,19,NULL),(1396,753,1,20,NULL),(1397,753,2,20,NULL),(1398,754,1,21,NULL),(1399,754,2,21,NULL),(1400,755,1,22,NULL),(1401,755,2,22,NULL),(1402,756,1,23,NULL),(1403,756,2,23,NULL),(1404,757,1,24,NULL),(1405,757,2,24,NULL),(1406,758,1,19,NULL),(1407,758,2,19,NULL),(1408,759,1,20,NULL),(1409,759,2,20,NULL),(1410,760,1,21,NULL),(1411,760,2,21,NULL),(1412,761,1,22,NULL),(1413,761,2,22,NULL),(1414,762,1,23,NULL),(1415,762,2,23,NULL),(1416,763,1,24,NULL),(1417,763,2,24,NULL),(1418,764,1,19,NULL),(1419,764,2,19,NULL),(1420,765,1,20,NULL),(1421,765,2,20,NULL),(1422,766,1,21,NULL),(1423,766,2,21,NULL),(1424,767,1,22,NULL),(1425,767,2,22,NULL),(1426,768,1,23,NULL),(1427,768,2,23,NULL),(1428,769,1,24,NULL),(1429,769,2,24,NULL),(1430,770,1,22,NULL),(1431,770,2,22,NULL),(1432,771,1,22,NULL),(1433,771,2,22,NULL),(1434,772,1,22,NULL),(1435,772,2,22,NULL),(1436,773,1,22,NULL),(1437,774,1,16,NULL),(1438,774,2,16,NULL),(1439,775,1,16,NULL),(1440,775,2,16,NULL),(1441,776,1,16,NULL),(1442,777,1,22,NULL),(1443,778,1,NULL,NULL),(1444,778,2,NULL,NULL),(1445,779,1,NULL,NULL),(1446,779,2,NULL,NULL),(1447,780,1,NULL,NULL),(1448,780,2,NULL,NULL),(1449,781,1,NULL,NULL),(1450,781,2,NULL,NULL),(1451,782,1,NULL,NULL),(1452,782,2,NULL,NULL),(1453,783,1,NULL,NULL),(1454,783,2,NULL,NULL),(1455,784,1,22,NULL),(1456,784,2,22,NULL),(1457,785,1,22,NULL),(1458,785,2,22,NULL),(1459,786,1,18,'adm_name'),(1460,787,1,36,'ids'),(1461,788,1,36,'ids'),(1462,789,1,36,'ids'),(1463,790,1,36,'ids'),(1464,791,1,36,'ids'),(1465,794,1,19,NULL),(1466,794,2,19,NULL),(1467,795,1,20,NULL),(1468,795,2,20,NULL),(1469,796,1,21,NULL),(1470,796,2,21,NULL),(1471,797,1,22,NULL),(1472,797,2,22,NULL),(1473,798,1,23,NULL),(1474,798,2,23,NULL),(1475,799,1,24,NULL),(1476,799,2,24,NULL),(1477,800,1,19,NULL),(1478,800,2,19,NULL),(1479,801,1,20,NULL),(1480,801,2,20,NULL),(1481,802,1,21,NULL),(1482,802,2,21,NULL),(1483,803,1,22,NULL),(1484,803,2,22,NULL),(1485,804,1,23,NULL),(1486,804,2,23,NULL),(1487,805,1,24,NULL),(1488,805,2,24,NULL),(1489,806,1,19,NULL),(1490,806,2,19,NULL),(1491,807,1,20,NULL),(1492,807,2,20,NULL),(1493,808,1,21,NULL),(1494,808,2,21,NULL),(1495,809,1,22,NULL),(1496,809,2,22,NULL),(1497,810,1,23,NULL),(1498,810,2,23,NULL),(1499,811,1,24,NULL),(1500,811,2,24,NULL),(1501,812,1,19,NULL),(1502,812,2,19,NULL),(1503,813,1,20,NULL),(1504,813,2,20,NULL),(1505,814,1,21,NULL),(1506,814,2,21,NULL),(1507,815,1,22,NULL),(1508,815,2,22,NULL),(1509,816,1,23,NULL),(1510,816,2,23,NULL),(1511,817,1,24,NULL),(1512,817,2,24,NULL),(1513,818,1,19,NULL),(1514,818,2,19,NULL),(1515,819,1,20,NULL),(1516,819,2,20,NULL),(1517,820,1,21,NULL),(1518,820,2,21,NULL),(1519,821,1,22,NULL),(1520,821,2,22,NULL),(1521,822,1,23,NULL),(1522,822,2,23,NULL),(1523,823,1,24,NULL),(1524,823,2,24,NULL),(1525,824,1,19,NULL),(1526,824,2,19,NULL),(1527,825,1,20,NULL),(1528,825,2,20,NULL),(1529,826,1,21,NULL),(1530,826,2,21,NULL),(1531,827,1,22,NULL),(1532,827,2,22,NULL),(1533,828,1,23,NULL),(1534,828,2,23,NULL),(1535,829,1,24,NULL),(1536,829,2,24,NULL),(1537,830,1,22,NULL),(1538,830,2,22,NULL),(1539,831,1,22,NULL),(1540,831,2,22,NULL),(1541,832,1,22,NULL),(1542,832,2,22,NULL),(1543,833,1,22,NULL),(1544,834,1,16,NULL),(1545,834,2,16,NULL),(1546,835,1,16,NULL),(1547,835,2,16,NULL),(1548,836,1,16,NULL),(1549,837,1,22,NULL),(1550,838,1,NULL,NULL),(1551,838,2,NULL,NULL),(1552,839,1,NULL,NULL),(1553,839,2,NULL,NULL),(1554,840,1,NULL,NULL),(1555,840,2,NULL,NULL),(1556,841,1,NULL,NULL),(1557,841,2,NULL,NULL),(1558,842,1,NULL,NULL),(1559,842,2,NULL,NULL),(1560,843,1,NULL,NULL),(1561,843,2,NULL,NULL),(1562,844,1,22,NULL),(1563,844,2,22,NULL),(1564,845,1,22,NULL),(1565,845,2,22,NULL),(1566,846,1,18,'adm_name'),(1567,847,1,36,'ids'),(1568,848,1,36,'ids'),(1569,849,1,36,'ids'),(1570,850,1,36,'ids'),(1571,851,1,36,'ids'),(1572,852,1,19,NULL),(1573,852,2,19,NULL),(1574,853,1,20,NULL),(1575,853,2,20,NULL),(1576,854,1,21,NULL),(1577,854,2,21,NULL),(1578,855,1,22,NULL),(1579,855,2,22,NULL),(1580,856,1,23,NULL),(1581,856,2,23,NULL),(1582,857,1,24,NULL),(1583,857,2,24,NULL),(1584,858,1,19,NULL),(1585,858,2,19,NULL),(1586,859,1,20,NULL),(1587,859,2,20,NULL),(1588,860,1,21,NULL),(1589,860,2,21,NULL),(1590,861,1,22,NULL),(1591,861,2,22,NULL),(1592,862,1,23,NULL),(1593,862,2,23,NULL),(1594,863,1,24,NULL),(1595,863,2,24,NULL),(1596,864,1,19,NULL),(1597,864,2,19,NULL),(1598,865,1,20,NULL),(1599,865,2,20,NULL),(1600,866,1,21,NULL),(1601,866,2,21,NULL),(1602,867,1,22,NULL),(1603,867,2,22,NULL),(1604,868,1,23,NULL),(1605,868,2,23,NULL),(1606,869,1,24,NULL),(1607,869,2,24,NULL),(1608,870,1,19,NULL),(1609,870,2,19,NULL),(1610,871,1,20,NULL),(1611,871,2,20,NULL),(1612,872,1,21,NULL),(1613,872,2,21,NULL),(1614,873,1,22,NULL),(1615,873,2,22,NULL),(1616,874,1,23,NULL),(1617,874,2,23,NULL),(1618,875,1,24,NULL),(1619,875,2,24,NULL),(1620,876,1,19,NULL),(1621,876,2,19,NULL),(1622,877,1,20,NULL),(1623,877,2,20,NULL),(1624,878,1,21,NULL),(1625,878,2,21,NULL),(1626,879,1,22,NULL),(1627,879,2,22,NULL),(1628,880,1,23,NULL),(1629,880,2,23,NULL),(1630,881,1,24,NULL),(1631,881,2,24,NULL),(1632,882,1,19,NULL),(1633,882,2,19,NULL),(1634,883,1,20,NULL),(1635,883,2,20,NULL),(1636,884,1,21,NULL),(1637,884,2,21,NULL),(1638,885,1,22,NULL),(1639,885,2,22,NULL),(1640,886,1,23,NULL),(1641,886,2,23,NULL),(1642,887,1,24,NULL),(1643,887,2,24,NULL),(1644,888,1,22,NULL),(1645,888,2,22,NULL),(1646,889,1,22,NULL),(1647,889,2,22,NULL),(1648,890,1,22,NULL),(1649,890,2,22,NULL),(1650,891,1,22,NULL),(1651,892,1,16,NULL),(1652,892,2,16,NULL),(1653,893,1,16,NULL),(1654,893,2,16,NULL),(1655,894,1,16,NULL),(1656,895,1,22,NULL),(1657,896,1,NULL,NULL),(1658,896,2,NULL,NULL),(1659,897,1,NULL,NULL),(1660,897,2,NULL,NULL),(1661,898,1,NULL,NULL),(1662,898,2,NULL,NULL),(1663,899,1,NULL,NULL),(1664,899,2,NULL,NULL),(1665,900,1,NULL,NULL),(1666,900,2,NULL,NULL),(1667,901,1,NULL,NULL),(1668,901,2,NULL,NULL),(1669,902,1,22,NULL),(1670,902,2,22,NULL),(1671,903,1,22,NULL),(1672,903,2,22,NULL),(1673,904,1,18,'adm_name'),(1674,905,1,36,'ids'),(1675,906,1,36,'ids'),(1676,907,1,36,'ids'),(1677,908,1,36,'ids'),(1678,909,1,36,'ids'),(1679,910,1,19,NULL),(1680,910,2,19,NULL),(1681,911,1,20,NULL),(1682,911,2,20,NULL),(1683,912,1,21,NULL),(1684,912,2,21,NULL),(1685,913,1,22,NULL),(1686,913,2,22,NULL),(1687,914,1,23,NULL),(1688,914,2,23,NULL),(1689,915,1,24,NULL),(1690,915,2,24,NULL),(1691,916,1,19,NULL),(1692,916,2,19,NULL),(1693,917,1,20,NULL),(1694,917,2,20,NULL),(1695,918,1,21,NULL),(1696,918,2,21,NULL),(1697,919,1,22,NULL),(1698,919,2,22,NULL),(1699,920,1,23,NULL),(1700,920,2,23,NULL),(1701,921,1,24,NULL),(1702,921,2,24,NULL),(1703,922,1,19,NULL),(1704,922,2,19,NULL),(1705,923,1,20,NULL),(1706,923,2,20,NULL),(1707,924,1,21,NULL),(1708,924,2,21,NULL),(1709,925,1,22,NULL),(1710,925,2,22,NULL),(1711,926,1,23,NULL),(1712,926,2,23,NULL),(1713,927,1,24,NULL),(1714,927,2,24,NULL),(1715,928,1,19,NULL),(1716,928,2,19,NULL),(1717,929,1,20,NULL),(1718,929,2,20,NULL),(1719,930,1,21,NULL),(1720,930,2,21,NULL),(1721,931,1,22,NULL),(1722,931,2,22,NULL),(1723,932,1,23,NULL),(1724,932,2,23,NULL),(1725,933,1,24,NULL),(1726,933,2,24,NULL),(1727,934,1,19,NULL),(1728,934,2,19,NULL),(1729,935,1,20,NULL),(1730,935,2,20,NULL),(1731,936,1,21,NULL),(1732,936,2,21,NULL),(1733,937,1,22,NULL),(1734,937,2,22,NULL),(1735,938,1,23,NULL),(1736,938,2,23,NULL),(1737,939,1,24,NULL),(1738,939,2,24,NULL),(1739,940,1,19,NULL),(1740,940,2,19,NULL),(1741,941,1,20,NULL),(1742,941,2,20,NULL),(1743,942,1,21,NULL),(1744,942,2,21,NULL),(1745,943,1,22,NULL),(1746,943,2,22,NULL),(1747,944,1,23,NULL),(1748,944,2,23,NULL),(1749,945,1,24,NULL),(1750,945,2,24,NULL),(1751,946,1,22,NULL),(1752,946,2,22,NULL),(1753,947,1,22,NULL),(1754,947,2,22,NULL),(1755,948,1,22,NULL),(1756,948,2,22,NULL),(1757,949,1,22,NULL),(1758,950,1,16,NULL),(1759,950,2,16,NULL),(1760,951,1,16,NULL),(1761,951,2,16,NULL),(1762,952,1,16,NULL),(1763,953,1,22,NULL),(1764,954,1,NULL,NULL),(1765,954,2,NULL,NULL),(1766,955,1,NULL,NULL),(1767,955,2,NULL,NULL),(1768,956,1,NULL,NULL),(1769,956,2,NULL,NULL),(1770,957,1,NULL,NULL),(1771,957,2,NULL,NULL),(1772,958,1,NULL,NULL),(1773,958,2,NULL,NULL),(1774,959,1,NULL,NULL),(1775,959,2,NULL,NULL),(1776,960,1,22,NULL),(1777,960,2,22,NULL),(1778,961,1,22,NULL),(1779,961,2,22,NULL),(1780,962,1,18,'adm_name'),(1781,963,1,36,'ids'),(1782,964,1,36,'ids'),(1783,965,1,36,'ids'),(1784,966,1,36,'ids'),(1785,967,1,36,'ids'),(1786,968,1,19,NULL),(1787,968,2,19,NULL),(1788,969,1,20,NULL),(1789,969,2,20,NULL),(1790,970,1,21,NULL),(1791,970,2,21,NULL),(1792,971,1,22,NULL),(1793,971,2,22,NULL),(1794,972,1,23,NULL),(1795,972,2,23,NULL),(1796,973,1,24,NULL),(1797,973,2,24,NULL),(1798,974,1,19,NULL),(1799,974,2,19,NULL),(1800,975,1,20,NULL),(1801,975,2,20,NULL),(1802,976,1,21,NULL),(1803,976,2,21,NULL),(1804,977,1,22,NULL),(1805,977,2,22,NULL),(1806,978,1,23,NULL),(1807,978,2,23,NULL),(1808,979,1,24,NULL),(1809,979,2,24,NULL),(1810,980,1,19,NULL),(1811,980,2,19,NULL),(1812,981,1,20,NULL),(1813,981,2,20,NULL),(1814,982,1,21,NULL),(1815,982,2,21,NULL),(1816,983,1,22,NULL),(1817,983,2,22,NULL),(1818,984,1,23,NULL),(1819,984,2,23,NULL),(1820,985,1,24,NULL),(1821,985,2,24,NULL),(1822,986,1,19,NULL),(1823,986,2,19,NULL),(1824,987,1,20,NULL),(1825,987,2,20,NULL),(1826,988,1,21,NULL),(1827,988,2,21,NULL),(1828,989,1,22,NULL),(1829,989,2,22,NULL),(1830,990,1,23,NULL),(1831,990,2,23,NULL),(1832,991,1,24,NULL),(1833,991,2,24,NULL),(1834,992,1,19,NULL),(1835,992,2,19,NULL),(1836,993,1,20,NULL),(1837,993,2,20,NULL),(1838,994,1,21,NULL),(1839,994,2,21,NULL),(1840,995,1,22,NULL),(1841,995,2,22,NULL),(1842,996,1,23,NULL),(1843,996,2,23,NULL),(1844,997,1,24,NULL),(1845,997,2,24,NULL),(1846,998,1,19,NULL),(1847,998,2,19,NULL),(1848,999,1,20,NULL),(1849,999,2,20,NULL),(1850,1000,1,21,NULL),(1851,1000,2,21,NULL),(1852,1001,1,22,NULL),(1853,1001,2,22,NULL),(1854,1002,1,23,NULL),(1855,1002,2,23,NULL),(1856,1003,1,24,NULL),(1857,1003,2,24,NULL),(1858,1004,1,22,NULL),(1859,1004,2,22,NULL),(1860,1005,1,22,NULL),(1861,1005,2,22,NULL),(1862,1006,1,22,NULL),(1863,1006,2,22,NULL),(1864,1007,1,22,NULL),(1865,1008,1,16,NULL),(1866,1008,2,16,NULL),(1867,1009,1,16,NULL),(1868,1009,2,16,NULL),(1869,1010,1,16,NULL),(1870,1011,1,22,NULL),(1871,1012,1,NULL,NULL),(1872,1012,2,NULL,NULL),(1873,1013,1,NULL,NULL),(1874,1013,2,NULL,NULL),(1875,1014,1,NULL,NULL),(1876,1014,2,NULL,NULL),(1877,1015,1,NULL,NULL),(1878,1015,2,NULL,NULL),(1879,1016,1,NULL,NULL),(1880,1016,2,NULL,NULL),(1881,1017,1,NULL,NULL),(1882,1017,2,NULL,NULL),(1883,1018,1,22,NULL),(1884,1018,2,22,NULL),(1885,1019,1,22,NULL),(1886,1019,2,22,NULL),(1887,1020,1,18,'adm_name'),(1888,1021,1,36,'ids'),(1889,1022,1,36,'ids'),(1890,1023,1,36,'ids'),(1891,1024,1,36,'ids'),(1892,1025,1,36,'ids'),(1893,1027,1,19,NULL),(1894,1027,2,19,NULL),(1895,1028,1,20,NULL),(1896,1028,2,20,NULL),(1897,1029,1,21,NULL),(1898,1029,2,21,NULL),(1899,1030,1,22,NULL),(1900,1030,2,22,NULL),(1901,1031,1,23,NULL),(1902,1031,2,23,NULL),(1903,1032,1,24,NULL),(1904,1032,2,24,NULL),(1905,1033,1,19,NULL),(1906,1033,2,19,NULL),(1907,1034,1,20,NULL),(1908,1034,2,20,NULL),(1909,1035,1,21,NULL),(1910,1035,2,21,NULL),(1911,1036,1,22,NULL),(1912,1036,2,22,NULL),(1913,1037,1,23,NULL),(1914,1037,2,23,NULL),(1915,1038,1,24,NULL),(1916,1038,2,24,NULL),(1917,1039,1,19,NULL),(1918,1039,2,19,NULL),(1919,1040,1,20,NULL),(1920,1040,2,20,NULL),(1921,1041,1,21,NULL),(1922,1041,2,21,NULL),(1923,1042,1,22,NULL),(1924,1042,2,22,NULL),(1925,1043,1,23,NULL),(1926,1043,2,23,NULL),(1927,1044,1,24,NULL),(1928,1044,2,24,NULL),(1929,1045,1,19,NULL),(1930,1045,2,19,NULL),(1931,1046,1,20,NULL),(1932,1046,2,20,NULL),(1933,1047,1,21,NULL),(1934,1047,2,21,NULL),(1935,1048,1,22,NULL),(1936,1048,2,22,NULL),(1937,1049,1,23,NULL),(1938,1049,2,23,NULL),(1939,1050,1,24,NULL),(1940,1050,2,24,NULL),(1941,1051,1,19,NULL),(1942,1051,2,19,NULL),(1943,1052,1,20,NULL),(1944,1052,2,20,NULL),(1945,1053,1,21,NULL),(1946,1053,2,21,NULL),(1947,1054,1,22,NULL),(1948,1054,2,22,NULL),(1949,1055,1,23,NULL),(1950,1055,2,23,NULL),(1951,1056,1,24,NULL),(1952,1056,2,24,NULL),(1953,1057,1,19,NULL),(1954,1057,2,19,NULL),(1955,1058,1,20,NULL),(1956,1058,2,20,NULL),(1957,1059,1,21,NULL),(1958,1059,2,21,NULL),(1959,1060,1,22,NULL),(1960,1060,2,22,NULL),(1961,1061,1,23,NULL),(1962,1061,2,23,NULL),(1963,1062,1,24,NULL),(1964,1062,2,24,NULL),(1965,1063,1,22,NULL),(1966,1063,2,22,NULL),(1967,1064,1,22,NULL),(1968,1064,2,22,NULL),(1969,1065,1,22,NULL),(1970,1065,2,22,NULL),(1971,1066,1,22,NULL),(1972,1067,1,16,NULL),(1973,1067,2,16,NULL),(1974,1068,1,16,NULL),(1975,1068,2,16,NULL),(1976,1069,1,16,NULL),(1977,1070,1,22,NULL),(1978,1071,1,NULL,NULL),(1979,1071,2,NULL,NULL),(1980,1072,1,NULL,NULL),(1981,1072,2,NULL,NULL),(1982,1073,1,NULL,NULL),(1983,1073,2,NULL,NULL),(1984,1074,1,NULL,NULL),(1985,1074,2,NULL,NULL),(1986,1075,1,NULL,NULL),(1987,1075,2,NULL,NULL),(1988,1076,1,NULL,NULL),(1989,1076,2,NULL,NULL),(1990,1077,1,22,NULL),(1991,1077,2,22,NULL),(1992,1078,1,22,NULL),(1993,1078,2,22,NULL),(1994,1079,1,18,'adm_name'),(1995,1080,1,36,'ids'),(1996,1081,1,36,'ids'),(1997,1082,1,36,'ids'),(1998,1083,1,36,'ids'),(1999,1084,1,36,'ids'),(2000,1086,1,19,NULL),(2001,1086,2,19,NULL),(2002,1087,1,20,NULL),(2003,1087,2,20,NULL),(2004,1088,1,21,NULL),(2005,1088,2,21,NULL),(2006,1089,1,22,NULL),(2007,1089,2,22,NULL),(2008,1090,1,23,NULL),(2009,1090,2,23,NULL),(2010,1091,1,24,NULL),(2011,1091,2,24,NULL),(2012,1092,1,19,NULL),(2013,1092,2,19,NULL),(2014,1093,1,20,NULL),(2015,1093,2,20,NULL),(2016,1094,1,21,NULL),(2017,1094,2,21,NULL),(2018,1095,1,22,NULL),(2019,1095,2,22,NULL),(2020,1096,1,23,NULL),(2021,1096,2,23,NULL),(2022,1097,1,24,NULL),(2023,1097,2,24,NULL),(2024,1098,1,19,NULL),(2025,1098,2,19,NULL),(2026,1099,1,20,NULL),(2027,1099,2,20,NULL),(2028,1100,1,21,NULL),(2029,1100,2,21,NULL),(2030,1101,1,22,NULL),(2031,1101,2,22,NULL),(2032,1102,1,23,NULL),(2033,1102,2,23,NULL),(2034,1103,1,24,NULL),(2035,1103,2,24,NULL),(2036,1104,1,19,NULL),(2037,1104,2,19,NULL),(2038,1105,1,20,NULL),(2039,1105,2,20,NULL),(2040,1106,1,21,NULL),(2041,1106,2,21,NULL),(2042,1107,1,22,NULL),(2043,1107,2,22,NULL),(2044,1108,1,23,NULL),(2045,1108,2,23,NULL),(2046,1109,1,24,NULL),(2047,1109,2,24,NULL),(2048,1110,1,19,NULL),(2049,1110,2,19,NULL),(2050,1111,1,20,NULL),(2051,1111,2,20,NULL),(2052,1112,1,21,NULL),(2053,1112,2,21,NULL),(2054,1113,1,22,NULL),(2055,1113,2,22,NULL),(2056,1114,1,23,NULL),(2057,1114,2,23,NULL),(2058,1115,1,24,NULL),(2059,1115,2,24,NULL),(2060,1116,1,19,NULL),(2061,1116,2,19,NULL),(2062,1117,1,20,NULL),(2063,1117,2,20,NULL),(2064,1118,1,21,NULL),(2065,1118,2,21,NULL),(2066,1119,1,22,NULL),(2067,1119,2,22,NULL),(2068,1120,1,23,NULL),(2069,1120,2,23,NULL),(2070,1121,1,24,NULL),(2071,1121,2,24,NULL),(2072,1122,1,22,NULL),(2073,1122,2,22,NULL),(2074,1123,1,22,NULL),(2075,1123,2,22,NULL),(2076,1124,1,22,NULL),(2077,1124,2,22,NULL),(2078,1125,1,22,NULL),(2079,1126,1,16,NULL),(2080,1126,2,16,NULL),(2081,1127,1,16,NULL),(2082,1127,2,16,NULL),(2083,1128,1,16,NULL),(2084,1129,1,22,NULL),(2085,1130,1,NULL,NULL),(2086,1130,2,NULL,NULL),(2087,1131,1,NULL,NULL),(2088,1131,2,NULL,NULL),(2089,1132,1,NULL,NULL),(2090,1132,2,NULL,NULL),(2091,1133,1,NULL,NULL),(2092,1133,2,NULL,NULL),(2093,1134,1,NULL,NULL),(2094,1134,2,NULL,NULL),(2095,1135,1,NULL,NULL),(2096,1135,2,NULL,NULL),(2097,1136,1,22,NULL),(2098,1136,2,22,NULL),(2099,1137,1,22,NULL),(2100,1137,2,22,NULL),(2101,1138,1,18,'adm_name'),(2102,1139,1,36,'ids'),(2103,1140,1,36,'ids'),(2104,1141,1,36,'ids'),(2105,1142,1,36,'ids'),(2106,1143,1,36,'ids'),(2107,1145,1,19,NULL),(2108,1145,2,19,NULL),(2109,1146,1,20,NULL),(2110,1146,2,20,NULL),(2111,1147,1,21,NULL),(2112,1147,2,21,NULL),(2113,1148,1,22,NULL),(2114,1148,2,22,NULL),(2115,1149,1,23,NULL),(2116,1149,2,23,NULL),(2117,1150,1,24,NULL),(2118,1150,2,24,NULL),(2119,1151,1,19,NULL),(2120,1151,2,19,NULL),(2121,1152,1,20,NULL),(2122,1152,2,20,NULL),(2123,1153,1,21,NULL),(2124,1153,2,21,NULL),(2125,1154,1,22,NULL),(2126,1154,2,22,NULL),(2127,1155,1,23,NULL),(2128,1155,2,23,NULL),(2129,1156,1,24,NULL),(2130,1156,2,24,NULL),(2131,1157,1,19,NULL),(2132,1157,2,19,NULL),(2133,1158,1,20,NULL),(2134,1158,2,20,NULL),(2135,1159,1,21,NULL),(2136,1159,2,21,NULL),(2137,1160,1,22,NULL),(2138,1160,2,22,NULL),(2139,1161,1,23,NULL),(2140,1161,2,23,NULL),(2141,1162,1,24,NULL),(2142,1162,2,24,NULL),(2143,1163,1,19,NULL),(2144,1163,2,19,NULL),(2145,1164,1,20,NULL),(2146,1164,2,20,NULL),(2147,1165,1,21,NULL),(2148,1165,2,21,NULL),(2149,1166,1,22,NULL),(2150,1166,2,22,NULL),(2151,1167,1,23,NULL),(2152,1167,2,23,NULL),(2153,1168,1,24,NULL),(2154,1168,2,24,NULL),(2155,1169,1,19,NULL),(2156,1169,2,19,NULL),(2157,1170,1,20,NULL),(2158,1170,2,20,NULL),(2159,1171,1,21,NULL),(2160,1171,2,21,NULL),(2161,1172,1,22,NULL),(2162,1172,2,22,NULL),(2163,1173,1,23,NULL),(2164,1173,2,23,NULL),(2165,1174,1,24,NULL),(2166,1174,2,24,NULL),(2167,1175,1,19,NULL),(2168,1175,2,19,NULL),(2169,1176,1,20,NULL),(2170,1176,2,20,NULL),(2171,1177,1,21,NULL),(2172,1177,2,21,NULL),(2173,1178,1,22,NULL),(2174,1178,2,22,NULL),(2175,1179,1,23,NULL),(2176,1179,2,23,NULL),(2177,1180,1,24,NULL),(2178,1180,2,24,NULL),(2179,1181,1,22,NULL),(2180,1181,2,22,NULL),(2181,1182,1,22,NULL),(2182,1182,2,22,NULL),(2183,1183,1,22,NULL),(2184,1183,2,22,NULL),(2185,1184,1,22,NULL),(2186,1185,1,16,NULL),(2187,1185,2,16,NULL),(2188,1186,1,16,NULL),(2189,1186,2,16,NULL),(2190,1187,1,16,NULL),(2191,1188,1,22,NULL),(2192,1189,1,NULL,NULL),(2193,1189,2,NULL,NULL),(2194,1190,1,NULL,NULL),(2195,1190,2,NULL,NULL),(2196,1191,1,NULL,NULL),(2197,1191,2,NULL,NULL),(2198,1192,1,NULL,NULL),(2199,1192,2,NULL,NULL),(2200,1193,1,NULL,NULL),(2201,1193,2,NULL,NULL),(2202,1194,1,NULL,NULL),(2203,1194,2,NULL,NULL),(2204,1195,1,22,NULL),(2205,1195,2,22,NULL),(2206,1196,1,22,NULL),(2207,1196,2,22,NULL),(2208,1197,1,18,'adm_name'),(2209,1198,1,36,'ids'),(2210,1199,1,36,'ids'),(2211,1200,1,36,'ids'),(2212,1201,1,36,'ids'),(2213,1202,1,36,'ids'),(2214,1204,1,19,NULL),(2215,1204,2,19,NULL),(2216,1205,1,20,NULL),(2217,1205,2,20,NULL),(2218,1206,1,21,NULL),(2219,1206,2,21,NULL),(2220,1207,1,22,NULL),(2221,1207,2,22,NULL),(2222,1208,1,23,NULL),(2223,1208,2,23,NULL),(2224,1209,1,24,NULL),(2225,1209,2,24,NULL),(2226,1210,1,19,NULL),(2227,1210,2,19,NULL),(2228,1211,1,20,NULL),(2229,1211,2,20,NULL),(2230,1212,1,21,NULL),(2231,1212,2,21,NULL),(2232,1213,1,22,NULL),(2233,1213,2,22,NULL),(2234,1214,1,23,NULL),(2235,1214,2,23,NULL),(2236,1215,1,24,NULL),(2237,1215,2,24,NULL),(2238,1216,1,19,NULL),(2239,1216,2,19,NULL),(2240,1217,1,20,NULL),(2241,1217,2,20,NULL),(2242,1218,1,21,NULL),(2243,1218,2,21,NULL),(2244,1219,1,22,NULL),(2245,1219,2,22,NULL),(2246,1220,1,23,NULL),(2247,1220,2,23,NULL),(2248,1221,1,24,NULL),(2249,1221,2,24,NULL),(2250,1222,1,19,NULL),(2251,1222,2,19,NULL),(2252,1223,1,20,NULL),(2253,1223,2,20,NULL),(2254,1224,1,21,NULL),(2255,1224,2,21,NULL),(2256,1225,1,22,NULL),(2257,1225,2,22,NULL),(2258,1226,1,23,NULL),(2259,1226,2,23,NULL),(2260,1227,1,24,NULL),(2261,1227,2,24,NULL),(2262,1228,1,19,NULL),(2263,1228,2,19,NULL),(2264,1229,1,20,NULL),(2265,1229,2,20,NULL),(2266,1230,1,21,NULL),(2267,1230,2,21,NULL),(2268,1231,1,22,NULL),(2269,1231,2,22,NULL),(2270,1232,1,23,NULL),(2271,1232,2,23,NULL),(2272,1233,1,24,NULL),(2273,1233,2,24,NULL),(2274,1234,1,19,NULL),(2275,1234,2,19,NULL),(2276,1235,1,20,NULL),(2277,1235,2,20,NULL),(2278,1236,1,21,NULL),(2279,1236,2,21,NULL),(2280,1237,1,22,NULL),(2281,1237,2,22,NULL),(2282,1238,1,23,NULL),(2283,1238,2,23,NULL),(2284,1239,1,24,NULL),(2285,1239,2,24,NULL),(2286,1240,1,22,NULL),(2287,1240,2,22,NULL),(2288,1241,1,22,NULL),(2289,1241,2,22,NULL),(2290,1242,1,22,NULL),(2291,1242,2,22,NULL),(2292,1243,1,22,NULL),(2293,1244,1,16,NULL),(2294,1244,2,16,NULL),(2295,1245,1,16,NULL),(2296,1245,2,16,NULL),(2297,1246,1,16,NULL),(2298,1247,1,22,NULL),(2299,1248,1,NULL,NULL),(2300,1248,2,NULL,NULL),(2301,1249,1,NULL,NULL),(2302,1249,2,NULL,NULL),(2303,1250,1,NULL,NULL),(2304,1250,2,NULL,NULL),(2305,1251,1,NULL,NULL),(2306,1251,2,NULL,NULL),(2307,1252,1,NULL,NULL),(2308,1252,2,NULL,NULL),(2309,1253,1,NULL,NULL),(2310,1253,2,NULL,NULL),(2311,1254,1,22,NULL),(2312,1254,2,22,NULL),(2313,1255,1,22,NULL),(2314,1255,2,22,NULL),(2315,1256,1,18,'adm_name'),(2316,1257,1,36,'ids'),(2317,1258,1,36,'ids'),(2318,1259,1,36,'ids'),(2319,1260,1,36,'ids'),(2320,1261,1,36,'ids'),(2321,1263,1,19,NULL),(2322,1263,2,19,NULL),(2323,1264,1,20,NULL),(2324,1264,2,20,NULL),(2325,1265,1,21,NULL),(2326,1265,2,21,NULL),(2327,1266,1,22,NULL),(2328,1266,2,22,NULL),(2329,1267,1,23,NULL),(2330,1267,2,23,NULL),(2331,1268,1,24,NULL),(2332,1268,2,24,NULL),(2333,1269,1,19,NULL),(2334,1269,2,19,NULL),(2335,1270,1,20,NULL),(2336,1270,2,20,NULL),(2337,1271,1,21,NULL),(2338,1271,2,21,NULL),(2339,1272,1,22,NULL),(2340,1272,2,22,NULL),(2341,1273,1,23,NULL),(2342,1273,2,23,NULL),(2343,1274,1,24,NULL),(2344,1274,2,24,NULL),(2345,1275,1,19,NULL),(2346,1275,2,19,NULL),(2347,1276,1,20,NULL),(2348,1276,2,20,NULL),(2349,1277,1,21,NULL),(2350,1277,2,21,NULL),(2351,1278,1,22,NULL),(2352,1278,2,22,NULL),(2353,1279,1,23,NULL),(2354,1279,2,23,NULL),(2355,1280,1,24,NULL),(2356,1280,2,24,NULL),(2357,1281,1,19,NULL),(2358,1281,2,19,NULL),(2359,1282,1,20,NULL),(2360,1282,2,20,NULL),(2361,1283,1,21,NULL),(2362,1283,2,21,NULL),(2363,1284,1,22,NULL),(2364,1284,2,22,NULL),(2365,1285,1,23,NULL),(2366,1285,2,23,NULL),(2367,1286,1,24,NULL),(2368,1286,2,24,NULL),(2369,1287,1,19,NULL),(2370,1287,2,19,NULL),(2371,1288,1,20,NULL),(2372,1288,2,20,NULL),(2373,1289,1,21,NULL),(2374,1289,2,21,NULL),(2375,1290,1,22,NULL),(2376,1290,2,22,NULL),(2377,1291,1,23,NULL),(2378,1291,2,23,NULL),(2379,1292,1,24,NULL),(2380,1292,2,24,NULL),(2381,1293,1,19,NULL),(2382,1293,2,19,NULL),(2383,1294,1,20,NULL),(2384,1294,2,20,NULL),(2385,1295,1,21,NULL),(2386,1295,2,21,NULL),(2387,1296,1,22,NULL),(2388,1296,2,22,NULL),(2389,1297,1,23,NULL),(2390,1297,2,23,NULL),(2391,1298,1,24,NULL),(2392,1298,2,24,NULL),(2393,1299,1,22,NULL),(2394,1299,2,22,NULL),(2395,1300,1,22,NULL),(2396,1300,2,22,NULL),(2397,1301,1,22,NULL),(2398,1301,2,22,NULL),(2399,1302,1,22,NULL),(2400,1303,1,16,NULL),(2401,1303,2,16,NULL),(2402,1304,1,16,NULL),(2403,1304,2,16,NULL),(2404,1305,1,16,NULL),(2405,1306,1,22,NULL),(2406,1307,1,NULL,NULL),(2407,1307,2,NULL,NULL),(2408,1308,1,NULL,NULL),(2409,1308,2,NULL,NULL),(2410,1309,1,NULL,NULL),(2411,1309,2,NULL,NULL),(2412,1310,1,NULL,NULL),(2413,1310,2,NULL,NULL),(2414,1311,1,NULL,NULL),(2415,1311,2,NULL,NULL),(2416,1312,1,NULL,NULL),(2417,1312,2,NULL,NULL),(2418,1313,1,22,NULL),(2419,1313,2,22,NULL),(2420,1314,1,22,NULL),(2421,1314,2,22,NULL),(2422,1315,1,18,'adm_name'),(2423,1316,1,36,'ids'),(2424,1317,1,36,'ids'),(2425,1318,1,36,'ids'),(2426,1319,1,36,'ids'),(2427,1320,1,36,'ids'),(2428,1323,1,19,NULL),(2429,1323,2,19,NULL),(2430,1324,1,20,NULL),(2431,1324,2,20,NULL),(2432,1325,1,21,NULL),(2433,1325,2,21,NULL),(2434,1326,1,22,NULL),(2435,1326,2,22,NULL),(2436,1327,1,23,NULL),(2437,1327,2,23,NULL),(2438,1328,1,24,NULL),(2439,1328,2,24,NULL),(2440,1329,1,19,NULL),(2441,1329,2,19,NULL),(2442,1330,1,20,NULL),(2443,1330,2,20,NULL),(2444,1331,1,21,NULL),(2445,1331,2,21,NULL),(2446,1332,1,22,NULL),(2447,1332,2,22,NULL),(2448,1333,1,23,NULL),(2449,1333,2,23,NULL),(2450,1334,1,24,NULL),(2451,1334,2,24,NULL),(2452,1335,1,19,NULL),(2453,1335,2,19,NULL),(2454,1336,1,20,NULL),(2455,1336,2,20,NULL),(2456,1337,1,21,NULL),(2457,1337,2,21,NULL),(2458,1338,1,22,NULL),(2459,1338,2,22,NULL),(2460,1339,1,23,NULL),(2461,1339,2,23,NULL),(2462,1340,1,24,NULL),(2463,1340,2,24,NULL),(2464,1341,1,19,NULL),(2465,1341,2,19,NULL),(2466,1342,1,20,NULL),(2467,1342,2,20,NULL),(2468,1343,1,21,NULL),(2469,1343,2,21,NULL),(2470,1344,1,22,NULL),(2471,1344,2,22,NULL),(2472,1345,1,23,NULL),(2473,1345,2,23,NULL),(2474,1346,1,24,NULL),(2475,1346,2,24,NULL),(2476,1347,1,19,NULL),(2477,1347,2,19,NULL),(2478,1348,1,20,NULL),(2479,1348,2,20,NULL),(2480,1349,1,21,NULL),(2481,1349,2,21,NULL),(2482,1350,1,22,NULL),(2483,1350,2,22,NULL),(2484,1351,1,23,NULL),(2485,1351,2,23,NULL),(2486,1352,1,24,NULL),(2487,1352,2,24,NULL),(2488,1353,1,19,NULL),(2489,1353,2,19,NULL),(2490,1354,1,20,NULL),(2491,1354,2,20,NULL),(2492,1355,1,21,NULL),(2493,1355,2,21,NULL),(2494,1356,1,22,NULL),(2495,1356,2,22,NULL),(2496,1357,1,23,NULL),(2497,1357,2,23,NULL),(2498,1358,1,24,NULL),(2499,1358,2,24,NULL),(2500,1359,1,22,NULL),(2501,1359,2,22,NULL),(2502,1360,1,22,NULL),(2503,1360,2,22,NULL),(2504,1361,1,22,NULL),(2505,1361,2,22,NULL),(2506,1362,1,22,NULL),(2507,1363,1,16,NULL),(2508,1363,2,16,NULL),(2509,1364,1,16,NULL),(2510,1364,2,16,NULL),(2511,1365,1,16,NULL),(2512,1366,1,22,NULL),(2513,1367,1,NULL,NULL),(2514,1367,2,NULL,NULL),(2515,1368,1,NULL,NULL),(2516,1368,2,NULL,NULL),(2517,1369,1,NULL,NULL),(2518,1369,2,NULL,NULL),(2519,1370,1,NULL,NULL),(2520,1370,2,NULL,NULL),(2521,1371,1,NULL,NULL),(2522,1371,2,NULL,NULL),(2523,1372,1,NULL,NULL),(2524,1372,2,NULL,NULL),(2525,1373,1,22,NULL),(2526,1373,2,22,NULL),(2527,1374,1,22,NULL),(2528,1374,2,22,NULL),(2529,1375,1,18,'adm_name'),(2530,1376,1,36,'ids'),(2531,1377,1,36,'ids'),(2532,1378,1,36,'ids'),(2533,1379,1,36,'ids'),(2534,1380,1,36,'ids'),(2535,1381,1,19,NULL),(2536,1381,2,19,NULL),(2537,1382,1,20,NULL),(2538,1382,2,20,NULL),(2539,1383,1,21,NULL),(2540,1383,2,21,NULL),(2541,1384,1,22,NULL),(2542,1384,2,22,NULL),(2543,1385,1,23,NULL),(2544,1385,2,23,NULL),(2545,1386,1,24,NULL),(2546,1386,2,24,NULL),(2547,1387,1,19,NULL),(2548,1387,2,19,NULL),(2549,1388,1,20,NULL),(2550,1388,2,20,NULL),(2551,1389,1,21,NULL),(2552,1389,2,21,NULL),(2553,1390,1,22,NULL),(2554,1390,2,22,NULL),(2555,1391,1,23,NULL),(2556,1391,2,23,NULL),(2557,1392,1,24,NULL),(2558,1392,2,24,NULL),(2559,1393,1,19,NULL),(2560,1393,2,19,NULL),(2561,1394,1,20,NULL),(2562,1394,2,20,NULL),(2563,1395,1,21,NULL),(2564,1395,2,21,NULL),(2565,1396,1,22,NULL),(2566,1396,2,22,NULL),(2567,1397,1,23,NULL),(2568,1397,2,23,NULL),(2569,1398,1,24,NULL),(2570,1398,2,24,NULL),(2571,1399,1,19,NULL),(2572,1399,2,19,NULL),(2573,1400,1,20,NULL),(2574,1400,2,20,NULL),(2575,1401,1,21,NULL),(2576,1401,2,21,NULL),(2577,1402,1,22,NULL),(2578,1402,2,22,NULL),(2579,1403,1,23,NULL),(2580,1403,2,23,NULL),(2581,1404,1,24,NULL),(2582,1404,2,24,NULL),(2583,1405,1,19,NULL),(2584,1405,2,19,NULL),(2585,1406,1,20,NULL),(2586,1406,2,20,NULL),(2587,1407,1,21,NULL),(2588,1407,2,21,NULL),(2589,1408,1,22,NULL),(2590,1408,2,22,NULL),(2591,1409,1,23,NULL),(2592,1409,2,23,NULL),(2593,1410,1,24,NULL),(2594,1410,2,24,NULL),(2595,1411,1,19,NULL),(2596,1411,2,19,NULL),(2597,1412,1,20,NULL),(2598,1412,2,20,NULL),(2599,1413,1,21,NULL),(2600,1413,2,21,NULL),(2601,1414,1,22,NULL),(2602,1414,2,22,NULL),(2603,1415,1,23,NULL),(2604,1415,2,23,NULL),(2605,1416,1,24,NULL),(2606,1416,2,24,NULL),(2607,1417,1,22,NULL),(2608,1417,2,22,NULL),(2609,1418,1,22,NULL),(2610,1418,2,22,NULL),(2611,1419,1,22,NULL),(2612,1419,2,22,NULL),(2613,1420,1,22,NULL),(2614,1421,1,16,NULL),(2615,1421,2,16,NULL),(2616,1422,1,16,NULL),(2617,1422,2,16,NULL),(2618,1423,1,16,NULL),(2619,1424,1,22,NULL),(2620,1425,1,NULL,NULL),(2621,1425,2,NULL,NULL),(2622,1426,1,NULL,NULL),(2623,1426,2,NULL,NULL),(2624,1427,1,NULL,NULL),(2625,1427,2,NULL,NULL),(2626,1428,1,NULL,NULL),(2627,1428,2,NULL,NULL),(2628,1429,1,NULL,NULL),(2629,1429,2,NULL,NULL),(2630,1430,1,NULL,NULL),(2631,1430,2,NULL,NULL),(2632,1431,1,22,NULL),(2633,1431,2,22,NULL),(2634,1432,1,22,NULL),(2635,1432,2,22,NULL),(2636,1433,1,18,'adm_name'),(2637,1434,1,36,'ids'),(2638,1435,1,36,'ids'),(2639,1436,1,36,'ids'),(2640,1437,1,36,'ids'),(2641,1438,1,36,'ids'),(2642,1446,1,19,NULL),(2643,1447,1,19,NULL),(2644,1448,1,19,NULL),(2645,1448,2,19,NULL),(2646,1449,1,20,NULL),(2647,1449,2,20,NULL),(2648,1450,1,21,NULL),(2649,1450,2,21,NULL),(2650,1451,1,22,NULL),(2651,1451,2,22,NULL),(2652,1452,1,23,NULL),(2653,1452,2,23,NULL),(2654,1453,1,24,NULL),(2655,1453,2,24,NULL),(2656,1454,1,19,NULL),(2657,1454,2,19,NULL),(2658,1455,1,20,NULL),(2659,1455,2,20,NULL),(2660,1456,1,21,NULL),(2661,1456,2,21,NULL),(2662,1457,1,22,NULL),(2663,1457,2,22,NULL),(2664,1458,1,23,NULL),(2665,1458,2,23,NULL),(2666,1459,1,24,NULL),(2667,1459,2,24,NULL),(2668,1460,1,19,NULL),(2669,1460,2,19,NULL),(2670,1461,1,20,NULL),(2671,1461,2,20,NULL),(2672,1462,1,21,NULL),(2673,1462,2,21,NULL),(2674,1463,1,22,NULL),(2675,1463,2,22,NULL),(2676,1464,1,23,NULL),(2677,1464,2,23,NULL),(2678,1465,1,24,NULL),(2679,1465,2,24,NULL),(2680,1466,1,19,NULL),(2681,1466,2,19,NULL),(2682,1467,1,20,NULL),(2683,1467,2,20,NULL),(2684,1468,1,21,NULL),(2685,1468,2,21,NULL),(2686,1469,1,22,NULL),(2687,1469,2,22,NULL),(2688,1470,1,23,NULL),(2689,1470,2,23,NULL),(2690,1471,1,24,NULL),(2691,1471,2,24,NULL),(2692,1472,1,19,NULL),(2693,1472,2,19,NULL),(2694,1473,1,20,NULL),(2695,1473,2,20,NULL),(2696,1474,1,21,NULL),(2697,1474,2,21,NULL),(2698,1475,1,22,NULL),(2699,1475,2,22,NULL),(2700,1476,1,23,NULL),(2701,1476,2,23,NULL),(2702,1477,1,24,NULL),(2703,1477,2,24,NULL),(2704,1478,1,19,NULL),(2705,1478,2,19,NULL),(2706,1479,1,20,NULL),(2707,1479,2,20,NULL),(2708,1480,1,21,NULL),(2709,1480,2,21,NULL),(2710,1481,1,22,NULL),(2711,1481,2,22,NULL),(2712,1482,1,23,NULL),(2713,1482,2,23,NULL),(2714,1483,1,24,NULL),(2715,1483,2,24,NULL),(2716,1484,1,22,NULL),(2717,1484,2,22,NULL),(2718,1485,1,22,NULL),(2719,1485,2,22,NULL),(2720,1486,1,22,NULL),(2721,1486,2,22,NULL),(2722,1487,1,22,NULL),(2723,1488,1,19,NULL),(2724,1488,2,19,NULL),(2725,1489,1,20,NULL),(2726,1489,2,20,NULL),(2727,1490,1,21,NULL),(2728,1490,2,21,NULL),(2729,1491,1,22,NULL),(2730,1491,2,22,NULL),(2731,1492,1,23,NULL),(2732,1492,2,23,NULL),(2733,1493,1,24,NULL),(2734,1493,2,24,NULL),(2735,1494,1,19,NULL),(2736,1494,2,19,NULL),(2737,1495,1,20,NULL),(2738,1495,2,20,NULL),(2739,1496,1,21,NULL),(2740,1496,2,21,NULL),(2741,1497,1,22,NULL),(2742,1497,2,22,NULL),(2743,1498,1,23,NULL),(2744,1498,2,23,NULL),(2745,1499,1,24,NULL),(2746,1499,2,24,NULL),(2747,1500,1,19,NULL),(2748,1500,2,19,NULL),(2749,1501,1,20,NULL),(2750,1501,2,20,NULL),(2751,1502,1,21,NULL),(2752,1502,2,21,NULL),(2753,1503,1,22,NULL),(2754,1503,2,22,NULL),(2755,1504,1,23,NULL),(2756,1504,2,23,NULL),(2757,1505,1,24,NULL),(2758,1505,2,24,NULL),(2759,1506,1,19,NULL),(2760,1506,2,19,NULL),(2761,1507,1,20,NULL),(2762,1507,2,20,NULL),(2763,1508,1,21,NULL),(2764,1508,2,21,NULL),(2765,1509,1,22,NULL),(2766,1509,2,22,NULL),(2767,1510,1,23,NULL),(2768,1510,2,23,NULL),(2769,1511,1,24,NULL),(2770,1511,2,24,NULL),(2771,1512,1,19,NULL),(2772,1512,2,19,NULL),(2773,1513,1,20,NULL),(2774,1513,2,20,NULL),(2775,1514,1,21,NULL),(2776,1514,2,21,NULL),(2777,1515,1,22,NULL),(2778,1515,2,22,NULL),(2779,1516,1,23,NULL),(2780,1516,2,23,NULL),(2781,1517,1,24,NULL),(2782,1517,2,24,NULL),(2783,1518,1,19,NULL),(2784,1518,2,19,NULL),(2785,1519,1,20,NULL),(2786,1519,2,20,NULL),(2787,1520,1,21,NULL),(2788,1520,2,21,NULL),(2789,1521,1,22,NULL),(2790,1521,2,22,NULL),(2791,1522,1,23,NULL),(2792,1522,2,23,NULL),(2793,1523,1,24,NULL),(2794,1523,2,24,NULL),(2795,1524,1,22,NULL),(2796,1524,2,22,NULL),(2797,1525,1,22,NULL),(2798,1525,2,22,NULL),(2799,1526,1,22,NULL),(2800,1526,2,22,NULL),(2801,1527,1,22,NULL),(2802,1528,1,16,NULL),(2803,1528,2,16,NULL),(2804,1529,1,16,NULL),(2805,1529,2,16,NULL),(2806,1530,1,16,NULL),(2807,1531,1,22,NULL),(2808,1532,1,19,NULL),(2809,1532,2,19,NULL),(2810,1533,1,20,NULL),(2811,1533,2,20,NULL),(2812,1534,1,21,NULL),(2813,1534,2,21,NULL),(2814,1535,1,22,NULL),(2815,1535,2,22,NULL),(2816,1536,1,23,NULL),(2817,1536,2,23,NULL),(2818,1537,1,24,NULL),(2819,1537,2,24,NULL),(2820,1538,1,19,NULL),(2821,1538,2,19,NULL),(2822,1539,1,20,NULL),(2823,1539,2,20,NULL),(2824,1540,1,21,NULL),(2825,1540,2,21,NULL),(2826,1541,1,22,NULL),(2827,1541,2,22,NULL),(2828,1542,1,23,NULL),(2829,1542,2,23,NULL),(2830,1543,1,24,NULL),(2831,1543,2,24,NULL),(2832,1544,1,19,NULL),(2833,1544,2,19,NULL),(2834,1545,1,20,NULL),(2835,1545,2,20,NULL),(2836,1546,1,21,NULL),(2837,1546,2,21,NULL),(2838,1547,1,22,NULL),(2839,1547,2,22,NULL),(2840,1548,1,23,NULL),(2841,1548,2,23,NULL),(2842,1549,1,24,NULL),(2843,1549,2,24,NULL),(2844,1550,1,19,NULL),(2845,1550,2,19,NULL),(2846,1551,1,20,NULL),(2847,1551,2,20,NULL),(2848,1552,1,21,NULL),(2849,1552,2,21,NULL),(2850,1553,1,22,NULL),(2851,1553,2,22,NULL),(2852,1554,1,23,NULL),(2853,1554,2,23,NULL),(2854,1555,1,24,NULL),(2855,1555,2,24,NULL),(2856,1556,1,19,NULL),(2857,1556,2,19,NULL),(2858,1557,1,20,NULL),(2859,1557,2,20,NULL),(2860,1558,1,21,NULL),(2861,1558,2,21,NULL),(2862,1559,1,22,NULL),(2863,1559,2,22,NULL),(2864,1560,1,23,NULL),(2865,1560,2,23,NULL),(2866,1561,1,24,NULL),(2867,1561,2,24,NULL),(2868,1562,1,19,NULL),(2869,1562,2,19,NULL),(2870,1563,1,20,NULL),(2871,1563,2,20,NULL),(2872,1564,1,21,NULL),(2873,1564,2,21,NULL),(2874,1565,1,22,NULL),(2875,1565,2,22,NULL),(2876,1566,1,23,NULL),(2877,1566,2,23,NULL),(2878,1567,1,24,NULL),(2879,1567,2,24,NULL),(2880,1568,1,22,NULL),(2881,1568,2,22,NULL),(2882,1569,1,22,NULL),(2883,1569,2,22,NULL),(2884,1570,1,22,NULL),(2885,1570,2,22,NULL),(2886,1571,1,22,NULL),(2887,1572,1,16,NULL),(2888,1572,2,16,NULL),(2889,1573,1,16,NULL),(2890,1573,2,16,NULL),(2891,1574,1,16,NULL),(2892,1575,1,22,NULL),(2893,1576,1,NULL,NULL),(2894,1577,1,19,NULL),(2895,1577,2,19,NULL),(2896,1578,1,20,NULL),(2897,1578,2,20,NULL),(2898,1579,1,21,NULL),(2899,1579,2,21,NULL),(2900,1580,1,22,NULL),(2901,1580,2,22,NULL),(2902,1581,1,23,NULL),(2903,1581,2,23,NULL),(2904,1582,1,24,NULL),(2905,1582,2,24,NULL),(2906,1583,1,19,NULL),(2907,1583,2,19,NULL),(2908,1584,1,20,NULL),(2909,1584,2,20,NULL),(2910,1585,1,21,NULL),(2911,1585,2,21,NULL),(2912,1586,1,22,NULL),(2913,1586,2,22,NULL),(2914,1587,1,23,NULL),(2915,1587,2,23,NULL),(2916,1588,1,24,NULL),(2917,1588,2,24,NULL),(2918,1589,1,19,NULL),(2919,1589,2,19,NULL),(2920,1590,1,20,NULL),(2921,1590,2,20,NULL),(2922,1591,1,21,NULL),(2923,1591,2,21,NULL),(2924,1592,1,22,NULL),(2925,1592,2,22,NULL),(2926,1593,1,23,NULL),(2927,1593,2,23,NULL),(2928,1594,1,24,NULL),(2929,1594,2,24,NULL),(2930,1595,1,19,NULL),(2931,1595,2,19,NULL),(2932,1596,1,20,NULL),(2933,1596,2,20,NULL),(2934,1597,1,21,NULL),(2935,1597,2,21,NULL),(2936,1598,1,22,NULL),(2937,1598,2,22,NULL),(2938,1599,1,23,NULL),(2939,1599,2,23,NULL),(2940,1600,1,24,NULL),(2941,1600,2,24,NULL),(2942,1601,1,19,NULL),(2943,1601,2,19,NULL),(2944,1602,1,20,NULL),(2945,1602,2,20,NULL),(2946,1603,1,21,NULL),(2947,1603,2,21,NULL),(2948,1604,1,22,NULL),(2949,1604,2,22,NULL),(2950,1605,1,23,NULL),(2951,1605,2,23,NULL),(2952,1606,1,24,NULL),(2953,1606,2,24,NULL),(2954,1607,1,19,NULL),(2955,1607,2,19,NULL),(2956,1608,1,20,NULL),(2957,1608,2,20,NULL),(2958,1609,1,21,NULL),(2959,1609,2,21,NULL),(2960,1610,1,22,NULL),(2961,1610,2,22,NULL),(2962,1611,1,23,NULL),(2963,1611,2,23,NULL),(2964,1612,1,24,NULL),(2965,1612,2,24,NULL),(2966,1613,1,22,NULL),(2967,1613,2,22,NULL),(2968,1614,1,22,NULL),(2969,1614,2,22,NULL),(2970,1615,1,22,NULL),(2971,1615,2,22,NULL),(2972,1616,1,22,NULL),(2973,1617,1,16,NULL),(2974,1617,2,16,NULL),(2975,1618,1,16,NULL),(2976,1618,2,16,NULL),(2977,1619,1,16,NULL),(2978,1620,1,22,NULL),(2979,1621,1,NULL,NULL),(2980,1622,1,19,NULL),(2981,1622,2,19,NULL),(2982,1623,1,20,NULL),(2983,1623,2,20,NULL),(2984,1624,1,21,NULL),(2985,1624,2,21,NULL),(2986,1625,1,22,NULL),(2987,1625,2,22,NULL),(2988,1626,1,23,NULL),(2989,1626,2,23,NULL),(2990,1627,1,24,NULL),(2991,1627,2,24,NULL),(2992,1628,1,19,NULL),(2993,1628,2,19,NULL),(2994,1629,1,20,NULL),(2995,1629,2,20,NULL),(2996,1630,1,21,NULL),(2997,1630,2,21,NULL),(2998,1631,1,22,NULL),(2999,1631,2,22,NULL),(3000,1632,1,23,NULL),(3001,1632,2,23,NULL),(3002,1633,1,24,NULL),(3003,1633,2,24,NULL),(3004,1634,1,19,NULL),(3005,1634,2,19,NULL),(3006,1635,1,20,NULL),(3007,1635,2,20,NULL),(3008,1636,1,21,NULL),(3009,1636,2,21,NULL),(3010,1637,1,22,NULL),(3011,1637,2,22,NULL),(3012,1638,1,23,NULL),(3013,1638,2,23,NULL),(3014,1639,1,24,NULL),(3015,1639,2,24,NULL),(3016,1640,1,19,NULL),(3017,1640,2,19,NULL),(3018,1641,1,20,NULL),(3019,1641,2,20,NULL),(3020,1642,1,21,NULL),(3021,1642,2,21,NULL),(3022,1643,1,22,NULL),(3023,1643,2,22,NULL),(3024,1644,1,23,NULL),(3025,1644,2,23,NULL),(3026,1645,1,24,NULL),(3027,1645,2,24,NULL),(3028,1646,1,19,NULL),(3029,1646,2,19,NULL),(3030,1647,1,20,NULL),(3031,1647,2,20,NULL),(3032,1648,1,21,NULL),(3033,1648,2,21,NULL),(3034,1649,1,22,NULL),(3035,1649,2,22,NULL),(3036,1650,1,23,NULL),(3037,1650,2,23,NULL),(3038,1651,1,24,NULL),(3039,1651,2,24,NULL),(3040,1652,1,19,NULL),(3041,1652,2,19,NULL),(3042,1653,1,20,NULL),(3043,1653,2,20,NULL),(3044,1654,1,21,NULL),(3045,1654,2,21,NULL),(3046,1655,1,22,NULL),(3047,1655,2,22,NULL),(3048,1656,1,23,NULL),(3049,1656,2,23,NULL),(3050,1657,1,24,NULL),(3051,1657,2,24,NULL),(3052,1658,1,22,NULL),(3053,1658,2,22,NULL),(3054,1659,1,22,NULL),(3055,1659,2,22,NULL),(3056,1660,1,22,NULL),(3057,1660,2,22,NULL),(3058,1661,1,22,NULL),(3059,1662,1,16,NULL),(3060,1662,2,16,NULL),(3061,1663,1,16,NULL),(3062,1663,2,16,NULL),(3063,1664,1,16,NULL),(3064,1665,1,22,NULL),(3065,1666,1,NULL,NULL),(3066,1666,2,NULL,NULL),(3067,1667,1,NULL,NULL),(3068,1667,2,NULL,NULL),(3069,1668,1,NULL,NULL),(3070,1668,2,NULL,NULL),(3071,1669,1,NULL,NULL),(3072,1669,2,NULL,NULL),(3073,1670,1,NULL,NULL),(3074,1670,2,NULL,NULL),(3075,1671,1,NULL,NULL),(3076,1671,2,NULL,NULL),(3077,1672,1,22,NULL),(3078,1672,2,22,NULL),(3079,1673,1,22,NULL),(3080,1673,2,22,NULL),(3081,1675,1,19,NULL),(3082,1675,2,19,NULL),(3083,1676,1,20,NULL),(3084,1676,2,20,NULL),(3085,1677,1,21,NULL),(3086,1677,2,21,NULL),(3087,1678,1,22,NULL),(3088,1678,2,22,NULL),(3089,1679,1,23,NULL),(3090,1679,2,23,NULL),(3091,1680,1,24,NULL),(3092,1680,2,24,NULL),(3093,1681,1,19,NULL),(3094,1681,2,19,NULL),(3095,1682,1,20,NULL),(3096,1682,2,20,NULL),(3097,1683,1,21,NULL),(3098,1683,2,21,NULL),(3099,1684,1,22,NULL),(3100,1684,2,22,NULL),(3101,1685,1,23,NULL),(3102,1685,2,23,NULL),(3103,1686,1,24,NULL),(3104,1686,2,24,NULL),(3105,1687,1,19,NULL),(3106,1687,2,19,NULL),(3107,1688,1,20,NULL),(3108,1688,2,20,NULL),(3109,1689,1,21,NULL),(3110,1689,2,21,NULL),(3111,1690,1,22,NULL),(3112,1690,2,22,NULL),(3113,1691,1,23,NULL),(3114,1691,2,23,NULL),(3115,1692,1,24,NULL),(3116,1692,2,24,NULL),(3117,1693,1,19,NULL),(3118,1693,2,19,NULL),(3119,1694,1,20,NULL),(3120,1694,2,20,NULL),(3121,1695,1,21,NULL),(3122,1695,2,21,NULL),(3123,1696,1,22,NULL),(3124,1696,2,22,NULL),(3125,1697,1,23,NULL),(3126,1697,2,23,NULL),(3127,1698,1,24,NULL),(3128,1698,2,24,NULL),(3129,1699,1,19,NULL),(3130,1699,2,19,NULL),(3131,1700,1,20,NULL),(3132,1700,2,20,NULL),(3133,1701,1,21,NULL),(3134,1701,2,21,NULL),(3135,1702,1,22,NULL),(3136,1702,2,22,NULL),(3137,1703,1,23,NULL),(3138,1703,2,23,NULL),(3139,1704,1,24,NULL),(3140,1704,2,24,NULL),(3141,1705,1,19,NULL),(3142,1705,2,19,NULL),(3143,1706,1,20,NULL),(3144,1706,2,20,NULL),(3145,1707,1,21,NULL),(3146,1707,2,21,NULL),(3147,1708,1,22,NULL),(3148,1708,2,22,NULL),(3149,1709,1,23,NULL),(3150,1709,2,23,NULL),(3151,1710,1,24,NULL),(3152,1710,2,24,NULL),(3153,1711,1,22,NULL),(3154,1711,2,22,NULL),(3155,1712,1,22,NULL),(3156,1712,2,22,NULL),(3157,1713,1,22,NULL),(3158,1713,2,22,NULL),(3159,1714,1,22,NULL),(3160,1715,1,16,NULL),(3161,1715,2,16,NULL),(3162,1716,1,16,NULL),(3163,1716,2,16,NULL),(3164,1717,1,16,NULL),(3165,1718,1,22,NULL),(3166,1719,1,NULL,NULL),(3167,1719,2,NULL,NULL),(3168,1720,1,NULL,NULL),(3169,1720,2,NULL,NULL),(3170,1721,1,NULL,NULL),(3171,1721,2,NULL,NULL),(3172,1722,1,NULL,NULL),(3173,1722,2,NULL,NULL),(3174,1723,1,NULL,NULL),(3175,1723,2,NULL,NULL),(3176,1724,1,NULL,NULL),(3177,1724,2,NULL,NULL),(3178,1725,1,22,NULL),(3179,1725,2,22,NULL),(3180,1726,1,22,NULL),(3181,1726,2,22,NULL),(3182,1728,1,19,NULL),(3183,1728,2,19,NULL),(3184,1729,1,20,NULL),(3185,1729,2,20,NULL),(3186,1730,1,21,NULL),(3187,1730,2,21,NULL),(3188,1731,1,22,NULL),(3189,1731,2,22,NULL),(3190,1732,1,23,NULL),(3191,1732,2,23,NULL),(3192,1733,1,24,NULL),(3193,1733,2,24,NULL),(3194,1734,1,19,NULL),(3195,1734,2,19,NULL),(3196,1735,1,20,NULL),(3197,1735,2,20,NULL),(3198,1736,1,21,NULL),(3199,1736,2,21,NULL),(3200,1737,1,22,NULL),(3201,1737,2,22,NULL),(3202,1738,1,23,NULL),(3203,1738,2,23,NULL),(3204,1739,1,24,NULL),(3205,1739,2,24,NULL),(3206,1740,1,19,NULL),(3207,1740,2,19,NULL),(3208,1741,1,20,NULL),(3209,1741,2,20,NULL),(3210,1742,1,21,NULL),(3211,1742,2,21,NULL),(3212,1743,1,22,NULL),(3213,1743,2,22,NULL),(3214,1744,1,23,NULL),(3215,1744,2,23,NULL),(3216,1745,1,24,NULL),(3217,1745,2,24,NULL),(3218,1746,1,19,NULL),(3219,1746,2,19,NULL),(3220,1747,1,20,NULL),(3221,1747,2,20,NULL),(3222,1748,1,21,NULL),(3223,1748,2,21,NULL),(3224,1749,1,22,NULL),(3225,1749,2,22,NULL),(3226,1750,1,23,NULL),(3227,1750,2,23,NULL),(3228,1751,1,24,NULL),(3229,1751,2,24,NULL),(3230,1752,1,19,NULL),(3231,1752,2,19,NULL),(3232,1753,1,20,NULL),(3233,1753,2,20,NULL),(3234,1754,1,21,NULL),(3235,1754,2,21,NULL),(3236,1755,1,22,NULL),(3237,1755,2,22,NULL),(3238,1756,1,23,NULL),(3239,1756,2,23,NULL),(3240,1757,1,24,NULL),(3241,1757,2,24,NULL),(3242,1758,1,19,NULL),(3243,1758,2,19,NULL),(3244,1759,1,20,NULL),(3245,1759,2,20,NULL),(3246,1760,1,21,NULL),(3247,1760,2,21,NULL),(3248,1761,1,22,NULL),(3249,1761,2,22,NULL),(3250,1762,1,23,NULL),(3251,1762,2,23,NULL),(3252,1763,1,24,NULL),(3253,1763,2,24,NULL),(3254,1764,1,22,NULL),(3255,1764,2,22,NULL),(3256,1765,1,22,NULL),(3257,1765,2,22,NULL),(3258,1766,1,22,NULL),(3259,1766,2,22,NULL),(3260,1767,1,22,NULL),(3261,1768,1,16,NULL),(3262,1768,2,16,NULL),(3263,1769,1,16,NULL),(3264,1769,2,16,NULL),(3265,1770,1,16,NULL),(3266,1771,1,22,NULL),(3267,1772,1,NULL,NULL),(3268,1772,2,NULL,NULL),(3269,1773,1,NULL,NULL),(3270,1773,2,NULL,NULL),(3271,1774,1,NULL,NULL),(3272,1774,2,NULL,NULL),(3273,1775,1,NULL,NULL),(3274,1775,2,NULL,NULL),(3275,1776,1,NULL,NULL),(3276,1776,2,NULL,NULL),(3277,1777,1,NULL,NULL),(3278,1777,2,NULL,NULL),(3279,1778,1,22,NULL),(3280,1778,2,22,NULL),(3281,1779,1,22,NULL),(3282,1779,2,22,NULL),(3283,1781,1,19,NULL),(3284,1781,2,19,NULL),(3285,1782,1,20,NULL),(3286,1782,2,20,NULL),(3287,1783,1,21,NULL),(3288,1783,2,21,NULL),(3289,1784,1,22,NULL),(3290,1784,2,22,NULL),(3291,1785,1,23,NULL),(3292,1785,2,23,NULL),(3293,1786,1,24,NULL),(3294,1786,2,24,NULL),(3295,1787,1,19,NULL),(3296,1787,2,19,NULL),(3297,1788,1,20,NULL),(3298,1788,2,20,NULL),(3299,1789,1,21,NULL),(3300,1789,2,21,NULL),(3301,1790,1,22,NULL),(3302,1790,2,22,NULL),(3303,1791,1,23,NULL),(3304,1791,2,23,NULL),(3305,1792,1,24,NULL),(3306,1792,2,24,NULL),(3307,1793,1,19,NULL),(3308,1793,2,19,NULL),(3309,1794,1,20,NULL),(3310,1794,2,20,NULL),(3311,1795,1,21,NULL),(3312,1795,2,21,NULL),(3313,1796,1,22,NULL),(3314,1796,2,22,NULL),(3315,1797,1,23,NULL),(3316,1797,2,23,NULL),(3317,1798,1,24,NULL),(3318,1798,2,24,NULL),(3319,1799,1,19,NULL),(3320,1799,2,19,NULL),(3321,1800,1,20,NULL),(3322,1800,2,20,NULL),(3323,1801,1,21,NULL),(3324,1801,2,21,NULL),(3325,1802,1,22,NULL),(3326,1802,2,22,NULL),(3327,1803,1,23,NULL),(3328,1803,2,23,NULL),(3329,1804,1,24,NULL),(3330,1804,2,24,NULL),(3331,1805,1,19,NULL),(3332,1805,2,19,NULL),(3333,1806,1,20,NULL),(3334,1806,2,20,NULL),(3335,1807,1,21,NULL),(3336,1807,2,21,NULL),(3337,1808,1,22,NULL),(3338,1808,2,22,NULL),(3339,1809,1,23,NULL),(3340,1809,2,23,NULL),(3341,1810,1,24,NULL),(3342,1810,2,24,NULL),(3343,1811,1,19,NULL),(3344,1811,2,19,NULL),(3345,1812,1,20,NULL),(3346,1812,2,20,NULL),(3347,1813,1,21,NULL),(3348,1813,2,21,NULL),(3349,1814,1,22,NULL),(3350,1814,2,22,NULL),(3351,1815,1,23,NULL),(3352,1815,2,23,NULL),(3353,1816,1,24,NULL),(3354,1816,2,24,NULL),(3355,1817,1,22,NULL),(3356,1817,2,22,NULL),(3357,1818,1,22,NULL),(3358,1818,2,22,NULL),(3359,1819,1,22,NULL),(3360,1819,2,22,NULL),(3361,1820,1,22,NULL),(3362,1821,1,16,NULL),(3363,1821,2,16,NULL),(3364,1822,1,16,NULL),(3365,1822,2,16,NULL),(3366,1823,1,16,NULL),(3367,1824,1,22,NULL),(3368,1825,1,NULL,NULL),(3369,1825,2,NULL,NULL),(3370,1826,1,NULL,NULL),(3371,1826,2,NULL,NULL),(3372,1827,1,NULL,NULL),(3373,1827,2,NULL,NULL),(3374,1828,1,NULL,NULL),(3375,1828,2,NULL,NULL),(3376,1829,1,NULL,NULL),(3377,1829,2,NULL,NULL),(3378,1830,1,NULL,NULL),(3379,1830,2,NULL,NULL),(3380,1831,1,22,NULL),(3381,1831,2,22,NULL),(3382,1832,1,22,NULL),(3383,1832,2,22,NULL),(3384,1833,1,18,'adm_name'),(3385,1835,1,19,NULL),(3386,1835,2,19,NULL),(3387,1836,1,20,NULL),(3388,1836,2,20,NULL),(3389,1837,1,21,NULL),(3390,1837,2,21,NULL),(3391,1838,1,22,NULL),(3392,1838,2,22,NULL),(3393,1839,1,23,NULL),(3394,1839,2,23,NULL),(3395,1840,1,24,NULL),(3396,1840,2,24,NULL),(3397,1841,1,19,NULL),(3398,1841,2,19,NULL),(3399,1842,1,20,NULL),(3400,1842,2,20,NULL),(3401,1843,1,21,NULL),(3402,1843,2,21,NULL),(3403,1844,1,22,NULL),(3404,1844,2,22,NULL),(3405,1845,1,23,NULL),(3406,1845,2,23,NULL),(3407,1846,1,24,NULL),(3408,1846,2,24,NULL),(3409,1847,1,19,NULL),(3410,1847,2,19,NULL),(3411,1848,1,20,NULL),(3412,1848,2,20,NULL),(3413,1849,1,21,NULL),(3414,1849,2,21,NULL),(3415,1850,1,22,NULL),(3416,1850,2,22,NULL),(3417,1851,1,23,NULL),(3418,1851,2,23,NULL),(3419,1852,1,24,NULL),(3420,1852,2,24,NULL),(3421,1853,1,19,NULL),(3422,1853,2,19,NULL),(3423,1854,1,20,NULL),(3424,1854,2,20,NULL),(3425,1855,1,21,NULL),(3426,1855,2,21,NULL),(3427,1856,1,22,NULL),(3428,1856,2,22,NULL),(3429,1857,1,23,NULL),(3430,1857,2,23,NULL),(3431,1858,1,24,NULL),(3432,1858,2,24,NULL),(3433,1859,1,19,NULL),(3434,1859,2,19,NULL),(3435,1860,1,20,NULL),(3436,1860,2,20,NULL),(3437,1861,1,21,NULL),(3438,1861,2,21,NULL),(3439,1862,1,22,NULL),(3440,1862,2,22,NULL),(3441,1863,1,23,NULL),(3442,1863,2,23,NULL),(3443,1864,1,24,NULL),(3444,1864,2,24,NULL),(3445,1865,1,19,NULL),(3446,1865,2,19,NULL),(3447,1866,1,20,NULL),(3448,1866,2,20,NULL),(3449,1867,1,21,NULL),(3450,1867,2,21,NULL),(3451,1868,1,22,NULL),(3452,1868,2,22,NULL),(3453,1869,1,23,NULL),(3454,1869,2,23,NULL),(3455,1870,1,24,NULL),(3456,1870,2,24,NULL),(3457,1871,1,22,NULL),(3458,1871,2,22,NULL),(3459,1872,1,22,NULL),(3460,1872,2,22,NULL),(3461,1873,1,22,NULL),(3462,1873,2,22,NULL),(3463,1874,1,22,NULL),(3464,1875,1,16,NULL),(3465,1875,2,16,NULL),(3466,1876,1,16,NULL),(3467,1876,2,16,NULL),(3468,1877,1,16,NULL),(3469,1878,1,22,NULL),(3470,1879,1,NULL,NULL),(3471,1879,2,NULL,NULL),(3472,1880,1,NULL,NULL),(3473,1880,2,NULL,NULL),(3474,1881,1,NULL,NULL),(3475,1881,2,NULL,NULL),(3476,1882,1,NULL,NULL),(3477,1882,2,NULL,NULL),(3478,1883,1,NULL,NULL),(3479,1883,2,NULL,NULL),(3480,1884,1,NULL,NULL),(3481,1884,2,NULL,NULL),(3482,1885,1,22,NULL),(3483,1885,2,22,NULL),(3484,1886,1,22,NULL),(3485,1886,2,22,NULL),(3486,1887,1,18,'adm_name'),(3487,1888,1,36,'ids'),(3488,1889,1,36,'ids'),(3489,1890,1,36,'ids'),(3490,1891,1,36,'ids'),(3491,1892,1,36,'ids'),(3492,1893,1,19,NULL),(3493,1893,2,19,NULL),(3494,1894,1,20,NULL),(3495,1894,2,20,NULL),(3496,1895,1,21,NULL),(3497,1895,2,21,NULL),(3498,1896,1,22,NULL),(3499,1896,2,22,NULL),(3500,1897,1,23,NULL),(3501,1897,2,23,NULL),(3502,1898,1,24,NULL),(3503,1898,2,24,NULL),(3504,1899,1,19,NULL),(3505,1899,2,19,NULL),(3506,1900,1,20,NULL),(3507,1900,2,20,NULL),(3508,1901,1,21,NULL),(3509,1901,2,21,NULL),(3510,1902,1,22,NULL),(3511,1902,2,22,NULL),(3512,1903,1,23,NULL),(3513,1903,2,23,NULL),(3514,1904,1,24,NULL),(3515,1904,2,24,NULL),(3516,1905,1,19,NULL),(3517,1905,2,19,NULL),(3518,1906,1,20,NULL),(3519,1906,2,20,NULL),(3520,1907,1,21,NULL),(3521,1907,2,21,NULL),(3522,1908,1,22,NULL),(3523,1908,2,22,NULL),(3524,1909,1,23,NULL),(3525,1909,2,23,NULL),(3526,1910,1,24,NULL),(3527,1910,2,24,NULL),(3528,1911,1,19,NULL),(3529,1911,2,19,NULL),(3530,1912,1,20,NULL),(3531,1912,2,20,NULL),(3532,1913,1,21,NULL),(3533,1913,2,21,NULL),(3534,1914,1,22,NULL),(3535,1914,2,22,NULL),(3536,1915,1,23,NULL),(3537,1915,2,23,NULL),(3538,1916,1,24,NULL),(3539,1916,2,24,NULL),(3540,1917,1,19,NULL),(3541,1917,2,19,NULL),(3542,1918,1,20,NULL),(3543,1918,2,20,NULL),(3544,1919,1,21,NULL),(3545,1919,2,21,NULL),(3546,1920,1,22,NULL),(3547,1920,2,22,NULL),(3548,1921,1,23,NULL),(3549,1921,2,23,NULL),(3550,1922,1,24,NULL),(3551,1922,2,24,NULL),(3552,1923,1,19,NULL),(3553,1923,2,19,NULL),(3554,1924,1,20,NULL),(3555,1924,2,20,NULL),(3556,1925,1,21,NULL),(3557,1925,2,21,NULL),(3558,1926,1,22,NULL),(3559,1926,2,22,NULL),(3560,1927,1,23,NULL),(3561,1927,2,23,NULL),(3562,1928,1,24,NULL),(3563,1928,2,24,NULL),(3564,1929,1,22,NULL),(3565,1929,2,22,NULL),(3566,1930,1,22,NULL),(3567,1930,2,22,NULL),(3568,1931,1,22,NULL),(3569,1931,2,22,NULL),(3570,1932,1,22,NULL),(3571,1933,1,16,NULL),(3572,1933,2,16,NULL),(3573,1934,1,16,NULL),(3574,1934,2,16,NULL),(3575,1935,1,16,NULL),(3576,1936,1,22,NULL),(3577,1937,1,NULL,NULL),(3578,1937,2,NULL,NULL),(3579,1938,1,NULL,NULL),(3580,1938,2,NULL,NULL),(3581,1939,1,NULL,NULL),(3582,1939,2,NULL,NULL),(3583,1940,1,NULL,NULL),(3584,1940,2,NULL,NULL),(3585,1941,1,NULL,NULL),(3586,1941,2,NULL,NULL),(3587,1942,1,NULL,NULL),(3588,1942,2,NULL,NULL),(3589,1943,1,22,NULL),(3590,1943,2,22,NULL),(3591,1944,1,22,NULL),(3592,1944,2,22,NULL),(3593,1945,1,18,'adm_name'),(3594,1946,1,36,'ids'),(3595,1947,1,36,'ids'),(3596,1948,1,36,'ids'),(3597,1949,1,36,'ids'),(3598,1950,1,36,'ids');
/*!40000 ALTER TABLE `type_name_value_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_int_entry`
--

DROP TABLE IF EXISTS `type_name_value_int_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_int_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` int DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_int_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_int_entry`
--

LOCK TABLES `type_name_value_int_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_int_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_int_entry` VALUES (2644,NULL),(2645,NULL),(2656,NULL),(2657,NULL),(2668,NULL),(2669,NULL),(2680,NULL),(2681,NULL),(2692,NULL),(2693,NULL),(2704,NULL),(2705,NULL),(2723,NULL),(2724,NULL),(2735,NULL),(2736,NULL),(2747,NULL),(2748,NULL),(2759,NULL),(2760,NULL),(2771,NULL),(2772,NULL),(2783,NULL),(2784,NULL),(2808,NULL),(2809,NULL),(2820,NULL),(2821,NULL),(2832,NULL),(2833,NULL),(2844,NULL),(2845,NULL),(2856,NULL),(2857,NULL),(2868,NULL),(2869,NULL),(2894,NULL),(2895,NULL),(2906,NULL),(2907,NULL),(2918,NULL),(2919,NULL),(2930,NULL),(2931,NULL),(2942,NULL),(2943,NULL),(2954,NULL),(2955,NULL),(2980,NULL),(2981,NULL),(2992,NULL),(2993,NULL),(3004,NULL),(3005,NULL),(3016,NULL),(3017,NULL),(3028,NULL),(3029,NULL),(3040,NULL),(3041,NULL),(3081,NULL),(3082,NULL),(3093,NULL),(3094,NULL),(3105,NULL),(3106,NULL),(3117,NULL),(3118,NULL),(3129,NULL),(3130,NULL),(3141,NULL),(3142,NULL),(3182,NULL),(3183,NULL),(3194,NULL),(3195,NULL),(3206,NULL),(3207,NULL),(3218,NULL),(3219,NULL),(3230,NULL),(3231,NULL),(3242,NULL),(3243,NULL),(3283,NULL),(3284,NULL),(3295,NULL),(3296,NULL),(3307,NULL),(3308,NULL),(3319,NULL),(3320,NULL),(3331,NULL),(3332,NULL),(3343,NULL),(3344,NULL),(3385,NULL),(3386,NULL),(3397,NULL),(3398,NULL),(3409,NULL),(3410,NULL),(3421,NULL),(3422,NULL),(3433,NULL),(3434,NULL),(3445,NULL),(3446,NULL),(3492,NULL),(3493,NULL),(3504,NULL),(3505,NULL),(3516,NULL),(3517,NULL),(3528,NULL),(3529,NULL),(3540,NULL),(3541,NULL),(3552,NULL),(3553,NULL);
/*!40000 ALTER TABLE `type_name_value_int_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_obj_entry`
--

DROP TABLE IF EXISTS `type_name_value_obj_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_obj_entry` (
  `tnv_id` int unsigned NOT NULL,
  `obj_actual_definition_id` int unsigned DEFAULT NULL,
  KEY `obj_actual_definition_id` (`obj_actual_definition_id`),
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_obj_entry_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE SET NULL,
  CONSTRAINT `type_name_value_obj_entry_ibfk_2` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_obj_entry`
--

LOCK TABLES `type_name_value_obj_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_obj_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_obj_entry` VALUES (3487,NULL),(3488,NULL),(3489,NULL),(3490,NULL),(3491,NULL),(3594,NULL),(3595,NULL),(3596,NULL),(3597,NULL),(3598,NULL);
/*!40000 ALTER TABLE `type_name_value_obj_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_real32_entry`
--

DROP TABLE IF EXISTS `type_name_value_real32_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_real32_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` float DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_real32_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_real32_entry`
--

LOCK TABLES `type_name_value_real32_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_real32_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_real32_entry` VALUES (2652,NULL),(2653,NULL),(2664,NULL),(2665,NULL),(2676,NULL),(2677,NULL),(2688,NULL),(2689,NULL),(2700,NULL),(2701,NULL),(2712,NULL),(2713,NULL),(2731,NULL),(2732,NULL),(2743,NULL),(2744,NULL),(2755,NULL),(2756,NULL),(2767,NULL),(2768,NULL),(2779,NULL),(2780,NULL),(2791,NULL),(2792,NULL),(2816,NULL),(2817,NULL),(2828,NULL),(2829,NULL),(2840,NULL),(2841,NULL),(2852,NULL),(2853,NULL),(2864,NULL),(2865,NULL),(2876,NULL),(2877,NULL),(2902,NULL),(2903,NULL),(2914,NULL),(2915,NULL),(2926,NULL),(2927,NULL),(2938,NULL),(2939,NULL),(2950,NULL),(2951,NULL),(2962,NULL),(2963,NULL),(2988,NULL),(2989,NULL),(3000,NULL),(3001,NULL),(3012,NULL),(3013,NULL),(3024,NULL),(3025,NULL),(3036,NULL),(3037,NULL),(3048,NULL),(3049,NULL),(3089,NULL),(3090,NULL),(3101,NULL),(3102,NULL),(3113,NULL),(3114,NULL),(3125,NULL),(3126,NULL),(3137,NULL),(3138,NULL),(3149,NULL),(3150,NULL),(3190,NULL),(3191,NULL),(3202,NULL),(3203,NULL),(3214,NULL),(3215,NULL),(3226,NULL),(3227,NULL),(3238,NULL),(3239,NULL),(3250,NULL),(3251,NULL),(3291,NULL),(3292,NULL),(3303,NULL),(3304,NULL),(3315,NULL),(3316,NULL),(3327,NULL),(3328,NULL),(3339,NULL),(3340,NULL),(3351,NULL),(3352,NULL),(3393,NULL),(3394,NULL),(3405,NULL),(3406,NULL),(3417,NULL),(3418,NULL),(3429,NULL),(3430,NULL),(3441,NULL),(3442,NULL),(3453,NULL),(3454,NULL),(3500,NULL),(3501,NULL),(3512,NULL),(3513,NULL),(3524,NULL),(3525,NULL),(3536,NULL),(3537,NULL),(3548,NULL),(3549,NULL),(3560,NULL),(3561,NULL);
/*!40000 ALTER TABLE `type_name_value_real32_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_real64_entry`
--

DROP TABLE IF EXISTS `type_name_value_real64_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_real64_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` double DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_real64_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_real64_entry`
--

LOCK TABLES `type_name_value_real64_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_real64_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_real64_entry` VALUES (2654,NULL),(2655,NULL),(2666,NULL),(2667,NULL),(2678,NULL),(2679,NULL),(2690,NULL),(2691,NULL),(2702,NULL),(2703,NULL),(2714,NULL),(2715,NULL),(2733,NULL),(2734,NULL),(2745,NULL),(2746,NULL),(2757,NULL),(2758,NULL),(2769,NULL),(2770,NULL),(2781,NULL),(2782,NULL),(2793,NULL),(2794,NULL),(2818,NULL),(2819,NULL),(2830,NULL),(2831,NULL),(2842,NULL),(2843,NULL),(2854,NULL),(2855,NULL),(2866,NULL),(2867,NULL),(2878,NULL),(2879,NULL),(2904,NULL),(2905,NULL),(2916,NULL),(2917,NULL),(2928,NULL),(2929,NULL),(2940,NULL),(2941,NULL),(2952,NULL),(2953,NULL),(2964,NULL),(2965,NULL),(2990,NULL),(2991,NULL),(3002,NULL),(3003,NULL),(3014,NULL),(3015,NULL),(3026,NULL),(3027,NULL),(3038,NULL),(3039,NULL),(3050,NULL),(3051,NULL),(3091,NULL),(3092,NULL),(3103,NULL),(3104,NULL),(3115,NULL),(3116,NULL),(3127,NULL),(3128,NULL),(3139,NULL),(3140,NULL),(3151,NULL),(3152,NULL),(3192,NULL),(3193,NULL),(3204,NULL),(3205,NULL),(3216,NULL),(3217,NULL),(3228,NULL),(3229,NULL),(3240,NULL),(3241,NULL),(3252,NULL),(3253,NULL),(3293,NULL),(3294,NULL),(3305,NULL),(3306,NULL),(3317,NULL),(3318,NULL),(3329,NULL),(3330,NULL),(3341,NULL),(3342,NULL),(3353,NULL),(3354,NULL),(3395,NULL),(3396,NULL),(3407,NULL),(3408,NULL),(3419,NULL),(3420,NULL),(3431,NULL),(3432,NULL),(3443,NULL),(3444,NULL),(3455,NULL),(3456,NULL),(3502,NULL),(3503,NULL),(3514,NULL),(3515,NULL),(3526,NULL),(3527,NULL),(3538,NULL),(3539,NULL),(3550,NULL),(3551,NULL),(3562,NULL),(3563,NULL);
/*!40000 ALTER TABLE `type_name_value_real64_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_string_entry`
--

DROP TABLE IF EXISTS `type_name_value_string_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_string_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` varchar DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_string_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_string_entry`
--

LOCK TABLES `type_name_value_string_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_string_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_string_entry` VALUES (3384,NULL),(3486,NULL),(3593,NULL);
/*!40000 ALTER TABLE `type_name_value_string_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_uint_entry`
--

DROP TABLE IF EXISTS `type_name_value_uint_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_uint_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` int unsigned DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_uint_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_uint_entry`
--

LOCK TABLES `type_name_value_uint_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_uint_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_uint_entry` VALUES (2646,NULL),(2647,NULL),(2658,NULL),(2659,NULL),(2670,NULL),(2671,NULL),(2682,NULL),(2683,NULL),(2694,NULL),(2695,NULL),(2706,NULL),(2707,NULL),(2725,NULL),(2726,NULL),(2737,NULL),(2738,NULL),(2749,NULL),(2750,NULL),(2761,NULL),(2762,NULL),(2773,NULL),(2774,NULL),(2785,NULL),(2786,NULL),(2810,NULL),(2811,NULL),(2822,NULL),(2823,NULL),(2834,NULL),(2835,NULL),(2846,NULL),(2847,NULL),(2858,NULL),(2859,NULL),(2870,NULL),(2871,NULL),(2896,NULL),(2897,NULL),(2908,NULL),(2909,NULL),(2920,NULL),(2921,NULL),(2932,NULL),(2933,NULL),(2944,NULL),(2945,NULL),(2956,NULL),(2957,NULL),(2982,NULL),(2983,NULL),(2994,NULL),(2995,NULL),(3006,NULL),(3007,NULL),(3018,NULL),(3019,NULL),(3030,NULL),(3031,NULL),(3042,NULL),(3043,NULL),(3083,NULL),(3084,NULL),(3095,NULL),(3096,NULL),(3107,NULL),(3108,NULL),(3119,NULL),(3120,NULL),(3131,NULL),(3132,NULL),(3143,NULL),(3144,NULL),(3184,NULL),(3185,NULL),(3196,NULL),(3197,NULL),(3208,NULL),(3209,NULL),(3220,NULL),(3221,NULL),(3232,NULL),(3233,NULL),(3244,NULL),(3245,NULL),(3285,NULL),(3286,NULL),(3297,NULL),(3298,NULL),(3309,NULL),(3310,NULL),(3321,NULL),(3322,NULL),(3333,NULL),(3334,NULL),(3345,NULL),(3346,NULL),(3387,NULL),(3388,NULL),(3399,NULL),(3400,NULL),(3411,NULL),(3412,NULL),(3423,NULL),(3424,NULL),(3435,NULL),(3436,NULL),(3447,NULL),(3448,NULL),(3494,NULL),(3495,NULL),(3506,NULL),(3507,NULL),(3518,NULL),(3519,NULL),(3530,NULL),(3531,NULL),(3542,NULL),(3543,NULL),(3554,NULL),(3555,NULL);
/*!40000 ALTER TABLE `type_name_value_uint_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_uvast_entry`
--

DROP TABLE IF EXISTS `type_name_value_uvast_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_uvast_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` bigint unsigned DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_uvast_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_uvast_entry`
--

LOCK TABLES `type_name_value_uvast_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_uvast_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_uvast_entry` VALUES (2650,NULL),(2651,NULL),(2662,NULL),(2663,NULL),(2674,NULL),(2675,NULL),(2686,NULL),(2687,NULL),(2698,NULL),(2699,NULL),(2710,NULL),(2711,NULL),(2716,NULL),(2717,NULL),(2718,NULL),(2719,NULL),(2720,NULL),(2721,NULL),(2722,NULL),(2729,NULL),(2730,NULL),(2741,NULL),(2742,NULL),(2753,NULL),(2754,NULL),(2765,NULL),(2766,NULL),(2777,NULL),(2778,NULL),(2789,NULL),(2790,NULL),(2795,NULL),(2796,NULL),(2797,NULL),(2798,NULL),(2799,NULL),(2800,NULL),(2801,NULL),(2807,NULL),(2814,NULL),(2815,NULL),(2826,NULL),(2827,NULL),(2838,NULL),(2839,NULL),(2850,NULL),(2851,NULL),(2862,NULL),(2863,NULL),(2874,NULL),(2875,NULL),(2880,NULL),(2881,NULL),(2882,NULL),(2883,NULL),(2884,NULL),(2885,NULL),(2886,NULL),(2892,NULL),(2900,NULL),(2901,NULL),(2912,NULL),(2913,NULL),(2924,NULL),(2925,NULL),(2936,NULL),(2937,NULL),(2948,NULL),(2949,NULL),(2960,NULL),(2961,NULL),(2966,NULL),(2967,NULL),(2968,NULL),(2969,NULL),(2970,NULL),(2971,NULL),(2972,NULL),(2978,NULL),(2986,NULL),(2987,NULL),(2998,NULL),(2999,NULL),(3010,NULL),(3011,NULL),(3022,NULL),(3023,NULL),(3034,NULL),(3035,NULL),(3046,NULL),(3047,NULL),(3052,NULL),(3053,NULL),(3054,NULL),(3055,NULL),(3056,NULL),(3057,NULL),(3058,NULL),(3064,NULL),(3077,NULL),(3078,NULL),(3079,NULL),(3080,NULL),(3087,NULL),(3088,NULL),(3099,NULL),(3100,NULL),(3111,NULL),(3112,NULL),(3123,NULL),(3124,NULL),(3135,NULL),(3136,NULL),(3147,NULL),(3148,NULL),(3153,NULL),(3154,NULL),(3155,NULL),(3156,NULL),(3157,NULL),(3158,NULL),(3159,NULL),(3165,NULL),(3178,NULL),(3179,NULL),(3180,NULL),(3181,NULL),(3188,NULL),(3189,NULL),(3200,NULL),(3201,NULL),(3212,NULL),(3213,NULL),(3224,NULL),(3225,NULL),(3236,NULL),(3237,NULL),(3248,NULL),(3249,NULL),(3254,NULL),(3255,NULL),(3256,NULL),(3257,NULL),(3258,NULL),(3259,NULL),(3260,NULL),(3266,NULL),(3279,NULL),(3280,NULL),(3281,NULL),(3282,NULL),(3289,NULL),(3290,NULL),(3301,NULL),(3302,NULL),(3313,NULL),(3314,NULL),(3325,NULL),(3326,NULL),(3337,NULL),(3338,NULL),(3349,NULL),(3350,NULL),(3355,NULL),(3356,NULL),(3357,NULL),(3358,NULL),(3359,NULL),(3360,NULL),(3361,NULL),(3367,NULL),(3380,NULL),(3381,NULL),(3382,NULL),(3383,NULL),(3391,NULL),(3392,NULL),(3403,NULL),(3404,NULL),(3415,NULL),(3416,NULL),(3427,NULL),(3428,NULL),(3439,NULL),(3440,NULL),(3451,NULL),(3452,NULL),(3457,NULL),(3458,NULL),(3459,NULL),(3460,NULL),(3461,NULL),(3462,NULL),(3463,NULL),(3469,NULL),(3482,NULL),(3483,NULL),(3484,NULL),(3485,NULL),(3498,NULL),(3499,NULL),(3510,NULL),(3511,NULL),(3522,NULL),(3523,NULL),(3534,NULL),(3535,NULL),(3546,NULL),(3547,NULL),(3558,NULL),(3559,NULL),(3564,NULL),(3565,NULL),(3566,NULL),(3567,NULL),(3568,NULL),(3569,NULL),(3570,NULL),(3576,NULL),(3589,NULL),(3590,NULL),(3591,NULL),(3592,NULL);
/*!40000 ALTER TABLE `type_name_value_uvast_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `type_name_value_vast_entry`
--

DROP TABLE IF EXISTS `type_name_value_vast_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `type_name_value_vast_entry` (
  `tnv_id` int unsigned NOT NULL,
  `entry_value` bigint DEFAULT NULL,
  KEY `tnv_id` (`tnv_id`),
  CONSTRAINT `type_name_value_vast_entry_ibfk_1` FOREIGN KEY (`tnv_id`) REFERENCES `type_name_value_entry` (`tnv_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `type_name_value_vast_entry`
--

LOCK TABLES `type_name_value_vast_entry` WRITE;
/*!40000 ALTER TABLE `type_name_value_vast_entry` DISABLE KEYS */;
INSERT INTO `type_name_value_vast_entry` VALUES (2648,NULL),(2649,NULL),(2660,NULL),(2661,NULL),(2672,NULL),(2673,NULL),(2684,NULL),(2685,NULL),(2696,NULL),(2697,NULL),(2708,NULL),(2709,NULL),(2727,NULL),(2728,NULL),(2739,NULL),(2740,NULL),(2751,NULL),(2752,NULL),(2763,NULL),(2764,NULL),(2775,NULL),(2776,NULL),(2787,NULL),(2788,NULL),(2812,NULL),(2813,NULL),(2824,NULL),(2825,NULL),(2836,NULL),(2837,NULL),(2848,NULL),(2849,NULL),(2860,NULL),(2861,NULL),(2872,NULL),(2873,NULL),(2898,NULL),(2899,NULL),(2910,NULL),(2911,NULL),(2922,NULL),(2923,NULL),(2934,NULL),(2935,NULL),(2946,NULL),(2947,NULL),(2958,NULL),(2959,NULL),(2984,NULL),(2985,NULL),(2996,NULL),(2997,NULL),(3008,NULL),(3009,NULL),(3020,NULL),(3021,NULL),(3032,NULL),(3033,NULL),(3044,NULL),(3045,NULL),(3085,NULL),(3086,NULL),(3097,NULL),(3098,NULL),(3109,NULL),(3110,NULL),(3121,NULL),(3122,NULL),(3133,NULL),(3134,NULL),(3145,NULL),(3146,NULL),(3186,NULL),(3187,NULL),(3198,NULL),(3199,NULL),(3210,NULL),(3211,NULL),(3222,NULL),(3223,NULL),(3234,NULL),(3235,NULL),(3246,NULL),(3247,NULL),(3287,NULL),(3288,NULL),(3299,NULL),(3300,NULL),(3311,NULL),(3312,NULL),(3323,NULL),(3324,NULL),(3335,NULL),(3336,NULL),(3347,NULL),(3348,NULL),(3389,NULL),(3390,NULL),(3401,NULL),(3402,NULL),(3413,NULL),(3414,NULL),(3425,NULL),(3426,NULL),(3437,NULL),(3438,NULL),(3449,NULL),(3450,NULL),(3496,NULL),(3497,NULL),(3508,NULL),(3509,NULL),(3520,NULL),(3521,NULL),(3532,NULL),(3533,NULL),(3544,NULL),(3545,NULL),(3556,NULL),(3557,NULL);
/*!40000 ALTER TABLE `type_name_value_vast_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `variable_actual_definition`
--

DROP TABLE IF EXISTS `variable_actual_definition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `variable_actual_definition` (
  `obj_actual_definition_id` int unsigned NOT NULL,
  `data_type_id` int unsigned NOT NULL,
  `expression_id` int unsigned NOT NULL,
  PRIMARY KEY (`obj_actual_definition_id`),
  UNIQUE KEY `data_type_id` (`data_type_id`,`expression_id`),
  KEY `expression_id` (`expression_id`),
  CONSTRAINT `variable_actual_definition_ibfk_1` FOREIGN KEY (`obj_actual_definition_id`) REFERENCES `obj_actual_definition` (`obj_actual_definition_id`) ON DELETE CASCADE,
  CONSTRAINT `variable_actual_definition_ibfk_2` FOREIGN KEY (`data_type_id`) REFERENCES `data_type` (`data_type_id`) ON DELETE CASCADE,
  CONSTRAINT `variable_actual_definition_ibfk_3` FOREIGN KEY (`expression_id`) REFERENCES `expression` (`expression_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `variable_actual_definition`
--

LOCK TABLES `variable_actual_definition` WRITE;
/*!40000 ALTER TABLE `variable_actual_definition` DISABLE KEYS */;
/*!40000 ALTER TABLE `variable_actual_definition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary view structure for view `vw_ctrl_actual`
--

DROP TABLE IF EXISTS `vw_ctrl_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_ctrl_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_ctrl_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `ap_spec_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_ctrl_definition`
--

DROP TABLE IF EXISTS `vw_ctrl_definition`;
/*!50001 DROP VIEW IF EXISTS `vw_ctrl_definition`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_ctrl_definition` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_edd_actual`
--

DROP TABLE IF EXISTS `vw_edd_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_edd_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_edd_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `ap_spec_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_edd_formal`
--

DROP TABLE IF EXISTS `vw_edd_formal`;
/*!50001 DROP VIEW IF EXISTS `vw_edd_formal`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_edd_formal` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_mac_actual`
--

DROP TABLE IF EXISTS `vw_mac_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_mac_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_mac_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `ap_spec_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_mac_definition`
--

DROP TABLE IF EXISTS `vw_mac_definition`;
/*!50001 DROP VIEW IF EXISTS `vw_mac_definition`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_mac_definition` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `max_call_depth`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_oper_actual`
--

DROP TABLE IF EXISTS `vw_oper_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_oper_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_oper_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `data_type_id`,
 1 AS `num_operands`,
 1 AS `tnvc_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_rpt_actual`
--

DROP TABLE IF EXISTS `vw_rpt_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_rpt_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_rpt_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `ap_spec_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_rptt_formal`
--

DROP TABLE IF EXISTS `vw_rptt_formal`;
/*!50001 DROP VIEW IF EXISTS `vw_rptt_formal`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_rptt_formal` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_formal_definition_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_sbr_actual`
--

DROP TABLE IF EXISTS `vw_sbr_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_sbr_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_sbr_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `expression_id`,
 1 AS `run_count`,
 1 AS `start_time`,
 1 AS `ac_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_tblt_actual`
--

DROP TABLE IF EXISTS `vw_tblt_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_tblt_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_tblt_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `tnvc_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_tbr_actual`
--

DROP TABLE IF EXISTS `vw_tbr_actual`;
/*!50001 DROP VIEW IF EXISTS `vw_tbr_actual`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_tbr_actual` AS SELECT 
 1 AS `obj_metadata_id`,
 1 AS `obj_name`,
 1 AS `namespace_id`,
 1 AS `obj_actual_definition_id`,
 1 AS `run_count`,
 1 AS `start_time`,
 1 AS `ac_id`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary view structure for view `vw_tnvc`
--

DROP TABLE IF EXISTS `vw_tnvc`;
/*!50001 DROP VIEW IF EXISTS `vw_tnvc`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_tnvc` AS SELECT 
 1 AS `tnvc_id`,
 1 AS `tnv_id`,
 1 AS `order_num`,
 1 AS `data_type_id`,
 1 AS `data_name`,
 1 AS `use_desc`*/;
SET character_set_client = @saved_cs_client;

--
-- Dumping routines for database 'amp_core'
--
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_const_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_const_actual_definition`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
BEGIN
	IF (p_obj_id != null) THEN
		SET @metadata_id = (SELECT obj_metadata_id FROM obj_actual_definition WHERE obj_actual_definition_id = p_obj_id );
	ELSE
		IF (p_obj_name != NULL) THEN 
			SET @metadata_id = (SELECT obj_metadata_id FROM obj_metadata WHERE obj_name = p_obj_name); 
        END IF;
	END IF;
    CALL SP__delete_obj_metadata(@metadata_id);
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_control_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_control_actual_definition`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
ctrl_actual_definition_del:BEGIN
	
	DECLARE done INT DEFAULT FALSE;
    DECLARE actual_definition_id_hold, ap_spec_id_hold int(10) unsigned;
    DECLARE actual_definition_cursor CURSOR
			FOR SELECT actual_definition_id, ap_spec_id FROM vw_ctrl_actual_definition WHERE obj_name = p_obj_name;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;	
    
    
	If( p_inst_id is not null) then 
		SET @ap_id = (SELECT ap_spec_id from ctrl_actual_definition where actual_definition_id = p_inst_id );
		IF( @ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = @ap_id;
		END IF;
		DELETE FROM ctrl_actual_definition 
WHERE
    intance_id = p_actual_definition_id;
		CALL SP__delete_obj_actual_definition(p_actual_definition_id, p_obj_name);
	ELSE 
		IF( p_obj_name is null) then 
			leave ctrl_actual_definition_del;
		END IF;
		OPEN actual_definition_cursor;
        read_loop: LOOP
			FETCH NEXT FROM actual_definition_cursor INTO 
				actual_definition_id_hold, ap_spec_id_hold;
			IF done THEN
				LEAVE read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;

			DELETE FROM ctrl_actual_definition
WHERE
    amp_core.ctrl_actual_definition.actual_definition_id = actual_definition_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_definition_cursor;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_control_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_control_formal_definition`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
BEGIN
	CALL SP__delete_obj_formal_definition(p_obj_id, p_obj_name);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_edd_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_edd_actual_definition`(IN p_actual_definition_id int(10) unsigned, p_obj_name varchar)
edd_actual_definition_del:BEGIN
	
	DECLARE done INT DEFAULT FALSE;
    DECLARE actual_definition_id_hold, ap_spec_id_hold int(10) unsigned;
    DECLARE actual_definition_cursor CURSOR
			FOR SELECT actual_definition_id, ap_spec_id FROM vw_edd_actual WHERE obj_name = p_obj_name;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;	
    
    
	If( p_inst_id is not null) then 
		SET @ap_id = (SELECT ap_spec_id from edd_actual_definition where actual_definition_id = p_inst_id );
		IF( @ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = @ap_id;
		END IF;
		DELETE FROM edd_actual_definition 
WHERE
    intance_id = p_actual_definition_id;
		CALL SP__delete_obj_actual_definition(p_actual_definition_id, p_obj_name);
	ELSE 
		IF( p_obj_name is null) then 
			leave edd_actual_definition_del;
		END IF;
		OPEN actual_definition_cursor;
        read_loop: LOOP
			FETCH NEXT FROM actual_definition_cursor INTO 
				actual_definition_id_hold, ap_spec_id_hold;
			IF done THEN
				LEAVE read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;

			DELETE FROM edd_actual_definition
WHERE
    amp_core.edd_actual_definition.actual_definition_id = actual_definition_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_definition_cursor;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_edd_formal_defintion` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_edd_formal_defintion`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
edd_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE edd_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_edd_formal where obj_id = p_obj_id);
    END IF ;
    
	
	CALL SP__delete_edd_actual_definition(null, p_obj_name);
    
	SET @def_id = (select obj_formal_definition_id from obj_formal_definition where obj_metadata_id = p_obj_id);
	SET @fp_id = (SELECT fp_spec_id from edd_formal_definition where obj_formal_definition_id = @def_id );
	DELETE FROM formal_parmspec WHERE fp_spec_id = @fp_id;
    
	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_expression` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_expression`(IN p_expr_id INT(10) UNSIGNED)
BEGIN 
	DELETE from  amp_core.expression WHERE (expression_id = p_expr_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_literal_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_literal_actual_definition`(IN p_obj_id int(10) unsigned)
BEGIN
	CALL SP__delete_obj_definition(p_obj_id); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_mac_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_mac_actual_definition`(IN p_inst_id int(10) unsigned, p_obj_name varchar)
mac_inst_del:BEGIN
	
	DECLARE done INT DEFAULT FALSE;
    DECLARE actual_id_hold, ap_spec_id_hold, ac_id_hold int(10) unsigned;
    DECLARE actual_cursor CURSOR
			FOR SELECT actual_id, ap_spec_id, ac_id  WHERE obj_name = p_obj_name;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;	
    
    
	If( p_inst_id is not null) then 
		SET @ap_id = (SELECT ap_spec_id from macro_actual_definition where actual_id = p_inst_id );
		IF( @ap_id is not null) THEN
			DELETE FROM actual_parmspec WHERE ap_spec_id = @ap_id;
		END IF;
		DELETE FROM macro_actual 
WHERE
    intance_id = p_inst_id;
		CALL SP__delete_obj_actual(p_inst_id, p_obj_name);
	ELSE 
		IF( p_obj_name is null) then 
			leave mac_inst_del;
		END IF;
		OPEN actual_cursor;
        read_loop: LOOP
			FETCH NEXT FROM actual_cursor INTO 
				actual_id_hold, ap_spec_id_hold, ac_id_hold;
			IF done THEN
				LEAVE read_loop;
			END IF;

			DELETE FROM actual_parmspec WHERE ap_spec_id = ap_spec_id_hold;
			DELETE FROM ari_collection WHERE ac_id = ac_id_hold;

			DELETE FROM macro_actual_definition
WHERE
    amp_core.macro_actual.actual_id = actual_id_hold;			
		END LOOP;
        CALL SP__delete_obj_actual_definition(null, p_obj_name);
    end if;
    CLOSE actual_cursor;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_mac_formal_defintion` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_mac_formal_defintion`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
mac_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE mac_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_mac_formal where obj_id = p_obj_id);
    END IF ;
    
	
	CALL SP__delete_macro_actual(null, p_obj_name);
	
	SET @def_id = (select definition_id from obj_formal_definition where obj_id = p_obj_id);
	SET @fp_id = (SELECT fp_spec_id from macro_formal_definition where definition_id = @def_id );
    SET @ac_id = (SELECT ac_id FROM macro_formal_definition where definition_id = @def_id );
    
	DELETE FROM formal_parmspec WHERE fp_spec_id = @fp_id;
	DELETE FROM ari_collection WHERE ac_id = @ac_id;

	CALL SP__delete_obj_formal_defintion(p_obj_id, p_obj_name);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_obj_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_obj_actual_definition`(p_act_id int(10) UNSIGNED, p_obj_name varchar )
obj_inst_del:BEGIN 

	
	IF( p_inst_id is not null) THEN
		DELETE FROM obj_actual_definition WHERE actual_definition_id = p_act_id;
    
	ELSE 
		IF( p_obj_name is null or p_namespace_id) then 
			leave obj_inst_del;
		END IF;
		DELETE FROM amp_core.obj_actual_definition where obj_metadata_id =(select obj_metadata_id from obj_metadat where obj_name =  p_obj_name ); 
	END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_obj_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_obj_formal_definition`(p_obj_id int(10) unsigned, p_obj_name varchar )
obj_delete:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE obj_delete;
    END IF;
    
	DELETE FROM amp_core.obj_metadata WHERE obj_id = p_obj_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_obj_metadata` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_obj_metadata`(IN p_obj_id INT UNSIGNED)
BEGIN 
	DELETE FROM obj_metadata
WHERE obj_metadata_id = p_obj_id;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_oper_actual_defintion` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_oper_actual_defintion`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
oper_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE oper_def_del;
    END IF;
    
    SET @def_id = (select definition_id from obj_actual_definition where obj_actual_definition_id = p_obj_id);
	SET @fp_id = (SELECT fp_spec_id from macro_actual_definition where obj_actual_definition_id = @def_id );
    SET @ac_id = (SELECT ac_id FROM macro_actual_definition where obj_actual_definition_id = @def_id );
    
	DELETE FROM formal_parmspec WHERE fp_spec_id = @fp_id;
	DELETE FROM ari_collection WHERE ac_id = @ac_id;

	CALL SP__delete_obj_actual_defintion(p_obj_id, p_obj_name);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_sbr_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_sbr_actual_definition`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
sbr_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE sbr_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_sbr_formal where obj_id = p_obj_id);
    END IF ;
    
	SET @exp_id = (SELECT expression_id FROM vw_sbr_actual WHERE obj_actual_definition = p_obj_id); 
	SET @ac_id = (SELECT ac_id FROM vw_sbr_actual WHERE obj_actual_definition = p_obj_id); 
	
    DELETE FROM ari_collection WHERE ac_id = @ac_id;
    DELETE FROM expression WHERE expression_id = @exp_id;
	
    CALL SP__delete_obj_atual_defintion(p_obj_id, p_obj_name);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_table_template_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_table_template_actual_definition`(IN p_obj_id int(10) unsigned)
BEGIN
	IF p_obj_id != NULL THEN BEGIN
		SET @tnvc_id = (select tnvc_id from table_template_actual_definition where obj_actual_definition_id = p_obj_id);
	END;
	ELSE BEGIN
		SET @tnvc_id = 
			(select tnvc_id from table_template_actual_definition where obj_atual_definition_id = 
				(select obj_actual_definition_id  from obj_actual_definition where obj_metadata_id = 
					(select obj_metadata_id from obj_metadata where obj_name = p_obj_name)));
	END;
   END IF;
   CALL SP__delete_tnvc(@tnvc_id);
   CALL SP__delete_obj_actual_definition(p_obj_id, null);
   
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_tbr_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_tbr_actual_definition`(IN p_obj_id int(10) unsigned, p_obj_name varchar)
tbr_def_del:BEGIN
	IF( p_obj_id is Null AND p_obj_name is not NULL) THEN
		SET p_obj_id =  (select obj_id FROM amp_core.obj_metadata WHERE obj_name = p_obj_name);
    ELSE 
		LEAVE tbr_def_del;
    END IF;
    
    IF(p_obj_name is NULL) THEN
		SET p_obj_name = (select obj_name from vw_tbr_formal where obj_id = p_obj_id);
    END IF ;
    

	SET @ac_id = (SELECT ac_id FROM vw_tbr_actual WHERE obj_actual_definition = p_obj_id); 
	
    DELETE FROM ari_collection WHERE ac_id = @ac_id;
	
    CALL SP__delete_obj_atual_defintion(p_obj_id, p_obj_name);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__delete_variable_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__delete_variable_definition`(IN p_definition_id int(10) unsigned, p_obj_name varchar)
BEGIN
	
    SET @exp_id = (SELECT expression_id from variable_actual_definition where p_definition_id = obj_actual_definition);
    CALL SP__delete_expression(@exp_id);
    CALL SP__delete_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
	
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_actual_parmspec` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_actual_parmspec`(IN p_fp_spec_id int(10) unsigned, p_num_parms int(10) unsigned, p_use_desc varchar, OUT r_ap_spec_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.actual_parmspec(fp_spec_id, num_parm, use_desc) VALUES(p_fp_spec_id, p_num_parms, p_use_desc); 
    SET r_ap_spec_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_actual_parms_ac` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_actual_parms_ac`(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_ac_id INT(10) UNSIGNED)
BEGIN 
	INSERT INTO amp_core.actual_parm( ap_spec_id, order_num) VALUES(p_ap_spec_id, p_order_num);
	SET @r_ap_id = last_insert_id();
    
	INSERT INTO actual_parm_ac (ap_id, ac_id)
	VALUES (@r_ap_id, p_ac_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_actual_parms_names` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_actual_parms_names`(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_data_type_id varchar, p_fp_id INT(10) UNSIGNED)
BEGIN 
	INSERT INTO amp_core.actual_parm( ap_spec_id, order_num) VALUES(p_ap_spec_id, p_order_num);
    SET @r_ap_id = last_insert_id();
    
    INSERT INTO amp_core.actual_parm_name(ap_id, data_type_id, fp_id) VALUES(@r_ap_id, (select data_type_id from data_type where type_name = p_data_type_id), p_fp_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_actual_parms_object` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_actual_parms_object`(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_data_type_id varchar , p_obj_actual_definition INT(10) UNSIGNED)
BEGIN 
	INSERT INTO amp_core.actual_parm( ap_spec_id, order_num) VALUES(p_ap_spec_id, p_order_num);
    SET @r_ap_id = last_insert_id();
    
    INSERT INTO amp_core.actual_parm_object(ap_id, data_type_id, obj_actual_definition_id) VALUES(@r_ap_id, (SELECT data_type_id from data_type where type_name = p_data_type_id), p_obj_actual_definition);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_actual_parms_tnvc` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_actual_parms_tnvc`(IN p_ap_spec_id INT(10) unsigned, p_order_num int(10) unsigned, p_tnvc_id INT(10) UNSIGNED)
BEGIN 
	INSERT INTO amp_core.actual_parm( ap_spec_id, order_num) VALUES(p_ap_spec_id, p_order_num);
	SET @r_ap_id = last_insert_id();
    
	INSERT INTO actual_parm_tnvc (ap_id, tnvc_id)
	VALUES (@r_ap_id, p_tnvc_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_ac_actual_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_ac_actual_entry`(IN p_ac_id int(10) unsigned, p_definition_id int(10) unsigned, p_order_num int(10) unsigned, 
OUT r_ac_entry_id int(10) unsigned)
BEGIN 
	
    
		INSERT INTO amp_core.ari_collection_entry(ac_id, order_num) VALUES(p_ac_id, p_order_num);
		SET r_ac_entry_id = LAST_INSERT_ID();
		INSERT INTO amp_core.ari_collection_actual_entry(ac_entry_id, obj_actual_definition_id) VALUES(r_ac_entry_id, p_definition_id); 

   
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_ac_formal_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_ac_formal_entry`(IN p_ac_id int(10) unsigned, p_definition_id int(10) unsigned,  p_order_num int(10) unsigned, OUT r_ac_entry_id int(10) unsigned)
BEGIN 
	
		INSERT INTO amp_core.ari_collection_entry(ac_id, order_num) VALUES(p_ac_id, p_order_num);
		SET r_ac_entry_id = LAST_INSERT_ID();
		INSERT INTO amp_core.ari_collection_formal_entry(ac_entry_id, obj_formal_definition_id) VALUES(r_ac_entry_id, p_definition_id); 
	
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_ac_id` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_ac_id`(IN p_num_entries int(10) unsigned, p_use_desc varchar,  OUT r_ac_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.ari_collection(num_entries, use_desc) VALUES(p_num_entries, p_use_desc); 
    SET r_ac_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_adm_defined_namespace` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_adm_defined_namespace`(IN p_issuing_org varchar, p_namespace_string varchar, 
p_version varchar, p_adm_name_string varchar, p_adm_enum int(10) unsigned, p_adm_enum_label varchar, 
p_use_desc varchar, OUT r_namespace_id int(10) unsigned)
BEGIN
	CALL SP__insert_namespace('MODERATED', p_issuing_org, p_namespace_string, p_version, r_namespace_id);
    INSERT INTO amp_core.adm(namespace_id, adm_name, adm_enum, adm_enum_label, use_desc) VALUES(r_namespace_id, 
    p_adm_name_string, p_adm_enum, p_adm_enum_label, p_use_desc); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_agent` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_agent`(IN p_agent_id_string varchar, OUT r_registered_agents_id INT(10) unsigned)
BEGIN 
	SET @cur_time = NOW(); 
    SET @lower_name = LOWER(p_agent_id_string);
	IF EXISTS (SELECT * FROM registered_agents WHERE @lower_name = agent_id_string) THEN 
	BEGIN 
		UPDATE registered_agents
		SET
		last_registered = @cur_time
		WHERE agent_id_string = @lower_name;
    END;
    ELSE BEGIN
    INSERT INTO registered_agents (agent_id_string,first_registered, last_registered)
		VALUES (@lower_name, @cur_time, @cur_time);
		SET r_registered_agents_id = LAST_INSERT_ID();
    END;
    END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_const_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_const_actual_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_data_type varchar, p_data_value_string varchar, OUT r_actual_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_actual_definition_id); 
    Set @data_id  = (SELECT data_type_id FROM amp_core.data_type WHERE type_name  = p_data_type);
    INSERT INTO amp_core.const_actual_definition(obj_actual_definition_id, data_type_id, data_value) VALUES(r_actual_definition_id, @data_id, p_data_value_string); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_control_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_control_actual_definition`(IN p_obj_definition_id int(10) unsigned, p_ap_spec_id int(10) unsigned, p_use_desc varchar, OUT r_instance_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_instance_id); 
    INSERT INTO amp_core.control_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_instance_id, p_ap_spec_id);  
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_control_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_control_formal_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_fp_spec_id int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.control_formal_definition(obj_formal_definition_id, fp_spec_id) VALUES(r_definition_id, p_fp_spec_id); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_edd_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_edd_actual_definition`(IN p_obj_definition_id int(10) unsigned, p_use_desc varchar, p_ap_spec_id int(10) unsigned,  OUT r_actual_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_definition_id); 
    INSERT INTO amp_core.edd_actual_definition(obj_actual_definition_id, ap_spec_id) VALUES(r_actual_definition_id, p_ap_spec_id);  
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_edd_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_edd_formal_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_fp_spec_id int(10) unsigned, p_external_data_type varchar, OUT r_formal_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_formal_definition_id); 
    INSERT INTO amp_core.edd_formal_definition(obj_formal_definition_id, fp_spec_id, data_type_id) VALUES(r_formal_definition_id, p_fp_spec_id, (SELECT data_type_id FROM amp_core.data_type WHERE type_name  = p_external_data_type)); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_expression` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_expression`(IN p_out_type INT(10) UNSIGNED,   
p_postfix_operations INT(10) UNSIGNED, OUT r_expr_id INT(10) UNSIGNED)
BEGIN 
	
	
	
			INSERT INTO amp_core.expression(data_type_id, ac_id) VALUES(p_out_type, p_postfix_operations); 
			SET r_expr_id = LAST_INSERT_ID();
	
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_formal_parmspec` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_formal_parmspec`(IN p_num_parms int(10) unsigned, p_use_desc varchar,  OUT r_fp_spec_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.formal_parmspec(num_parms, use_desc) VALUES(p_num_parms, p_use_desc); 
    SET r_fp_spec_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_formal_parmspec_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_formal_parmspec_entry`(IN p_fp_spec_id int(10) unsigned,  p_order_num int(10) unsigned, p_parm_name varchar, p_data_type varchar, p_obj_definition_id int(10) unsigned, OUT r_fp_id int(10) unsigned)
BEGIN
	INSERT INTO formal_parm
(
fp_spec_id,
order_num,
parm_name,
data_type_id,
obj_actual_definition_id)
VALUES
(p_fp_spec_id,
p_order_num,
p_parm_name,
(select data_type_id from data_type where type_name = p_data_type),
p_obj_definition_id);

SET r_fp_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_formal_parms_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_formal_parms_set`(IN p_num_parms int(10) unsigned, p_use_desc varchar, p_data_types_list varchar(10000), p_parm_names_list varchar(10000),
 p_default_values_list varchar(10000), OUT r_fp_spec_id int(10) unsigned)
BEGIN 
	CALL SP__insert_formal_parmspec(p_num_parms, p_use_desc, r_fp_spec_id); 
    SET @fp_spec_id = r_fp_spec_id; 
    SET @s = 'INSERT INTO amp_core.formal_parm(fp_spec_id, order_num, parm_name, data_type_id, obj_actual_definition_id) VALUES'; 
    SET @loops = 1; 
    WHILE @loops < p_num_parms DO 
		BEGIN
			
				SET @data_type = TRIM(SUBSTRING_INDEX(p_data_types_list, ',', 1));
				SET p_data_types_list = REPLACE(p_data_types_list, CONCAT(@data_type, ','), ''); 
    
 			
				SET @parm_name = TRIM(SUBSTRING_INDEX(p_parm_names_list, ',', 1)); 
				SET p_parm_names_list = REPLACE(p_parm_names_list, CONCAT(@parm_name, ','), '');
                
            
				SET @default_value = TRIM(SUBSTRING_INDEX(p_default_values_list, ',', 1));
                IF @default_value = 'NULL' THEN SET @default_value = null;
                ELSEIF @default_value = 'null' THEN SET @default_value = null;
				END IF;
                SET p_default_values_list = REPLACE(p_default_values_list, CONCAT(@default_value, ','), '');
            
				SET @s = CONCAT(@s, '(', @fp_spec_id, ',', @loops, ',', '"', @parm_name, '"', ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @default_value, '"', '),');
                SET @loops = @loops + 1; 
        END; 
    END WHILE; 
 
    
	SET @data_type = TRIM((SUBSTRING_INDEX(p_data_types_list, ',', 1)));
    
	
	SET @parm_name = TRIM(SUBSTRING_INDEX(p_parm_names_list, ',', 1)); 
                
	
	IF @default_value = 'NULL' THEN SET @default_value = null;
                ELSEIF @default_value = 'null' THEN SET @default_value = null;
				END IF;
                SET p_default_values_list = REPLACE(p_default_values_list, CONCAT(@default_value, ','), '');

	SET @s = CONCAT(@s, '(', @fp_spec_id, ',', @loops, ',', (SELECT data_type_id FROM amp_core.data_type WHERE type_name = @data_type), ',', '"', @parm_name, '"', ',', '"', @default_value, '"', ')');
	PREPARE stmt FROM @s; 
    EXECUTE stmt; 

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_incoming_message_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_incoming_message_entry`(IN p_set_id int(10) unsigned, p_message_order int(10) unsigned, p_start_ts datetime, p_ac_id int(10) unsigned, OUT r_message_id int(10) unsigned)
BEGIN
INSERT INTO amp_core.incoming_message_entry (set_id, message_order, start_ts, ac_id)
VALUES (p_set_id, p_message_order, p_start_ts, p_ac_id);
SET r_message_id = last_insert_id();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_incoming_message_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_incoming_message_set`(IN p_created_ts datetime,
p_modified_ts datetime, p_state int(10) unsigned, p_agent_id int(10) unsigned, OUT r_set_id int(10) unsigned)
BEGIN
INSERT INTO amp_core.incoming_message_set (created_ts, modified_ts, state, agent_id)
VALUES (p_created_ts, p_modified_ts, p_state, p_agent_id);

SET r_set_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_literal_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_literal_actual_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_data_type varchar, p_data_value_string varchar, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.literal_actual_definition(obj_actual_definition_id, data_type_id, data_value) VALUES(r_definition_id, (SELECT data_type_id FROM amp_core.data_type WHERE type_name  = p_data_type), p_data_value_string); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_macro_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_macro_actual_definition`(IN p_obj_definition_id int(10) unsigned, p_ap_spec_id int(10) unsigned, p_actual_ac int(10) unsigned, p_use_desc varchar, OUT r_actual_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_actual_id); 
    INSERT INTO amp_core.macro_actual(obj_actual_definition_id, ap_spec_id, ac_id) VALUES(r_actual_id, p_ap_spec_id, p_actual_ac);  
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_macro_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_macro_formal_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_fp_spec_id int(10) unsigned, p_max_call_depth int(10) unsigned, p_definition_ac int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.macro_formal_definition(obj_formal_definition_id, fp_spec_id, ac_id, max_call_depth) VALUES(r_definition_id, p_fp_spec_id, p_definition_ac, p_max_call_depth); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_namespace` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_namespace`(IN p_namespace_type varchar, p_issuing_org varchar, p_name_string 
varchar, p_version varchar, OUT r_namespace_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.namespace(namespace_type, issuing_org, name_string, version_name) VALUES(p_namespace_type, p_issuing_org, p_name_string, p_version); 
    SET r_namespace_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_network_defined_namespace` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_network_defined_namespace`(IN p_issuing_org varchar, p_name_string varchar, 
p_version varchar, p_issuer_binary_string varchar, p_tag varchar, OUT r_namespace_id int(10) unsigned)
BEGIN
	CALL SP__insert_namespace('NETWORK_CONFIG', p_issuing_org, p_name_string, p_version, r_namespace_id); 
    INSERT INTO amp_core.network_config(namespace_id, issuer_binary_string, tag) VALUES(r_namespace_id, p_issuer_binary_string, p_tag); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_obj_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_obj_actual_definition`(IN p_obj_metadata_id int(10) unsigned, p_use_desc varchar, OUT r_actual_id int(10) unsigned)
BEGIN
	INSERT IGNORE INTO amp_core.obj_actual_definition(obj_metadata_id, use_desc) VALUES(p_obj_metadata_id, p_use_desc); 
    SET r_actual_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_obj_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_obj_formal_definition`(IN p_obj_metadata_id int(10) unsigned, p_use_desc varchar, OUT r_formal_id int(10) unsigned)
BEGIN
	INSERT IGNORE INTO amp_core.obj_formal_definition(obj_metadata_id, use_desc) VALUES(p_obj_metadata_id, p_use_desc); 
    SET r_formal_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_obj_metadata` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_obj_metadata`(IN p_obj_type_id INT(10) unsigned, p_obj_name varchar, p_namespace_id int(10) unsigned, OUT r_obj_id int(10) unsigned)
BEGIN
	INSERT IGNORE INTO amp_core.obj_metadata(data_type_id, obj_name, namespace_id) VALUES(p_obj_type_id, p_obj_name, p_namespace_id); 
    SET r_obj_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_operator_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_operator_actual_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, 
p_result_type varchar, p_num_inputs int(10) unsigned, p_tnvc_id int(10) unsigned,  OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
    INSERT INTO amp_core.operator_actual_definition(obj_actual_definition_id, data_type_id, num_operands, tnvc_id)
    VALUES(r_definition_id, (SELECT data_type_id FROM amp_core.data_type WHERE type_name = p_result_type), p_num_inputs, p_tnvc_id); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_outgoing_message_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_outgoing_message_entry`(IN p_set_id int(10) unsigned, p_message_order int(10) unsigned, p_start_ts datetime, p_ac_id int(10) unsigned, OUT r_message_id int(10) unsigned)
BEGIN
INSERT INTO amp_core.outgoing_message_entry (set_id, message_order, start_ts, ac_id)
VALUES (p_set_id, p_message_order, p_start_ts, p_ac_id);
SET r_message_id = last_insert_id();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_outgoing_message_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_outgoing_message_set`(IN p_created_ts DATETIME,
p_modified_ts DATETIME, p_state int(10) unsigned, p_agent_id int(10) unsigned, OUT r_set_id int(10) unsigned )
BEGIN
INSERT INTO amp_core.outgoing_message_set (created_ts, modified_ts, state, agent_id)
VALUES (p_created_ts, p_modified_ts, p_state, p_agent_id);

SET r_set_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_report_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_report_actual_definition`(IN p_obj_definition_id int(10) unsigned, p_ap_spec_id int(10) unsigned, p_use_desc varchar,OUT r_obj_actual_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_definition_id, p_use_desc, r_obj_actual_id); 
    INSERT INTO amp_core.report_template_actual_definition(obj_actual_definition_id, ap_spec_id ) VALUES(r_obj_actual_id, p_ap_spec_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_report_template_formal_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_report_template_formal_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_formal_parmspec_id int(10) unsigned, p_ac_id int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_formal_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.report_template_formal_definition(obj_formal_definition_id, fp_spec_id, ac_id) VALUES(r_definition_id, p_formal_parmspec_id, p_ac_id); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_report_template_metadata_format` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_report_template_metadata_format`(IN p_metadata_count int(10) unsigned, p_metadata_types_list varchar, p_metadata_names_list varchar, p_metadata_desc varchar, OUT r_tnvc_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.type_name_value_collection(num_entries, use_desc) VALUES(p_metadata_count, p_metadata_desc); 
    SET r_tnvc_id = LAST_INSERT_ID();
	SET @s = 'INSERT INTO amp_core.type_name_value(tnvc_id, data_type, data_name, order_num) VALUES'; 
    SET @loops = 1; 
    WHILE @loops < p_metadata_count DO 
		BEGIN
			-- @metadata_type
				SET @metadata_type = TRIM(SUBSTRING_INDEX(p_metadata_types_list, ',', 1));
				SET p_metadata_types_list = REPLACE(p_metadata_types_list, CONCAT(@metadata_type, ','), ''); 
    
 			-- @metadata_name
				SET @metadata_name = TRIM(SUBSTRING_INDEX(p_metadata_names_list, ',', 1)); 
				SET p_metadata_names_list = REPLACE(p_metadata_names_list, CONCAT(@metadata_name, ','), '');
                
				SET @s = CONCAT(@s, '(', r_tnvc_id, ',', (SELECT enum_id FROM amp_core.data_type WHERE type_name = @metadata_type), ',', '\'', @metadata_name, '\'', ',', @loops, '),');
                SET @loops = @loops + 1; 
        END; 
    END WHILE; 
 
    -- @metadata_type
	SET @metadata_type = TRIM((SUBSTRING_INDEX(p_metadata_types_list, ',', 1)));
    
	-- @metadata_name
	SET @metadata_name = TRIM(SUBSTRING_INDEX(p_metadata_names_list, ',', 1)); 

	SET @s = CONCAT(@s, '(', r_tnvc_id, ',', (SELECT enum_id FROM amp_core.data_type WHERE type_name = @metadata_type), ',', '\'', @metadata_name, '\'', ',', @loops, ');');
	PREPARE stmt FROM @s; 
    EXECUTE stmt; 
	
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_sbr_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_sbr_actual_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar,  p_expr_id int(10) UNSIGNED, p_ac_id int(10) UNSIGNED, p_start_time time , OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.sbr_actual_definition(obj_actual_definition_id, expression_id, run_count, start_time, ac_id) VALUES(r_definition_id, p_expr_id, p_run_count, p_start_time, p_ac_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_table_template_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_table_template_actual_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, p_columns_id int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
    INSERT INTO amp_core.table_template_actual_definition(obj_actual_definition_id, tnvc_id) VALUES(r_definition_id, p_columns_id); 
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tbr_actual_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tbr_actual_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, 
p_wait_per time, p_run_count bigint unsigned, p_start_time time, p_ac_id int(10) UNSIGNED, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id); 
    INSERT INTO amp_core.tbr_actual_definition(obj_actual_definition_id, wait_period, run_count, start_time, ac_id) VALUES(r_definition_id, p_wait_per, p_run_count, p_start_time, p_ac_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_ari_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_ari_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value int(10) unsigned , OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'ari', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_obj_entry (tnv_id, obj_actual_definition_id) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_bool_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_bool_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value bool, OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'bool', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_bool_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_byte_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_byte_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value tinyint, OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'byte', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_byte_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_collection` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_collection`(IN p_use_desc varchar, OUT r_tnvc_id int(10) unsigned)
BEGIN
	INSERT INTO amp_core.type_name_value_collection(use_desc) VALUES(p_use_desc);
    SET r_tnvc_id = LAST_INSERT_ID();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned, p_data_type_name varchar, p_data_name varchar, OUT r_tnvc_entry_id int(10) unsigned)
BEGIN
	INSERT INTO `amp_core`.`type_name_value_entry`
(`tnvc_id`,
`order_num`,
`data_type_id`,
`data_name`)
VALUES
(p_tnvc_id,
p_order_num,
(SELECT data_type_id FROM data_type WHERE type_name = UPPER(p_data_type_name)),
p_data_name);

SET r_tnvc_entry_id = LAST_INSERT_ID();

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_int_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_int_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value int(10), OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'int', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_int_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_obj_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_obj_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned, p_data_type_name varchar, p_data_name varchar,  p_entry_value int(10) unsigned , OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'obj', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_obj_entry (tnv_id, obj_actual_definitionrid) VALUES (p_tnv_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_real32_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_real32_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value float, OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'real32', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_real32_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_real64_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_real64_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value double, OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'real64', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_real64_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_string_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_string_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value varchar, OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'string', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_string_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_str_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_str_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value varchar, OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'STR', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_string_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_uint_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_uint_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value int(10) unsigned , OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'uint', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_uint_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_unk_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_unk_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'unk', p_data_name, r_tnvc_entry_id);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_uvast_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_uvast_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value bigint unsigned , OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'uvast', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_uvast_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_tnvc_vast_entry` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_tnvc_vast_entry`(IN p_tnvc_id int(10) unsigned, p_order_num int(10) unsigned,  p_data_name varchar,  p_entry_value bigint , OUT r_tnvc_entry_id int(10) unsigned )
BEGIN
    CALL SP__insert_tnvc_entry(p_tnvc_id, p_order_num, 'vast', p_data_name, r_tnvc_entry_id);
    INSERT INTO type_name_value_vast_entry (tnv_id, entry_value) VALUES (r_tnvc_entry_id, p_entry_value);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__insert_variable_definition` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__insert_variable_definition`(IN p_obj_id int(10) unsigned, p_use_desc varchar, 
p_out_type int(10) unsigned,  p_expression_id int(10) unsigned, OUT r_definition_id int(10) unsigned)
BEGIN
	CALL SP__insert_obj_actual_definition(p_obj_id, p_use_desc, r_definition_id);
	
    CALL SP__insert_expression(p_out_type, p_expression_id, @r_expr_id);
	INSERT INTO amp_core.variable_actual_definition(obj_actual_definition_id, data_type_id, expression_id) VALUES(r_definition_id, p_out_type, @r_expr_id); 

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__update_incoming_message_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__update_incoming_message_set`(IN p_set_id int(10) unsigned, p_state int(10) unsigned)
BEGIN
	
	UPDATE amp_core.incoming_message_set
	SET
	modified_ts = NOW(),
	state = p_state
	WHERE set_id = p_set_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `SP__update_outgoing_message_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `SP__update_outgoing_message_set`(IN p_set_id int(10) unsigned, p_state int(10) unsigned)
BEGIN
	
	UPDATE amp_core.outgoing_message_set
	SET
	modified_ts = NOW(),
	state = p_state
	WHERE set_id = p_set_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Current Database: `amp_core`
--

USE `amp_core`;

--
-- Final view structure for view `vw_ctrl_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_ctrl_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_ctrl_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`join2`.`ap_spec_id` AS `ap_spec_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`view1`.`use_desc` AS `use_desc`,`view1`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`view1`.`ap_spec_id` AS `ap_spec_id` from (`obj_formal_definition` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`control_actual_definition`.`ap_spec_id` AS `ap_spec_id` from (`obj_actual_definition` join `control_actual_definition` on((`obj_actual_definition`.`obj_actual_definition_id` = `control_actual_definition`.`obj_actual_definition_id`)))) `view1` on((`view1`.`obj_metadata_id` = `obj_formal_definition`.`obj_metadata_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_ctrl_definition`
--

/*!50001 DROP VIEW IF EXISTS `vw_ctrl_definition`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_ctrl_definition` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_formal_definition`.`use_desc` AS `use_desc`,`control_formal_definition`.`fp_spec_id` AS `fp_spec_id` from (`obj_formal_definition` join `control_formal_definition` on((`control_formal_definition`.`obj_formal_definition_id` = `obj_formal_definition`.`obj_formal_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_edd_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_edd_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_edd_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`join2`.`ap_spec_id` AS `ap_spec_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`view1`.`use_desc` AS `use_desc`,`view1`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`view1`.`ap_spec_id` AS `ap_spec_id` from (`obj_formal_definition` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`edd_actual_definition`.`ap_spec_id` AS `ap_spec_id` from (`obj_actual_definition` join `edd_actual_definition` on((`obj_actual_definition`.`obj_actual_definition_id` = `edd_actual_definition`.`obj_actual_definition_id`)))) `view1` on((`view1`.`obj_metadata_id` = `obj_formal_definition`.`obj_metadata_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_edd_formal`
--

/*!50001 DROP VIEW IF EXISTS `vw_edd_formal`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_edd_formal` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_formal_definition`.`use_desc` AS `use_desc`,`edd_formal_definition`.`fp_spec_id` AS `fp_spec_id` from (`obj_formal_definition` join `edd_formal_definition` on((`edd_formal_definition`.`obj_formal_definition_id` = `obj_formal_definition`.`obj_formal_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_mac_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_mac_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_mac_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`join2`.`ap_spec_id` AS `ap_spec_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`view1`.`use_desc` AS `use_desc`,`view1`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`view1`.`ap_spec_id` AS `ap_spec_id` from (`obj_formal_definition` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`macro_actual_definition`.`ap_spec_id` AS `ap_spec_id` from (`obj_actual_definition` join `macro_actual_definition` on((`obj_actual_definition`.`obj_actual_definition_id` = `macro_actual_definition`.`obj_actual_definition_id`)))) `view1` on((`view1`.`obj_metadata_id` = `obj_formal_definition`.`obj_metadata_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_mac_definition`
--

/*!50001 DROP VIEW IF EXISTS `vw_mac_definition`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_mac_definition` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`max_call_depth` AS `max_call_depth`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_formal_definition`.`use_desc` AS `use_desc`,`macro_formal_definition`.`fp_spec_id` AS `fp_spec_id`,`macro_formal_definition`.`max_call_depth` AS `max_call_depth` from (`obj_formal_definition` join `macro_formal_definition` on((`macro_formal_definition`.`obj_formal_definition_id` = `obj_formal_definition`.`obj_formal_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_oper_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_oper_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_oper_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_metadata`.`data_type_id` AS `data_type_id`,`join2`.`num_operands` AS `num_operands`,`join2`.`tnvc_id` AS `tnvc_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`operator_actual_definition`.`data_type_id` AS `data_type_id`,`operator_actual_definition`.`num_operands` AS `num_operands`,`operator_actual_definition`.`tnvc_id` AS `tnvc_id` from (`obj_actual_definition` join `operator_actual_definition` on((`operator_actual_definition`.`obj_actual_definition_id` = `obj_actual_definition`.`obj_actual_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_rpt_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_rpt_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_rpt_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`join2`.`ap_spec_id` AS `ap_spec_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`view1`.`use_desc` AS `use_desc`,`view1`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`view1`.`ap_spec_id` AS `ap_spec_id` from (`obj_formal_definition` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`report_template_actual_definition`.`ap_spec_id` AS `ap_spec_id` from (`obj_actual_definition` join `report_template_actual_definition` on((`obj_actual_definition`.`obj_actual_definition_id` = `report_template_actual_definition`.`obj_actual_definition_id`)))) `view1` on((`view1`.`obj_metadata_id` = `obj_formal_definition`.`obj_metadata_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_rptt_formal`
--

/*!50001 DROP VIEW IF EXISTS `vw_rptt_formal`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_rptt_formal` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_formal_definition`.`obj_formal_definition_id` AS `obj_formal_definition_id`,`obj_formal_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_formal_definition`.`use_desc` AS `use_desc`,`report_template_formal_definition`.`fp_spec_id` AS `fp_spec_id` from (`obj_formal_definition` join `report_template_formal_definition` on((`report_template_formal_definition`.`obj_formal_definition_id` = `obj_formal_definition`.`obj_formal_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_sbr_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_sbr_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_sbr_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`join2`.`expression_id` AS `expression_id`,`join2`.`run_count` AS `run_count`,`join2`.`start_time` AS `start_time`,`join2`.`ac_id` AS `ac_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`sbr_actual_definition`.`expression_id` AS `expression_id`,`sbr_actual_definition`.`run_count` AS `run_count`,`sbr_actual_definition`.`start_time` AS `start_time`,`sbr_actual_definition`.`ac_id` AS `ac_id` from (`obj_actual_definition` join `sbr_actual_definition` on((`sbr_actual_definition`.`obj_actual_definition_id` = `obj_actual_definition`.`obj_actual_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_tblt_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_tblt_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_tblt_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`view1`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`view1`.`tnvc_id` AS `tnvc_id`,`view1`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`table_template_actual_definition`.`tnvc_id` AS `tnvc_id`,`obj_actual_definition`.`use_desc` AS `use_desc` from (`obj_actual_definition` join `table_template_actual_definition` on((`obj_actual_definition`.`obj_actual_definition_id` = `table_template_actual_definition`.`obj_actual_definition_id`)))) `view1` on((`view1`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_tbr_actual`
--

/*!50001 DROP VIEW IF EXISTS `vw_tbr_actual`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_tbr_actual` AS select `obj_metadata`.`obj_metadata_id` AS `obj_metadata_id`,`obj_metadata`.`obj_name` AS `obj_name`,`obj_metadata`.`namespace_id` AS `namespace_id`,`join2`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`join2`.`run_count` AS `run_count`,`join2`.`start_time` AS `start_time`,`join2`.`ac_id` AS `ac_id`,`join2`.`use_desc` AS `use_desc` from (`obj_metadata` join (select `obj_actual_definition`.`obj_actual_definition_id` AS `obj_actual_definition_id`,`obj_actual_definition`.`obj_metadata_id` AS `obj_metadata_id`,`obj_actual_definition`.`use_desc` AS `use_desc`,`tbr_actual_definition`.`run_count` AS `run_count`,`tbr_actual_definition`.`start_time` AS `start_time`,`tbr_actual_definition`.`ac_id` AS `ac_id` from (`obj_actual_definition` join `tbr_actual_definition` on((`tbr_actual_definition`.`obj_actual_definition_id` = `obj_actual_definition`.`obj_actual_definition_id`)))) `join2` on((`join2`.`obj_metadata_id` = `obj_metadata`.`obj_metadata_id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `vw_tnvc`
--

/*!50001 DROP VIEW IF EXISTS `vw_tnvc`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_tnvc` AS select `type_name_value_entry`.`tnvc_id` AS `tnvc_id`,`type_name_value_entry`.`tnv_id` AS `tnv_id`,`type_name_value_entry`.`order_num` AS `order_num`,`type_name_value_entry`.`data_type_id` AS `data_type_id`,`type_name_value_entry`.`data_name` AS `data_name`,`type_name_value_collection`.`use_desc` AS `use_desc` from (`type_name_value_entry` join `type_name_value_collection`) where (`type_name_value_collection`.`tnvc_id` = `type_name_value_entry`.`tnvc_id`) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2020-08-20 19:35:31
