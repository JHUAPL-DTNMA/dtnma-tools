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

# Pull and prepare for a wireshark build.
# This script relies on git and facter being installed and usable.
#
set -e

WIRESHARKTAG=5bbaf9ef354ed1bae50060800a1775f315b666f1

# Allow override sudo behavior
SUDO=${SUDO:=sudo}

SELFDIR=$(dirname "${BASH_SOURCE[0]}")
cd "${SELFDIR}"

echo "Fetching upstream..."
if [[ ! -d upstream ]]; then
    git clone https://gitlab.com/wireshark/wireshark.git upstream
fi

pushd upstream
git config advice.detachedHead false
git checkout ${WIRESHARKTAG}
popd


OS_FAMILY=$(facter os.family)
case "${OS_FAMILY}" in
    RedHat)
	${SUDO} dnf install -y ninja-build perl xsltproc
	${SUDO} ./upstream/tools/rpm-setup.sh --install-qt6-deps -fy
	;;
    Debian)
	${SUDO} apt-get update
	${SUDO} apt-get install -y rsync ninja-build liblua5.2-dev perl xsltproc
	${SUDO} ./upstream/tools/debian-setup.sh --install-qt6-deps --install-test-deps -fy
	;;
    *)
	echo "No packages for OS family: ${OS_FAMILY}"
	exit 1
	;;
esac

