#!/bin/bash
##
## Copyright (c) 2023 The Johns Hopkins University Applied Physics
## Laboratory LLC.
##
## This file is part of the Delay-Tolerant Networking Management
## Architecture (DTNMA) Tools package.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##     http://www.apache.org/licenses/LICENSE-2.0
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

# Synchronize and build an updated wireshark version.
# This relies on steps from `prep.sh` having been run.
#
set -e

# Allow override sudo behavior
SUDO=${SUDO:=sudo}

SELFDIR=$(dirname "${BASH_SOURCE[0]}")
cd "${SELFDIR}"

ACTION=$1
shift 1 || true


echo "Combining sources..."
rsync --recursive --archive upstream/ combined/
rsync --recursive --archive overlay/ combined/

echo "Building..."
if [[ ! -d combined/build ]]; then
    cmake -S combined -B combined/build -GNinja -DCMAKE_BUILD_TYPE=Debug
fi
cmake --build combined/build

if [[ "$ACTION" = "run" ]]; then
    export PATH=./combined/build/run/:$PATH
    export LD_LIBRARY_PATH=./combined/build/run/:$LD_LIBRARY_PATH
    "$@"
fi
if [[ "$ACTION" = "install" ]]; then
    $SUDO cmake --install combined/build
    $SUDO chmod u+s /usr/local/bin/dumpcap
fi
if [[ "$ACTION" = "check" ]]; then
      pushd combined/build
      cmake --build . --target test-programs
      ctest --output-on-failure -R 'suite_unittests|suite_dissection'
      popd
fi
