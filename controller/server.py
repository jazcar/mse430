import asyncio
from robot import Robot
#from vision import Vision


class Server():

    TIMEOUT = 2.0
    
    def __init__(self, *args, **kwargs):
        self.loop = asyncio.get_event_loop()
        self.robot = Robot('MSE430-5', self.loop)
        self.server = None
        
    def run(self):
        self.server = self.loop.run_until_complete(self.loop.create_server(
            lambda: ServerProtocol(self), port=55555))
        print('Serving on {}'.format(self.server.sockets[0].getsockname()))
        try:
            self.loop.run_until_complete(self.robot.connect())
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

    def objects(self):
        return 'objects woo'

    def getrobot(self):
        return 'robots yay'

    def obstacles(self):
        return 'some obstacles'

    def setspeed(self, speed_a, speed_b):
        speed_a = int(speed_a)
        speed_b = int(speed_b)
        asyncio.ensure_future(self.robot.set_speed(speed_a, speed_b),
                              loop=self.loop)
        return 'speed: {:d}, {:d}'.format(speed_a, speed_b)

    def setpower(self, power_a, power_b):
        power_a = int(power_a)
        power_b = int(power_b)
        asyncio.ensure_future(self.robot.set_power(power_a, power_b),
                              loop=self.loop)
        return 'power: {:d}, {:d}'.format(power_a, power_b)

    def setparam(self, name, value):
        raise NotImplementedError('Nope')

        
class ServerProtocol(asyncio.Protocol):
    def __init__(self, server):
        self.server = server
        self.transport = None
        self.commands = {
            'objects': self.server.objects,
            'robot': self.server.getrobot,
            'obstacles': self.server.obstacles,
            'setspeed': self.server.setspeed,
            'setpower': self.server.setpower,
            'setparam': self.server.setparam,
        }
        
    def connection_made(self, transport):
        self.transport = transport
        print('Connection made')
        self.write('MSE430 Server (CS 470 BYU)')
        
    def connection_lost(self, exc):
        print('Connection lost')
        self.server.stop()
        
    def write(self, data):
        data = str(data)
        print('Sent: {}'.format(data))
        self.transport.write((data+'\n').encode())

    def data_received(self, data):
        if not data:
            return
        print('Received: {}'.format(data.strip().decode()))
        cmd = data.split()[0].lower().decode()
        args = data.split()[1:]
        try:
            self.write(self.commands[cmd](*args))
        except KeyError:
            self.write('Unkown command: {}'.format(cmd))
        except Exception as e:
            self.write('{} occurred while handling "{}": {}'.format(
                type(e).__name__, data.strip().decode(), str(e)))
            raise e
            
    def eof_received(self):
        return False


if __name__ == '__main__':
    Server().run()
