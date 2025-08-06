@page refda Reference DTNMA Agent
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

The Reference DTNMA Agent (REFDA) is implemented as two portions: a reusable library (`refda`) and a set of daemons which bind the REFDA to a specific transport.

# REFDA Library

This library is meant to implement all of the Agent behaviors defined in Section 3 and Section 6 of the AMM @cite ietf-dtn-amm-05 in a way which is independent of the exact context in which the Agent is being deployed and independent of the specific transport used for ARI/AMP messaging.

# Application Interface

The application-side interface of the REFDA is provided by C11 functions related to registering application model objects in a ::cace_amm_obj_store_t instance under a specific model namespace ::cace_amm_obj_ns_t instance.

The Python package CAMP is specifically intended to automate the use of this interface by automated conversion of ADM modules (input file) into a REFDA-compatible registration implementation (C compilation unit).

# Transport Interface

The transport-side interface to and from a REFDA deployment is defined by a ::cace_amm_msg_if_t instance with callback functions specific to the transport mechanism being used by the deployment.

# Factory-Supplied Transport Bindings

This project implements off-the-shelf transport bindings for the REFDA described in the following subsections.

All of the bindings use the following command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -h         | Show help                | Show command help message and exit early
 | -l \<level\> | Filter Logging Level     | If set, logging will be enabled on startup. Else it must be set in the UI

## Unix Domain Datagram Sockets {#refda-socket}

This binding uses unix datagram sockets (`AF_UNIX` with `SOCK_DGRAM`) to bind to a specific socket file and communicate with Manager(s) on their own similar socket files.
The daemon executable is named `refda-socket`.

The daemon for this binding has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -a \<path\>  | Bind path                | Bind to and listen on this socket file
 | -m \<path\>  | Hello path               | Send an initial Hello report a Manager bound to this socket file

## ION BPv7 API {#refda-ion}

This binding uses the C API from JPL ION to register a single endpoint in a local BPv7 Agent and communicate with Manager(s) on their own BPv7 endpoints.
The daemon executable is named `refda-ion`.

The daemon for this binding has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -a \<EID\>   | Register endpoint        | Register on and listen on this BPv7 EID
 | -m \<EID\>   | Hello endpoint           | Send an initial Hello report a Manager registered on this BPv7 EID


## Process Standard Input/Output {#refda-stdio}

This binding uses the process standard input (`stdin`) and standard output (`stdout`) to mock communications with a single Manager.
The daemon executable is named `refda-stdio`.

@warning This binding does not implement AMP! All of its messaging is in text form ARIs.

The daemon for this binding has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -a \<URI\>   | Register endpoint        | Behave as if the stdin/stdout transport had an identifier
