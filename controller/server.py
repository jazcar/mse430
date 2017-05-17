import asyncio
import json
from robot import Robot
from vision import Vision


class Server():

    def __init__(self, robot, *args, **kwargs):
        self.loop = asyncio.get_event_loop()
        self.robot = Robot(robot, self.loop)
        self.vision = Vision(self.loop, self.robot.num)
        self.server = None
        self.commands = {
            'objects': self.objects,
            'robot': self.getrobot,
            'obstacles': self.obstacles,
            'setspeed': self.setspeed,
            'getspeed': self.getspeed,
            'setpower': self.setpower,
            'getpower': self.getpower,
            'setparam': self.setparam,
            'getparam': self.getparam,
            'help': self.help,
            '?': self.help
        }
        
    def run(self):
        try:
            self.loop.run_until_complete(self.robot.connect())
            asyncio.ensure_future(self.vision.run())
            self.server = self.loop.run_until_complete(
                self.loop.create_server(lambda: ServerProtocol(self),
                                        port=55555))
            print('Serving on {}'.format(
                self.server.sockets[0].getsockname()))
            self.loop.run_forever()
        except KeyboardInterrupt:
            pass
        print('Exiting')
        self.close()  # Should this be here?

    def stop(self):
        self.loop.stop()

    def close(self):
        self.robot.close()
        self.server.close()
        self.loop.run_until_complete(self.server.wait_closed())
        self.loop.close()

    async def objects(self):
        """objects -- Return positions of all tracked objects in view"""

        return {'objects': self.vision.objects}

    async def getrobot(self):
        """robot -- Return position of the robot"""

        return {'robot': self.vision.robot}

    async def obstacles(self):
        """obstacles -- Return positions of all obstacles in view"""

        return {'obstacles': self.vision.obstacles}

    async def setspeed(self, speed_a, speed_b):
        """setspeed speed_a speed_b -- Set motor target speed

        Top speed is somewhere around 80, but there is an artificial
        limit on the robot so it will go in straight lines at top
        speed. This value can be changed as the parameter ms.
        """
        speed_a = int(speed_a)
        speed_b = int(speed_b)
        await self.robot.set_speed(speed_a, speed_b)
        return {'res': 'OK'}

    async def getspeed(self):
        """getspeed -- Returns current motor speeds"""
        speeds = await self.robot.get_speed()
        return {'speed_a': speeds[0], 'speed_b': speeds[1]}

    async def setpower(self, power_a, power_b):
        """setpower power_a power_b -- Directly set motor power
 
        Motor power is (currently) between -512 and 512. Power doesn't
        correspond direclty with speed.
        """
        power_a = int(power_a)
        power_b = int(power_b)
        await self.robot.set_power(power_a, power_b)
        return {'res': 'OK'}

    async def getpower(self):
        """getpower -- Returns the power being applied to the motors

        Motor power is (currently) between -512 and 512. Power doesn't
        correspond directly with speed, but this might help see what
        the PID is doing internally.
        """
        powers = await self.robot.get_power()
        return {'power_a': powers[0], 'power_b': powers[1]}

    async def getparam(self, name):
        """getparam name -- Read a value from the robot

        Parameters:
        kp: Proportional term gain
        ki: Integral term gain
        kd: Derivative term gain
        ic: Integral term cap
        id: Integral term domain (integral ignored if error > id)
        ms: Max speed
        """
        value = await self.robot.get_param(name)
        return {name: value}
        
    async def setparam(self, name, value):
        """setparam name value -- Configure the robot

        Parameters:
        kp: Proportional term gain
        ki: Integral term gain
        kd: Derivative term gain
        ic: Integral term cap
        id: Integral term domain (integral ignored if error > id)
        ms: Max speed
        """
        await self.robot.set_param(name, float(value))
        return {'res': 'OK'}

    async def help(self, command=None):
        """help | ? [command] -- Display all commands or details of one"""
        if command:
            return self.commands[command.lower()].__doc__
        else:
            return '\n'.join([x.__doc__.split('\n')[0]
                              for x in self.commands.values()])

    async def handle_command(self, data, write): 
        cmd = data.split()[0].lower()
        args = data.split()[1:]
        if cmd in self.commands:
            try:
                res = await self.commands[cmd](*args)
                write(json.dumps(res) if type(res) is dict else res)
            except Exception as e:
                write('{} occurred while handling "{}": {}'.format(
                    type(e).__name__, data, str(e)))
                raise e
        else:
            write('Unknown command: "{}"'.format(cmd))


class ServerProtocol(asyncio.Protocol):
    def __init__(self, server):
        self.server = server
        self.transport = None
        
    def connection_made(self, transport):
        self.transport = transport
        print('Connection made')
        self.write('MSE430 Server (CS 470 BYU)')
        
    def connection_lost(self, exc):
        print('Connection lost')
        self.transport = None
        
    def write(self, data):
        data = str(data)
        print('Sent: {}'.format(data))
        self.transport.write((data+'\n').encode())

    def data_received(self, data):
        data = data.strip().decode()
        if not data:
            return
        print('Received: {}'.format(data))
        asyncio.ensure_future(self.server.handle_command(data, self.write))

    def eof_received(self):
        return False


if __name__ == '__main__':
    from sys import argv
    Server(argv[1]).run()
