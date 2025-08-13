@page refdm Reference DTNMA Manager
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

The Reference DTNMA Manager (REFDM) is implemented as two portions: a reusable library (`refdm`) and a set of daemons which bind the REFDM to a specific transport.

# REFDM Library

This library is meant to implement logical behaviors defined in Section 2.3 and Section 6.9 of the AMM @cite ietf-dtn-amm-05 in a way which is independent of the exact context in which the Manager is being deployed and independent of the specific transport used for ARI/AMP messaging.

# Application Interface

The application-side interface of the REFDM is provided by a RESTful API operating over HTTP 1.1, with the REFDM acting as the HTTP server.


The following APIs are available from the base URI of `http://localhost:8089/nm/api/` (_i.e._ the first endpoint `http://localhost:8089/nm/api/version`).

The URI template parameter `{/TYPE,ID}` below refers to a unique identifier for an Agent as one of either:

 * The `TYPE` of "eid" and the `ID` being a transport endpoint identifier (which itself is a URI value).
 * The `TYPE` of "idx" and the `ID` being ordinal index of the same agent (as a decimal integer).
   This index is dependent upon the order in which the agents are seen by a specific manager instance.

The template parameter `{?form}` below refers to a choice of encoded form for ARIs to be sent or retrieved, as one of either:
 * The form "text" meaning a newline-separated, URI-encoded form of ARIs consistent with the "application/uri-list" media type and Section 9.2 of ARI @cite ietf-dtn-ari-06.
 * The form "hex" meaning a newline-separated, base16-encoded, CBOR-encoded form of ARIs consistent with the "text/plain" media type and Section 9.2 of ARI @cite ietf-dtn-ari-06.

 | Method | Path                              | Description                                  |
 |--------|-----------------------------------|----------------------------------------------|
 | GET    | `/version`                        | Return version information as a JSON object. |
 | GET    | `/agents`                         | Get a listing of registered agents as a JSON object. |
 | POST   | `/agents`                         | Register a new Agent at specified EID. The EID is encoded as a URI in request body. |
 | GET    | `/agents/{/TYP,ID}`               | Retrieve node information, including index order, name, and number of reports available. |
 | PUT    | `/agents/{/TYP,ID}/clear_reports` | Clear all available reports for given Agent. |
 | PUT    | `/agents/{/TYP,ID}/send{?form}`   | Send one or more EXECSET to the specific Agent. The encoded form is in the request body. |
 | GET    | `/agents/{/TYP,ID}/reports{?form}`| Retrieve list of RPTSET for a specific Agent. The encoded form is in the response body. |

# Transport Interface

The transport-side interface to and from a REFDM deployment is defined by a ::cace_amm_msg_if_t instance with callback functions specific to the transport mechanism being used by the deployment.

# Factory-Supplied Transport Bindings

This project implements off-the-shelf transport bindings for the REFDM described in the following subsections.

All of the bindings use the following command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -h         | Show help                | Show command help message and exit early
 | -l \<level\> | Filter Logging Level     | If set, logging will be enabled on startup. Else it must be set in the UI

## Unix Domain Datagram Sockets {#refdm-socket}

This binding uses Unix datagram sockets (`AF_UNIX` with `SOCK_DGRAM`) to bind to a specific socket file and communicate with Agent(s) on their own similar socket files.
The daemon executable is named `refdm-socket`.

The daemon for this binding has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -a \<path\>  | Bind path                | Bind to and listen on this socket file

## ION BPv7 API {#refdm-ion}

This binding uses the C API from JPL ION to register a single endpoint in a local BPv7 Agent and communicate with Agent(s) on their own BPv7 endpoints.
The daemon executable is named `refdm-ion`.

The daemon for this binding has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -a \<EID\>   | Register endpoint        | Register on and listen on this BPv7 EID


## AMP Proxy Reliable Datagram Sockets {#refdm-proxy}

This binding uses the Unix reliable datagram sockets (`AF_UNIX` with `SOCK_SEQPACKET`) to connect to a local AMP proxy server and communicate in accordance with Section 4.2 of AMP @cite ietf-dtn-amp-02.
The daemon executable is named `refdm-proxy`.

The daemon will attempt to connect to the proxy server at start up, with linear back-off if the server is not responsive.
If an initial connection cannot be made after 600 attempts the daemon will exit with an error code.

The daemon for this binding has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -a \<path\>  | Proxy server path        | Connect to this proxy socket file

### Proxy Server

An example of a proxy server implementation which itself registers as a BPv7 endpoint with a local JPL ION Agent is provided for reference.
The daemon executable is named `ion-app-proxy`.

The daemon will register its endpoint in the local BP Agent at startup and bind to the listening socket file path.
If either of these fail the daemon will exit with an error code.

The server daemon has the following additional command-line options.

 | Option     | Name                     | Description
 |------------|--------------------------|------------
 | -b \<path\>  | Bind path                | Bind to and listen on this proxy socket file
 | -e \<EID\>   | Register endpoint        | Register on and listen on this BPv7 EID

# Persistent Database Support

To build with MySQL or PostgreSQL database support, simply configure the CMake project with the associated library development OS packages installed.
When detected, the use of database persistence will be enabled for the REFDM and the associated command options will be made available at runtime.

The runtime configuration of which Postgres server to connect to is controlled by the following environment variables.
The names of these variables is identical to and consistent with other command tools such as `psql`.

| Variable      | Description
|---------------|------------
| `DB_HOST`     | The DNS name or IP address to connect to. This defaults to a local Unix socket name.
| `DB_USER`     | The account name to login as. This defaults to the current shell user name.
| `DB_PASSWORD` | The password to login with. There is no default.
| `DB_NAME`     | The database schema name to access. There is no default.
