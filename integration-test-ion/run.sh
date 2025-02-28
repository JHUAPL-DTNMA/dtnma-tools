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
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
cd "${SELFDIR}"

DOCKER=${DOCKER:-docker}

if [ "$1" = "start" ]
then
    export DOCKER_BUILDKIT=1
    
    ${DOCKER} compose build
    ${DOCKER} compose up -d --force-recreate --remove-orphans
elif [ "$1" = "stop" ]
then
    ${DOCKER} compose stop
    ${DOCKER} compose rm -fv
elif [ "$1" = "check" ]
then
    ${DOCKER} compose ps

    DEXEC="${DOCKER} compose exec -T -e REFDA_EID=ipn:2.6 manager"

    # Wait a few seconds for ION to start
    for IX in $(seq 10)
    do
        sleep 1

        WAITING=0
        for SVC in ion refdm-ion refda-ion
        do
              echo
              if ! ${DEXEC} service_is_running ${SVC}
              then
                WAITING=$((WAITING + 1))
                echo "Logs for ${SVC}:"
                ${DEXEC} journalctl --unit ${SVC}
              fi
        done
        echo "Waiting on ${WAITING} services"
        if [[ ${WAITING} -eq 0 ]]
        then
            break
        fi
    done
    if [[ ${WAITING} -ne 0 ]]
    then
        echo "Services did not all start"
        exit 2
    fi

    CURLOPTS="-svf --variable '%REFDA_EID'"
    # All manager actions operate with this base
    URIBASE="http://manager:8089/nm/api"

    CMD="curl ${CURLOPTS} -XPOST ${URIBASE}/agents -H 'Content-Type: text/plain' --expand-data '{{REFDA_EID}}'"
    echo $CMD | ${DEXEC} bash
    echo

    CMD="curl ${CURLOPTS} -XPOST --expand-url ${URIBASE}/agents/eid/{{REFDA_EID:trim:url}}/clear_reports"
    echo $CMD | ${DEXEC} bash
    echo

    # send an inspect execution with a nonce, expecting a report back
    CMD="echo 'ari:/EXECSET/n=12345;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version))' | \
        ace_ari --inform text --outform cborhex --must-nickname | \
        curl ${CURLOPTS} -XPOST --expand-url ${URIBASE}/agents/eid/{{REFDA_EID:trim:url}}/send?form=hex -H 'Content-Type: text/plain' --data-binary @-; echo"
    echo $CMD | ${DEXEC} bash
    echo

    RPTOBJ=""
    for IX in $(seq 10)
    do
        echo "Waiting on report back..."
        sleep 1

        CMD="curl ${CURLOPTS} -XGET --expand-url ${URIBASE}/agents/eid/{{REFDA_EID:trim:url}}/reports?form=text"
        RPTLINES=$(echo $CMD | ${DEXEC} bash)
        if [ -n "$RPTLINES" ]
        then
            break
        fi
    done

    if [ -n "$RPTLINES" ]
    then
      echo "Got Report lines:"
      echo ${RPTLINES}
      echo
    else
      exit 3
    fi

    # view the hex-binary version also
    CMD="curl ${CURLOPTS} -XGET --expand-url ${URIBASE}/agents/eid/{{REFDA_EID:trim:url}}/reports?form=hex | xxd -r -p | cborseq2diag.rb"
    echo $CMD | ${DEXEC} bash

fi
