import socket
import asyncio
import selectors


class BTComm:
    def __init__(self, addr):
        self.addr = addr
        self.sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM,
                                  socket.BTPROTO_RFCOMM)
        self.sock.setblocking(0)
        
    async def connect(self):
        self.sock.connect((self.addr, 1))
        selector = selectors.DefaultSelector()
        selector.register(self.sock, selectors.EVENT_WRITE)
        while not selector.select(0):
            await asyncio.sleep(0.05)
        selector.close()

    async def 
        

def reader():
    data

def btcomm(addr):
    pass
