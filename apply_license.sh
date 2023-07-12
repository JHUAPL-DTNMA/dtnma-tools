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

LICENSEOPTS="${LICENSEOPTS} --tmpl ${SELFDIR}/license.tmpl"
LICENSEOPTS="${LICENSEOPTS} --current-year"
# Excludes only apply to directory (--dir) mode and not file mode
LICENSEOPTS="${LICENSEOPTS} --exclude *.yml *.yaml *.min.* "


# Specific paths
if [ $# -gt 0 ]
then
    echo "Applying markings to selected $@ ..."
    licenseheaders ${LICENSEOPTS} --dir $@
    exit 0
fi


echo "Applying markings to source..."
for SUBDIR in cmake src test testenv doc
do
    licenseheaders ${LICENSEOPTS} --dir ${SELFDIR}/${SUBDIR}
done
# Restore non-managed files
git restore ${SELFDIR}/src/ion_if/agent/adm_*
git restore ${SELFDIR}/src/ion_if/mgr/adm_*
git restore ${SELFDIR}/src/ion_if/shared/adm/adm_*
git restore ${SELFDIR}/src/ion_if/bpv7/adm/adm_*
