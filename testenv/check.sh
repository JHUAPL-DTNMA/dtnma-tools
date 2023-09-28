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
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
cd "${SELFDIR}"

docker-compose ps

DEXEC="docker-compose exec -T nm-mgr"

# Wait a few seconds for ION to start
for IX in $(seq 10)
do
  sleep 1
  if ${DEXEC} service_is_running ion
  then
    break
  fi
  echo "Waiting for ion..."
done

for SVC in ion ion-nm-mgr ion-nm-agent
do
  echo
  if ! ${DEXEC} service_is_running ${SVC}
  then
    echo "Logs for ${SVC}:"
    ${DEXEC} journalctl --unit ${SVC}
  fi
done

# All manager actions operate with this base
URIBASE="http://localhost:8089/nm/api/agents/eid/ipn:1.6"

${DEXEC} curl -sv -XPUT ${URIBASE}/clear_reports

echo 'ari:/IANA:amp_agent/CTRL.gen_rpts([ari:/IANA:amp_agent/RPTT.full_report],[])' | \
    ace_ari --inform text --outform cborhex | \
    ${DEXEC} curl -sv -XPUT ${URIBASE}/hex -H 'Content-Type: text/plain' --data-binary @-

RPTOBJ=""
for IX in $(seq 10)
do
  sleep 1
  RPTOBJ=$(${DEXEC} curl -sv -XGET ${URIBASE}/reports/json | jq .reports[0])
  if [ -n "$RPTOBJ" ]
  then
    break
  fi
done

if [ -n "$RPTOBJ" ]
then
  echo "Got Report:"
  echo ${RPTOBJ} | jq
else
  exit 1
fi
