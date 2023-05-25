#!/bin/bash
#
# From a fresh checkout perform pre-build steps
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source setenv.sh

if [ ! -e ${SELFDIR}/testroot/usr/include/ion.h ]
then
  pushd ${SELFDIR}/deps/ion

#  git restore .
  if grep nmrest configure.ac >/dev/null
  then
    patch -p1 <../ion-4.1.2-remove-nm.patch
  fi
  
  autoreconf -vif
  ./configure --prefix=/usr
  make -j$(nproc) clean
  make -j$(nproc)
  make install DESTDIR=${SELFDIR}/testroot
  make -j$(nproc) clean
  popd
fi

if [ ! -e ${SELFDIR}/testroot/usr/include/m-lib ]
then
  pushd ${SELFDIR}/deps/mlib
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
  -DCMAKE_BUILD_TYPE=Debug \
  -G Ninja
