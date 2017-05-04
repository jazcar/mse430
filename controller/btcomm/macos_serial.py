import serial
import asyncio


class RobotProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
    def data_received(self, data):
        pass
    def connection_lost(self, exc):
        pass

def btcomm(*, addr=None, name=None, loop=None):
    if not name:
        if addr:
            pass  # Derive name from addr
        else:
            raise ValueError('btcomm needs a MAC address or a name')
    coro = serial.aio.create_serial_connection


raise NotImplementedError('Still working on it')
