import asyncio
import cv2
import numpy as np
import argparse


class Vision:

    def __init__(self, loop, robotid, cam=0, focus=0, **kwargs):
        self.loop = loop
        self.robotid = int(kwargs['robot_tag'] or robotid)
        self.coords = kwargs['coords']

        self.running = False

        self.cap = cv2.VideoCapture(int(cam) if cam.isnumeric() else cam)
        assert self.cap.isOpened()  # Fail if camera not opened

        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

        if focus !='auto':
            self.cap.set(cv2.CAP_PROP_AUTOFOCUS, 0)
            self.cap.set(cv2.CAP_PROP_FOCUS, float(focus))
            print('Set focus to {}'.format(float(focus)))

        self.markers = cv2.aruco.getPredefinedDictionary(
            cv2.aruco.DICT_4X4_100)
        self.objects = {}

    async def run(self):
        self.running = True
        cv2.namedWindow("MSE430")

        while self.running:
            self.cap.grab()
            self.objects = {'time': self.loop.time()}
            _, frame = self.cap.retrieve()
            self.imagesize = frame.shape
            corners, ids, _ = cv2.aruco.detectMarkers(frame, self.markers)

            for n in range(len(corners)):
                label = 'robot' if int(ids[n]) == self.robotid else int(ids[n])
                self.objects[label] = self.process_aruco(corners[n][0])
            
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)
            if 'robot' in self.objects and self.coords == 'raw':
                point1 = tuple(map(int, self.objects['robot']['center']))
                point2 = (round(100 * self.objects['robot']['orientation'][0]
                                + point1[0]),
                          round(100 * self.objects['robot']['orientation'][1]
                                + point1[1]))
                cv2.arrowedLine(frame, point1, point2, (0, 0, 255), 2)
            cv2.imshow("MSE430", frame)
            if cv2.waitKey(1) == 27:  # Stop on ESC key press (not great)
                self.loop.stop()
            await asyncio.sleep(0.1)  # FIXME: probably not accurate?

    def stop(self):
        self.running = False

    def close(self):
        cv2.destroyAllWindows()
        self.cap.release()

    def process_aruco(self, corners):
        if self.coords == 'inverted':
            corners = np.array([(x, self.imagesize[0]-y) for x,y in corners])
        center = np.mean(corners, 0)
        facing = np.mean(corners[:2], 0)
        facing -= center
        facing /= np.linalg.norm(facing)
        return {'corners': corners.tolist(), 'orientation': facing.tolist(),
                'center': center.tolist()}
    
    @staticmethod
    def cli_arguments(parser):
        parser.add_argument('--robot-tag', help='Alternative marker on robot')
        parser.add_argument('--cam', default='0', help='Camera Path or number')
        parser.add_argument('--focus', default='0.0', help='Desired behavior '
                            'for camera focus, either \'auto\' or a number '
                            'representing the desired value (defaults to 0)')
        parser.add_argument('--coords', help='Coordinate system to be used '
                            'with camera', choices=['raw', 'inverted'], 
                            default='raw')
