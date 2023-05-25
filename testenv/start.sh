#!/bin/bash
set -e

export DOCKER_BUILDKIT=1
COMPOSEOPTS="-f testenv/docker-compose.yml"
docker-compose ${COMPOSEOPTS} build
docker-compose ${COMPOSEOPTS} up -d
