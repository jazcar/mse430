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
        self.futures = []
        self.commands = {
            'where': self.where,
            'speed': self.speed,
            'power': self.power,
            'param': self.param,
            'shutdown': self.shutdown,
            'help': self.help,
        }
        
    def run(self):
        try:
            self.loop.run_until_complete(self.robot.connect())
            self.futures.append(asyncio.ensure_future(self.vision.run()))
            self.server = self.loop.run_until_complete(
                self.loop.create_server(lambda: ServerProtocol(self),
                                        port=55555))
            print('Serving on {}'.format(
                self.server.sockets[0].getsockname()))
            self.loop.run_forever()
        except KeyboardInterrupt:
            pass
        print('\nExiting')
        self.stop()

    def stop(self):
        self.robot.stop()
        self.vision.stop()
        self.loop.run_until_complete(self.futures[0])
        self.server.close()
        self.loop.run_until_complete(self.server.wait_closed())
        self.loop.stop()
        self.robot.close()
        self.loop.close()

    async def where(self):
        """where -- return the locations of objects tracked by the camera
        
        The dictionary of results can be extensive, but it's fairly
        self-explanatory.

        """

        return self.vision.objects

    async def speed(self, *args):
        """speed [speed_a speed_b] -- Get or set motor speed

        With arguments (speed_a and speed_b), this will set targets
        for the speed controller on the robot. Without arguments it
        can be used to just check on the actual speed. This command
        will always return the current speeds of both motors.

        Top speed is somewhere around 80, but there is an artificial
        limit on the robot so it will go in straight lines at top
        speed. This value can be changed as the parameter ms.

        """
        if len(args) in [0, 2]:
            res = await self.robot.speed(tuple(map(int, args)))
        else:
            raise TypeError('speed() takes 0 or 2 arguments ({} given)'.format(
                len(args)))
        return res

    async def power(self, *args):
        """power [power_a power_b] -- Directly get or set motor power

        Motor power is (currently) between -512 and 512. Power doesn't
        correspond directly with speed, but this might help to see
        what the PID is doing internally.

        """
        if len(args) in [0, 2]:
            res = await self.robot.power(tuple(map(int, args)))
        else:
            raise TypeError('power() takes 0 or 2 arguments ({} given)'.format(
                len(args)))
        return res

    async def param(self, name, value=None):
        """param name [value] -- Get or set controller parameters on robot

        If a value is provided, the specified parameter will be set to
        that value. This command always returns the actual value of
        the parameter as well. Due to fixed-point encodings, this may
        sometimes be different than the value passed to it.

        Parameters:
        kp: Proportional term gain
        ki: Integral term gain
        kd: Derivative term gain
        ic: Integral term cap
        ms: Max speed

        """
        return await self.robot.param(name, value and float(value))

    async def help(self, command=None):
        """help [command] -- Display all commands or show details of one"""
        if command:
            return self.commands[command.lower()].__doc__
        else:
            return '\n'.join([x.__doc__.split('\n')[0] for x in
                              self.commands.values()])

    async def shutdown(self):
        """shutdown -- Shutdown the server

        The server will keep running after a client disconnects, but
        this command will shut it down.

        """
        self.loop.call_soon(self.loop.stop)
        return 'Shutting down'
        
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
