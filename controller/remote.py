#!/usr/bin/env python3

from glob import glob
from sys import exit, stderr
from time import sleep
from struct import pack
from platform import system


class Remote:
    def __init__(self, app=None):
        self.app = app
        if system() == 'Darwin':
            dev_files = glob('/dev/tty.HC-06*')
            if len(dev_files) is 1:
                print('Opening port to robot at', dev_files[0], end='')
                self.robot = open(dev_files[0], 'w+b')
                print(' Done')
            elif len(dev_files) > 1:
                print('More than one robot found. Which one do you want?',
                      file=stderr)
                exit(len(dev_files))
            else:
                print('Could not find a usbmodem file for the lights.')
                exit(1)
        # elif system() == 'Linux':
        #     print('Opening port to robot at /dev/tty????', end='')
        #     exit(1)
        #     self.robot = open('/dev/tty????', 'wb')
        #     print(' Done')
        else:
            print('System configuration ', system(), ' is not compatible.')
            exit(1)

    def set_power(self, power):
        if type(power) is tuple and len(power) == 2:

            if not (-2 ** 15 < power[0] < 2 ** 15 and
                    -2 ** 15 < power[1] < 2 ** 15):
                return False

            message = pack('<chh', b'P', power[0], power[1])

            print('[SEND              ] ', message.hex())
            if self.app is not None:
                self.app.a_cell.text = str(power[0])
                self.app.b_cell.text = str(power[1])
            self.robot.write(message)
            self.robot.flush()

    def shutdown(self):
        self.set_power((0, 0))
        self.robot.close()


if __name__ == '__main__':
    r = Remote()
    while True:
        r.set_power((500, -500))
        sleep(5)
        r.set_power((-500, 500))
        sleep(5)
        r.set_power((100, 100))
        sleep(5)
        r.set_power((500, 500))
        sleep(5)
    r.shutdown()
