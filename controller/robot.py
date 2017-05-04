from btcomm import BTcomm
from struct import pack, unpack

class Robot:
    def __init__(self, name, loop):
        self.name = 'MSE430-' + name.split('-')[-1]
        self.loop = loop
        try:
            self.addr = ADDRESSES[name]
        except KeyError:
            raise ValueError('Unrecognized name: {} interpreted as {}'.format(
                name, self.name))
        self.btcomm = bt

    def connect(self):
        pass
    
    def set_power(self, power_a, power_b):
        message = pack('<chh', b'P', power_a, power_b)

    def set_speed(self, speed_a, speed_b):
        message = pack('<chh', b'S', speed_a, speed_b)

    def get_speed(self):
        return 0, 0

    def set_param(self, name, value):
        raise NotImplementedError()
