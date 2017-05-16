import asyncio
import cv2
import numpy as np


class Vision:

    def __init__(self, loop, robotid, cam=1):
        self.loop = loop
        self.robotid = robotid
        self.cap = cv2.VideoCapture(cam)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)
        self.markers = cv2.aruco.getPredefinedDictionary(
            cv2.aruco.DICT_4X4_100)
        self.frametime = 0
        self._objects = {}

    async def run(self):
        cv2.namedWindow("Lookit")
        while True:
            res, frame = self.cap.read()
            self.frametime = self.loop.time()
            corners, ids, params = cv2.aruco.detectMarkers(frame, self.markers)
            self.objects = {int(ids[n]): corners[n][0].tolist() for n in
                             range(len(corners))}
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)
            point1 = self.robot[0]
            if point1:
                point2 = tuple((100 * np.array(self.robot[1]) + point1).astype(
                    int))
                point1 = tuple(map(int, point1))
                cv2.arrowedLine(frame, point1, point2, (0, 0, 255), 2)
            cv2.imshow("Lookit", frame)
            cv2.waitKey(1)
            await asyncio.sleep(0.1)
            
    @property
    def objects(self):
        return self._objects

    @objects.setter
    def objects(self, value):
        self._objects = value
        self._obstacles = None
        self._robot = None

    OBSTACLE_START=20

    @property
    def obstacles(self):
        if self._obstacles is None:
            self._obstacles = {k: v for k, v in self.objects.items()
                               if k >= self.OBSTACLE_START}
        return self._obstacles
    
    @property
    def robot(self):
        if self._robot is None:
            if self.robotid in self.objects:
                points = np.array(self.objects[self.robotid])
                center = np.mean(points, 0)
                facing = np.mean(points[:2], 0)
                facing -= center
                facing /= np.linalg.norm(facing)
                self._robot = [center.tolist(), facing.tolist()]
            else:
                self._robot = [None, None]
        return self._robot
