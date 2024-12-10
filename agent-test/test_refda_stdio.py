
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
from helpers.runner import CmdRunner, Timeout

OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)

HEXSTR = r'^[0-9a-fA-F]+'


class TestStdioAgent(unittest.TestCase):
    ''' Verify whole-agent behavior with the stdio_agent '''

    def setUp(self):
        logging.getLogger('sqlalchemy.engine').setLevel(logging.WARNING)

        path = os.path.abspath(os.path.join(OWNPATH, '..'))
        os.chdir(path)
        LOGGER.info('Working in %s', path)

        args = ['./run.sh', 'refda-stdio', '-l', 'debug']
        self._agent = CmdRunner(args)

        # ADM handling
        adms = AdmSet(cache_dir=False)
        adms.load_from_dirs([os.path.join(OWNPATH, 'deps', 'adms')])
        self._adms = adms

    def tearDown(self):
        self._agent.stop()

    def _start(self):
        ''' Spawn the process and wait for the startup HELLO report. '''
        self._agent.start()
        self._wait_rptset()

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

    def _send_execset(self, text:str):
        ''' Send an EXECSET with a number of target ARIs. '''
        LOGGER.info('Sending value %s', text)
        data = self._ari_obj_to_cbor(self._ari_text_to_obj(text))
        line = binascii.b2a_hex(data).decode('ascii')
        LOGGER.info('Sending line %s', line)
        self._agent.send_stdin(line + '\n')

    def _wait_rptset(self) -> ARI:
        ''' Wait for a RPTSET and decode it. '''
        line = self._agent.wait_for_text(HEXSTR).strip()
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
#        self._agent.wait_for_text(HEXSTR)

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

    def test_exec(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(//ietf-dtnma-agent/CTRL/inspect(//ietf-dtnma-agent/EDD/sw-version))'
        )
        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf-dtnma-agent/ctrl/inspect(//ietf-dtnma-agent/EDD/sw-version)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI('0.0.0')], rpt.items)

    def test_exec_delayed(self):
        self._start()

        self._send_execset(
            'ari:/EXECSET/n=123;(/AC/(//ietf-dtnma-agent/CTRL/wait-for(/TD/PT1.5S),//ietf-dtnma-agent/CTRL/inspect(//ietf-dtnma-agent/EDD/sw-version)))'
        )

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf-dtnma-agent/CTRL/wait-for(/TD/PT1.5S)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI(None)], rpt.items)

        rptset = self._wait_rptset().value
        self.assertIsInstance(rptset, ari.ReportSet)
        self.assertEqual(1, len(rptset.reports))
        rpt = rptset.reports[0]
        LOGGER.info('Got rpt %s', rpt)
        self.assertIsInstance(rpt, ari.Report)
        self.assertEqual(self._ari_text_to_obj('//ietf-dtnma-agent/CTRL/inspect(//ietf-dtnma-agent/EDD/sw-version)'), rpt.source)
        # items of the report
        self.assertEqual([ari.LiteralARI('0.0.0')], rpt.items)
