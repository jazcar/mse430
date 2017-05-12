import asyncio
import json
from time import sleep


def main(host, port):
    loop = asyncio.get_event_loop()
    reader, writer = loop.run_until_complete(
        asyncio.open_connection(host, port))
    print(reader.readline())

    def do(command):
        print('>>>', command)
        writer.write(command.strip().encode())
        res = loop.run_until_complete(reader.readline()).decode().strip()
        print('<<<', res)
        print()
        return res
    
    for _ in range(3):
        do('setspeed 20 -20')
        sleep(3)

        do('setspeed 0 0')
        sleep(1)

        do('setspeed -40 40')
        sleep(2)

        do('setpower 0 0')
        sleep(1)

    writer.close()


if __name__ == '__main__':
    from sys import argv
    main(*argv[1:])
