#!/bin/bash
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
cd "${SELFDIR}"

export DOCKER_BUILDKIT=1

docker-compose build
docker-compose up -d
