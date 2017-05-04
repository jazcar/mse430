import asyncio
from struct import pack, unpack
from btcomm import BTComm


class Robot:
    def __init__(self, name, loop):
        self.name = 'MSE430-' + name.split('-')[-1]
        self.loop = loop
        try:
            self.addr = ADDRESSES[name]
        except KeyError:
            raise ValueError('Unrecognized name: {} interpreted as {}'.format(
                name, self.name))
        self.btcomm = BTComm(self.addr, self.loop)

    async def connect(self):
        await self.btcomm.connect()
        asyncio.ensure_future(self.read_task(), loop=self.loop)

    def close(self):
        self.btcomm.close()
    
    async def read_task(self):
        data = b''
        while True:
            data += await self.btcomm.queue.get()
            print(data)
            data = b''

    async def set_power(self, power_a, power_b):
        message = pack('<chh', b'P', power_a, power_b)
        self.btcomm.write(message)

    async def set_speed(self, speed_a, speed_b):
        message = pack('<chh', b'S', speed_a, speed_b)
        self.btcomm.write(message)

    def get_speed(self):
        return 0, 0

    def set_param(self, name, value):
        raise NotImplementedError()


ADDRESSES = {
    'MSE430-0': '20:16:01:20:14:43',
    'MSE430-2': '20:15:12:28:78:04',
    'MSE430-5': '20:16:01:18:86:01'
    }
