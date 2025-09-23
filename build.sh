#!/bin/bash
##
## Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

#
# From a fresh checkout perform a full build
#
set -e
set -o pipefail

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source ${SELFDIR}/setenv.sh

BUILDDIR=${SELFDIR}/build/default

if [ "$1" = "docs" ]
then
    cmake --build ${BUILDDIR} --target docs-api-html docs-api-misspelling
elif [ "$1" = "install" ]
then
    shift
    cmake --install ${BUILDDIR} "$@"
elif [ "$1" = "check" ]
then
    cmake --build ${BUILDDIR} --target test
elif [ "$1" = "coverage" ]
then
    cmake --build ${BUILDDIR} -j1 --target \
        coverage-cace-html coverage-cace-xml \
        coverage-refda-html coverage-refda-xml \
        coverage-refdm-html coverage-refdm-xml
elif [ "$1" = "coverage-summary" ]
then
    for DIRNAME in cace refda refdm
    do
        COV_XPATH="format-number(/coverage/@line-rate * 100, '#.0')"
        COV_PERC=$(xmlstarlet sel -t -v "${COV_XPATH}" -n build/default/coverage-${DIRNAME}-xml.xml 2>/dev/null)
        echo "Source ${DIRNAME} coverage: ${COV_PERC}%"
    done
else
    cmake --build ${BUILDDIR} "$@"
fi
