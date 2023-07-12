# DTNMA Tools

This software package contains tools related to the Delay-Tolerant Networking Management Architecture (DTNMA), specifically an implementation of the DTNMA Agent and Manager.

The current tools are based on the following specifications:

* [draft-birrane-dtn-amp-08](https://datatracker.ietf.org/doc/html/draft-birrane-dtn-amp-08) for messaging
* [draft-birrane-dtn-adm-03](https://datatracker.ietf.org/doc/html/draft-birrane-dtn-adm-03) for data models and ARI processing

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
civetweb
libcivetweb-dev
libssl-dev
libcjson-dev
```

To use the systemd wrapper utility around the executables, also install the PIP package
```
systemd-python
```

Once dependencies are installed, the following scripts can be run to build and test the project:
```
./prep.sh
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

## Support
The wiki for this project contains additional details outside of the source and API documentation, and the issue tracker for this project is used for defect reports and enhancement requests.
Additional details are in the [Contributing](CONTRIBUTING.md) document.

## Contributing
See the separate [Contributing](CONTRIBUTING.md) document for details on contributing to this project.

## Authors and acknowledgment
Original authors of the DTNMA Agent and Manager within the NASA ION project are Ed Birrane, David Linko, Sarah Heiner.

## License
This project uses the [Apache-2.0](LICENSE.txt) license.
