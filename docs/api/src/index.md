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
Introduction                             {#mainpage}
========

The [nm_mgr](@ref nmmgr) is the manager application providing a
[user interface](@ref nmui) to manage registered [agents](@ref nmagent).



# References

NM implements or utilizes the following:

- [Asynchronous Management Protocol](https://datatracker.ietf.org/doc/draft-birrane-dtn-amp) (AMP)
- [AMA Application Data Model](https://tools.ietf.org/html/draft-birrane-dtn-adm-03)

## Third-Party

The following third party libraries are included as part of NM.

- [QCBOR](https://github.com/laurencelundblade/QCBOR) - A small CBOR encoder/decoder. 
    -  Provided under a BSD-compatible license.
    -  Bundled distribution includes enhancements made to support the AMP OCTETS model.
