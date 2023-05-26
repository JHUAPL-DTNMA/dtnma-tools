#!/bin/bash
##
## Copyright (c) 2023 The Johns Hopkins University Applied Physics
## Laboratory LLC.
##
## This file is part of the Asynchronous Network Managment System (ANMS).
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
## This work was performed for the Jet Propulsion Laboratory, California
## Institute of Technology, sponsored by the United States Government under
## the prime contract 80NM0018D0004 between the Caltech and NASA under
## subcontract 1658085.
##

# Ping a set of peer Node Number taken from command arguments.
# The own Node Number must be the first argument.
set -e

OWN_NODE=$1
PEERS="$@"
if [ -z "${PEERS}" ]; then
    exit 1
fi

while ! service_is_running ion; do
    sleep 1
done

EXIT=0
for PEER_NODE in $PEERS; do
    echo "Pinging node ${PEER_NODE}..."
    bping -c3 -i0 -t10 -q1 ipn:${OWN_NODE}.1 ipn:${PEER_NODE}.4 || (echo "FAILED!" && EXIT=2)
    echo
done

exit ${EXIT}
