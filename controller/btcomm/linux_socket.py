import socket
import asyncio


class BTComm:
    def __init__(self, addr, loop):
        self.addr = addr
        self.loop = loop
        self.sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM,
                                  socket.BTPROTO_RFCOMM)
        self.queue = asyncio.Queue(loop=loop)

    async def connect(self):
        print('Connecting to {}'.format(self.addr))
        self.sock.connect((self.addr, 1))
        self.sock.setblocking(0)
        # selector = asyncio.selectors.DefaultSelector()
        # selector.register(self.sock, asyncio.selectors.EVENT_WRITE)
        # while not selector.select():
        #     print('Bluetooth waiting...')
        #     asyncio.sleep(0.1)
        # selector.close()
        self.loop.add_reader(self.sock, lambda: self.queue.put_nowait(
            self.sock.recv(100)))
        print('Bluetooth connection successful')

    def close(self):
        self.loop.remove_reader(self.sock)
        self.sock.close()

    def write(self, data):
        print('Robot send: {}'.format(data.hex()))
        self.sock.send(data)
        # Wait for complete?
