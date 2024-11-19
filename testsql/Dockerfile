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

FROM anms-init AS ion-base
RUN dnf install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-9.noarch.rpm
RUN dnf install  -y  \
    autoconf libtool \
    cmake ninja-build \
    ruby rsync  git \
    systemd systemd-sysv \
    patch autoconf libtool \
    make gcc gcc-c++ libpq-devel \
    civetweb-devel cjson-devel\ 
    perl-CPAN  \
    gdb less && \
    dnf clean all && rm -rf /var/cache/yum && \
    PERL_MM_USE_DEFAULT=1 cpan -T JSON REST::Client Expect File::Slurp

COPY ../deps/ion /usr/src/nm/deps/ion
COPY ../deps/ion*.patch /usr/src/nm/deps/
RUN cd /usr/src/nm/deps/ion && \
    patch -p1 <../ion-4.1.2-remove-nm.patch && \
    patch -p1 <../ion-4.1.2-local-deliver.patch && \
    patch -p1 <../ion-4.1.2-private-headers.patch && \
    autoreconf -vif && \
    ./configure \
        --prefix=/usr/local --libdir=/usr/local/lib64 && \
    make -j$(nproc) && \
    make install && \
    echo /usr/local/lib64 > /etc/ld.so.conf.d/local.conf && \
    ldconfig 

COPY ../deps/QCBOR /usr/src/nm/deps/QCBOR
COPY ../deps/qcbor*.patch /usr/src/nm/deps/
RUN cd /usr/src/nm/deps/QCBOR && \
    patch -p1 <../qcbor-install.patch && \
    patch -p2 <../qcbor-expose-private.patch && \
    make -j$(nproc) && \
    make install && \
    make -j$(nproc) clean

COPY ../deps/mlib /usr/src/nm/deps/mlib
RUN cd /usr/src/nm/deps/mlib && \
    make -j$(nproc) && \
    make install && \
    make -j$(nproc) clean



# Agent to test
COPY ../deps /usr/src/nm/deps
COPY ../cmake /usr/src/nm/cmake
COPY ../src /usr/src/nm/src
COPY ../test /usr/src/nm/test
COPY ../CMakeLists.txt /usr/src/nm/
RUN ls -lt /usr/src/nm/
RUN cd /usr/src/nm && \
    cmake -S . -B build/default \
    -DCMAKE_BUILD_TYPE=Debug \
      -G Ninja && \
    cmake --build build/default && \
    cmake --install build/default 

# Helper utilities
RUN dnf -y install \
    python3 python3-pip \
    gcc python3-devel systemd-devel && \
    dnf clean all && rm -rf /var/cache/yum && \
    pip3 install systemd-python
COPY --chmod=755 utils/ion_nm_wrap.py /usr/local/bin/ion_nm_wrap
COPY --chmod=755 utils/service_is_running.sh /usr/local/bin/service_is_running
COPY --chmod=755 utils/ion_restart_ducts.sh /usr/local/bin/ion_restart_ducts
COPY --chmod=755 utils/ion_ping_peers.sh /usr/local/bin/ion_ping_peers

# Systemd services
COPY tmpfiles.conf /etc/tmpfiles.d/ion.conf
COPY --chmod=644 systemd/ion.service systemd/ion-stats.service systemd/bpecho@.service /usr/local/lib/systemd/system/
COPY --chmod=644 systemd/ion-stats.timer /usr/local/lib/systemd/system/
RUN systemctl enable ion bpecho@4

FROM ion-base AS ion-manager
COPY configs/simple-2-node/mgr.rc /etc/ion.rc

COPY --chmod=644 systemd/ion-nm-mgr.service systemd/ion-nm-agent.service /usr/local/lib/systemd/system/
RUN systemctl enable ion-nm-mgr ion-nm-agent && \
    mkdir -p /var/run/ion

HEALTHCHECK --start-period=10s --interval=30s --timeout=5s --retries=5 \
    CMD ["service_is_running", "ion", "ion-nm-mgr"]


FROM ion-base AS ion-agent
COPY configs/simple-2-node/agent.rc.in /etc/ion.rc

COPY --chmod=644 systemd/ion-nm-agent.service /usr/local/lib/systemd/system/
RUN systemctl enable ion-nm-agent && \
    mkdir -p /var/run/ion

HEALTHCHECK --start-period=10s --interval=30s --timeout=5s --retries=5 \
    CMD ["service_is_running", "ion", "ion-nm-agent"]