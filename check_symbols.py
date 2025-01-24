#!/usr/bin/env python3
#
# Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
# Laboratory LLC.
#
# This file is part of the Delay-Tolerant Networking Management
# Architecture (DTNMA) Tools package.
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
''' Check built libraries for valid symbol names. '''
import argparse
import logging
import os
import re
import subprocess
import sys

SELFDIR = os.path.realpath(os.path.dirname(__file__))

LOGGER = logging.getLogger()

NMOPTS = [
    '-C',
    '--format=posix', '--line-numbers',
    '--extern-only', '--dynamic',
]

def check_lib(libname):
    file_name = f'lib{libname}.so'
    file_path = None
    for root, _, files in os.walk(os.path.join(SELFDIR, 'testroot')):
        if file_name in files:
            file_path = os.path.join(root, file_name)
            break
    if not file_path:
        raise ValueError(f'Missing library for {libname}')

    src_path = os.path.join(SELFDIR, 'src')

    LOGGER.info('Checking %s', file_path)
    stdout = subprocess.check_output(['nm'] + NMOPTS + [file_path], text=True)
    bad_syms = set()
    for line in stdout.splitlines():
        parts = re.split(r'\s+', line.rstrip())
        sym = parts[0]
        sec = parts[1]
        if sec.casefold() != 't':
            continue
        try:
            src_name = parts[4]
        except IndexError:
            continue
        if not src_name.startswith(src_path):
            continue

        if not sym.casefold().startswith(f'{libname}_'):
            LOGGER.warning('Bad symbol %s', sym)
            bad_syms.add(sym)

    return bad_syms

def main():
    parser = argparse.ArgumentParser(
        description=__doc__
    )
    parser.add_argument('--log-level', choices=('debug', 'info', 'warning', 'error'),
                        default='info',
                        help='The minimum log severity.')
    parser.add_argument('libname', nargs='+',
                        help='The library name to check')
    args = parser.parse_args()
    logging.basicConfig(level=args.log_level.upper())

    bad_tot = set()
    for libname in args.libname:
        bad_tot |= check_lib(libname)
    return len(bad_tot)

if __name__ == '__main__':
    sys.exit(main())
