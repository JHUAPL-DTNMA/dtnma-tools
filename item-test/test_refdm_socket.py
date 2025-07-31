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
import requests
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers import CmdRunner, Timer, compose_args

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)

HEXSTR = r'^[0-9a-fA-F]+'


def split_content_type(text):
    if ';' in text:
        text = text.split(';', 2)[0]
    return text


class TestRefdmSocket(unittest.TestCase):
    ''' Verify whole-agent behavior with the refdm-socket '''

    def setUp(self):
        logging.getLogger('sqlalchemy.engine').setLevel(logging.WARNING)

        path = os.path.abspath(os.path.join(OWNPATH, '..'))
        os.chdir(path)
        LOGGER.info('Working in %s', path)

        self._req = requests.Session()
        self._base_url = 'http://localhost:8089/nm/api/'

        self._tmp = tempfile.TemporaryDirectory()
        self._mgr_sock_path = os.path.join(self._tmp.name, 'mgr.sock')

        def bound_sock(name):
            sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
            sock.settimeout(0.1)

            path = os.path.join(self._tmp.name, name)
            LOGGER.info('binding to socket at %s', path)
            sock.bind(path)

            return {'path': path, 'sock': sock}

        self._agent_bind = [
            bound_sock(f'agent{index}.sock')
            for index in range(3)
        ]

        args = compose_args([
            'refdm-socket',
            '-l', 'debug',
            '-a', self._mgr_sock_path
        ])
        self._mgr = CmdRunner(args)

        # ADM handling
        adms = AdmSet(cache_dir=False)
        adms.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])
        self._adms = adms

    def tearDown(self):
        self._adms = None

        self._mgr.stop()
        self._mgr = None

        self._agent_bind = None
        self._mgr_sock_path = None
        self._tmp = None

        self._req = None

    def _start(self):
        ''' Spawn the process. '''
        self._mgr.start()

        with Timer(10) as timer:
            while timer:
                timer.sleep(0.1)

                sock_ready = os.path.exists(self._mgr_sock_path)

                try:
                    resp = self._req.options(self._base_url)
                    rest_ready = True
                except requests.exceptions.ConnectionError:
                    rest_ready = False

                if sock_ready and rest_ready:
                    timer.finish()
                    return

                if not sock_ready:
                    LOGGER.info('waiting for manager socket at %s', self._mgr_sock_path)
                if not rest_ready:
                    LOGGER.info('waiting for manager response on %s', self._base_url)

        self.fail(f'Manager did not create socket at {self._mgr_sock_path}')

    def _ari_text_to_obj(self, text:str) -> ARI:
        nn_func = nickname.Converter(nickname.Mode.TO_NN, self._adms.db_session(), must_nickname=True)

        with io.StringIO(text) as buf:
            ari = ari_text.Decoder().decode(buf)
        ari = nn_func(ari)
        return ari

    def _ari_obj_to_text(self, ari:ARI) -> str:
        buf = io.StringIO()
        ari_text.Encoder().encode(ari, buf)
        return buf.getvalue()

    def _ari_obj_to_cbor(self, ari:ARI) -> bytes:
        buf = io.BytesIO()
        ari_cbor.Encoder().encode(ari, buf)
        return buf.getvalue()

    def _ari_obj_from_cbor(self, databuf:bytes) -> ARI:
        with io.BytesIO(databuf) as buf:
            ari = ari_cbor.Decoder().decode(buf)
        return ari

    def _send_rptset(self, text:str, agent_ix=0) -> str:
        ''' Send an RPTSET with a number of target ARIs.

        :param text: The ARI text form to send.
        :param agent_ix: The agent index to send from.
        :return: The socket path from which it was sent.
        '''
        LOGGER.info('Sending value %s', text)
        data = cbor2.dumps(1) + self._ari_obj_to_cbor(self._ari_text_to_obj(text))
        addr = self._mgr_sock_path
        LOGGER.info('Sending message %s to %s', data.hex(), addr)
        bind = self._agent_bind[agent_ix]
        bind['sock'].sendto(data, addr)
        return bind['path']

    def _wait_execset(self, agent_ix=0) -> List[ARI]:
        ''' Wait for an EXECSET and decode it.

        :param agent_ix: The agent index to receive on.
        :return: The ARI decoded form.
        '''
        recv_sock = self._agent_bind[agent_ix]['sock']

        (data, addr) = recv_sock.recvfrom(1024)
        LOGGER.info('Received message %s from %s', data.hex(), addr)
        self.assertEqual(self._mgr_sock_path, addr)

        values = []
        dec = ari_cbor.Decoder()
        with io.BytesIO(data) as infile:
            vers = cbor2.load(infile)
            self.assertEqual(1, vers, msg='Invalid AMP version')
            ari = dec.decode(infile)
            values.append(ari)

            if LOGGER.isEnabledFor(logging.INFO):
                textbuf = io.StringIO()
                ari_text.Encoder().encode(ari, textbuf)
                LOGGER.info('Received value: %s', textbuf.getvalue())

        return values

    def test_start_terminate(self):
        self._start()

        LOGGER.info('Sending SIGINT')
        self._mgr.proc.send_signal(signal.SIGINT)
        self.assertEqual(0, self._mgr.proc.wait(timeout=5))
        self.assertEqual(0, self._mgr.proc.returncode)

    def test_rest_version(self):
        self._start()
        resp = self._req.get(self._base_url + 'version')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        LOGGER.info(resp.json())

        # invalid methods
        resp = self._req.post(self._base_url + 'version')
        self.assertEqual(405, resp.status_code)
        resp = self._req.put(self._base_url + 'version')
        self.assertEqual(405, resp.status_code)

        resp = self._req.get(self._base_url + 'versionplus')
        self.assertEqual(404, resp.status_code)

    def _get_agent_names(self) -> Set[str]:
        resp = self._req.get(self._base_url + 'agents')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        return set([agt['name'] for agt in data['agents']])

    def test_rest_agents_add_valid(self):
        self._start()
        self.assertEqual(set(), self._get_agent_names())

        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid1\r\n',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        resp = self._req.post(
            self._base_url + 'agents',
            data='\r\nfile:/tmp/invalid2\r\n',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        self.assertEqual(set(['file:/tmp/invalid1', 'file:/tmp/invalid2']), self._get_agent_names())

    def test_rest_agents_add_invalid(self):
        self._start()

        resp = self._req.put(self._base_url + 'agents')
        self.assertEqual(405, resp.status_code)

        # missing content type
        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid\r\n'
        )
        self.assertEqual(415, resp.status_code)

        # bad content type
        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid\r\n',
            headers={
                'content-type': 'text/csv',
            }
        )
        self.assertEqual(415, resp.status_code)

        # no name content
        resp = self._req.post(
            self._base_url + 'agents',
            data='\r\n',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(422, resp.status_code)

    def test_rest_agents_add_duplicate(self):
        self._start()

        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid\r\n',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid\r\n',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(400, resp.status_code)

        self.assertEqual(set(['file:/tmp/invalid']), self._get_agent_names())

    def test_agents_eid_reports_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing')
        self.assertEqual(404, resp.status_code)

    def test_agents_eid_reports_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports')
        self.assertEqual(404, resp.status_code)

    def test_agents_eid_reports_text_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports?form=text')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_eid_reports_text_404_longname(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing-with-a-{"longer-" * 40}eid/reports?form=text')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_eid_reports_hex_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports?form=hex')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_idx_reports_text_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/10/reports?form=text')
        self.assertEqual(404, resp.status_code)

    def test_agents_idx_reports_hex_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/10/reports?form=hex')
        self.assertEqual(404, resp.status_code)

    def test_agents_idx_reports_text_400(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports?form=text')
        self.assertEqual(404, resp.status_code)

    def test_agents_idx_reports_hex_400(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports?form=hex')
        self.assertEqual(404, resp.status_code)

    def test_recv_one_report(self):
        self._start()

        # initial state
        self.assertEqual(set(), self._get_agent_names())

        # first check behavior with one report
        sock_path = self._send_rptset(
            'ari:/RPTSET/n=null;r=/TP/20240102T030405Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))',
        )
        agent_eid = f'file:{sock_path}'
        eid_seg = quote(agent_eid, safe="")

        LOGGER.info('Waiting for agent %s', agent_eid)
        with Timer(5) as timer:
            while timer:
                available = self._get_agent_names()
                if agent_eid in available:
                    timer.finish()
                    break
                timer.sleep(0.1)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports?form=hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(1, len(lines))

        # Verify path segment handling
        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports?form=hex&test=ignored')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))
        other_lines = resp.text.splitlines()
        self.assertEqual(lines, other_lines)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports?form=text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(1, len(lines))
        for line in lines:
            self.assertTrue(line.startswith('ari:/RPTSET/'))

    def test_recv_two_reports(self):
        self._start()

        self._send_rptset(
            'ari:/RPTSET/n=null;r=/TP/20240102T030406Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))',
        )
        sock_path = self._send_rptset(
            'ari:/RPTSET/n=null;r=/TP/20240102T030405Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))',
        )
        agent_eid = f'file:{sock_path}'
        eid_seg = quote(agent_eid, safe="")

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports?form=hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(2, len(lines))

        # index resource gets the same result
        resp = self._req.get(self._base_url + f'agents/idx/0/reports?form=hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))
        other_lines = resp.text.splitlines()
        self.assertEqual(lines, other_lines)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports?form=text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(2, len(lines))
        for line in lines:
            self.assertTrue(line.startswith('ari:/RPTSET/'))

        # index resource gets the same result
        resp = self._req.get(self._base_url + f'agents/idx/0/reports?form=text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        other_lines = resp.text.splitlines()
        self.assertEqual(lines, other_lines)

    def test_agents_send_hex(self):
        self._start()

        agent_bind = self._agent_bind[0]
        agent_eid = f'file:{agent_bind["path"]}'
        eid_seg = quote(agent_eid, safe="")

        resp = self._req.post(
            self._base_url + 'agents',
            data=(agent_eid + '\r\n'),
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        textform = "ari:/EXECSET/n=h'6869';(//ietf/dtnma-agent/CTRL/inspect)"
        send_ari = self._ari_text_to_obj(textform)
        send_data = self._ari_obj_to_cbor(send_ari)

        resp = self._req.post(
            self._base_url + f'agents/eid/{eid_seg}/send?form=hex',
            data=f'{send_data.hex()}\r\n',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))

        values = self._wait_execset(0)
        self.assertEqual([send_ari], values)

        # no other datagrams
        with self.assertRaises(TimeoutError):
            self._wait_execset(0)

    def test_agents_send_text(self):
        self._start()

        agent_bind = self._agent_bind[0]
        agent_eid = f'file:{agent_bind["path"]}'
        eid_seg = quote(agent_eid, safe="")

        resp = self._req.post(
            self._base_url + 'agents',
            data=(agent_eid + '\r\n'),
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        textform = "ari:/EXECSET/n=h'6869';(//ietf/dtnma-agent/CTRL/inspect)"
        send_ari = self._ari_text_to_obj(textform)
        send_text = self._ari_obj_to_text(send_ari)

        resp = self._req.post(
            self._base_url + f'agents/eid/{eid_seg}/send?form=text',
            data=f'{send_text}\r\n',
            headers={
                'content-type': 'text/uri-list',
            }
        )
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))

        values = self._wait_execset(0)
        self.assertEqual([send_ari], values)

        # no other datagrams
        with self.assertRaises(TimeoutError):
            self._wait_execset(0)
