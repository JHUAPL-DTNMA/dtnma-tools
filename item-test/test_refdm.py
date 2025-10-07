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
''' Test the local socket and proxy socket transport of the REFDM.

This uses the environment variable DB_HOST to determine whether to spawn a
private PostgreSQL server or use an external one.
'''
import glob
import io
import logging
import os
import signal
import socket
import subprocess
import tempfile
import time
from typing import List, Set
from urllib.parse import quote
import unittest
import cbor2
import requests
import sqlalchemy
from ace import (AdmSet, ARI, ari, ari_text, ari_cbor, nickname)
from helpers import CmdRunner, Timer, compose_args

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)

ADMS = AdmSet(cache_dir=False)
''' ADM handling outside of tests '''
logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)
ADMS.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])

ORIG_DB_HOST = os.environ.get('DB_HOST')
''' Environment at start of process '''


def split_content_type(text):
    if ';' in text:
        text = text.split(';', 2)[0]
    return text


class BaseRefdm(unittest.TestCase):
    ''' General purpose REFDM support state and functions '''

    @classmethod
    def _sql_start(cls):
        ''' Spawn an SQL server if necessary.
        '''
        if ORIG_DB_HOST is not None:
            # nothing to do for processes
            cls._sqldir = None
            cls._sqldb = None
            os.environ['PGHOST'] = os.environ['DB_HOST']
            os.environ['PGUSER'] = os.environ['DB_USER']
            os.environ['PGPASSWORD'] = os.environ['DB_PASSWORD']
            return

        cls._sqldir = tempfile.TemporaryDirectory()
        sql_data = os.path.join(cls._sqldir.name, 'data')
        os.makedirs(sql_data)
        sql_sock = os.path.join(cls._sqldir.name, 'run')
        os.makedirs(sql_sock)

        initdb = CmdRunner([
            'initdb',
            '-D', sql_data,
            '--auth-local=trust',
        ])
        initdb.start()
        if initdb.wait(timeout=10) != 0:
            raise RuntimeError('Failed to run initdb')

        args = [
            'postgres',
            '-D', sql_data,
            '-h', '',
            '-k', sql_sock
        ]
        cls._sqldb = CmdRunner(args)
        cls._sqldb.start()

        # After all setup, expose the state
        os.environ['DB_HOST'] = sql_sock
        os.environ['DB_USER'] = ''
        os.environ.pop('DB_PASSWORD', None)

        os.environ['PGHOST'] = sql_sock
        os.environ['PGUSER'] = ''
        os.environ.pop('PGPASSWORD', None)

        isready = CmdRunner([
            'pg_isready',
            '-d', 'postgres',
        ])
        timer = Timer(10)
        while timer:
            timer.sleep(0.1)

            isready.start()
            if isready.wait() == 0:
                timer.finish()

    @classmethod
    def _sql_stop(cls):
        if cls._sqldb:
            cls._sqldb.stop()
            cls._sqldb = None
        if cls._sqldir:
            cls._sqldir.cleanup()
            cls._sqldir = None

    DB_NAME = 'refdm'
    ''' The test database name to recycle for each case '''

    def _database_create(self) -> str:
        ''' Create a test database in a running postgres server '''
        self._database_drop()

        psql = CmdRunner([
            'psql',
            '-w',
            '-d', 'postgres',
            '-c', f'CREATE DATABASE {self.DB_NAME}',
        ])
        psql.start()
        if psql.wait() != 0:
            raise RuntimeError('Failed to run create database')

        db_host = os.environ['DB_HOST']
        os.environ['DB_NAME'] = self.DB_NAME
        db_uri = 'postgresql+psycopg2:' + f'///{self.DB_NAME}?host={db_host}'

        script_pat = os.path.join(OWNPATH, '..', 'refdb-sql', 'postgres', 'Database_Scripts', '*.sql')
        # execute in alphabetic order
        for filepath in sorted(glob.glob(script_pat)):
            psql = CmdRunner([
                'psql',
                '-w',
                '-d', self.DB_NAME,
                '-f', filepath,
            ], stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
            psql.start()
            if psql.wait() != 0:
                raise RuntimeError('Failed to run psql')

        return db_uri

    def _database_drop(self):
        ''' Drop the test database if necessary '''
        psql = CmdRunner([
            'psql',
            '-w',
            '-d', 'postgres',
            '-c', f'DROP DATABASE IF EXISTS {self.DB_NAME} WITH (FORCE)',
        ])
        psql.start()
        if psql.wait(timeout=10) != 0:
            raise RuntimeError('Failed to run drop database')

    @classmethod
    def setUpClass(cls) -> None:
        logging.getLogger('sqlalchemy.engine').setLevel(logging.WARNING)
        logging.getLogger('ace.adm_yang').setLevel(logging.ERROR)

        cls._sql_start()

    @classmethod
    def tearDownClass(cls) -> None:
        cls._sql_stop()

    def _get_agent_names(self) -> Set[str]:
        resp = self._req.get(self._base_url + 'agents')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('application/json', split_content_type(resp.headers['content-type']))
        data = resp.json()
        return set([agt['name'] for agt in data['agents']])

    def _wait_for_db_table(self, table_name: str, need_count: int):
        LOGGER.info('Waiting for DB table %s with %d rows', table_name, need_count)
        with self._db_eng.connect() as conn:
            query = sqlalchemy.select(sqlalchemy.func.count(sqlalchemy.literal_column('1'))).select_from(sqlalchemy.table(table_name))
            timer = Timer(5)
            while timer:
                timer.sleep(0.1)
                count = conn.execute(query).scalar()
                if count == need_count:
                    timer.finish()
                    break

            LOGGER.info('Have %d rows after %0.1f s', count, timer.elapsed())
            if LOGGER.isEnabledFor(logging.DEBUG):
                query = sqlalchemy.select(sqlalchemy.literal_column('*')).select_from(sqlalchemy.table(table_name))
                for row in conn.execute(query).fetchall():
                    LOGGER.debug('Row: %s', row)

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


class TestRefdmSocket(BaseRefdm):
    ''' Verify whole-agent behavior with the refdm-socket '''

    def setUp(self) -> None:
        self._req = requests.Session()
        self._base_url = 'http://localhost:8089/nm/api/'

        db_uri = self._database_create()
        self._db_eng = sqlalchemy.create_engine(db_uri)

        self._tmp = tempfile.TemporaryDirectory()
        self._mgr_sock_path = os.path.join(self._tmp.name, 'mgr.sock')

        def bound_sock(name):
            sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

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

    def tearDown(self) -> None:
        mgr_exit = self._mgr.stop()
        self._mgr = None

        for bind in self._agent_bind:
            sock = bind['sock']
            sock.close()
        self._agent_bind = None

        self._mgr_sock_path = None
        self._tmp = None

        if self._db_eng:
            self._db_eng.dispose()
        self._db_eng = None
        self._database_drop()

        self._req = None

        # assert after all other shutdown
        self.assertEqual(0, mgr_exit)

    def _start(self) -> None:
        ''' Spawn the REFDM process. '''
        self._mgr.start()

        delay = 0.1
        timer = Timer(10)
        while timer:
            # linear back-off
            timer.sleep(delay)
            delay += 0.1

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

    def _send_msg(self, values: List[ARI], agent_ix: int = 0) -> str:
        ''' Send an AMP message with RPTSET values.

        :param values: The ARI items to send.
        :param agent_ix: The agent index to send from.
        :return: The socket path from which it was sent.
        '''
        msg_data = cbor2.dumps(1)
        for val in values:
            LOGGER.info('Sending value %s', self._ari_obj_to_text(val))
            msg_data += self._ari_obj_to_cbor(val)
        addr = self._mgr_sock_path
        bind = self._agent_bind[agent_ix]
        LOGGER.info('Sending message %s from %s to %s', msg_data.hex(), bind['path'], addr)
        bind['sock'].sendto(msg_data, addr)
        return bind['path']

    def _wait_msg(self, agent_ix: int, timeout: float=1) -> List[ARI]:
        ''' Wait for an AMP message with EXECSET values and decode it.

        :param agent_ix: The agent index to receive on.
        :param timeout: The time to wait in seconds.
        :return: The contained ARIs in decoded form.
        :raise TimeoutError: If not received in time.
        '''
        bind = self._agent_bind[agent_ix]
        recv_sock = bind['sock']

        recv_sock.settimeout(timeout)

        try:
            (data, addr) = recv_sock.recvfrom(10240)
        except (socket.timeout, TimeoutError) as err:
            raise TimeoutError("No message received") from err
        LOGGER.info('Received message %s to %s from %s', data.hex(), bind['path'], addr)
        self.assertEqual(self._mgr_sock_path, addr)

        values = []
        dec = ari_cbor.Decoder()
        with io.BytesIO(data) as infile:
            vers = cbor2.load(infile)
            self.assertEqual(1, vers, msg='Invalid AMP version')

            while infile.tell() < len(data):
                val = dec.decode(infile)
                LOGGER.info('Received value %s', self._ari_obj_to_text(val))
                self.assertIsInstance(val.value, ari.ExecutionSet)
                values.append(val)

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

    def test_rest_agents_add_valid(self):
        self._start()
        self.assertSetEqual(set(), self._get_agent_names())

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

        self.assertSetEqual(set(['file:/tmp/invalid1', 'file:/tmp/invalid2']), self._get_agent_names())

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

        self.assertSetEqual(set(['file:/tmp/invalid']), self._get_agent_names())

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

    def test_recv_one_agent_one_rptset(self):
        self._start()

        # initial state
        self.assertSetEqual(set(), self._get_agent_names())

        # first check behavior with one report
        sock_path = self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=1234;r=/TP/20240102T030405Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')]
        )
        agent_eid = 'file:' + sock_path
        eid_seg = quote(agent_eid, safe="")

        LOGGER.info('Waiting for agent %s', agent_eid)
        timer = Timer(5)
        while timer:
            timer.sleep(0.1)
            available = self._get_agent_names()
            if agent_eid in available:
                timer.finish()
                break

        self._wait_for_db_table('ari_rptset', 1)

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

    def test_recv_one_agent_three_rptset(self):
        self._start()

        # each primitive type of nonce
        self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=null;r=/TP/20240102T030407Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')]
        )
        self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=\'test\';r=/TP/20240102T030406Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')]
        )
        sock_path = self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=1234;r=/TP/20240102T030405Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')]
        )
        eid_seg = quote('file:' + sock_path, safe="")
        rptset_count = 3

        self._wait_for_db_table('ari_rptset', rptset_count)

        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg}/reports?form=hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))
        lines = resp.text.splitlines()
        self.assertEqual(rptset_count, len(lines))

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
        self.assertEqual(rptset_count, len(lines))
        for line in lines:
            self.assertTrue(line.startswith('ari:/RPTSET/'))

        # index resource gets the same result
        resp = self._req.get(self._base_url + f'agents/idx/0/reports?form=text')
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/uri-list', split_content_type(resp.headers['content-type']))
        other_lines = resp.text.splitlines()
        self.assertEqual(lines, other_lines)

    def test_recv_two_agents_clear_rptset(self):
        self._start()

        # one each from different agents
        sock_path = self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=null;r=/TP/20240102T030407Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')]
        )
        eid_seg0 = quote('file:' + sock_path, safe="")

        sock_path = self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=null;r=/TP/20240102T030407Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')],
            agent_ix=1
        )
        eid_seg1 = quote('file:' + sock_path, safe="")

        self._wait_for_db_table('ari_rptset', 2)
        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg0}/reports?form=hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual(1, len(resp.text.splitlines()))
        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg1}/reports?form=hex')
        self.assertEqual(200, resp.status_code)
        self.assertEqual(1, len(resp.text.splitlines()))

        # clear RPTSETs explicitly
        resp = self._req.post(self._base_url + f'agents/eid/{eid_seg0}/clear_reports')
        self.assertEqual(204, resp.status_code)
        self._wait_for_db_table('ari_rptset', 1)
        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg0}/reports?form=hex')
        self.assertEqual(204, resp.status_code)
        # repeats do nothing
        resp = self._req.post(self._base_url + f'agents/eid/{eid_seg0}/clear_reports')
        self.assertEqual(204, resp.status_code)

        resp = self._req.post(self._base_url + f'agents/eid/{eid_seg1}/clear_reports')
        self.assertEqual(204, resp.status_code)
        self._wait_for_db_table('ari_rptset', 0)
        resp = self._req.get(self._base_url + f'agents/eid/{eid_seg1}/reports?form=hex')
        self.assertEqual(204, resp.status_code)

    def test_agents_send_hex(self):
        self._start()

        agent_bind = self._agent_bind[0]
        agent_eid = 'file:' + agent_bind["path"]
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

        msg_vals = self._wait_msg(agent_ix=0)
        self.assertEqual([send_ari], msg_vals)

        # no other datagrams
        with self.assertRaises(TimeoutError):
            self._wait_msg(agent_ix=0)

    def test_agents_send_text(self):
        self._start()

        agent_bind = self._agent_bind[0]
        agent_eid = 'file:' + agent_bind["path"]
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

        msg_vals = self._wait_msg(agent_ix=0)
        self.assertEqual([send_ari], msg_vals)

        # no other datagrams
        with self.assertRaises(TimeoutError):
            self._wait_msg(agent_ix=0)

    def test_send_three_execsets(self):
        self._start()

        agent_bind = self._agent_bind[0]
        agent_eid = 'file:' + agent_bind["path"]
        eid_seg = quote(agent_eid, safe="")

        resp = self._req.post(
            self._base_url + 'agents',
            data=(agent_eid + '\r\n'),
            headers={
                'content-type': 'text/plain',
            }
        )
        self.assertEqual(200, resp.status_code)

        # each primitive type of nonce
        send_text = "\
ari:/EXECSET/n=null;(//ietf/dtnma-agent/CTRL/inspect)\r\n\
ari:/EXECSET/n=1234;(//ietf/dtnma-agent/CTRL/inspect)\r\n\
ari:/EXECSET/n='test';(//ietf/dtnma-agent/CTRL/inspect)\r\n\
"
        resp = self._req.post(
            self._base_url + f'agents/eid/{eid_seg}/send?form=text',
            data=send_text,
            headers={
                'content-type': 'text/uri-list',
            }
        )
        self.assertEqual(200, resp.status_code)
        self.assertEqual('text/plain', split_content_type(resp.headers['content-type']))

        self._wait_for_db_table('execution_set', 3)

        # three sent together
        msg_vals = self._wait_msg(agent_ix=0)
        self.assertEqual(3, len(msg_vals))

        # no other datagrams
        with self.assertRaises(TimeoutError):
            self._wait_msg(agent_ix=0)


class TestRefdmProxy(BaseRefdm):
    ''' Verify whole-agent behavior with the refdm-proxy '''

    def setUp(self) -> None:
        self._req = requests.Session()
        self._base_url = 'http://localhost:8089/nm/api/'

        db_uri = self._database_create()
        self._db_eng = sqlalchemy.create_engine(db_uri)

        self._tmp = tempfile.TemporaryDirectory()
        self._proxy_sock_path = os.path.join(self._tmp.name, 'proxy.sock')

        self._proxy_sock = None
        # Name for accepted connection
        self._proxy_sock_conn = None

        args = compose_args([
            'refdm-proxy',
            '-l', 'debug',
            '-a', self._proxy_sock_path,
        ])
        self._mgr = CmdRunner(args)

    def tearDown(self) -> None:
        mgr_exit = self._mgr.stop()
        self._mgr = None

        if self._proxy_sock_conn:
            self._proxy_sock_conn.close()
            self._proxy_sock_conn = None
        if self._proxy_sock:
            self._proxy_sock.close()
            self._proxy_sock = None

        self._tmp = None

        if self._db_eng:
            self._db_eng.dispose()
        self._db_eng = None
        self._database_drop()

        self._req = None

        # assert after all other shutdown
        self.assertEqual(0, mgr_exit)

    def _proxy_listen(self):
        self._proxy_sock = socket.socket(socket.AF_UNIX, socket.SOCK_SEQPACKET)
        LOGGER.info('binding to socket at %s', self._proxy_sock_path)
        self._proxy_sock.bind(self._proxy_sock_path)
        self._proxy_sock.listen(1)

    def _start(self) -> None:
        ''' Spawn the REFDM process. '''
        self._proxy_listen()

        self._mgr.start()

        delay = 0.1
        timer = Timer(10)
        while timer:
            # linear back-off
            timer.sleep(delay)
            delay += 0.1

            if self._proxy_sock_conn is None:
                self._proxy_sock.settimeout(timer.remaining())
                self._proxy_sock_conn, addr = self._proxy_sock.accept()
                self.assertEqual(addr, '')
                sock_ready = True
            else:
                sock_ready = False

            try:
                resp = self._req.options(self._base_url)
                rest_ready = True
            except requests.exceptions.ConnectionError:
                rest_ready = False

            if sock_ready and rest_ready:
                timer.finish()
                return

            if not sock_ready:
                LOGGER.info('waiting for manager connection to %s', self._proxy_sock_path)
            if not rest_ready:
                LOGGER.info('waiting for manager response on %s', self._base_url)

        self.fail(f'Manager did not connect to socket at {self._proxy_sock_path}')

    def _send_msg(self, values: List[ARI], agent_eid: str) -> None:
        ''' Send an AMP message with RPTSET values.

        :param values: The ARI items to send.
        :param agent_eid: The agent EID to proxy from.
        '''
        msg_data = cbor2.dumps(1)
        for val in values:
            LOGGER.info('Sending value %s', self._ari_obj_to_text(val))
            msg_data += self._ari_obj_to_cbor(val)
        LOGGER.info('Sending message %s from %s', msg_data.hex(), agent_eid)
        prox_data = cbor2.dumps(agent_eid) + msg_data
        self._proxy_sock_conn.send(prox_data)

    def _wait_msg(self, agent_eid: str, timeout: float=1) -> List[ARI]:
        ''' Wait for an AMP message with EXECSET values and decode it.

        :param agent_eid: The agent EID to proxy for.
        :param timeout: The time to wait in seconds.
        :return: The contained ARIs in decoded form.
        :raise TimeoutError: If not received in time.
        '''
        self._proxy_sock_conn.settimeout(timeout)

        try:
            prox_data = self._proxy_sock_conn.recv(10240)
        except (socket.timeout, TimeoutError) as err:
            raise TimeoutError("No message received") from err

        with io.BytesIO(prox_data) as infile:
            prox_eid = cbor2.load(infile)
            # remainder
            msg_data = infile.read()

        LOGGER.info('Received message %s from %s', msg_data.hex(), prox_eid)

        values = []
        dec = ari_cbor.Decoder()
        with io.BytesIO(msg_data) as infile:
            vers = cbor2.load(infile)
            self.assertEqual(1, vers, msg='Invalid AMP version')

            while infile.tell() < len(msg_data):
                val = dec.decode(infile)
                LOGGER.info('Received value %s', self._ari_obj_to_text(val))
                self.assertIsInstance(val.value, ari.ExecutionSet)
                values.append(val)

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

    def test_reconnect_proxy(self):
        self._start()
        self.assertSetEqual(set(), self._get_agent_names())

        time.sleep(0.1)
        # close connection, still listen
        self._proxy_sock_conn.close()

        self._proxy_sock_conn, addr = self._proxy_sock.accept()
        self.assertIsNotNone(self._proxy_sock_conn)
        self.assertEqual(addr, '')

        self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=1234;r=/TP/20240102T030405Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')],
            agent_eid='data:peer'
        )
        self._wait_for_db_table('ari_rptset', 1)
        self.assertSetEqual(set(['data:peer']), self._get_agent_names())

    def test_start_before_proxy(self):
        # start daemon before listen
        self._mgr.start()

        self._proxy_listen()
        self._proxy_sock_conn, addr = self._proxy_sock.accept()
        self.assertIsNotNone(self._proxy_sock_conn)
        self.assertEqual(addr, '')
        self.assertSetEqual(set(), self._get_agent_names())

        self._send_msg(
            [self._ari_text_to_obj('ari:/RPTSET/n=1234;r=/TP/20240102T030405Z;(t=/TD/PT;s=//ietf/dtnma-agent/CTRL/inspect;(null))')],
            agent_eid='data:peer'
        )
        self._wait_for_db_table('ari_rptset', 1)
        self.assertSetEqual(set(["data:peer"]), self._get_agent_names())

    def test_agents_send_hex(self):
        self._start()

        agent_eid = 'data:agent'
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

        msg_vals = self._wait_msg(agent_eid)
        self.assertEqual([send_ari], msg_vals)

        # no other datagrams
        with self.assertRaises(TimeoutError):
            self._wait_msg(agent_eid)
