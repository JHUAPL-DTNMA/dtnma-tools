
import binascii
import io
import logging
import os
import signal
import time
import unittest
import cbor2
from ace import (AdmSet, ari_text, ari_cbor, nickname)
from helpers.runner import CmdRunner, Timeout


OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)


class TestStdioAgent(unittest.TestCase):
    ''' Verify whole-agent behavior with the stdio_agent '''
    
    def setUp(self):
        path = os.path.abspath(os.path.join(OWNPATH, '..'))
        os.chdir(path)
        LOGGER.info('Working in %s', path)

        args = ['bash', 'run.sh', 'stdio_agent']
        self._agent = CmdRunner(args)

        # ADM handling
        adms = AdmSet()
        adms.load_from_dir(os.path.join(OWNPATH, '..', 'adms'))
        self._adms = adms

    def tearDown(self):
        self._agent.stop()

    def _start(self):
        ''' Spawn the process and wait for the startup READY message. '''
        self._agent.start()
        self.assertEqual('READY\n', self._agent.wait_for_text(r'^READY$'))

    def _ari_to_cbor(self, text):
        nn_func = nickname.Converter(nickname.Mode.TO_NN, self._adms, must_nickname=True)

        ari = ari_text.Decoder().decode(io.StringIO(text))
        nn_func(ari)
        buf = io.BytesIO()
        ari_cbor.Encoder().encode(ari, buf)
        return buf.getvalue()

    def _ari_from_cbor(self, databuf):
        nn_func = nickname.Converter(nickname.Mode.FROM_NN, self._adms, must_nickname=True)

        ari = ari_cbor.Decoder().decode(databuf)
        nn_func(ari)
        textbuf = io.StringIO()
        ari_text.Encoder().encode(ari, textbuf)
        return textbuf.getvalue()

    def _send_exec(self, targets):
        ''' Send an execution message with a number of target ARIs. '''
        msg = [0]
        for tgt in targets:
            msg.append(b'\x02\x00\x81' + tgt)
        hexdata = binascii.b2a_hex(cbor2.dumps(msg))
        line = '0x' + hexdata.decode('ascii')
        LOGGER.debug('Sending line %s', line)
        self._agent.send_stdin(line + '\n')

    def _wait_report(self):
        line = self._agent.wait_for_text(r'^0x')
        LOGGER.debug('Received line %s', line)
        msg = cbor2.loads(binascii.a2b_hex(line[2:-1]))
        LOGGER.debug('Got msg %s', msg)

        buf = io.BytesIO(msg[1])
        dec = cbor2.CBORDecoder(buf)
        head = dec.decode()
        self.assertEqual(0x01, head)
        mgrs = dec.decode()
        self.assertEqual(list, type(mgrs))

        # the report set is an array
        rptset_head = buf.read(1)[0]
        self.assertEqual(4, rptset_head >> 5)
        rptset_count = rptset_head & 0x1F
        self.assertLess(0, rptset_count)
        self.assertGreater(24, rptset_count)

        for ix in range(rptset_count):
            # each report is an array
            rpt_head = buf.read(1)[0]
            self.assertEqual(4, rpt_head >> 5)
            rpt_count = rpt_head & 0x1F
            self.assertIn(rpt_count, {2, 3})
            LOGGER.debug('Report set %s %s', rptset_count, rpt_count)

            rptt = self._ari_from_cbor(buf)
            LOGGER.debug('Report template %s', rptt)

            if rpt_count == 3:
                ts = dec.decode()
                LOGGER.debug('Report timestamp %s', ts)

            # FIXME: this is messy to decode
            tnvc = ari_cbor.Decoder()._decode_tnvc(dec)
            for item in tnvc:
                LOGGER.info('Item %s', item)


    def test_start_terminate(self):
        self._start()
        self._agent.wait_for_text(r'^0x')

        LOGGER.debug('Sending SIGINT')
        self._agent.proc.send_signal(signal.SIGINT)
        self.assertEqual(0, self._agent.proc.wait(timeout=5))
        self.assertEqual(0, self._agent.proc.returncode)

    def test_start_close(self):
        self._start()
        self._agent.wait_for_text(r'^0x')

        LOGGER.debug('Closing stdin')
        self._agent.proc.stdin.close()
        self.assertEqual(0, self._agent.proc.wait(timeout=5))
        self.assertEqual(0, self._agent.proc.returncode)

    def test_cmd(self):
        self._agent.start()
        self._agent.wait_for_text(r'^0x')

        tgt = self._ari_to_cbor(
          'ari:/IANA:amp_agent/CTRL.gen_rpts([ari:/IANA:amp_agent/RPTT.full_report],[])'
        )
        self.assertEqual(bytes, type(tgt))
        self.assertEqual(
            b'c11541050502252381871819410000',
            binascii.b2a_hex(tgt)
        )
        self._send_exec([tgt])
        rpt = self._wait_report()
