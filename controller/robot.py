#!/usr/bin/env python3

import bluetooth as bt
from struct import pack


class Robot:
    def __init__(self, dev, log=False):
        self.socket = None
        self.addr, self.name = *dev
        if self.name[:7] != 'MSE430-':
            raise ValueError('Invalid device name', self.name)
        self.log = log

    def connect(self):
        while not self.socket:
            try:
                if self.log:
                    print('[CONNECT ]', dev, flush=True)
                self.socket = bt.BluetoothSocket(bt.RFCOMM)
                self.socket.connect((dev, 1))
            except:  # !!!
                self.socket.close()
                self.socket = None

    def set_power(self, power):
        if type(power) is not tuple or len(power) is not 2:
            raise ValueError(
                'Argument \'power\' must be a tuple of length 2')
            try:
                message = pack('<chh', b'P', power[0], power[1])
            except struct.error:
                raise ValueError(
                    'Power values must be in range [-2**15, 2**15)')
            if self.log:
                print('[SEND    ]', message.hex())
            self.socket.send(message)

    def shutdown(self):
        if self.log:
            print('[SHUTDOWN]')
        self.set_power((0, 0))
        self.socket.close()

    @staticmethod
    def discover(repeat=None, verbose=False):
        devs = []
        if verbose:
            print('Searching for robots...', end='', flush=True)
        while not devs:
            devs = list(filter(lambda x: 'MSE430' in x[1],
                               bt.discover_devices(lookup_names=True)))
            if repeat:
                repeat -= 1
                if repeat == 0:
                    break
        if verbose:
            print(devs)
        return devs


def main():
    dev = Robot.discover(verbose=True)[0]
    robot = Robot(dev, log=True)
    robot.connect()

    # Now what??


if __name__ == '__main__':
    main()
