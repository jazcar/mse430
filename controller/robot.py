import asyncio
from struct import pack, unpack
from btcomm import BTComm


class Robot:
    def __init__(self, name, loop):
        self.name = 'MSE430-' + name.split('-')[-1]
        self.loop = loop
        self.num = (lambda x: int(x) if x.isnumeric()
                    else int(x, 16))(self.name.split('-')[-1])
        try:
            self.addr = ADDRESSES[self.name]
        except KeyError:
            raise ValueError('Unrecognized name: {} interpreted as {}'.format(
                name, self.name))
        self.btcomm = BTComm(self.addr, self.loop)

    async def connect(self):
        self.btcomm.connect()

    def close(self):
        self.btcomm.close()
    
    async def set_power(self, power_a, power_b):
        message = pack('<chh', b'P', power_a, power_b)
        self.btcomm.write(message)
        res = await self.btcomm.reader.readexactly(1)
        if not res == b'P':
            raise Exception('Unexpected response to set_power "{}"'.format(
                res.decode()))

    async def get_power(self):
        self.btcomm.write(b'p')
        res = await self.btcomm.reader.readexactly(5)
        tag, power_a, power_b = unpack('<chh', res)
        if tag == b'p':
            return power_a, power_b
        else:
            raise Exception('Unexpected response to get_power "{}"'.format(
                tag.decode()))

    async def set_speed(self, speed_a, speed_b):
        message = pack('<chh', b'S', speed_a, speed_b)
        self.btcomm.write(message)
        res = await self.btcomm.reader.readexactly(1)
        if not res == b'S':
            raise Exception('Unexpected response to set_speed "{}"'.format(
                res.decode()))

    async def get_speed(self):
        self.btcomm.write(b's')
        res = await self.btcomm.reader.readexactly(5)
        tag, speed_a, speed_b = unpack('<chh', res)
        if tag == b's':
            return speed_a, speed_b
        else:
            raise Exception('Unexpected response to get_speed "{}"'.format(
                tag.decode()))

    PARAM_SCALES = {'kp': 10, 'ki': 10, 'kd': 10, 'ic': 0, 'id': 0, 'ms': 0}
    
    async def set_param(self, name, value):
        if name in self.PARAM_SCALES:
            message = pack('<ccci', b'K', *[c.encode() for c in name],
                           round(value * (1 << self.PARAM_SCALES[name])))
            self.btcomm.write(message)
            res = await self.btcomm.reader.readexactly(1)
            if not res == b'K':
                raise Exception('Unexpected response to set_param "{}"'.format(
                    res.decode()))
        else:
            raise ValueError('Invalid parameter name')

    async def get_param(self, name):
        if name in self.PARAM_SCALES:
            message = pack('<ccc', b'k', *[c.encode() for c in name])
            self.btcomm.write(message)
            res = await self.btcomm.reader.readexactly(5)
            tag, param = unpack('<ci', res)
            if tag == b'k':
                return param
            else:
                raise Exception('Unexpected response to get_param '
                                '"{}"'.format(tag.decode()))
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
