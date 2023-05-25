#!/usr/bin/env python3
#
# Copyright (c) 2023 The Johns Hopkins University Applied Physics
# Laboratory LLC.
#
# This file is part of the Asynchronous Network Managment System (ANMS).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This work was performed for the Jet Propulsion Laboratory, California
# Institute of Technology, sponsored by the United States Government under
# the prime contract 80NM0018D0004 between the Caltech and NASA under
# subcontract 1658085.
#
import argparse
import os
import pty
import select
import socket
from subprocess import Popen
import sys
import systemd.daemon
import systemd.journal


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--listen', required=True)
    parser.add_argument('cmd', nargs='+')
    args = parser.parse_args()

    # Listen socket, not connection
    try:
        # If a path to the socket already exists, unlink
        if os.path.exists(args.listen):
            os.unlink(args.listen)
    except OSError:
        raise

    # AF_UNIX socket: Bind to a file system node (listen arg)
    # SOCK_STREAM: TCP. Sequenced, 2-way connection
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.bind((args.listen))
    # Backlog (number of unaccepted connections permitted) is 1
    sock.listen(1)

    # open pseudo-terminal to interact with subprocess
    # master_fd is the pty
    # slave_fd is the tty
    master_fd, slave_fd = pty.openpty()
    
    # use start_new_session=True to make it run in a new process group, or bash job control will not be enabled
    proc = Popen(args.cmd,
                 start_new_session=True,
                 stdin=slave_fd,
                 stdout=slave_fd,
                 stderr=slave_fd)

    if systemd.daemon.notify("READY=1"):
        log = systemd.journal.stream(args.cmd[0])
    else:
        log = sys.stdout

    # dynamic source list
    readers = [sock, master_fd]

    while proc.poll() is None:
        try:
            r, w, e = select.select(readers, [], [])
            if sock in r:
                conn, addr = sock.accept()
                r.append(conn)
                r.remove(sock)
            if master_fd in r:
                buf = os.read(master_fd, 10240)

                # Strip out prompt from the NM Automator interface
                from binascii import hexlify
                buf = buf.replace(b'\r\n#-NM->', b'\n')
                
                if buf:
                    log.write(buf.decode('utf8'))
                    log.flush()
                r.remove(master_fd)

            for conn in r:
                # one of the connections
                buf = conn.recv(10240)
                if not buf:
                    conn.close()
                    r.remove(conn)
                else:
                    os.write(master_fd, buf)

        except KeyboardInterrupt:
            proc.terminate()
            # no error
            break
        except:
            proc.terminate()
            raise

    return proc.returncode


if __name__ == '__main__':
    sys.exit(main())

