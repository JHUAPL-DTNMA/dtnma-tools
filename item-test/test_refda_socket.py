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
import numpy
import os
import signal
import socket
import subprocess
import sys
import tempfile
from typing import List, Optional, Type
import urllib.parse
import unittest
import cbor2
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers import CmdRunner, Timer, compose_args
import numpy

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)
logging.getLogger('ace').setLevel(logging.ERROR)

# ADM handling outside of tests
ADMS = AdmSet(cache_dir=False)
ADMS.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])


def quote(text: str)->str:
    ''' URL-encode all non-unreserved characters. '''
    return urllib.parse.quote(text, safe="")


def literal_prim_type(item: ARI) -> Optional[Type]:
    ''' Get a single primitive type to use for test assertions '''
    if isinstance(item, ari.LiteralARI):
        return type(item.value)
    else:
        return None


def literal_prim_types(items: list[ARI]) -> list[Optional[Type]]:
    ''' Get a list of literal primitive types to use for test assertions '''
    return list(map(literal_prim_type, items))


class TestRefdaSocket(unittest.TestCase):
    ''' Verify whole-agent behavior with the refda-socket '''
    maxDiff = None

    def setUp(self) -> None:
        tmp_kwargs = {}
        if sys.version_info >= (3, 12):
            tmp_kwargs['delete'] = os.environ.get('TEST_DIR_KEEP') is None
        self._tmp = tempfile.TemporaryDirectory(**tmp_kwargs)
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

        startup_path = os.path.join(self._tmp.name, 'startup.uri')
        with open(startup_path, 'w') as startup_file:
            startup_file.writelines([
                '//ietf/dtnma-agent-acl/ctrl/ensure-group(1,test-agents)\n',
                '//ietf/dtnma-agent-acl/ctrl/ensure-group-members(1,/ac/(//ietf/network-base/ident/uri-regexp-pattern(%22file%3A.%2A%22)))\n',
                '//ietf/dtnma-agent-acl/CTRL/ensure-access(1,/ac/(1),h\'0102\',/ac/('
                + '//ietf/dtnma-agent-acl/ident/execute,'
                + '//ietf/dtnma-agent-acl/ident/produce'
                + '))\n',
            ])

        args = compose_args([
            'refda-socket',
            '-l', os.environ.get('TEST_LOG_LEVEL', 'debug'),
            '-s', startup_path,
            '-a', ('file:' + self._agent_sock_path),
            '-m', ('file:' + self._mgr_bind[0]['path']),
        ])
        self._agent = CmdRunner(args)

        # buffer of received RPTSET pending _wait_reports
        self._rptsets = {}

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
        ''' Spawn the process and wait for the startup report. '''
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
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), timeout=5)
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)

    def _ari_text_to_obj(self, text: str, id_convert: bool=True, nn: bool=True) -> ARI:
        with io.StringIO(text) as buf:
            val = ari_text.Decoder().decode(buf)

        if id_convert:
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

    def _ari_strip_params(self, val: ARI) -> ARI:
        ''' Remove any parameters from a reference value. '''
        if isinstance(val, ari.ReferenceARI):
            val.params = None
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

        :param mgr_ix: The manager index to receive on.
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

    def _wait_reports(self, mgr_ix: int, nonce: Optional[ARI], timeout: float=1, stop_count: int=1) -> List[ari.Report]:
        ''' Wait for all RPTSETs within a time window and unwrap all contained
        reports.

        :param mgr_ix: The manager index to receive on.
        :param nonce: The nonce value to match for, or None to match all.
        :param timeout: The total time to wait in seconds.
        :param stop_count: If positive, the waiting will stop upon receiving
            at least this many matching reports.
        :return: All contained Report instances in the order in which
            they were received.
        :raise TimeoutError: If not received in time.
        '''
        reports = []
        self._rptsets.setdefault(mgr_ix, [])

        timer = Timer(timeout)
        try:
            while timer:
                self._rptsets[mgr_ix] += self._wait_msg(mgr_ix=mgr_ix, timeout=timer.remaining())

                remain = []
                for val in self._rptsets[mgr_ix]:
                    if nonce is None or val.value.nonce == nonce:
                        reports += val.value.reports
                    else:
                        remain.append(val)
                self._rptsets[mgr_ix] = remain

                if stop_count > 0 and len(reports) >= stop_count:
                    break
        except TimeoutError:
            # simply stop listening
            pass

        return reports

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
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts[0]
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI('0.0.0')], rpt.items)

    def test_exec_report_on_multi_mgr(self):
        self._start()

        mgr_eids = [
            quote('"file:' + self._mgr_bind[0]['path'] + '"'),
            quote('"file:' + self._mgr_bind[1]['path'] + '"'),
        ]
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/dtnma-agent/CONST/hello,/ac/(' + ','.join(mgr_eids) + ')))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None))
        self.assertEqual(1, len(rpts))
        # RPTSET for the generated report
        rpt = rpts[0]
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)
        # items of the report
        self.assertLessEqual(3, len(rpt.items))

        rpts = self._wait_reports(mgr_ix=1, nonce=ari.LiteralARI(None))
        self.assertEqual(1, len(rpts))
        # RPTSET for the generated report
        rpt = rpts[0]
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)
        # items of the report
        self.assertLessEqual(3, len(rpt.items))

        # RPTSET for the execution itself
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts[0]
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/report-on'), self._ari_strip_params(rpt.source))
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

    def test_exec_report_on_no_destination(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/dtnma-agent/CONST/hello))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None))
        self.assertEqual(1, len(rpts))
        # RPTSET for the generated report
        rpt = rpts[0]
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/const/hello'), rpt.source)
        # items of the report
        self.assertLessEqual(3, len(rpt.items))

        # RPTSET for the execution itself
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts[0]
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/report-on(//ietf/dtnma-agent/CONST/hello)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

    def test_exec_report_on_variations(self):
        self._start()

        # macro to configure preconditions:
        #  ../!odm/const/rptt-two contains RPTT with two counter items
        #  ../!odm/const/expr contains EXPR resolving to one counter
        #  ../!odm/var/rptt-sum contains EXPR resolving to one counter
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!odm,-1),'
                + '//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!odm,rptt-two,1,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/rptt),/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed)),'
                + '//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!odm,expr-sum,2,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/expr),/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed,//ietf/dtnma-agent/oper/add)),'
                + '//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!odm,rptt-sum,3,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/rptt),/ac/(//ietf/!odm/const/expr-sum))'
                + '))',
                nn=False
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=4)
        self.assertEqual(4, len(rpts))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=null;('
                # inline RPTT, contains undefined template item
                + '//ietf/dtnma-agent/CTRL/report-on(/ac/(undefined,//ietf/dtnma-agent/EDD/num-msg-rx-failed)),'
                # inline RPTT, two valid references
                + '//ietf/dtnma-agent/CTRL/report-on(/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed)),'
                # produced RPTT with two valid references
                + '//ietf/dtnma-agent/CTRL/report-on(//ietf/!odm/const/rptt-two),'
                # inline RPTT with a literal reference (not evaluated)
                + '//ietf/dtnma-agent/CTRL/report-on(/ac/(//ietf/!odm/const/expr-sum)),'
                # inline RPTT with an expression (containing sub-expression reference)
                + '//ietf/dtnma-agent/CTRL/report-on(/ac/(/ac/(//ietf/!odm/const/expr-sum))),'
                # produced RPTT with an expression
                + '//ietf/dtnma-agent/CTRL/report-on(//ietf/!odm/var/rptt-sum)'
                + ')',
                nn=False
            )]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), stop_count=6)
        self.assertEqual(6, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('/ac/(undefined,//ietf/dtnma-agent/EDD/num-msg-rx-failed)'),
            rpt.source
        )
        self.assertEqual([ari.UndefinedPrimitiveType, int], literal_prim_types(rpt.items))

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed)'),
            rpt.source
        )
        self.assertEqual([int, int], literal_prim_types(rpt.items))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/!odm/const/rptt-two', nn=False), rpt.source)
        self.assertEqual([int, int], literal_prim_types(rpt.items))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/!odm/const/expr-sum)', nn=False), rpt.source)
        # the produced AC directly
        self.assertEqual([list], literal_prim_types(rpt.items))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('/ac/(/ac/(//ietf/!odm/const/expr-sum))', nn=False), rpt.source)
        # the evaluated result
        self.assertEqual([int], literal_prim_types(rpt.items))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/!odm/var/rptt-sum', nn=False), rpt.source)
        self.assertEqual([int], literal_prim_types(rpt.items))

    def test_exec_delayed(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(/AC/('
                                   + '//ietf/dtnma-agent/CTRL/wait-for(/TD/PT1.5S),'
                                   + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2, timeout=5)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/wait-for(/TD/PT1.5S)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
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

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rpt = rpts.pop(0)
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

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
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

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rpt = rpts.pop(0)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//!private/!odm/EDD/missing)', nn=False), rpt.source)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_msg(mgr_ix=0, timeout=0.1)

    def test_exec_introspection(self):
        self._start()

        # two separate sequences with different nonces
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/edd/exec-running))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 3), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, rpt.items[0].value.flat)
        pid_col = list(rpt.items[0].value[:, 0])
        self.assertListEqual([ari.typed_uvast(3)], pid_col)  # PID 1 and 2 is startup

        # two separate targets
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/edd/exec-running),'
                + '//ietf/dtnma-agent/CTRL/wait-for(/TD/PT0.1S)'  # do nothing, but add to table
                + ')'
            )]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 3), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, rpt.items[0].value.flat)
        pid_col = list(rpt.items[0].value[:, 0])
        self.assertListEqual([ari.typed_uvast(4), ari.typed_uvast(5)], pid_col)

    def test_odm(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-odm'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

        # FIXME: Add following test
        # ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-odm(//example/!test-model-1))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/odm-list))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/odm-list)'), rpt.source)
        # items of the report
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))

        # ODM Rules
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr,1,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-tbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([int], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr2,2,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/1,1,false))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-tbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([int], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/AC/(false),/TD/999999,1,false))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-sbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([int], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sbr-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 7), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 7), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true)', nn=False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr)', nn=False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2)', nn=False), rpt.source)
        # items of the report
        self.assertEqual(1, len(rpt.items))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/tbr-list(false))'), rpt.source)
        # items of the report
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 7), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))
        self.assertEqual(False, rpt.items[0].value[0, 5].value)  # Confirm rule is disabled

    def test_edd_counters(self):
        self._start()

        # Baseline
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-started))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([int], literal_prim_types(rpt.items))
        self.assertLessEqual(1, rpt.items[0].value)  # 4 from startup macro, 1 from <inspect> above

        # Count number of successful exec's
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-succeeded))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([int], literal_prim_types(rpt.items))
        self.assertLessEqual(1, rpt.items[0].value)

        # Count number of failed exec's
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-exec-failed))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([int], literal_prim_types(rpt.items))
        self.assertEqual(0, rpt.items[0].value)

        # Sanity check expected count of tx failures
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/num-msg-tx-failed))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([int], literal_prim_types(rpt.items))
        self.assertEqual(0, rpt.items[0].value)

        # Send EXECSET request to get last message receive time
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/last-msg-rx-time))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual([numpy.datetime64], literal_prim_types(rpt.items))

    def test_odm_var_const(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify var-list is empty
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))

        # Add a variable
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,test-var,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),1))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify VAR can be read back
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))
        self.assertEqual(
            self._ari_text_to_obj('//1/-1/var/1', nn=False),
            rpt.items[0].value[0, 0]
        )
        self.assertEqual(
            self._ari_text_to_obj('//ietf/amm-semtype/IDENT/type-use(name=/ARITYPE/int)', id_convert=False),
            rpt.items[0].value[0, 1]
        )

        # Obsolete the VAR
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-var(//ietf/!test-model-1/VAR/test-var))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))

        # Verify var list is empty again
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)

    def test_odm_var_invalid(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # attempt to create with an invalid type
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/-100),/AC/(//ietf/dtnma-agent/EDD/num-msg-rx)))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # attempt to create with an invalid value
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/RPTT),/AC/(10)))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # verify failures are not listed
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/var-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))

    def test_odm_const(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify list of constants is empty
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))

        # Add a constant
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,test-const,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),1))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify const is listed now
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))
        self.assertEqual(
            self._ari_text_to_obj('//1/-1/const/1', nn=False),
            rpt.items[0].value[0, 0]
        )
        self.assertEqual(
            self._ari_text_to_obj('//ietf/amm-semtype/IDENT/type-use(name=/ARITYPE/int)', id_convert=False),
            rpt.items[0].value[0, 1]
        )

        # Obsolete the CONST
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-const(//ietf/!test-model-1/CONST/test-const))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify CONST is no longer listed
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)

    def test_agent_control_flow_ctrls(self):
        self._start()

        LOGGER.setLevel(logging.INFO)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/if-then-else(/AC/(true), null, null))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        self.assertEqual(1, len(msg_vals))
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, bool)
        self.assertEqual(True, rpt.items[0].value)

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/if-then-else(/AC/(true), //ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version), null))')]
        )
        msg_vals = self._wait_msg(mgr_ix=0)
        rptset = msg_vals[0].value
        rpt = rptset.reports[0]
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, bool)
        self.assertEqual(True, rpt.items[0].value)

    def test_odm_const_invalid(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # attempt to create with an invalid type
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/-100),/AC/(//ietf/dtnma-agent/EDD/num-msg-rx)))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # attempt to create with an invalid value
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/RPTT),/AC/(10)))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.UNDEFINED], rpt.items)

        # verify failures are not listed
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/const-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))

    def test_odm_const_rptt(self):
        # Define a report template and report on it
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,rptt-num-msg-rx,4,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/RPTT),/AC/(//ietf/dtnma-agent/EDD/num-msg-rx)))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/!test-model-1/const/rptt-num-msg-rx))', nn=False)]
        )
        # the generated report
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(1, len(rpt.items))
        self.assertIsInstance(rpt.items[0].value, int)

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

    def test_acl(self):
        self._start()

        # Initial default state
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/current-groups),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/group-list),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'
                + ')'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=3)
        self.assertEqual(3, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/current-groups)'), rpt.source)
        self.assertEqual(1, len(rpt.items))
        self.assertEqual([ari.typed_uint(1)], rpt.items[0].value)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/group-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 5), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, rpt.items[0].value.flat)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 6), rpt.items[0].value.shape)
        # FIXME self.assertNotIn(ari.UNDEFINED, rpt.items[0].value.flat)

        # Add a group with catch-all on the test socket scheme
        pat = quote('"file:.*"')
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent-acl/CTRL/ensure-group(10,example),'
                + '//ietf/dtnma-agent-acl/CTRL/ensure-group(10,example),'  # duplicate no-op
                + '//ietf/dtnma-agent-acl/ctrl/ensure-group-members(10,/ac/(//ietf/network-base/ident/uri-regexp-pattern(' + pat + '))),'
                + '//ietf/dtnma-agent-acl/CTRL/ensure-access(10,/ac/(10),h\'0102\',/ac/('
                + '//ietf/dtnma-agent-acl/ident/execute,'
                + '//ietf/dtnma-agent-acl/ident/produce'
                + ')),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/current-groups),'  # no effect on this execution
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/group-list),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'
                + '))'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=7)
        self.assertEqual(7, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent-acl/CTRL/ensure-group'), self._ari_strip_params(rpt.source))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent-acl/CTRL/ensure-group'), self._ari_strip_params(rpt.source))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('//ietf/dtnma-agent-acl/ctrl/ensure-group-members'),
            self._ari_strip_params(rpt.source)
        )
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent-acl/CTRL/ensure-access'), self._ari_strip_params(rpt.source))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/current-groups)'), rpt.source)
        self.assertEqual(1, len(rpt.items))
        self.assertEqual([ari.typed_uint(1), ari.typed_uint(10)], rpt.items[0].value)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/group-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 5), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, rpt.items[0].value.flat)
        # new group number 10
        self.assertEqual([ari.typed_uint(1), ari.typed_uint(10)], rpt.items[0].value[:, 0])
        self.assertEqual(ari.LiteralARI("example"), rpt.items[0].value[1, 1])

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 6), rpt.items[0].value.shape)
        # new access number 10
        self.assertEqual([ari.typed_uint(1), ari.typed_uint(10)], rpt.items[0].value[:, 0])

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent-acl/CTRL/discard-group(10),'
                + '//ietf/dtnma-agent-acl/CTRL/discard-access(10),'
                + '//ietf/dtnma-agent-acl/CTRL/discard-group(10),'  # duplicate no-op
                + '//ietf/dtnma-agent-acl/CTRL/discard-access(10),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/current-groups),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/group-list),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'
                + '))'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=7)
        self.assertEqual(7, len(rpts))
        rpts = rpts[4:]

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/current-groups)'), rpt.source)
        self.assertEqual(1, len(rpt.items))
        # group number 10
        self.assertEqual([ari.typed_uint(1)], rpt.items[0].value)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/group-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 5), rpt.items[0].value.shape)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 6), rpt.items[0].value.shape)

        # failure adding non-existing permission
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent-acl/CTRL/ensure-access(1,/ac/(10),h\'0102\',/ac/('
                + '//ietf/dtnma-agent-acl/ident/other'
                + '))'
                + ')',
                nn=False
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=1)
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent-acl/CTRL/ensure-access'), self._ari_strip_params(rpt.source))
        self.assertEqual([ari.UNDEFINED], rpt.items)
