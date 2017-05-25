import asyncio
import json
from math import atan2, cos
from time import sleep


def main(host='localhost', port=55555):
    """In this example, I direct the robot to orient itself "up" as in
    align.py, and also to drive to the center line of the image.  With a
    steady hand, you can guide it along with the camera.

    """

    # Setup asyncio and open the connection
    loop = asyncio.get_event_loop()
    reader, writer = loop.run_until_complete(
        asyncio.open_connection(host, port))

    # Simple method to process a command. It's defined inside the main
    # method so reader and writer are in its namespace
    def do(command):
        print('>>>', command)

        # Send the command -- write() expect bytes, so we call encode()
        writer.write(command.strip().encode())

        # This is a lot in one line, but here's what's happening:
        #   reader.readline() starts an asyncio coroutine that reads
        #     until it gets a complete line (ending with \n) and then
        #     returns that coroutine.
        #   run_until_complete() runs the coroutine until it terminates
        #   decode() turns the bytes object into a string
        #   strip() removes whitespace at the beginning or end, like \n
        res = loop.run_until_complete(reader.readline()).decode().strip()
        print('<<<', res)
        try:
            # The response is a json encoded string, so we decode it
            res = json.loads(res)
        except json.decoder.JSONDecodeError:
            # If an error occurred, handle it gracefully
            print('Error decoding response')
            res = {}
        print()
        # Return the resulting dict
        return res

    def calc_angle(x, y):
        return atan2(-y, x)  # Reversed for upside-down y

    def dot_product(a, b):
        return sum(map(lambda x: x[0]*x[1], zip(a, b)))

    angle_target = calc_angle(0, -1)
    position_target = 1080 / 2

    # Running loop
    try:
        while True:
            # Get the position of the robot. The result should be a
            # dictionary with four corners, a center, an orientation
            # vector, and a timestamp that isn't very useful yet.
            res = do('where robot')

            # Check that the operation succeeded before proceeding
            if 'orientation' in res:
                lost_count = 0
                angle = calc_angle(*res['orientation'])

                # Calculate an error in the angle, which gives a
                # direction (sign) to turn and also an idea of what
                # speed to go (the magnitude). Note that this is the
                # same as the P term in a PID controller. A PD or PID
                # controller would do even better (hint hint).
                angle_error = angle_target - angle

                # Also calculate an error in the position; we want it
                # to be in the center of the image (on a centered
                # horizontal line). I compensate for the reversed
                # coordinates by negating the result.
                position_error = -(position_target - res['center'][1])

                # These two errors tell us how much we want to turn
                # and how much we want to move. We can use both of
                # these at the same time if we're clever.
                turn = 5 * angle_error
                drive = 0.05 * cos(angle_error) * position_error
                do('speed {} {}'.format(round(drive-turn), round(drive+turn)))

            else:
                # Sometimes the camera fails to find the robot, and it
                # will return a mostly empty response. I handle this
                # by trying a number of times, and if it fails too
                # much, stopping the robot (in case its movement is
                # blurring the image).
                lost_count += 1
                if lost_count == 10:
                    do('speed 0 0')

    # Stop with ^C
    except KeyboardInterrupt:
        print('\nStopping')

    # Close the connection (wait for the last command to return)
    sleep(0.5)
    writer.close()
    loop.close()


if __name__ == '__main__':
    from sys import argv
    main(*argv[1:])
