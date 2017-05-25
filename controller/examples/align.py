import asyncio
import json
from math import atan2


def main(host='localhost', port=55555):

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

    def calc_angle(x, y):
        return atan2(-y, x)  # Reversed for upside-down y

    target_angle = calc_angle(0, -1)

    # Feedback loop
    try:
        while True:
            res = do('where robot')
            if 'orientation' in res:
                lost_count = 0
                angle = calc_angle(*res['orientation'])
                error = target_angle - angle
                do('speed {} {}'.format(round(-5*error), round(5*error)))
            else:
                lost_count += 1
                if lost_count > 10:
                    do('speed 0 0')
                
    except KeyboardInterrupt:
        print('Stopping')

    # Close the connection
    writer.close()
    loop.close()


if __name__ == '__main__':
    from sys import argv
    main(*argv[1:])
