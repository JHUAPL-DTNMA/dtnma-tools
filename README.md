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
# DTNMA Tools

This software package contains tools related to the Delay-Tolerant Networking Management Architecture (DTNMA), specifically an implementation of the DTNMA Agent and Manager.

The current tools are based on the following specifications:

* [draft-ietf-dtn-amm-02](https://datatracker.ietf.org/doc/html/draft-ietf-dtn-amm) for behavioral models
* [draft-ietf-dtn-ari-03](https://datatracker.ietf.org/doc/html/draft-ietf-dtn-ari) for value encoding and typing
* [draft-ietf-dtn-adm-yang-02](https://datatracker.ietf.org/doc/html/draft-ietf-dtn-adm-yang) for ADM definitions
* [draft-ietf-dtn-amp-00](https://datatracker.ietf.org/doc/html/draft-ietf-dtn-amp) for messaging

:warning: These documents are not published standards and are subject to change over time as the standards are developed and finalized.

## Installation

This project uses the CMake and Ninja build systems and relies on a few third-party commercial, off-the-shelf (COTS) libraries to build and run properly.

To perform a basic build and installation, including for dependencies, the following packages must be installed. This list is generated for Ubuntu but similar ones exist for other major OSes.

```
cmake
ninja-build
ruby
build-essential
autoconf
libtool
flex 
libfl-dev
bison
libpcre2-dev
civetweb
libcivetweb-dev
libssl-dev
libcjson-dev
libsystemd-dev
gcovr
doxygen
valgrind
```

In addition to the above, this project also has dependencies provided as git submodules. These may be retrieved and built using the following commands:

```
git submodule init
git submodule update --recursive
./deps.sh
```

Once dependencies are installed, the following scripts can be run to build and test the project:
```
./deps.sh
./prep.sh
./build.sh
./build.sh docs
./build.sh check
```
After the build is successful, the following will install to a local prefix path (`$PWD/testroot/usr`):
```
./build.sh install
```

For further details, see the example in [testenv/Dockerfile](testenv/Dockerfile), which builds a stand-alone test environment container image, along with the other files in `testenv`.


## Usage

The example agent and manager both rely on ION for message transport, so a prerequisite for both is a running ION BPA instance.
An example of all of this, including the use of `systemd` scripts, is under the [testenv](testenv) directory.

To run the integrated test environment, the docker engine and compose must be installed per the [Docker installation instructions](https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository) and typically the user will need to be added to the `docker` system group for access.
The installation can be validated by the ability to run simple introspection commands like `docker ps` and see a successful result.

Once that is done, the images can be built and containers started with:
```
./testenv/start.sh
```
A checkout test can be run to ensure services are running and round-trip AMP control-to-report loop is closed with:
```
./testenv/check.sh
```
Finally, after test anything in the containers is finished they can be stopped and cleaned up with:
```
./testenv/stop.sh
```

## Stand-Alone Agent

As both a demonstration and a useful test fixture for AMP and ADMs, a stand-alone Agent which uses [stdio](https://en.cppreference.com/w/c/io) (`stdin` for commands and `stdout` for reporting) with hex-encoded AMP messages is included as the [refda-stdio](src/refda_stdio/main.c).
This provides a minimum implementation of an Agent executable without requriring a specific underlying transport for AMP messages.
This is not a recommended way to use the Agent in a networked environemnt but makes it easier to do things like exercising the Agent in a simple test fixture like what is done with [test_stdio_agent.py](agent-test/test_stdio_agent.py).

A simple validation that the Agent is build and installed properly is the sequence
```
./build.sh install
echo -ne "ari:/EXECSET/n=1234;(//1/CTRL/5(//1/EDD/sw-version))" | ./run.sh cace_ari | ./run.sh refda-stdio -l debug
```

A loopback text output can be converted with:
```
echo -ne "ari:/EXECSET/n=1234;(//1/CTRL/5(//1/EDD/sw-version))" | ./run.sh cace_ari | ./run.sh refda-stdio -l debug | ./run.sh cace_ari
```

## Socket-transport Manager and Agent

These daemons use the AMP message encoding as networked daemons would, but use local UNIX datagram sockets (`AF_UNIX` family) for transport.
This avoids the need for any more complex network configuration while allowing the daemons to use the full binary message encoding of AMP.

The manager (and others) are built and installed to the local `testroot` with:
```
./build.sh
./build.sh install
```

The manager can be run from the local installation using:
```
./run.sh gdb --args refdm-socket -l debug -a /tmp/mgr.sock
```

A manual RPTSET can be sent to the manger via command:
```
echo -n 01821583f61a2d2639a58382250083012205f6 | xxd -r -p | socat STDIO UNIX-SENDTO:/tmp/mgr.sock,bind=/tmp/agent.sock
```

Checking that the report was received with:
```
curl -qv http://localhost:8089/nm/api/agents/eid/file%3A%2Ftmp%2Fagent.sock/reports/hex | json_reformat
```

## Updating ADM Implementations

To update and synchronize the REFDA implementations of ADMs, a separate tool `camp` (hosted in the [dtnma-camp repo](https://github.com/JHUAPL-DTNMA/dtnma-camp)) can be used to generate C source files compatible with the REFDA object registration APIs.
The `camp` tool can be installed as described in its source repository.

To generate and format new ADM-supporting sources, the following procedure can be used.
```
for MODPATH in $(ls -1 item-test/deps/adms/ietf-*.yang item-test/deps/adms/iana-*.yang); do camp -s --only-ch --out src/refda/adm/ $MODPATH; done && \
./apply_format.sh src/refda/adm/*.h src/refda/adm/*.c && \
./apply_license.sh src/refda/adm/
```

After updating the sources the changes can be reviewed, tweaked manually if needed, and committed to a branch of the source tree.

## Support
The wiki for this project contains additional details outside of the source and API documentation, and the issue tracker for this project is used for defect reports and enhancement requests.
Additional details are in the [Contributing](CONTRIBUTING.md) document.

## Contributing
See the separate [Contributing](CONTRIBUTING.md) document for details on contributing to this project.

## Authors and acknowledgment
Original authors of the DTNMA Agent and Manager within the NASA ION project are Ed Birrane, David Linko, Sarah Heiner.

## License
This project uses the [Apache-2.0](LICENSE.txt) license.
