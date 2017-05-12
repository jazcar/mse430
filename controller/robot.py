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
        self.speeds = {'speed_a': 0, 'speed_b': 0, 'time': None}

    async def connect(self):
        self.btcomm.connect()
        asyncio.ensure_future(self.reader(), loop=self.loop)

    def close(self):
        self.btcomm.close()
    
    async def reader(self):
        data = b''
        while True:
            tag = await self.btcomm.reader.readexactly(1)
            if tag == b'S':
                speeds = unpack('<hh', await
                                self.btcomm.reader.readexactly(4))
                self.speeds['speed_a'] = speeds[0]
                self.speeds['speed_b'] = speeds[1]
                print('{:3d}, {:3d}'.format(speeds[0], speeds[1]))
            else:
                print('Robot: Unexpected character {} received'.format(tag))

    def set_power(self, power_a, power_b):
        message = pack('<chh', b'P', power_a, power_b)
        self.btcomm.write(message)

    def set_speed(self, speed_a, speed_b):
        message = pack('<chh', b'S', speed_a, speed_b)
        self.btcomm.write(message)

    PARAM_SCALES = {'kp': 8, 'ki': 8, 'kd': 8, 'ic': 0, 'id': 0, 'ms': 0}
    
    def set_param(self, name, value):
        if name in self.PARAM_SCALES:
            message = pack('<ccch', b'K', *[c.encode() for c in name],
                           round(value * (1 << self.PARAM_SCALES[name])))
            self.btcomm.write(message)
        else:
            raise ValueError('Invalid parameter name')


ADDRESSES = {
    'MSE430-0': '20:16:01:20:14:43',
    'MSE430-1': '20:15:12:22:05:59',
    'MSE430-2': '20:15:12:28:78:04',
    'MSE430-3': '20:15:12:21:90:81',
    'MSE430-4': '20:16:01:19:72:73',
    'MSE430-5': '20:16:01:18:86:01'
    }
