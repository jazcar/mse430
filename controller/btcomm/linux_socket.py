import socket
import asyncio


class BTComm:
    def __init__(self, addr, loop, retries=3):
        self.addr = addr
        self.loop = loop
        self.retries = retries
        self.sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM,
                                  socket.BTPROTO_RFCOMM)
        self.reader = asyncio.StreamReader(loop=self.loop)

    async def connect(self):
        print('Bluetooth connecting to {}'.format(self.addr))
        for n in range(self.retries, -1, -1):
            try:
                self.sock.connect((self.addr, 1))
            except OSError:
                print('Bluetooth connection failed.', end=' ')
                if n > 0:
                    print('Retrying... ({} remaining)'.format(n), flush=True)
                    self.sock.close()
                    self.sock = socket.socket(socket.AF_BLUETOOTH,
                                              socket.SOCK_STREAM,
                                              socket.BTPROTO_RFCOMM)
                else:
                    print('Aborting', flush=True)
                    raise OSError
            else:
                break
        self.sock.setblocking(0)
        self.loop.add_reader(self.sock, lambda: self.reader.feed_data(
            self.sock.recv(100)))
        print('Bluetooth connection successful')

    def close(self):
        self.loop.remove_reader(self.sock)
        self.sock.close()

    def write(self, data):
        print('Robot send: {}'.format(data.hex()))
        self.sock.send(data)
        # Wait for complete?
