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

import binascii
import io
import logging
import os
import signal
import subprocess
import time
from typing import List
import unittest
import cbor2
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers import CmdRunner, compose_args

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)

HEXPAT = r'^[0-9a-fA-F]+'
''' Generic hexadecimal regex pattern. '''

# ADM handling outside of tests
ADMS = AdmSet(cache_dir=False)
logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)
ADMS.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])


class TestStdioAgent(unittest.TestCase):
    ''' Verify whole-agent behavior with the stdio_agent '''

    @classmethod
    def setUpClass(cls):
        super().setUpClass()

        logging.getLogger('sqlalchemy.engine').setLevel(logging.WARNING)
        logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)

    def setUp(self):
        path = os.path.abspath(os.path.join(OWNPATH, '..'))
        os.chdir(path)
        LOGGER.info('Working in %s', path)

        args = compose_args(['refda-stdio', '-l', 'debug'])
        self._agent = CmdRunner(args)

    def tearDown(self):
        self._agent.stop()

    def _start(self):
        ''' Spawn the process and wait for the startup HELLO report. '''
        self._agent.start()
        self._wait_rptset()

    def _ari_text_to_obj(self, text: str, nn: bool=True) -> ARI:
        with io.StringIO(text) as buf:
            ari = ari_text.Decoder().decode(buf)

        nn_func = nickname.Converter(nickname.Mode.TO_NN, ADMS.db_session(), must_nickname=nn)
        ari = nn_func(ari)

        return ari

    def _ari_obj_to_cbor(self, ari:ARI) -> bytes:
        buf = io.BytesIO()
        ari_cbor.Encoder().encode(ari, buf)
        return buf.getvalue()

    def _ari_obj_from_cbor(self, databuf:bytes) -> ARI:
        with io.BytesIO(databuf) as buf:
            ari = ari_cbor.Decoder().decode(buf)
        return ari

    def _send_execset(self, text: str, nn: bool=True):
        ''' Send an EXECSET with a number of target ARIs. '''
        LOGGER.info('Sending value %s', text)
        data = self._ari_obj_to_cbor(self._ari_text_to_obj(text, nn))
        line = binascii.b2a_hex(data).decode('ascii')
        LOGGER.info('Sending line %s', line)
        self._agent.send_stdin(line + '\n')

    def _wait_rptset(self, timeout:float=5) -> ARI:
        ''' Wait for a RPTSET and decode it. '''
        line = self._agent.wait_for_text(HEXPAT, timeout=timeout).strip()
        LOGGER.info('Received line %s', line)
        data = binascii.a2b_hex(line)
        ari = self._ari_obj_from_cbor(data)

        if LOGGER.isEnabledFor(logging.INFO):
            textbuf = io.StringIO()
            ari_text.Encoder().encode(ari, textbuf)
            LOGGER.info('Received value: %s', textbuf.getvalue())

        return ari

    def test_start_terminate(self):
        self._start()
#        self._agent.wait_for_text(HEXPAT)

        LOGGER.info('Sending SIGINT')
        self._agent.proc.send_signal(signal.SIGINT)
        self.assertEqual(0, self._agent.proc.wait(timeout=5))
        self.assertEqual(0, self._agent.proc.returncode)

    def test_start_close(self):
        self._start()

        LOGGER.info('Closing stdin')
        self._agent.proc.stdin.close()
        self.assertEqual(0, self._agent.proc.wait(timeout=5))
        self.assertEqual(0, self._agent.proc.returncode)

    def test_exec_inspect(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version))'
        )
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI('0.0.0')], rpt.items)

    def test_exec_report_on_valid(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/dtnma-agent/CONST/hello,%22file%3Astdio%22))'
        )

        # RPTSET for the generated report
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(None), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)
        # items of the report
        self.assertLessEqual(3, len(rpt.items))

        # RPTSET for the execution itself
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/report-on(//ietf/dtnma-agent/CONST/hello,%22file%3Astdio%22)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

    def test_exec_report_on_no_destination(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/dtnma-agent/CONST/hello))'
        )

        # RPTSET for the execution itself
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/report-on(//ietf/dtnma-agent/CONST/hello)'), rpt.source)
        # items of the report
        self.assertEqual([ari.UNDEFINED], rpt.items)

    def test_exec_delayed(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(/AC/(//ietf/dtnma-agent/CTRL/wait-for(/TD/PT1.5S),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)))'
        )

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/wait-for(/TD/PT1.5S)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI('0.0.0')], rpt.items)

    def test_exec_macro_success(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(/ac/('
            +'//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
            +'//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
            +'))'
        )

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_rptset(0.1)

    def test_exec_macro_failure_expand(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(/ac/('
            +'//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
            +'//ietf/dtnma-agent/CTRL/inspect(false),'  # invalid parameter
            +'//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
            +'))',
            nn=False
        )
        #
        # rptset = self._wait_rptset().value
        # self.assertIsInstance(rptset, ari.ReportSet)
        # self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        # self.assertEqual(1, len(rptset.reports))
        # rpt = rptset.reports[0]
        # self.assertIsInstance(rpt, ari.Report)
        # self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(false)'), rpt.source)
        # self.assertEqual([ari.UNDEFINED], rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_rptset(0.1)

    def test_exec_macro_failure_exec(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(/ac/('
            +'//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
            +'//ietf/dtnma-agent/CTRL/inspect(//!private/!odm/EDD/missing),'  # invalid target
            +'//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
            +'))',
            nn=False
        )

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//!private/!odm/EDD/missing)', nn=False), rpt.source)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_rptset(0.1)

    def test_odm(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf, 1, !test-model-1, -1))'
        )

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-odm(ietf, 1, !test-model-1, -1)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

        # FIXME: Add following test
        # ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-odm(//example/!test-model-1))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/odm-list))'
        )

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/odm-list)'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        # # ODM Rules
        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr,1,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false))', False)
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr,1,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr2,2,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false))', False)
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr2,2,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/AC/(false),/TD/999999,1,false))', False)
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/AC/(false),/TD/999999,1,false)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sbr-list(false)))')
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(6, rpt.items[0].value.size);

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false)))')
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(12, rpt.items[0].value.size);

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true))', False)
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr))', False)
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2))', False)
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false)))')
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(6, rpt.items[0].value.size);
        self.assertEqual(False, rpt.items[0].value[0][5].value);  # Confirm rule is disabled

    def test_edd_counters(self):
        self._start()

        # Baseline
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-started))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, int)
        self.assertEqual(1, rpt.items[0].value)  # 1 because it counts inspect call above

        # Count number of successful exec's
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-succeeded))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(1, rpt.items[0].value)

        # Count number of failed exec's
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-failed))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(0, rpt.items[0].value)

        # Sanity check expected count of tx failures
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-msg-tx-failed))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(0, rpt.items[0].value)

    def test_odm_var_const(self):
        self._start()

        LOGGER.setLevel(logging.INFO)

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf, 1, !test-model-1, -1))'
        )
        rptset = self._wait_rptset().value

        # Verify var-list is empty
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)

        # Add a variable
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,test-var,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),/AC/(1)))', False)
        rptset = self._wait_rptset().value

        # Verify VAR can be read back
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(2, rpt.items[0].value.size)

        # Obsolete the VAR
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-var(//ietf/!test-model-1/VAR/test-var))', False)
        rptset = self._wait_rptset().value

        # Verify var list is empty again
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)

        # Verify list of constants is empty
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)

        # Add a constant
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,test-const,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),/AC/(1)))', False)
        rptset = self._wait_rptset().value

        # Verify const is listed now
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(2, rpt.items[0].value.size)

        # Obsolete the CONST
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-const(//ietf/!test-model-1/CONST/test-const))', False)
        rptset = self._wait_rptset().value

        # Verify CONST is no longer listed
        self._send_execset('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')
        rptset = self._wait_rptset().value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)
