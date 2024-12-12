#!/bin/bash
##
## Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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

# Execute the agent test in a virtualenv environment.
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))

cd ${SELFDIR}/..

echo "Installing virtualenv..."
python3 -m venv ./build/venv
source ./build/venv/bin/activate

echo "Installing dependencies..."
if [[ ! -d agent-test/deps/adms ]]
then
    git clone --branch apl-fy24 https://github.com/JHUAPL-DTNMA/dtnma-adms.git agent-test/deps/adms
fi
if [[ ! -d agent-test/deps/dtnma-ace ]]
then
    git clone --branch apl-fy24 https://github.com/JHUAPL-DTNMA/dtnma-ace.git agent-test/deps/dtnma-ace
fi
pip3 install -e agent-test/deps/dtnma-ace
pip3 install -r ./agent-test/requirements.txt

echo "Executing tests..."
python3 -m pytest agent-test --capture=no --log-cli-level=info $@
