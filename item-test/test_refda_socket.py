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
''' Test the local socket transport of the REFDA.
'''
from dataclasses import dataclass, field
import io
import logging
import numpy
import os
import queue
import select
import signal
import socket
import subprocess
import sys
import tempfile
import threading
from typing import Dict, List, Optional, Type
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

@dataclass
class BindInstance:
    path: str
    ''' Filesystem path of the bind '''
    sock: socket.socket
    ''' Local end of the bind '''

    recv: Dict[ari.LiteralARI, List[ari.Report]]
    ''' Queue of decoded report values '''
    recv_lock: threading.Lock
    ''' Synchronization for #recv '''
    recv_avail: threading.Condition

class TestRefdaSocket(unittest.TestCase):
    ''' Verify whole-agent behavior with the refda-socket '''
    maxDiff = None

    def setUp(self) -> None:
        tmp_kwargs = {}
        if sys.version_info >= (3, 12):
            tmp_kwargs['delete'] = os.environ.get('TEST_DIR_KEEP') is None
        self._tmp = tempfile.TemporaryDirectory(**tmp_kwargs)
        self._agent_sock_path = os.path.join(self._tmp.name, 'agent.sock')

        def bound_sock(name) -> BindInstance:
            sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

            path = os.path.join(self._tmp.name, name)
            LOGGER.info('binding to socket at %s', path)
            sock.bind(path)

            lock = threading.Lock()
            cond = threading.Condition(lock)
            return BindInstance(path=path, sock=sock, recv={}, recv_lock=lock, recv_avail=cond)

        self._mgr_bind: List[BindInstance] = [
            bound_sock(f'mgr{index}.sock')
            for index in range(3)
        ]
        # listen in work thread
        self._mgr_reader = threading.Thread(
            target=self._read_mgr,
            args=[self._mgr_bind],
            daemon=True
        )
        self._mgr_reader.start()

        startup_path = os.path.join(self._tmp.name, 'startup.uri')
        with open(startup_path, 'w') as startup_file:
            startup_file.writelines([
                '//ietf/dtnma-agent-acl/ctrl/ensure-group(1,test-agents)\n',
                '//ietf/dtnma-agent-acl/ctrl/ensure-group-members(1,/ac/(/label/0,//ietf/dtnma-agent/oper/match-regexp(%22file%3A.%2A%22)))\n',
                # group 0 (agent) and 1 (all test mgrs) have all access
                '//ietf/dtnma-agent-acl/CTRL/ensure-access(1,/ac/(0,1),/ac/(/objpat/(*)(*)(*)(*)),/ac/('
                + '//ietf/dtnma-agent-acl/ident/execute,'
                + '//ietf/dtnma-agent-acl/ident/produce'
                + '))\n',
            ])

        args = compose_args([
            'refda-socket',
            '-l', os.environ.get('TEST_LOG_LEVEL', 'debug'),
            '-s', startup_path,
            '-a', ('file:' + self._agent_sock_path),
            '-m', ('file:' + self._mgr_bind[0].path),
        ])
        self._agent = CmdRunner(args)

    def tearDown(self) -> None:
        agent_exit = self._agent.stop()
        self._agent = None

        for bind in self._mgr_bind:
            bind.sock.shutdown(socket.SHUT_RDWR)
            bind.sock.close()
        self._mgr_reader.join()
        self._mgr_reader = None
        self._mgr_bind = None

        self._agent_sock_path = None
        self._tmp = None

        # assert after all other shutdown
        self.assertEqual(0, agent_exit)

    def _read_mgr(self, mgr_bind: List[BindInstance]):
        LOGGER.debug('Starting reader thread')

        lookup: Dict[socket.socket, BindInstance] = {}
        poller = select.poll()
        for bind in mgr_bind:
            fd = bind.sock.fileno()
            lookup[fd] = bind
            poller.register(fd, select.POLLIN | select.POLLERR | select.POLLHUP | select.POLLNVAL)

        def recv(bind: BindInstance):
            try:
                (data, addr) = bind.sock.recvfrom(10240)
            except OSError:
                data = None
            if not data:
                return
            LOGGER.info('Received message %s to %s from %s', data.hex(), bind.path, addr)

            values: List[ari.ReportSet] = []
            dec = ari_cbor.Decoder()
            with io.BytesIO(data) as infile:
                vers = cbor2.load(infile)
                if vers != 1:
                    LOGGER.error('Invalid AMP version %s', vers)
                    return
    
                while infile.tell() < len(data):
                    val = dec.decode(infile)
                    LOGGER.info('Received value %s', self._ari_obj_to_text(val))
                    if not isinstance(val.value, ari.ReportSet):
                        LOGGER.warning('Received non-RPTSET item, ignoring it')
                        continue
                    values.append(val)

            with bind.recv_lock:
                for rpt in values:
                    reports = bind.recv.setdefault(val.value.nonce, [])
                    LOGGER.debug('Pushing %d new reports', len(val.value.reports))
                    reports += list(val.value.reports)
                bind.recv_avail.notify_all()

        running = True
        while running:
            for fd, event in poller.poll():
                if event & select.POLLIN:
                    bind = lookup[fd]
                    recv(bind)
                else:
                    running = False

        LOGGER.debug('Stopping reader thread')

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
            val = ari.ReferenceARI(ident=val.ident)
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
        LOGGER.info('Sending message %s from %s to %s', data.hex(), bind.path, addr)
        bind.sock.sendto(data, addr)
        return bind.path

    def _wait_reports(self, mgr_ix: int, nonce: Optional[ARI]=None, timeout: float=1, stop_count: int=1) -> List[ari.Report]:
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
        reports: List[ari.Report] = []
        bind = self._mgr_bind[mgr_ix]

        timer = Timer(timeout)
        while timer:
            LOGGER.debug('waiting up to %s for %d reports, have %d already', timer.remaining(), stop_count, len(reports))
            with bind.recv_lock:
                try:
                    if nonce is None:
                        # any reports
                        if not bind.recv and timer:
                            bind.recv_avail.wait(timeout=timer.remaining())
                        for _nonce, rpts in bind.recv.items():
                            reports += rpts
                        bind.recv.clear()
                    else:
                        # only for a single nonce
                        while nonce not in bind.recv and timer:
                            bind.recv_avail.wait(timeout=timer.remaining())
                        rpts = bind.recv.pop(nonce, [])
                        reports += rpts
                except (RuntimeError, TimeoutError):
                    # simply stop listening
                    pass

            if stop_count > 0 and len(reports) >= stop_count:
                break

        LOGGER.debug('stopping with %d reports', len(reports))
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
        self.assertEqual((ari.LiteralARI('0.0.0'),), rpt.items)

    def test_exec_report_on_multi_mgr(self):
        self._start()

        mgr_eids = [
            quote('"file:' + self._mgr_bind[0].path + '"'),
            quote('"file:' + self._mgr_bind[1].path + '"'),
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
        self.assertEqual((ari.LiteralARI(None),), rpt.items)

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
        self.assertEqual((ari.LiteralARI(None),), rpt.items)

    def test_exec_report_on_variations(self):
        self._start()

        # macro to configure preconditions:
        #  ../!odm/const/rptt-two contains RPTT with two counter items
        #  ../!odm/const/expr-sum contains EXPR evaluating to one counter
        #  ../!odm/var/rptt-sum-inline contains same EXPR evaluating to one counter
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!odm,-1),'
                + '//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!odm,rptt-two,1,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/rptt),/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed)),'
                + '//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!odm,expr-sum,2,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/expr),/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed,//ietf/dtnma-agent/oper/add)),'
                + '//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!odm,rptt-sum-inline,3,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/rptt),/ac/(/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed,//ietf/dtnma-agent/oper/add)))'
                + '))'
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
                + '//ietf/dtnma-agent/CTRL/report-on(//ietf/!odm/var/rptt-sum-inline)'
                + ')'
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
        self.assertEqual(self._ari_text_to_obj('//ietf/!odm/const/rptt-two'), rpt.source)
        self.assertEqual([int, int], literal_prim_types(rpt.items))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/!odm/const/expr-sum)'), rpt.source)
        # the produced AC directly
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        self.assertEqual(
            self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed,//ietf/dtnma-agent/oper/add)'),
            rpt.items[0]
        )

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('/ac/(/ac/(//ietf/!odm/const/expr-sum))'), rpt.source)
        # the evaluated expression with no operations
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        self.assertEqual(
            self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed,//ietf/dtnma-agent/oper/add)'),
            rpt.items[0]
        )

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/!odm/var/rptt-sum-inline'), rpt.source)
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
        self.assertEqual((ari.LiteralARI(None),), rpt.items)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertEqual((ari.LiteralARI('0.0.0'),), rpt.items)

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
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'), rpt.source)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_reports(mgr_ix=0, timeout=0.1)

    def test_exec_macro_failure_expand(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
                + '//ietf/dtnma-agent/CTRL/inspect(false),'  # invalid parameter
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'
                + '))'
            )]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(false)'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_reports(mgr_ix=0, timeout=0.1)

    def test_exec_macro_failure_exec(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),'
                + '//ietf/dtnma-agent/CTRL/inspect(//!private/!odm/EDD/missing),'  # invalid target
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
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//!private/!odm/EDD/missing)'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

        # no other RPTSET
        with self.assertRaises(TimeoutError):
            self._wait_reports(mgr_ix=0, timeout=0.1)

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

    def test_prod_variations(self):
        self._start()

        # macro to configure preconditions:
        #  ../!odm/const/expr-with-labels parameterized, contains an EXPR value with LABEL items
        #  ../!odm/var/rptt-with-label parameterized, contains a RPTT value with LABEL items
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(/ac/('
                + '//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!odm,-1),'
                + '//ietf/dtnma-agent/CTRL/ensure-const(' + (
                    '//ietf/!odm,expr-with-labels,1,'  # identity
                    + '//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/expr),'  # type
                    + '/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,/label/hello)'  # value
                    '),')
                + '//ietf/dtnma-agent/CTRL/ensure-const(' + (
                    '//ietf/!odm,expr-with-params-int,2,'  # identity
                    + '//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/expr),'  # type
                    + '/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,/label/0),'  # value
                    + '/tbl/c=3;(hello,//ietf/amm-semtype/IDENT/type-use(/aritype/object),undefined)'  # params
                    '),')
                + '//ietf/dtnma-agent/CTRL/ensure-const(' + (
                    '//ietf/!odm,expr-with-params-tstr,3,'  # identity
                    + '//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/expr),'  # type
                    + '/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,/label/hello),'  # value
                    + '/tbl/c=3;(hello,//ietf/amm-semtype/IDENT/type-use(/aritype/object),undefined)'  # params
                    '),')
                + '//ietf/dtnma-agent/CTRL/ensure-var(' + (
                    '//ietf/!odm,rptt-sum,3,'
                    + '//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/rptt),'
                    + '/ac/(//ietf/!odm/const/expr-sum)'
                    ')')
                + '))'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=5)
        self.assertEqual(5, len(rpts))
        for rpt_ix, rpt in enumerate(rpts):
            LOGGER.info('Checking report #%d', rpt_ix)
            self.assertEqual(1, len(rpt.items))
            self.assertNotIn(ari.UNDEFINED, rpt.items, msg=f'Failed on {self._ari_obj_to_text(rpt.source)}')

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                # no parameters, no label substitution
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-labels),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-int),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-int(//ietf/dtnma-agent/EDD/num-msg-rx-failed)),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-tstr),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-tstr(//ietf/dtnma-agent/EDD/num-msg-rx-failed))'
                + ')'
            )]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=5)
        self.assertEqual(5, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-labels)'),
            rpt.source
        )
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        # no substitution
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,/label/hello)'), rpt.items[0])

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-int)'),
            rpt.source
        )
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        # processed, but no given parameter
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,undefined)'), rpt.items[0])

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-int(//ietf/dtnma-agent/EDD/num-msg-rx-failed))'),
            rpt.source
        )
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        # substitution
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed)'), rpt.items[0])

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-tstr)'),
            rpt.source
        )
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        # processed, but no given parameter
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,undefined)'), rpt.items[0])

        rpt = rpts.pop(0)
        self.assertEqual(
            self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/const/expr-with-params-tstr(//ietf/dtnma-agent/EDD/num-msg-rx-failed))'),
            rpt.source
        )
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        # substitution
        self.assertEqual(self._ari_text_to_obj('/ac/(//ietf/dtnma-agent/EDD/num-msg-rx,//ietf/dtnma-agent/EDD/num-msg-rx-failed)'), rpt.items[0])

    def test_eval_strict_compare(self):
        # cases are organized so that only the same row compares strict equal
        cases = [
            'undefined',
            'null',
            'false',
            'true',
            '-10',
            '0',
            '10',
            '0.0',
            '10.0',
            '/null/null',
            '/bool/false',
            '/bool/true',
            '/byte/0',
            '/byte/10',
            '/int/0',
            '/int/10',
            '/vast/0',
            '/vast/10',
        ]

        self._start()

        # pipelined expected reports from source to items
        expect_sources: Dict[ARI, List[ARI]] = dict()
        # check in both directions
        for left in cases:
            with self.subTest(left):
                exprs = []
                expect_items = []

                for right in cases:
                    exprs += [
                        ('/ac/(' + left + ',' + right + ',' + '//ietf/dtnma-agent/oper/strict-eq)'),
                        ('/ac/(' + left + ',' + right + ',' + '//ietf/dtnma-agent/oper/strict-ne)'),
                    ]
                    expect_items += [
                        ari.TYPED_TRUE if left == right else ari.TYPED_FALSE,
                        ari.TYPED_FALSE if left == right else ari.TYPED_TRUE,
                    ]

                execset = self._ari_text_to_obj('ari:/EXECSET/n=null;(//ietf/dtnma-agent/CTRL/report-on(/ac/(' + ','.join(exprs) + ')))')
                # rpt source will be the first parameter to the target ctrl
                source = execset.value.targets[0].params[0]
                expect_sources[source] = expect_items
                self._send_msg([execset])

        # check reports in arbitrary order until all expected are seen
        LOGGER.info('Waiting on %d reports', len(expect_sources))
        while expect_sources:
            rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), timeout=5)
            for rpt in rpts:
                self.assertEqual(rpt.source.type_id, ari.StructType.AC)
                if rpt.source not in expect_sources:
                    self.fail(f'Report with unexpected source {rpt.source}')

                # items of the report
                expect_items = expect_sources.pop(rpt.source)
                self.assertEqual(len(expect_items), len(rpt.items))
                for expr, expect, got in zip(exprs, expect_items, rpt.items):
                    self.assertEqual(expect, got, msg=f'Failed item for expr {expr}')
            
            if expect_sources and not rpts:
                self.fail(f'No reports received and still expecting {len(expect_sources)} sources')

    def test_eval_loose_compare_opers(self):
        # expected result columns in cases table
        oper_name_col = {
            'compare-eq': 2,
            'compare-ne': 3,
            'compare-lt': 4,
            'compare-le': 5,
            'compare-gt': 6,
            'compare-ge': 7,
        }
        cases = [
            # not comparable
            ('undefined', '10', *([ari.UNDEFINED] * 6)),
            ('10', 'undefined', *([ari.UNDEFINED] * 6)),
            ('10', 'hello', *([ari.UNDEFINED] * 6)),
            # integers
            ('-10', '10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('1', '10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('10', '10', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_TRUE),
            ('100', '10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('-10', '-10', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_TRUE),
            # mix in one float, same comparisons
            ('-10', '10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('1', '10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('10', '10.0', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_TRUE),
            ('100', '10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('-10', '-10.0', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_TRUE),
            # time points
            # ('10', '/tp/10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('/td/10', '/tp/10', *([ari.UNDEFINED] * 6)),

            # time diffs
            # ('10', '/td/10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_FALSE),

            ('/tp/10', '/td/10', *([ari.UNDEFINED] * 6)),
            ('/td/-10', '/td/10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('/td/1', '/td/10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('/td/10', '/td/10.0', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_TRUE),
            ('/td/100', '/td/10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('/td/-10', '/td/-10.0', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_TRUE),
        ]

        self._start()

        for oper_name, case_col in oper_name_col.items():
            with self.subTest(oper_name):
                exprs = [
                    '/ac/(' + row[0] + ',' + row[1] + ',' + '//ietf/dtnma-agent/oper/' + oper_name + ')'
                    for row in cases
                ]
                expect_items = [row[case_col] for row in cases]

                self._send_msg(
                    [self._ari_text_to_obj('ari:/EXECSET/n=null;(//ietf/dtnma-agent/CTRL/report-on(/ac/(' + ','.join(exprs) + ')))')]
                )
        
                rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), stop_count=1, timeout=5)
                self.assertEqual(1, len(rpts))
        
                rpt = rpts.pop(0)
                self.assertIsInstance(rpt.source, ari.LiteralARI)
                self.assertEqual(rpt.source.type_id, ari.StructType.AC)
                # items of the report
                self.assertEqual(len(expect_items), len(rpt.items))
                for expr, expect, got in zip(exprs, expect_items, rpt.items):
                    self.assertEqual(expect, got, msg=f'Failed item for expr {expr}')

    def test_eval_predicate_opers(self):
        # expected result columns in cases table
        oper_name_col = {
            'is-undefined': 1,
            'is-not-undefined': 2,
            'is-truthy': 3,
        }
        cases = [
            ('undefined', ari.TYPED_TRUE, ari.TYPED_FALSE, ari.TYPED_FALSE),
            ('null', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE),
            ('false', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE),
            ('true', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_FALSE),
            ('10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('-10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('-10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('/tp/10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('/tp/10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('/td/10', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('/td/10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
            ('/td/-10.0', ari.TYPED_FALSE, ari.TYPED_TRUE, ari.TYPED_TRUE),
        ]

        self._start()

        for oper_name, case_col in oper_name_col.items():
            with self.subTest(oper_name):
                exprs = [
                    '/ac/(' + row[0] + ',//ietf/dtnma-agent/oper/' + oper_name + ')'
                    for row in cases
                ]
                expect_items = [row[case_col] for row in cases]

                self._send_msg(
                    [self._ari_text_to_obj('ari:/EXECSET/n=null;(//ietf/dtnma-agent/CTRL/report-on(/ac/(' + ','.join(exprs) + ')))')]
                )
        
                rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), stop_count=1, timeout=5)
                self.assertEqual(1, len(rpts))
        
                rpt = rpts.pop(0)
                self.assertIsInstance(rpt.source, ari.LiteralARI)
                self.assertEqual(rpt.source.type_id, ari.StructType.AC)
                # items of the report
                self.assertEqual(len(expect_items), len(rpt.items))
                for expr, expect, got in zip(exprs, expect_items, rpt.items):
                    self.assertEqual(expect, got, msg=f'Failed item for expr {expr}')

    def test_eval_opers(self):
        # evaluate through report template items with inline expressions
        exprs, expect_items = map(list, zip(
            ('/ac/(true)', ari.LiteralARI(True)),
            # list access
            ('/ac/(/ac/(1,2,3),//ietf/dtnma-agent/oper/list-get(1))', ari.LiteralARI(2)),
            ('/ac/(/ac/(1,2,3),//ietf/dtnma-agent/oper/list-get(10))', ari.UNDEFINED), # missing index
            ('/ac/(/ac/(1,2,3),//ietf/dtnma-agent/oper/list-get(hi))', ari.UNDEFINED), # bad parameter
            ('/ac/(2,//ietf/dtnma-agent/oper/list-get(0))', ari.UNDEFINED), # bad operand
            # map access
            ('/ac/(/am/(1=one,2=two),//ietf/dtnma-agent/oper/map-get(1))', ari.LiteralARI('one')),
            ('/ac/(/am/(1=one,2=two),//ietf/dtnma-agent/oper/map-get(10))', ari.UNDEFINED), # missing key
            ('/ac/(/am/(1=one,2=two),//ietf/dtnma-agent/oper/map-get(hi))', ari.UNDEFINED), # missing key but valid
            ('/ac/(2,//ietf/dtnma-agent/oper/map-get(0))', ari.UNDEFINED), # bad operand
            # table access
            ('/ac/(/tbl/c=2;(1,2)(3,4),//ietf/dtnma-agent/oper/tbl-get(1,0))', ari.LiteralARI(3)),
            ('/ac/(/tbl/c=2;(1,2)(3,4),//ietf/dtnma-agent/oper/tbl-get(0,1))', ari.LiteralARI(2)),
            ('/ac/(/tbl/c=2;(1,2)(3,4),//ietf/dtnma-agent/oper/tbl-get(10,0))', ari.UNDEFINED), # missing index
            ('/ac/(/tbl/c=2;(1,2)(3,4),//ietf/dtnma-agent/oper/tbl-get(0,10))', ari.UNDEFINED), # missing index
            ('/ac/(/tbl/c=2;(1,2)(3,4),//ietf/dtnma-agent/oper/tbl-get(hi,0))', ari.UNDEFINED), # bad parameter
            ('/ac/(/tbl/c=2;(1,2)(3,4),//ietf/dtnma-agent/oper/tbl-get(0,hi))', ari.UNDEFINED), # bad parameter
            ('/ac/(2,//ietf/dtnma-agent/oper/tbl-get(0,0))', ari.UNDEFINED), # bad operand
        ))

        self._start()
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=null;(//ietf/dtnma-agent/CTRL/report-on(/ac/(' + ','.join(exprs) + ')))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), stop_count=1, timeout=5)
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertIsInstance(rpt.source, ari.LiteralARI)
        self.assertEqual(rpt.source.type_id, ari.StructType.AC)
        # items of the report
        self.assertEqual(len(expect_items), len(rpt.items))
        for expr, expect, got in zip(exprs, expect_items, rpt.items):
            self.assertEqual(expect, got, msg=f'Failed item for expr {expr}')

    def test_eval_predicate_expressions(self):
        # precondition macro
        mac_items = [
           '//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!odm,-1)',
           '//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!odm,true-expr,1,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/expr),/ac/(2,2,//ietf/dtnma-agent/oper/compare-eq))'
        ]
        # evaluate through report template items with inline expressions
        exprs, expect_items = map(list, zip(
            ('/ac/(true)', ari.LiteralARI(True)),
            ('/ac/(false)', ari.LiteralARI(False)),
            ('/ac/(10)', ari.LiteralARI(10)),
            ('/ac/(//ietf/!odm/oper/undefined-name)', ari.UNDEFINED),
            # produced expression vs eval sub-expression
            ('/ac/(//ietf/!odm/const/true-expr)', self._ari_text_to_obj('/ac/(2,2,//ietf/dtnma-agent/oper/compare-eq)')),
            ('/ac/(//ietf/dtnma-agent/oper/eval(//ietf/!odm/const/true-expr))', ari.TYPED_TRUE),
            # direct predicate operators
            ('/ac/(hello,//ietf/dtnma-agent/oper/match-regexp(%22ll%22))', ari.TYPED_FALSE),
            ('/ac/(hello,//ietf/dtnma-agent/oper/match-regexp(%22.*ll.*%22))', ari.TYPED_TRUE),
            # bound evaluation operator as a predicate
            ('/ac/(2,10,//ietf/dtnma-agent/oper/compare-eq)', ari.TYPED_FALSE),
            ('/ac/(2,2,//ietf/dtnma-agent/oper/compare-eq)', ari.TYPED_TRUE),
            ('/ac/(2,//ietf/dtnma-agent/oper/unary-eval(/ac/(/label/0,10,//ietf/dtnma-agent/oper/compare-eq)))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/unary-eval(/ac/(/label/0,2,//ietf/dtnma-agent/oper/compare-eq)))', ari.TYPED_TRUE),
            # predicate composing operators
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-all(/ac/()))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-all(/ac/(//ietf/dtnma-agent/oper/is-not-undefined)))', ari.TYPED_TRUE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-all(/ac/(//ietf/dtnma-agent/oper/is-undefined)))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-all(/ac/(//ietf/dtnma-agent/oper/is-not-undefined,//ietf/dtnma-agent/oper/is-undefined)))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-any(/ac/()))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-any(/ac/(//ietf/dtnma-agent/oper/is-not-undefined)))', ari.TYPED_TRUE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-any(/ac/(//ietf/dtnma-agent/oper/is-undefined)))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-any(/ac/(//ietf/dtnma-agent/oper/is-not-undefined,//ietf/dtnma-agent/oper/is-undefined)))', ari.TYPED_TRUE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-none(/ac/()))', ari.TYPED_TRUE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-none(/ac/(//ietf/dtnma-agent/oper/is-not-undefined)))', ari.TYPED_FALSE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-none(/ac/(//ietf/dtnma-agent/oper/is-undefined)))', ari.TYPED_TRUE),
            ('/ac/(2,//ietf/dtnma-agent/oper/predicate-none(/ac/(//ietf/dtnma-agent/oper/is-not-undefined,//ietf/dtnma-agent/oper/is-undefined)))', ari.TYPED_FALSE),
        ))

        self._start()
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(/ac/(' + ','.join(mac_items) + '))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=len(mac_items))
        self.assertEqual(len(mac_items), len(rpts))
        for _ix in range(len(mac_items)):
            rpt = rpts.pop(0)
            self.assertNotIn(ari.UNDEFINED, rpt.items)

        # actual reporting
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=null;(//ietf/dtnma-agent/CTRL/report-on(/ac/(' + ','.join(exprs) + ')))')]
        )

        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), stop_count=1, timeout=5)
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertIsInstance(rpt.source, ari.LiteralARI)
        self.assertEqual(rpt.source.type_id, ari.StructType.AC)
        # items of the report
        self.assertEqual(len(expect_items), len(rpt.items))
        for expr, expect, got in zip(exprs, expect_items, rpt.items):
            self.assertEqual(expect, got, msg=f'Failed item for expr {expr}')

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
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr,1,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/PT1S,1,false))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-tbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-tbr(//ietf/!test-model-1,test-tbr2,2,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/TD/999999,/TD/PT1S,1,false))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-tbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,/AC/(//ietf/dtnma-agent/CTRL/obsolete-rule),/AC/(false),/TD/999999,1,false))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-sbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

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
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-rule-enabled(//ietf/!test-model-1/TBR/test-tbr,true)'), rpt.source)
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/reset-rule-enabled(//ietf/!test-model-1/TBR/test-tbr)'), rpt.source)
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/obsolete-rule(//ietf/!test-model-1/TBR/test-tbr2)'), rpt.source)
        # items of the report
        self.assertEqual([bool], literal_prim_types(rpt.items))

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

    def test_rule_sbr(self):
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

        mgr_eids = [
            quote('"file:' + self._mgr_bind[0].path + '"'),
        ]
        rptsrc = '/ac/(//ietf/dtnma-agent/EDD/num-msg-rx)'
        macro = '/AC/(//ietf/dtnma-agent/CTRL/report-on(' + rptsrc + ',/ac/(' + ','.join(mgr_eids) + ')))'
        cond = '/AC/(//ietf/dtnma-agent/EDD/num-msg-rx,3,//ietf/dtnma-agent/oper/compare-ge)'
        # rule is created as enabled
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-sbr(//ietf/!test-model-1,test-sbr,3,' + macro + ',' + cond + ',/TD/PT0.2S,0,true))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/ensure-sbr'), self._ari_strip_params(rpt.source))
        # items of the report
        self.assertEqual([ari.NoneType], literal_prim_types(rpt.items))

        # no more, wait for more than one condition cycle
        with self.assertRaises(TimeoutError):
            self._wait_reports(mgr_ix=0, timeout=0.5)

        # At this point ./edd/num-msg-rx is still 2, any next message will satisfy rule condition
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=null;()')]
        )
        # Wait for two action triggers, at least one eval period for each trigger
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None), stop_count=2, timeout=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj(rptsrc), rpt.source)
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj(rptsrc), rpt.source)

        # clean shutdown even while condition is satisfied

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
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([numpy.datetime64], literal_prim_types(rpt.items))

    def test_odm_ident(self):
        self._start()

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-odm(ietf,1,!test-model-1,-1))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify ident-list is empty
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/ident-list(false,false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)
        # Verify show-abstract list does have entries
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/ident-list(false,true)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)

        # Add an IDENT
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-ident(//ietf/!test-model-1,test-ident,1,false,/ac/(//ietf/alarms/IDENT/resource)))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify IDENT can be read back
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/ident-list(false,false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))
        self.assertEqual(
            self._ari_text_to_obj('//1/-1/ident/1', nn=False),
            rpt.items[0].value[0, 0]
        )
        # Verify base-filtered list also shows object
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/ident-list(false,false,//ietf/alarms/IDENT/resource)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 2), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, list(rpt.items[0].value.flat))
        self.assertEqual(
            self._ari_text_to_obj('//1/-1/ident/1', nn=False),
            rpt.items[0].value[0, 0]
        )
        # Verify other base-filtered list does not include object
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/ident-list(false,false,//ietf/alarms/IDENT/category)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)

        # Obsolete the IDENT
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-ident(//ietf/!test-model-1/ident/test-ident))', nn=False)]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # Verify list is empty again
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/ident-list(false)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)

    def test_odm_var(self):
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
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,test-var,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),1))')]
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
            self._ari_text_to_obj('//1/-1/var/1'),
            rpt.items[0].value[0, 0]
        )
        self.assertEqual(
            self._ari_text_to_obj('//ietf/amm-semtype/IDENT/type-use(name=/ARITYPE/int)', id_convert=False),
            rpt.items[0].value[0, 1]
        )

        # Obsolete the VAR
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-var(//ietf/!test-model-1/VAR/test-var))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

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
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/!test-modell-1/typedef/-100),/AC/(//ietf/dtnma-agent/EDD/num-msg-rx)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

        # attempt to create with an invalid value
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-var(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/RPTT),/AC/(10)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

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
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,test-const,1,//ietf/amm-semtype/IDENT/type-use(/ARITYPE/int),1))')]
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
            self._ari_text_to_obj('//1/-1/const/1'),
            rpt.items[0].value[0, 0]
        )
        self.assertEqual(
            self._ari_text_to_obj('//ietf/amm-semtype/IDENT/type-use(name=/ARITYPE/int)', id_convert=False),
            rpt.items[0].value[0, 1]
        )

        # Obsolete the CONST
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/obsolete-const(//ietf/!test-model-1/CONST/test-const))')]
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

    def test_exec_control_flow_if_then_else(self):
        self._start()

        # do nothing either way
        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/if-then-else(/AC/(true),null,null))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([bool], literal_prim_types(rpt.items))
        self.assertEqual(True, rpt.items[0].value)

        # true path
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/if-then-else(/AC/(true),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/if-then-else'), self._ari_strip_params(rpt.source))
        self.assertEqual(1, len(rpt.items))
        self.assertIs(True, rpt.items[0].value)
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertEqual(1, len(rpt.items))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # false path
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/if-then-else(/AC/(false),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/if-then-else'), self._ari_strip_params(rpt.source))
        self.assertEqual(1, len(rpt.items))
        self.assertIs(False, rpt.items[0].value)
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'), rpt.source)
        self.assertEqual(1, len(rpt.items))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # true path with default else parameter
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/if-then-else(/AC/(true),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/if-then-else'), self._ari_strip_params(rpt.source))
        self.assertEqual(1, len(rpt.items))
        self.assertIs(True, rpt.items[0].value)
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertEqual(1, len(rpt.items))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

    def test_exec_control_flow_catch(self):
        self._start()

        # successful try target
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/catch(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertEqual([str], literal_prim_types(rpt.items))
        # catch result after try target
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/catch'), self._ari_strip_params(rpt.source))
        self.assertEqual([bool], literal_prim_types(rpt.items))
        self.assertIs(True, rpt.items[0].value)

        # failed try target
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/catch(//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/missing),//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=3)
        self.assertEqual(3, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/missing)'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)
        # catch result after try target
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/catch'), self._ari_strip_params(rpt.source))
        self.assertEqual([bool], literal_prim_types(rpt.items))
        self.assertIs(False, rpt.items[0].value)
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'), rpt.source)
        self.assertEqual([str], literal_prim_types(rpt.items))

        # failed try target and failure target
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/catch(//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/missing),//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/another)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=3)
        self.assertEqual(3, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/missing)'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)
        # catch result after try target
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/catch'), self._ari_strip_params(rpt.source))
        self.assertEqual([bool], literal_prim_types(rpt.items))
        self.assertIs(False, rpt.items[0].value)
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/another)'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

    def test_exec_control_flow_exec_deadline(self):
        self._start()

        # successful finish of target
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/exec-deadline(//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version),/td/PT2S,//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        # target itself
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-version)'), rpt.source)
        self.assertEqual([str], literal_prim_types(rpt.items))
        # result after target
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/exec-deadline'), self._ari_strip_params(rpt.source))
        self.assertEqual([bool], literal_prim_types(rpt.items))
        self.assertIs(True, rpt.items[0].value)
        
        # failure finish of target
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/exec-deadline(//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/missing),/td/PT2S,//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))
        # target itself
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/!odm/EDD/missing)'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)
        # result after target
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/exec-deadline'), self._ari_strip_params(rpt.source))
        self.assertEqual((ari.UNDEFINED,), rpt.items)

        # timeout of target (a wait on a trivial falsy condition)
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/exec-deadline(//ietf/dtnma-agent/CTRL/wait-cond(/ac/(false)),/td/PT2S,//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=3, timeout=3)
        self.assertEqual(3, len(rpts))
        # target itself failed
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/wait-cond(/ac/(false))'), rpt.source)
        self.assertEqual((ari.UNDEFINED,), rpt.items)
        # result after target
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/exec-deadline'), self._ari_strip_params(rpt.source))
        self.assertEqual([bool], literal_prim_types(rpt.items))
        self.assertIs(False, rpt.items[0].value)
        # on-timeout
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/CTRL/inspect(//ietf/dtnma-agent/EDD/sw-vendor)'), rpt.source)
        self.assertEqual([str], literal_prim_types(rpt.items))

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
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/!test-model-1/typedef/-100),/AC/(//ietf/dtnma-agent/EDD/num-msg-rx)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

        # attempt to create with an invalid value
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,invalid,10,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/RPTT),/AC/(10)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual((ari.UNDEFINED,), rpt.items)

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
                'ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/ensure-const(//ietf/!test-model-1,rptt-num-msg-rx,4,//ietf/amm-semtype/IDENT/type-use(//ietf/amm-base/typedef/RPTT),/AC/(//ietf/dtnma-agent/EDD/num-msg-rx)))')]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        self._send_msg(
            [self._ari_text_to_obj('ari:/EXECSET/n=123;(//ietf/dtnma-agent/CTRL/report-on(//ietf/!test-model-1/const/rptt-num-msg-rx))')]
        )
        # the generated report
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(None))
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual([int], literal_prim_types(rpt.items))

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
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        self.assertEqual(self._ari_text_to_obj('/ac/(/uint/1)'), rpt.items[0])

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
                + '//ietf/dtnma-agent-acl/ctrl/ensure-group-members(10,/ac/(/label/0,//ietf/dtnma-agent/oper/match-regexp(' + pat + '))),'
                + '//ietf/dtnma-agent-acl/CTRL/ensure-access(10,/ac/(10),/ac/(/objpat/(*)(*)(*)(*)),/ac/('
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
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        self.assertEqual(self._ari_text_to_obj('/ac/(/uint/1,/uint/10)'), rpt.items[0])

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/group-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 5), rpt.items[0].value.shape)
        self.assertNotIn(ari.UNDEFINED, rpt.items[0].value.flat)
        # new group number 10
        self.assertEqual((ari.typed_uint(1), ari.typed_uint(10)), rpt.items[0].value[:, 0])
        self.assertEqual(ari.LiteralARI("example"), rpt.items[0].value[1, 1])

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/dtnma-agent-acl/EDD/access-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 6), rpt.items[0].value.shape)
        # new access number 10
        self.assertEqual((ari.typed_uint(1), ari.typed_uint(10)), rpt.items[0].value[:, 0])

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
        self.assertEqual([tuple], literal_prim_types(rpt.items))
        self.assertEqual(self._ari_text_to_obj('/ac/(/uint/1)'), rpt.items[0])

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
                + '//ietf/dtnma-agent-acl/CTRL/ensure-access(1,/ac/(10),/ac/(/objpat/(*)(*)(*)(*)),/ac/('
                + '//ietf/!missing-odm/ident/other'
                + '))'
                + ')'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=1)
        self.assertEqual(1, len(rpts))
        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent-acl/CTRL/ensure-access'), self._ari_strip_params(rpt.source))
        self.assertEqual((ari.UNDEFINED,), rpt.items)

    def test_alarms(self):
        self._start()

        # inventories are empty without apps using them
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/alarms/EDD/resource-inventory),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/alarms/EDD/category-inventory)'
                + ')'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/alarms/EDD/resource-inventory)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 1), rpt.items[0].value.shape)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/alarms/EDD/category-inventory)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 1), rpt.items[0].value.shape)

        # Initial default state
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/alarms/EDD/alarm-list),'
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/alarms/EDD/shelf-list)'
                + ')'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=2)
        self.assertEqual(2, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/alarms/EDD/alarm-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 9), rpt.items[0].value.shape)

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/alarms/EDD/shelf-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((0, 2), rpt.items[0].value.shape)

    def test_alarms_shelf_manage(self):
        self._start()

        # add two distinct entries
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/alarms/ctrl/ensure-shelf(/tbl/c=2;' + (
                    '(/ac/(/objpat/(-1)(2)(ident)(4)),/ac/(/objpat/(-1)(2)(ident)(5)))'
                    '(/ac/(/objpat/(*)(*)(*)(*)),/ac/(/objpat/(*)(*)(*)(*)))'
                ) + '))'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=1)
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/alarms/ctrl/ensure-shelf'), self._ari_strip_params(rpt.source))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # observe new state
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/alarms/EDD/shelf-list)'
                + ')'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=1)
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/alarms/EDD/shelf-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((2, 2), rpt.items[0].value.shape)
        # ordered rows with wildcard first
        self.assertEqual(
            self._ari_text_to_obj(
                '/tbl/c=2;' + (
                    '(/ac/(/objpat/(*)(*)(*)(*)),/ac/(/objpat/(*)(*)(*)(*)))'
                    '(/ac/(/objpat/(-1)(2)(ident)(4)),/ac/(/objpat/(-1)(2)(ident)(5)))'
                )
            ),
            rpt.items[0]
        )

        # discard one item by-value, and one that isn't present at all
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;(//ietf/alarms/ctrl/discard-shelf(/tbl/c=2;' + (
                    '(/ac/(/objpat/(-1)(2)(ident)(4)),/ac/(/objpat/(-1)(2)(ident)(5)))'
                    '(/ac/(/objpat/(-1)(2)(ident)(104)),/ac/(/objpat/(-1)(2)(ident)(105)))'
                ) + '))'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=1)
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/alarms/ctrl/discard-shelf'), self._ari_strip_params(rpt.source))
        self.assertNotIn(ari.UNDEFINED, rpt.items)

        # observe new state
        self._send_msg(
            [self._ari_text_to_obj(
                'ari:/EXECSET/n=123;('
                + '//ietf/dtnma-agent/CTRL/inspect(//ietf/alarms/EDD/shelf-list)'
                + ')'
            )]
        )
        rpts = self._wait_reports(mgr_ix=0, nonce=ari.LiteralARI(123), stop_count=1)
        self.assertEqual(1, len(rpts))

        rpt = rpts.pop(0)
        self.assertEqual(self._ari_text_to_obj('//ietf/dtnma-agent/ctrl/inspect(//ietf/alarms/EDD/shelf-list)'), rpt.source)
        self.assertEqual([ari.Table], literal_prim_types(rpt.items))
        self.assertEqual((1, 2), rpt.items[0].value.shape)
        self.assertEqual(
            self._ari_text_to_obj(
                '/tbl/c=2;' + (
                    '(/ac/(/objpat/(*)(*)(*)(*)),/ac/(/objpat/(*)(*)(*)(*)))'
                )
            ),
            rpt.items[0]
        )
