import serial
import asyncio


class BTComm:
    def __init__(self, addr, loop, retries=3):
        self.addr = addr
        self.loop = loop
        self.retries = retries
        
    def connect(self):
        print('Bluetooth connecting to {}'.format(self.addr))
        for n in range(self.retries):
            try:
                coro = serial.aio.create_serial_connection
                


class RobotProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
    def connection_lost(self, exc):
        pass
    def write(self, 
    def data_received(self, data):
        print(data)
        




raise NotImplementedError('Still working on it')
