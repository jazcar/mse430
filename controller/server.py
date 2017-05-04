import asyncio
#from robot import Robot
#from vision import Vision


class Server():

    def __init__(self, *args, **kwargs):
        self.loop = asyncio.get_event_loop()
        self.server = None
        
    def run(self):
        self.server = self.loop.run_until_complete(self.loop.create_server(
            lambda: ServerProtocol(self), port=55555))
        print('Serving on {}'.format(self.server.sockets[0].getsockname()))
        try:
            self.loop.run_forever()
        except KeyboardInterrupt:
            pass
        print('Exiting')
        self.close()  # Should this be here?

    def stop(self):
        self.loop.stop()

    def close(self):
        self.server.close()
        self.loop.run_until_complete(self.server.wait_closed())
        self.loop.close()

    def objects(self):
        return 'objects woo'

    def robot(self):
        return 'robots yay'

    def obstacles(self):
        return 'some obstacles'

    def setspeed(self, speed_a, speed_b):
        return 'speed: {:d}, {:d}'.format(int(speed_a), int(speed_b))

    def setpower(self, power_a, power_b):
        return 'power: {:d}, {:d}'.format(int(power_a), int(power_b))

    def setparam(self, name, value):
        raise NotImplementedError('Nope')

        
class ServerProtocol(asyncio.Protocol):
    def __init__(self, server):
        self.server = server
        self.transport = None
        self.commands = {
            'objects': self.server.objects,
            'robot': self.server.robot,
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
        self.transport.write((str(data)+'\n').encode())

    def data_received(self, data):
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
