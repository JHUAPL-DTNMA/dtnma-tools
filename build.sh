#!/bin/bash
#
# From a fresh checkout perform a full build
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source setenv.sh

cmake -S ${SELFDIR} -B ${SELFDIR}/build/default
cmake --build ${SELFDIR}/build/default

if [ "$1" = "install" ]
then
    cmake --install ${SELFDIR}/build/default
elif [ "$1" = "check" ]
then
    ctest --test-dir ${SELFDIR}/build/default \
	  --output-junit testresults.xml \
	  --verbose
fi
