#
# Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

import logging
import os
import re
import signal
import subprocess
import time
import threading
from typing import List, Optional
import queue

LOGGER = logging.getLogger(__name__)
''' Logger for this module. '''


def compose_args(args: List[str]) -> List[str]:
    ''' Combine executions arguments with any prefix scripts and/or tools
    needed to run from the `testroot` environment.
    '''
    args = list(args)
    if os.environ.get('TEST_MEMCHECK', ''):
        valgrind = [
            'valgrind',
            '--tool=memcheck',
            '--leak-check=full',
            '--suppressions=memcheck.supp',
            '--gen-suppressions=all',
            '--error-exitcode=2',
        ]
        args = valgrind + args
    args.insert(0, './run.sh')
    return args


class CmdRunner:
    ''' Manage the lifetime of a child process executed from a command.

    :param args: The command arguments to execute including the command
    itself.
    :param kwargs: Additional keyword arguments given to
    :py:func:`subprocess.Popen`
    '''

    def __init__(self, args: List[str], **kwargs):
        self.proc = None
        self._reader = None
        self._args = args
        self._kwargs = kwargs

        self._stdin_lines = queue.Queue()
        self._stdout_lines = queue.Queue()

    def _fmt_args(self):
        return ' '.join([
            '"{}"'.format(arg.replace('"', '\\"')) for arg in self._args
        ])

    def start(self):
        if self.proc:
            return

        while not self._stdout_lines.empty():
            self._stdout_lines.get()

        LOGGER.info('Starting process: %s', self._fmt_args())
        self.proc = subprocess.Popen(
            self._args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            text=True,
            **self._kwargs
        )
        self._reader = threading.Thread(
            target=self._read_stdout,
            args=[self.proc.stdout]
        )
        self._reader.start()
        self._writer = threading.Thread(
            target=self._write_stdin,
            args=[self.proc.stdin]
        )
        self._writer.start()

    def _finish(self):
        ''' Clean up the process state after exit.
        '''
        ret = self.proc.returncode
        self.proc = None
        LOGGER.info('Stopped with exit code: %s', ret)

        self._reader.join()
        self._reader = None

        self._stdin_lines.put(None)
        self._writer.join()
        self._writer = None

        return ret

    def wait(self, timeout=5) -> Optional[int]:
        ''' Wait for the process to finish.

        :param timeout: The time (in seconds) to wait for the process to exit.
        :return: The exit code, or None if it not already running.
        :raise subprocess.TimeoutExpired: If the wait has timed out.
        '''
        if not self.proc:
            return None

        LOGGER.info('Waiting on process: %s', self._fmt_args())
        if self.proc.returncode is None:
            self.proc.wait(timeout=timeout)

        return self._finish()

    def stop(self, timeout=5) -> Optional[int]:
        ''' Signal for the process to stop.
        If the process has not stopped after the timeout, it is killed.

        :param timeout: The time (in seconds) to wait for the process to exit.
        :return: The exit code, or None if it not already running.
        :raise subprocess.TimeoutExpired: If the wait has timed out.
        '''
        if not self.proc:
            return None

        LOGGER.info('Stopping process: %s', self._fmt_args())
        if self.proc.returncode is None:
            self.proc.send_signal(signal.SIGINT)
            try:
                self.proc.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                LOGGER.error('Timed-out after SIGINT, killing process: %s', self._fmt_args())
                self.proc.kill()
                self.proc.wait(timeout=timeout)

        return self._finish()

    def _read_stdout(self, stream):
        LOGGER.debug('Starting stdout thread')
        for line in iter(stream.readline, ''):
            LOGGER.debug('Got stdout: %s', line.strip())
            self._stdout_lines.put(line)
        LOGGER.debug('Stopping stdout thread')
        stream.close()

    def _write_stdin(self, stream):
        LOGGER.debug('Starting stdin thread')
        while True:
            text = self._stdin_lines.get()
            if text is None:
                break
            LOGGER.debug('Sending stdin: %s', text.strip())
            stream.write(text)
            stream.flush()
        LOGGER.debug('Stopping stdin thread')
        stream.close()

    def wait_for_line(self, timeout:float=5) -> str:
        ''' Wait for any received stdout line.

        :param timeout: The total time to wait for this line.
        :return The matching line.
        :raise TimeoutError: If the line was not seen in time.
        '''
        try:
            text = self._stdout_lines.get(timeout=timeout)
        except queue.Empty:
            raise TimeoutError('no lines received before timeout')
        return text

    def wait_for_text(self, pattern:str, timeout:float=5) -> str:
        ''' Iterate through the received stdout lines until a specific
        full matching line is seen.

        :param pattern: The pattern which must match the full line.
            Use prefix or suffix ".*" as needed.
        :param timeout: The total time to wait for this line.
        :return The matching line.
        :raise TimeoutError: If the line was not seen in time.
        '''
        expr = re.compile(pattern)
        LOGGER.debug('Waiting for pattern "%s" ...', pattern)

        deadline_time = time.time_ns() / 1e9 + timeout
        while True:
            remain_time = deadline_time - time.time_ns() / 1e9
            if remain_time <= 0:
                break
            try:
                text = self._stdout_lines.get(timeout=remain_time)
            except queue.Empty:
                raise TimeoutError('text not received before timeout')

            if expr.match(text) is not None:
                return text

    def send_stdin(self, text:str):
        ''' Send an exact line of text to the process stdin.

        :param text: The line to send, which should include a newline
            at the endd.
        '''
        self._stdin_lines.put(text)

    def close_stdin(self):
        ''' Flush and close the stdin stream.
        '''
        self._stdin_lines.put(None)
