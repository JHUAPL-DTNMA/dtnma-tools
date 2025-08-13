@page cace CACE Library
<!--
Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

This library is subdivided into the following subsystems, each with a corresponding directory in the source tree and include tree.

# Common Utilities

The `cace/util` directory contains common utility preprocessor defines, functions, etc. used by other areas of this project.
The specific header @ref cace/util/logging.h deals with @ref logging which is used by all libraries and executables of the project.

# ARI Handling

The `cace/ari` directory deals with the ARI data model and encoding / decoding (CODEC) functions related to processing ARI values.
There is also a top-level include @ref cace/ari.h which includes all other headers dealing with ARI handling.

The type ::cace_ari_t is the API focal point for this library.
There are related algorithmic types and functions like cace_ari_visit() and cace_ari_translate() defined here.

# AMM Definitions

The `cace/amm` directory deals with aspects of the AMM such as the abstract object structure and value typing subsystem.
Types within the AMM extend beyond the built-in types of ARI values into "semantic types" as defined in Section 3.3 of the AMM @cite ietf-dtn-amm-05.

The type ::cace_amm_obj_store_t is used to register and lookup abstract AMM object handles, which used within the @ref refda for state keeping.
There are related algorithmic types and functions like cace_amm_lookup_deref() for object dereferencing and cace_amm_actual_param_set_populate() for parameter handling.

The type [cace_amm_type_t](@ref cace_amm_type_s) is used to represent both built-in ARI types as well as different forms of semantic typing built upon the built-in types.
There are related algorithmic functions like cace_amm_type_match() and cace_amm_type_convert defined here.

# AMP Message Handling

The `cace/amp` directory deals with ARI values encoded in binary form into AMP messages
This includes POSIX socket utilities used by the `refda-socket` and `refdm-socket` daemons as a reference to what an AMP transport binding should look like and how it should behave.
