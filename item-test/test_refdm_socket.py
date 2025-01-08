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

import binascii
import io
import logging
import os
import signal
import socket
import subprocess
import tempfile
from typing import List
from urllib.parse import quote
import unittest
import cbor2
import requests
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers import CmdRunner, Timer, Timeout

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
        self._mgr_sock_name = os.path.join(self._tmp.name, 'mgr.sock')

        def bound_sock(name):
            sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
            path = os.path.join(self._tmp.name, name)
            sock.bind(path)
            return {'path': path, 'sock': sock}

        self._agent_bind = [
            bound_sock(f'agent{index}.sock')
            for index in range(3)
        ]

        args = ['./run.sh', 'refdm-socket', '-l', 'debug', '-a', self._mgr_sock_name]
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
        self._mgr_sock_name = None
        self._tmp = None

        self._req = None

    def _start(self):
        ''' Spawn the process. '''
        self._mgr.start()

        with Timer(10) as timer:
            while timer:
                timer.sleep(0.1)
                if os.path.exists(self._mgr_sock_name):
                    timer.finish()
                    return
                LOGGER.info('waiting for manager socket at %s', self._mgr_sock_name)

        self.fail(f'Manager did not create socket at {self._mgr_sock_name}')

    def _ari_text_to_obj(self, text:str) -> ARI:
        nn_func = nickname.Converter(nickname.Mode.TO_NN, self._adms.db_session(), must_nickname=True)

        with io.StringIO(text) as buf:
            ari = ari_text.Decoder().decode(buf)
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

    def _send_rptset(self, text:str, agent_ix=0) -> str:
        ''' Send an RPTSET with a number of target ARIs.

        :return: The socket path from which it was sent.
        '''
        LOGGER.info('Sending value %s', text)
        data = self._ari_obj_to_cbor(self._ari_text_to_obj(text))
        hexstr = binascii.b2a_hex(data).decode('ascii')
        LOGGER.info('Sending message %s', hexstr)
        bind = self._agent_bind[agent_ix]
        bind['sock'].sendto(data, self._mgr_sock_name)
        return bind['path']

    def _wait_execset(self) -> ARI:
        ''' Wait for an EXECSET and decode it. '''
        line = self._mgr.wait_for_text(HEXSTR).strip()
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

    def test_rest_agents(self):
        self._start()

        resp = self._req.get(self._base_url + 'agents')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        self.assertEqual([], data['agents'])

        resp = self._req.put(self._base_url + 'agents')
        self.assertEqual(405, resp.status_code)

        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid'
        )
        self.assertEqual(415, resp.status_code)
        resp = self._req.post(
            self._base_url + 'agents',
            data='file:/tmp/invalid',
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        resp = self._req.get(self._base_url + 'agents')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        self.assertEqual(set(['file:/tmp/invalid']), set([agt['name'] for agt in data['agents']]))

    def test_agents_eid_reports_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_eid_reports_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_eid_reports_text_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports/text')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_eid_reports_text_404_longname(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing-with-a-{"longer-" * 40}eid/reports/text')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_eid_reports_hex_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports/hex')
        self.assertEqual(404, resp.status_code)
        self.assertIn('Unknown agent', resp.text)

    def test_agents_idx_reports_text_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/10/reports/text')
        self.assertEqual(404, resp.status_code)

    def test_agents_idx_reports_hex_404(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/10/reports/hex')
        self.assertEqual(404, resp.status_code)

    def test_agents_idx_reports_text_400(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports/text')
        self.assertEqual(404, resp.status_code)

    def test_agents_idx_reports_hex_400(self):
        self._start()
        resp = self._req.get(self._base_url + f'agents/eid/missing/reports/hex')
        self.assertEqual(404, resp.status_code)

    def test_recv_one_report(self):
        self._start()

        # initial state
        resp = self._req.get(self._base_url + 'agents')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        self.assertEqual([], data['agents'])

        # first check behavior with one report
        sock_path = self._send_rptset(
            'ari:/RPTSET/n=null;r=20240102T030405Z;(t=PT;s=//ietf-dtnma-agent/CTRL/inspect;(null))',
        )
        agent_eid = f'file:{sock_path}'
        eid_seg = quote(agent_eid, safe="")

        LOGGER.info('Waiting for agent %s', agent_eid)
        with Timer(5) as timer:
            while timer:
                resp = self._req.get(self._base_url + 'agents')
                self.assertEqual(200, resp.status_code)
                data = resp.json()
                if data['agents'][0]['name'] == agent_eid:
                    timer.finish()
                    break
                timer.sleep(0.1)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports/hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        self.assertEqual(1, len(data['reports']))

        # Verify path segment handling
        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports/hex?test=ignored')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        other_data = resp.json()
        self.assertEqual(data, other_data)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports/text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(1, len(lines))
        for line in lines:
            self.assertTrue(line.startswith('ari:/RPTSET/'))

    def test_recv_two_reports(self):
        self._start()

        self._send_rptset(
            'ari:/RPTSET/n=null;r=20240102T030406Z;(t=PT;s=//ietf-dtnma-agent/CTRL/inspect;(null))',
        )
        sock_path = self._send_rptset(
            'ari:/RPTSET/n=null;r=20240102T030405Z;(t=PT;s=//ietf-dtnma-agent/CTRL/inspect;(null))',
        )
        agent_eid = f'file:{sock_path}'
        eid_seg = quote(agent_eid, safe="")

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports/hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        self.assertEqual(2, len(data['reports']))

        # index resource gets the same result
        resp = self._req.get(self._base_url + f'agents/idx/0/reports/hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        other_data = resp.json()
        self.assertEqual(data, other_data)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports/text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(2, len(lines))
        for line in lines:
            self.assertTrue(line.startswith('ari:/RPTSET/'))

        # index resource gets the same result
        resp = self._req.get(self._base_url + f'agents/idx/0/reports/text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        other_lines = resp.text.splitlines()
        self.assertEqual(lines, other_lines)
