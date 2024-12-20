#!/bin/bash
##
## Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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
# From a fresh checkout install local-sourced dependencies.
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source ${SELFDIR}/setenv.sh

DEPSDIR=${DEPSDIR:-${SELFDIR}/deps}
BUILDDIR=${BUILDDIR:-${SELFDIR}/deps/build}
echo "Building in ${BUILDDIR}"
echo "Installing to ${DESTDIR}"

if [ -z "false" -a ! -e ${DESTDIR}/usr/include/ion.h ]
then
  mkdir -p ${BUILDDIR}
  rsync --recursive ${DEPSDIR}/ion/ ${BUILDDIR}/ion/
  pushd ${BUILDDIR}/ion

  patch -p1 <${SELFDIR}/deps/ion-4.1.2-remove-nm.patch
  patch -p1 <${SELFDIR}/deps/ion-4.1.2-local-deliver.patch
  patch -p1 <${SELFDIR}/deps/ion-4.1.2-private-headers.patch
  autoreconf -vif
  ./configure --prefix=/usr
  make -j$(nproc) clean
  make -j$(nproc)
  make install DESTDIR=${DESTDIR}
  make -j$(nproc) clean
  popd
fi

if [ ! -e ${DESTDIR}/usr/include/qcbor/qcbor.h ]
then
  echo "Building QCBOR..."
  pushd ${DEPSDIR}/QCBOR
  cmake -S . -B ${BUILDDIR}/QCBOR \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=${DESTDIR}${PREFIX} \
    -DBUILD_SHARED_LIBS=YES
  cmake --build ${BUILDDIR}/QCBOR
  cmake --install ${BUILDDIR}/QCBOR
  rm -rf ${BUILDDIR}/QCBOR
  git restore .
  popd
fi

# if [ ! -e ${SELFDIR}/testroot/usr/include/civetweb.h ]
# then
#   rsync --recursive ${SELFDIR}/deps/civetweb/ ${SELFDIR}/deps/build/civetweb/
#   pushd ${SELFDIR}/deps/build/civetweb
# 
#   cmake -S . -B builddir \
#     -DCMAKE_INSTALL_PREFIX=${SELFDIR}/testroot/usr \
#     -DBUILD_SHARED_LIBS=YES \
#     -DCIVETWEB_ENABLE_SERVER_EXECUTABLE=NO \
#     -DCIVETWEB_BUILD_TESTING=NO \
#     -DCMAKE_BUILD_TYPE=Release \
#     -G Ninja
#   cmake --build builddir
#   cmake --install builddir
#   cmake --build builddir --target clean
#   popd
# fi

if [ ! -e ${DESTDIR}/usr/include/m-lib ]
then
  echo "Building MLIB..."
  rsync --recursive ${DEPSDIR}/mlib/ ${BUILDDIR}/mlib/
  pushd ${BUILDDIR}/mlib
  
  make -j$(nproc)
  make install PREFIX=${PREFIX} DESTDIR=${DESTDIR}
  make -j$(nproc) clean
  popd
fi

if [ ! -e ${DESTDIR}/usr/include/unity ]
then
  echo "Building Unity..."
  pushd ${DEPSDIR}/unity
  export CFLAGS="-DUNITY_INCLUDE_PRINT_FORMATTED -DUNITY_INCLUDE_FLOAT -DUNITY_INCLUDE_DOUBLE"
  cmake -S . -B ${BUILDDIR}/unity \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=${DESTDIR}${PREFIX}
  cmake --build ${BUILDDIR}/unity -v
  cmake --install ${BUILDDIR}/unity
  rm -rf ${BUILDDIR}/unity
  popd
fi

if [ ! -e ${DESTDIR}/usr/include/timespec.h ]
then
  echo "Building timespec..."
  rsync --recursive ${DEPSDIR}/timespec/ ${BUILDDIR}/timespec/
  rsync ${DEPSDIR}/timespec-CMakeLists.txt ${BUILDDIR}/timespec/CMakeLists.txt
  pushd ${BUILDDIR}/timespec
  cmake -S ${BUILDDIR}/timespec -B ${BUILDDIR}/timespec \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=${DESTDIR}${PREFIX} \
    -DBUILD_TESTING=ON
  cmake --build ${BUILDDIR}/timespec -v
  cmake --build ${BUILDDIR}/timespec --target test
  cmake --install ${BUILDDIR}/timespec
  rm -rf ${BUILDDIR}/timespec
  popd
fi

