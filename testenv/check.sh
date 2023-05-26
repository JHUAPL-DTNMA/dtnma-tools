#!/bin/bash
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
cd "${SELFDIR}"

docker-compose ps

DEXEC="docker-compose exec nm-mgr"

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