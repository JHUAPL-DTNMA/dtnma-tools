@mainpage Introduction
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

This documentation is for the DTNMA reference tools application programming interface (API) in the C language and technical discussion of its command line tools and daemons.
This is an implementation of DTNMA @cite rfc9675, its Application Management Model (AMM) @cite ietf-dtn-amm-05 objects, values, and typing system, Application Resource Identifier (ARI) encoding @cite ietf-dtn-ari-06, and Asynchronous Management Protocol (AMP) daemons @cite ietf-dtn-amp-02.

The breakdown of this project source and built artifacts are shown below.

@dot "DTNMA Tools Artifacts" width=500pt
digraph project {
    rankdir=LR;
    node [shape=record, fontname=Helvetica, fontsize=12];

    cace [ label="cace Library" ];
    refda [ label="refda Library" ];
    refdm [ label="refdm Library" ];
    
    subgraph execs {
      rank="same";
      cace_ari [ label="cace_ari tool" ];

      refda_stdio [ label="refda_stdio test tool" ];
      refda_socket [ label="refda_socket daemon" ];
      refda_ion [ label="refda_ion daemon" ];

      refdm_socket [ label="refdm_socket daemon" ];
      refdm_proxy [ label="refdm_proxy daemon" ];
      refdm_ion [ label="refdm_ion daemon" ];
    }

    cace_ari -> cace;
    refda -> cace;
    refda_stdio -> refda;
    refda_socket -> refda;
    refda_ion -> refda;

    refdm -> cace;
    refdm_socket -> refdm;
    refdm_ion -> refdm;
    refdm_proxy -> refdm;
}
@enddot


# Command-Line Tools and Daemons

This section gives an overview of the executables built with a default project configuration.
Some of these are command-line tools meant to be used either interactively or for batch processing (_e.g._ the `cace_ari` tool) and some are usable as headless daemons. 

## ARI Processing Library and Tool

The processing of encoded ARI values and the associated AMM typing system are built as the stand-alone `cace` library, which is used by all of the other portions of this project.

The tool `cace_ari` is for checking encoded ARI values (both URI text form and CBOR binary form) and for converting between the two forms.

More details are described in the @ref cace page.

## Reference Agent Library and Bindings

The reference DTNMA Agent (REFDA) (with its default ADM implementations) is built as the `refda` library and has default daemons which bind it to the following transports:

 * The `refda-socket` which is a simple binding to @ref refda-socket used to transport AMP messages.
   This DA is intended to be usable for testing and interoperation with the `refdm-socket` daemon without needing complex transport parameters.
 * The `refda-ion` which is a binding to the @ref refda-ion, and is a drop-in replacement to the earlier ION-integrated DTNMA Agent.
 * The `refda-stdio` which is a binding to @ref refda-stdio and is intended only as a test DA because it uses the process `stdin` and `stdout` for text-form ARI transport.
   This means that the exchange is not AMP messages and should not be used for interoperability testing.

More details are described in the @ref refda page.

## Reference Manager Library and Bindings

The reference DTNMA Manager (REFDM) is built as the `refdm` library and has default daemons which bind it to the following transports:

 * The `refdm-socket` which is a simple binding to @ref refdm-socket to transport AMP messages.
   This DA is intended to be usable for testing and interoperation with the `refdm-socket` daemon without needing complex transport parameters.
 * The `refdm-ion` which is a binding to the @ref refdm-ion, and is a drop-in replacement to the earlier ION-integrated DTNMA Manager.
 * The `refdm-proxy` which is a binding to @ref refdm-proxy connecting to an associated local AMP proxy daemon.
   The project contains an simple `ion-app-proxy` daemon to fulfill that purpose with limited configuration.

Each of the refdm daemons provides a REST API for user/application interaction.

More details are described in the @ref refdm page.

# Getting Started with the APIs

Details on conventions used by the APIs and expected of new development is defined in the @ref conventions page.

Depening on what you are trying to implement or integrate with, either the @ref cace, @ref refda, or @ref refdm are a starting point for specific top-level APIs.

# Build and Runtime Dependencies

The following third party libraries are needed at **build time** and **runtime**:

[QCBOR](https://github.com/laurencelundblade/QCBOR)
: A small CBOR encoder/decoder.

[M*LIB](https://github.com/P-p-H-d/mlib)
: A C11-compatible container library which is type-safe.

[timespec](https://github.com/solemnwarning/timespec)
: A set of supplemental `struct timespec` processing functions.

[Civetweb](https://github.com/civetweb/civetweb)
: A minimal HTTP server library supporting the REFDM application interface (see @ref refdm).

[cJSON](https://github.com/DaveGamble/cJSON)
: A JSON CODEC library supporting the REFDM application interface (see @ref refdm).

[JPL ION](https://github.com/nasa-jpl/ION-DTN):
: A BPv7 Agent implementation with a C-language API for endpoint binding used as REFDA and REFDM transports.

The following are useful for **development** of new application models:

[dtnma-camp](https://github.com/JHUAPL-DTNMA/dtnma-camp)
: A Python library for automating the registration of ADM implementation.
Also available directly from PIP with the package [`dtnma-camp`](https://pypi.org/project/dtnma-camp/).

The following are used for **testing**:

[Unity](https://github.com/ThrowTheSwitch/Unity)
: A C unit test fixture and assertion library.

[dtnma-ace](https://github.com/JHUAPL-DTNMA/dtnma-ace)
: A Python library for processing ARIs and ADMs for built-item testing.
Also available directly from PIP with the package [`dtnma-ace`](https://pypi.org/project/dtnma-ace/).
