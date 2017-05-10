import serial
import asyncio
import subprocess


class BTComm:
    def __init__(self, addr, loop, retries=3):
        self.addr = addr
        self.loop = loop
        self.retries = retries
        self.port = None
        self.reader = asyncio.StreamReader(loop=self.loop)

    def getport(self):
        btdump = subprocess.run(['system_profiler', 'SPBluetoothDataType'],
                                stdout=subprocess.PIPE).stdout
        lines = str(btdump.split(b'\n').strip(b' :'))
        del lines[:lines.index('Outgoing Serial Ports')]
        try:
            dex = lines.index(self.addr)
            return '/dev/tty.{}'.format(lines[dex-1])
        except ValueError:
            raise OSError('No port found for {}. Is the device paired?'.format(
                self.addr))
        
    def connect(self):
        print('Bluetooth connecting to {}'.format(self.addr))
        filename = self.getport()
        print('Opening {}'.format(filename))
        for n in range(self.retries):
            try:
                self.port = serial.Serial(filename)
            except OSError as ose:
                print('Bluetooth connection failed.', end=' ')
                if n > 0:
                    print('Retrying... ({} remaining)'.format(n), flush=True)
                else:
                    print('Aborting', flush=True)
                    raise ose
            else:
                break
        self.loop.add_reader(self.port.fd, lambda: self.reader.feed_data(
            self.port.read(self.port.in_waiting)))
        print('Bluetooth connection successful')

    def write(self, data):
        self.port.write(data)

    def close(self):
        self.loop.remove_reader(self.port.fd)
        self.port.close()
