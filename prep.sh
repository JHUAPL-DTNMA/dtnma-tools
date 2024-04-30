#!/bin/bash
##
## Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
# From a fresh checkout perform pre-build steps on this project.
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source ${SELFDIR}/setenv.sh

cmake -S ${SELFDIR} -B ${SELFDIR}/build/default \
  -DCMAKE_PREFIX_PATH=${DESTDIR}${PREFIX} \
  -DCMAKE_INSTALL_PREFIX=${DESTDIR}${PREFIX} \
  -DBUILD_SHARED_LIBS=YES \
  -DBUILD_DOCS=YES \
  -DBUILD_MANAGER=YES \
  -DBUILD_TESTING=YES \
  -DTEST_COVERAGE=YES \
  -DTEST_MEMCHECK=YES \
  -DCMAKE_BUILD_TYPE=Debug \
  -G Ninja \
  $@
