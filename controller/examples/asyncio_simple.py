import asyncio
import json


def main(host, port):

    # Setup asyncio and open the connection
    loop = asyncio.get_event_loop()
    reader, writer = loop.run_until_complete(
        asyncio.open_connection(host, port))

    # Simple method to process a command. It's defined inside the main
    # method so reader and writer are in its namespace
    def do(command):
        print('>>>', command)
        writer.write(command.strip().encode())
        res = loop.run_until_complete(reader.readline()).decode().strip()
        print('<<<', res)
        try:
            res = json.loads(res)
        except json.decoder.JSONDecodeError:
            print('Error decoding response')
            res = None
        print()
        return res

    # Spin around
    do('speed 20 -20')
    loop.run_until_complete(asyncio.sleep(1.5))

    do('speed 0 0')
    loop.run_until_complete(asyncio.sleep(0.5))

    do('speed -40 40')
    loop.run_until_complete(asyncio.sleep(1.0))

    do('power 0 0')

    # Close the connection
    writer.close()


if __name__ == '__main__':
    from sys import argv
    main(*argv[1:])
