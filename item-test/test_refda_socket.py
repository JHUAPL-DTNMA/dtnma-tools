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
''' Test the local socket transport of the REFDA.
'''
import io
import logging
import os
import signal
import socket
import subprocess
import tempfile
from typing import List, Set
from urllib.parse import quote
import unittest
import cbor2
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers import CmdRunner, Timer, compose_args

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)

# ADM handling outside of tests
ADMS = AdmSet(cache_dir=False)
logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)
ADMS.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])


class TestRefdaSocket(unittest.TestCase):
    ''' Verify whole-agent behavior with the refda-socket '''

    @classmethod
    def setUpClass(cls) -> None:
        logging.getLogger('sqlalchemy.engine').setLevel(logging.WARNING)
        logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)

    def setUp(self) -> None:
        self._tmp = tempfile.TemporaryDirectory()
        self._agent_sock_path = os.path.join(self._tmp.name, 'agent.sock')

        def bound_sock(name):
            sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

            path = os.path.join(self._tmp.name, name)
            LOGGER.info('binding to socket at %s', path)
            sock.bind(path)

            return {'path': path, 'sock': sock}

        self._mgr_bind = [
            bound_sock(f'mgr{index}.sock')
            for index in range(3)
        ]

        args = compose_args([
            'refda-socket',
            '-l', 'debug',
            '-a', self._agent_sock_path,
            '-m', self._mgr_bind[0]['path']
        ])
        self._agent = CmdRunner(args)

    def tearDown(self) -> None:
        agent_exit = self._agent.stop()
        self._agent = None

        for bind in self._mgr_bind:
            sock = bind['sock']
            sock.close()
        self._mgr_bind = None

        self._agent_sock_path = None
        self._tmp = None

        # assert after all other shutdown
        self.assertEqual(0, agent_exit)

    def _start(self) -> None:
        ''' Spawn the process and wait for the startup HELLO report. '''
        self._agent.start()

        delay = 0.1
        timer = Timer(10)
        while timer:
            # linear back-off
            timer.sleep(delay)
            delay += 0.1

            sock_ready = os.path.exists(self._agent_sock_path)

            if sock_ready:
                timer.finish()
                break

            if not sock_ready:
                LOGGER.info('waiting for agent socket at %s', self._agent_sock_path)

        # Initial HELLO
        msg_vals = self._wait_msg(mgr_ix=0, timeout=5)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(None), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)

    def _ari_text_to_obj(self, text: str, nn: bool = True) -> ARI:
        with io.StringIO(text) as buf:
            val = ari_text.Decoder().decode(buf)

        nn_func = nickname.Converter(nickname.Mode.TO_NN, ADMS.db_session(), must_nickname=nn)
        val = nn_func(val)

        return val

    def _ari_obj_to_text(self, val: ARI) -> str:
        buf = io.StringIO()
        ari_text.Encoder().encode(val, buf)
        return buf.getvalue()

    def _ari_obj_to_cbor(self, val: ARI) -> bytes:
        buf = io.BytesIO()
        ari_cbor.Encoder().encode(val, buf)
        return buf.getvalue()

    def _ari_obj_from_cbor(self, databuf: bytes) -> ARI:
        with io.BytesIO(databuf) as buf:
            val = ari_cbor.Decoder().decode(buf)
        return val

    def _send_msg(self, values: List[ARI], mgr_ix: int = 0) -> str:
        ''' Send an AMP message with EXECSET values.

        :param values: The ARI items to send.
        :param mgr_ix: The agent index to send from.
        :return: The socket path from which it was sent.
        '''
        data = cbor2.dumps(1)
        for val in values:
            LOGGER.info('Sending value %s', self._ari_obj_to_text(val))
            data += self._ari_obj_to_cbor(val)
        addr = self._agent_sock_path
        bind = self._mgr_bind[mgr_ix]
        LOGGER.info('Sending message %s from %s to %s', data.hex(), bind['path'], addr)
        bind['sock'].sendto(data, addr)
        return bind['path']

    def _wait_msg(self, mgr_ix: int, timeout: float=1) -> List[ARI]:
        ''' Wait for an AMP message with RPTSET values and decode it.

        :param agent_ix: The agent index to receive on.
        :param timeout: The time to wait in seconds.
        :return: The contained ARIs in decoded form.
        :raise TimeoutError: If not received in time.
        '''
        bind = self._mgr_bind[mgr_ix]
        recv_sock = bind['sock']

        recv_sock.settimeout(timeout)

        try:
            (data, addr) = recv_sock.recvfrom(10240)
        except (socket.timeout, TimeoutError) as err:
            raise TimeoutError("No message received") from err
        LOGGER.info('Received message %s to %s from %s', data.hex(), bind['path'], addr)
        self.assertEqual(self._agent_sock_path, addr)

        values = []
        dec = ari_cbor.Decoder()
        with io.BytesIO(data) as infile:
            vers = cbor2.load(infile)
            self.assertEqual(1, vers, msg='Invalid AMP version')

            while infile.tell() < len(data):
                val = dec.decode(infile)
                LOGGER.info('Received value %s', self._ari_obj_to_text(val))
                self.assertIsInstance(val.value, ari.ReportSet)
                values.append(val)

        return values

    def test_start_sigint(self):
        self._start()

        LOGGER.info('Sending SIGINT')
        self._agent.proc.send_signal(signal.SIGINT)
        self.assertEqual(0, self._agent.proc.wait(timeout=5))
        self.assertEqual(0, self._agent.proc.returncode)

    def test_start_sigterm(self):
        self._start()

        LOGGER.info('Sending SIGTERM')
        self._agent.proc.send_signal(signal.SIGTERM)
        self.assertEqual(0, self._agent.proc.wait(timeout=5))
        self.assertEqual(0, self._agent.proc.returncode)

    def test_exec_inspect(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version))')],
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
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

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/dtnma-agent/CONST/hello,%22file%3Astdio%22))')]
        )

        # RPTSET for the generated report
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(None), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)
        # items of the report
        self.assertLessEqual(3, len(rpt.items))

        # RPTSET for the execution itself
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
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

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/dtnma-agent/CONST/hello))')]
        )

        # RPTSET for the execution itself
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
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

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(/AC/('
                                   + '//ietf/dtnma-agent/CTRL/wait-for(/TD/PT1.5S),'
                                   + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)))')]
        )

        msg_vals = self._wait_msg(mgr_ix=0, timeout=3)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/wait-for(/TD/PT1.5S)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
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

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
                + '))'
            )]
        )

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_msg(mgr_ix=0, timeout=0.1)

    def test_exec_macro_failure_expand(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
                + '//ietf/dtnma-agent/CTRL/inspect(false),'  # invalid parameter
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
                + '))',
                nn=False
            )]
        )

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(false)'), rpt.source)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_msg(mgr_ix=0, timeout=0.1)

    def test_exec_macro_failure_exec(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
                + '//ietf/dtnma-agent/CTRL/inspect(//!private/!odm/EDD/missing),'  # invalid target
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
                + '))',
                nn=False
            )]
        )

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(ari.LiteralARI(123), rptset.nonce)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//!private/!odm/EDD/missing)', nn=False), rpt.source)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_msg(mgr_ix=0, timeout=0.1)

    def test_odm(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf, 1, !test-model-1, -1))')]
        )

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-odm(ietf, 1, !test-model-1, -1)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

        # FIXME: Add following test
        # ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-odm(//example/!test-model-1))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/odm-list))')]
        )

        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/odm-list)'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        # ODM Rules
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr,1,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj(
            '//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr,1,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr2,2,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj(
            '//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr2,2,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/AC/(false),/TD/999999,1,false))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj(
            '//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/AC/(false),/TD/999999,1,false)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sbr-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(6, rpt.items[0].value.size)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(12, rpt.items[0].value.size)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2)', False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(6, rpt.items[0].value.size)
        self.assertEqual(False, rpt.items[0].value[0][5].value)  # Confirm rule is disabled

    def test_edd_counters(self):
        self._start()

        # Baseline
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-started))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, int)
        self.assertLessEqual(1, rpt.items[0].value)  # 1 because it counts inspect call above

        # Count number of successful exec's
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-succeeded))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(1, rpt.items[0].value)

        # Count number of failed exec's
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-failed))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(0, rpt.items[0].value)

        # Sanity check expected count of tx failures
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-msg-tx-failed))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertEqual(0, rpt.items[0].value)

    def test_odm_var_const(self):
        self._start()

        LOGGER.setLevel(logging.INFO)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf, 1, !test-model-1, -1))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value

        # Verify var-list is empty
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)

        # Add a variable
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,test-var,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),/AC/(1)))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value

        # Verify VAR can be read back
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(2, rpt.items[0].value.size)

        # Obsolete the VAR
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-var(//ietf/!test-model-1/VAR/test-var))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value

        # Verify var list is empty again
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)

        # Verify list of constants is empty
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)

        # Add a constant
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,test-const,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),/AC/(1)))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value

        # Verify const is listed now
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(2, rpt.items[0].value.size)

        # Obsolete the CONST
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-const(//ietf/!test-model-1/CONST/test-const))', False)]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value

        # Verify CONST is no longer listed
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, ari.Table)
        self.assertEqual(0, rpt.items[0].value.size)
