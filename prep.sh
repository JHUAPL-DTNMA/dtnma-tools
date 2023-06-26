#!/bin/bash
#
# From a fresh checkout perform pre-build steps
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source setenv.sh

mkdir -p ${SELFDIR}/deps/build

if [ ! -e ${SELFDIR}/testroot/usr/include/ion.h ]
then
#   rm -rf ${SELFDIR}/deps/build/ion
  rsync --recursive ${SELFDIR}/deps/ion/ ${SELFDIR}/deps/build/ion/
  pushd ${SELFDIR}/deps/build/ion

  patch -p1 <${SELFDIR}/deps/ion-4.1.2-remove-nm.patch
  patch -p1 <${SELFDIR}/deps/ion-4.1.2-local-deliver.patch
  patch -p1 <${SELFDIR}/deps/ion-4.1.2-private-headers.patch
  autoreconf -vif
  ./configure --prefix=/usr
  make -j$(nproc) clean
  make -j$(nproc)
  make install DESTDIR=${SELFDIR}/testroot
  make -j$(nproc) clean
  popd
fi

if [ ! -e ${SELFDIR}/testroot/usr/include/qcbor/qcbor.h ]
then
  rsync --recursive ${SELFDIR}/deps/QCBOR/ ${SELFDIR}/deps/build/QCBOR/
  pushd ${SELFDIR}/deps/build/QCBOR

  patch -p1 <${SELFDIR}/deps/qcbor-install.patch
  patch -p2 <${SELFDIR}/deps/qcbor-expose-private.patch
  make -j$(nproc)
  make install PREFIX=/usr DESTDIR=${SELFDIR}/testroot
  make -j$(nproc) clean
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

if [ ! -e ${SELFDIR}/testroot/usr/include/m-lib ]
then
  rsync --recursive ${SELFDIR}/deps/mlib/ ${SELFDIR}/deps/build/mlib/
  pushd ${SELFDIR}/deps/build/mlib
  
  make -j$(nproc)
  make install PREFIX=/usr DESTDIR=${SELFDIR}/testroot
  make -j$(nproc) clean
  popd
fi

#if [ ! -e ${SELFDIR}/testroot/usr/include/unity ]
#then
#  pushd ${SELFDIR}/deps/unity
#  cmake -S . -B build \
#    -DCMAKE_INSTALL_PREFIX=${SELFDIR}/testroot/usr
#  cmake --build build
#  cmake --install build
#  popd
#fi

cmake -S ${SELFDIR} -B ${SELFDIR}/build/default \
  -DCMAKE_PREFIX_PATH=${SELFDIR}/testroot/usr \
  -DCMAKE_INSTALL_PREFIX=${SELFDIR}/testroot/usr \
  -DBUILD_MANAGER=YES \
  -DCMAKE_BUILD_TYPE=Debug \
  -G Ninja
