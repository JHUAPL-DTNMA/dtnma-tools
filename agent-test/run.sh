#!/bin/bash
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
python3 -m pytest agent-test --capture=no --log-cli-level=info
