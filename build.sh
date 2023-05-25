#!/bin/bash
#
# From a fresh checkout perform a full build
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source setenv.sh

cmake -S ${SELFDIR} -B ${SELFDIR}/build/default
cmake --build ${SELFDIR}/build/default
cmake --install ${SELFDIR}/build/default
