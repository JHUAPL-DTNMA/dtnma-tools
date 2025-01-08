<!--
Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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
This directory contains documentation resources for NM.

## Directories

### adms
This directory contains JSON files describing the currently implemented ADMs.  These files were used as input to automatically generate the associated ADM source files included in this distribution using the CAMP tool (available in the ../contrib directory).

The index.json file provides the full name and IETF references for each file.  The order of entires in the namespaces array matches the defined namespace identifier indexes.

### pod*
This file contains Perl POD formatted documentation used by ION to generate man page entries as a part of the standard build.

### doxygen
This directory contains Doxygen source and configuration files.  Doxygen can be used to generate detailed HTML documentation and API references.  See the README.md in this directory for instructions on running Doxygen.
