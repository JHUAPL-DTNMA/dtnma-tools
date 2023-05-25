FROM ubuntu:22.04 AS systemd-base
ENV DEBIAN_FRONTEND="noninteractive"

# APL network configuration from
# https://aplprod.servicenowservices.com/sp?id=kb_article&sys_id=c0de6fe91b83d85071b143bae54bcb34
RUN apt-get update && apt-get install -y ca-certificates curl && \
    (curl -sSL http://apllinuxdepot.jhuapl.edu/linux/APL-root-cert/JHUAPL-MS-Root-CA-05-21-2038-B64-text.cer -o /usr/local/share/ca-certificates/JHUAPL-MS-Root-CA-05-21-2038-B64-text.crt || true) && \
    update-ca-certificates
ENV PIP_CERT=/etc/ssl/certs/ca-certificates.crt

# Distro upgrade for security patches
RUN apt-get update && apt-get upgrade -y

# Use systemd as top-level process
RUN apt-get update && apt-get install -y systemd systemd-sysv
RUN systemctl mask systemd-logind && \
    systemctl mask console-getty && \
    systemctl disable getty@tty1 && \
    systemctl disable apt-daily.timer apt-daily-upgrade.timer && \
    systemctl disable systemd-timesyncd && \
    systemctl disable systemd-networkd && \
    echo "MulticastDNS=no" >>/etc/systemd/resolved.conf
CMD [ "/sbin/init" ]

# Testing utilities
RUN apt-get update && apt-get install -y \
    net-tools iproute2 iputils-ping \
    lsof iftop gdb valgrind socat


FROM ubuntu:22.04 as deps-local
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf libtool
COPY deps/ion /usr/src/nm/deps/ion
COPY deps/ion*.patch /usr/src/nm/deps/
RUN cd /usr/src/nm/deps/ion && \
    patch -p1 <../ion-4.1.2-remove-nm.patch && \
    autoreconf -vif && \
    ./configure && \
    make -j$(nproc) && \
    make install && \
    make -j$(nproc) clean

COPY deps/mlib /usr/src/nm/deps/mlib
RUN cd /usr/src/nm/deps/mlib && \
    make -j$(nproc) && \
    make install && \
    make -j$(nproc) clean


FROM systemd-base
COPY --from=deps-local /usr/local /usr/local

# Helper utilities
RUN apt-get update && apt-get install -y \
    python3 python3-pip \
    gcc python3-dev libsystemd-dev pkg-config && \
    pip3 install systemd-python
COPY --chmod=755 testenv/ion_nm_wrap.py /usr/local/bin/ion_nm_wrap
COPY --chmod=755 testenv/service_is_running.sh /usr/local/bin/service_is_running

# Agent to test
RUN apt-get update && apt-get install -y \
    cmake ninja-build ruby
COPY deps /usr/src/nm/deps
COPY cmake /usr/src/nm/cmake
COPY src /usr/src/nm/src
COPY test /usr/src/nm/test
COPY CMakeLists.txt /usr/src/nm/
RUN ls -lt /usr/src/nm/
RUN cd /usr/src/nm && \
    cmake -S . -B build/default \
      -DCMAKE_BUILD_TYPE=Debug \
      -G Ninja && \
    cmake --build build/default && \
    cmake --install build/default

# Systemd services
COPY testenv/tmpfiles.conf /etc/tmpfiles.d/ion.conf
COPY --chmod=644 systemd/ion.service systemd/ion-nm-*.service systemd/bpecho@.service /usr/local/lib/systemd/system/
RUN systemctl enable ion ion-nm-mgr  && \
    mkdir -p /var/run/ion

# Runtime config for this container
COPY testenv/node-1.rc /etc/ion/

ENV ION_NODE_NUM=1
