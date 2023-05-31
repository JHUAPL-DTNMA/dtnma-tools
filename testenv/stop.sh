#!/bin/bash
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
cd "${SELFDIR}"

docker-compose stop
docker-compose rm -fv
