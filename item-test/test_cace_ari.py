#
# Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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

import binascii
import io
import logging
import os
import signal
import subprocess
import time
from typing import Optional, List, Tuple
import unittest
import cbor2
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers.runner import CmdRunner, compose_args

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)

HEXPAT = r'^[0-9a-fA-F]+'
''' Generic hexadecimal regex pattern. '''
URIPAT = r'^ari:.+'
''' Text form of ARI regex pattern. '''

# ADM handling outside of tests
ADMS = AdmSet(cache_dir=False)
logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)
ADMS.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])


class TestCaceAri(unittest.TestCase):
    ''' Verify behavior of the cace_ari utility '''

    def setUp(self):
        logging.getLogger('sqlalchemy.engine').setLevel(logging.WARNING)

        path = os.path.abspath(os.path.join(OWNPATH, '..'))
        os.chdir(path)
        LOGGER.info('Working in %s', path)

        self._runner: Optional[CmdRunner] = None

    def tearDown(self):
        if self._runner:
            if self._runner.poll() in {None, False}:
                self._runner.close_stdin()
                self.assertEqual(0, self._runner.wait())
            self._runner = None

    def _start(self, *cmd_args: str) -> CmdRunner:
        ''' Spawn the process. '''
        base_args = (
            'cace_ari',
            '-l', os.environ.get('TEST_LOG_LEVEL', 'debug'),
        )
        args = compose_args(list(base_args + cmd_args))
        self._runner = CmdRunner(args)
        self._runner.start()
        return self._runner

    def _ari_obj_from_text(self, text: str) -> ARI:
        nn_func = nickname.Converter(nickname.Mode.TO_NN, ADMS.db_session(), must_nickname=True)

        with io.StringIO(text) as buf:
            ari = ari_text.Decoder().decode(buf)
        ari = nn_func(ari)
        return ari

    def _ari_obj_to_cbor(self, ari: ARI) -> bytes:
        buf = io.BytesIO()
        ari_cbor.Encoder().encode(ari, buf)
        return buf.getvalue()

    def _ari_obj_from_cbor(self, databuf: bytes) -> ARI:
        with io.BytesIO(databuf) as buf:
            ari = ari_cbor.Decoder().decode(buf)
        return ari

    def test_get_help(self):
        runner = self._start('-h')
        self.assertEqual(0, runner.proc.wait(timeout=1))

    def test_start_sigint(self):
        runner = self._start()

        LOGGER.info('Sending SIGINT')
        runner.proc.send_signal(signal.SIGINT)
        self.assertEqual(-2, self._runner.wait())

    def test_start_close(self):
        runner = self._start()

        LOGGER.info('Closing stdin')
        runner.proc.stdin.close()
        self.assertEqual(0, runner.proc.wait(timeout=1))

    def test_log_level_valid(self):
        runner = self._start('--log-level=debug')

    def test_log_level_invalid(self):
        runner = self._start('--log-level=other')
        self.assertEqual(1, runner.proc.wait(timeout=1))

    def test_translate_inform_auto_text(self):
        runner = self._start('--inform=auto', '--outform=cborhex')

        runner.send_stdin('ari:10\n')
        got = runner.wait_for_line()
        self.assertEqual('0A\n', got)

    def test_translate_inform_auto_cborhex(self):
        runner = self._start('--inform=auto')

        runner.send_stdin('0A\n')
        got = runner.wait_for_line()
        self.assertEqual('ari:10\n', got)

    def test_translate_inform_uri_empty(self):
        runner = self._start('--inform=uri', '--outform=cborhex')
        # should have no bad effects
        runner.close_stdin()

    def test_translate_inform_uri_one(self):
        runner = self._start('--inform=uri', '--outform=cborhex')

        runner.send_stdin('ari:10\n')
        got = runner.wait_for_line()
        self.assertEqual('0A\n', got)

    def test_translate_inform_cborhex_empty(self):
        runner = self._start('--inform=cborhex', '--outform=uri')
        # should have no bad effects
        runner.close_stdin()

    def test_translate_inform_cborhex_one(self):
        runner = self._start('--inform=cborhex', '--outform=uri')

        runner.send_stdin('0A\n')
        got = runner.wait_for_line()
        self.assertEqual('ari:10\n', got)

    def test_translate_inform_cbor_empty(self):
        runner = self._start('--inform=cbor', '--outform=uri')
        # should have no bad effects
        runner.close_stdin()

    def test_translate_inform_cbor_valid_one(self):
        runner = self._start('--inform=cbor', '--outform=uri')

        runner.send_stdin(bytes.fromhex('03'))
        runner.close_stdin()
        got = runner.wait_for_line()
        self.assertEqual('ari:3\n', got)

    def test_translate_inform_cbor_valid_many(self):
        runner = self._start('--inform=cbor', '--outform=uri')

        runner.send_stdin(bytes.fromhex('030405'))
        runner.close_stdin()
        got = runner.wait_for_line()
        self.assertEqual('ari:3\n', got)
        got = runner.wait_for_line()
        self.assertEqual('ari:4\n', got)
        got = runner.wait_for_line()
        self.assertEqual('ari:5\n', got)

    def test_translate_inform_cbor_invalid(self):
        runner = self._start('--inform=cbor', '--outform=uri')

        runner.send_stdin(bytes.fromhex('03'))
        # unterminated bytes
        runner.send_stdin(bytes.fromhex('436869'))
        runner.close_stdin()
        got = runner.wait_for_line()
        self.assertEqual('ari:3\n', got)
        # error after success
        runner.wait_for_text(r'.*Ran out of data to decode', stream='stderr')

    def test_translate_inform_multi_auto(self):
        runner = self._start('--inform=uri', '--inform=auto', '--outform=cbor', '--outform=auto')

        runner.send_stdin('0A\n')
        got = runner.wait_for_line()
        self.assertEqual('ari:10\n', got)
