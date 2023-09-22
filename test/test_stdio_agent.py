
import binascii
import io
import logging
import os
import re
import signal
import subprocess
import time
import threading
import queue
import unittest
import cbor2


OWNPATH = os.path.dirname(os.path.abspath(__file__))
LOGGER = logging.getLogger(__name__)


class Timeout(RuntimeError):
    ''' Represent a timeout for the CmdRunner class '''

class CmdRunner:
    def __init__(self, args):
        self.proc = None
        self._reader = None
        self._args = args
        self._stdin_lines = queue.Queue()
        self._stdout_lines = queue.Queue()

    def _fmt_args(self):
        return ' '.join([
            '"{}"'.format(arg.replace('"', '\\"')) for arg in self._args
        ])

    def start(self):
        if self.proc:
            return

        while not self._stdout_lines.empty():
            self._stdout_lines.get()

        LOGGER.debug('Starting process: %s', self._fmt_args())
        self.proc = subprocess.Popen(
            self._args, 
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            text=True
        )
        self._reader = threading.Thread(
            target=self._read_stdout, 
            args=[self.proc.stdout]
        )
        self._reader.start()
        self._writer = threading.Thread(
            target=self._write_stdin, 
            args=[self.proc.stdin]
        )
        self._writer.start()

    def stop(self, timeout=5):
        if not self.proc:
            return None

        if self.proc.returncode is None:
            LOGGER.debug('Stopping process: %s', self._fmt_args())
            self.proc.send_signal(signal.SIGINT)
            try:
                self.proc.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait(timeout=timeout)

        ret = self.proc.returncode
        self.proc = None
        LOGGER.debug('Stopped with exit code: %s', ret)
        
        self._reader.join()
        self._reader = None
        self._stdin_lines.put(None)
        self._writer.join()
        self._writer = None
        
        return ret

    def _read_stdout(self, stream):
        LOGGER.debug('Starting stdout thread')
        for line in iter(stream.readline, ''):
            LOGGER.debug('Got line: %s', line.strip())
            self._stdout_lines.put(line)
        LOGGER.debug('Stopping stdout thread')

    def _write_stdin(self, stream):
        LOGGER.debug('Starting stdin thread')
        while True:
            text = self._stdin_lines.get()
            if text is None:
                break
            LOGGER.debug('Sending text: %s', text)
            stream.write(text)
            stream.flush()
        LOGGER.debug('Stopping stdin thread')

    def wait_for_line(self, timeout=5):
        try:
            text = self._stdout_lines.get(timeout=timeout)
        except queue.Empty:
            raise Timeout()
        return text

    def wait_for_text(self, pattern, timeout=5):
        expr = re.compile(pattern)
        LOGGER.debug('Waiting for pattern "%s" ...', pattern)

        deadline_time = time.time_ns()/1e9 + timeout
        while True:
            remain_time = deadline_time - time.time_ns()/1e9
            if remain_time <= 0:
                break
            LOGGER.debug('Waiting for new line up to %s s', remain_time)
            try:
                text = self._stdout_lines.get(timeout=remain_time)
            except queue.Empty:
                raise Timeout()

            if expr.match(text) is not None:
                return text

    def send_stdin(self, text):
        self._stdin_lines.put(text)

class TestStdioAgent(unittest.TestCase):
    ''' Verify whole-agent behavior with the stdio_agent '''
    
    def setUp(self):
        path = os.path.abspath(os.path.join(OWNPATH, '..'))
        os.chdir(path)
        LOGGER.info('Working in %s', path)

        args = ['bash', 'run.sh', 'stdio_agent']
        self._agent = CmdRunner(args)
        
    def tearDown(self):
        self._agent.stop()

    def _start(self):
        ''' Spawn the process and wait for the startup READY message. '''
        self._agent.start()
        self.assertEqual('READY\n', self._agent.wait_for_text(r'^READY$'))

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

        self._agent.send_stdin('0x820052020081c11541050502252381871819410000\n')
        line = self._agent.wait_for_text(r'^0x')
        msg = cbor2.loads(binascii.a2b_hex(line[2:-1]))
        LOGGER.debug('Got msg %s', msg)
        
        dec = cbor2.decoder.CBORDecoder(io.BytesIO(msg[1]))
        items = []
        while True:
            try:
                items.append(dec.decode())
            except cbor2.types.CBORDecodeEOF:
                break
        LOGGER.debug('Got report %s', items)
