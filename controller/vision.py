import cv2
import numpy as np
from threading import Lock


class Vision:

    def __init__(self, cam=0):
        self.cap = cv2.VideoCapture(cam)
        self.lock = Lock()

    def run(self):
        while True:
            res, frame = self.cap.read()
            

    @property
    def robots(self):
        if self.lock.acquire(False):
            try:
                return None  # Assemble & copy the data
            finally:
                self.lock.release()
        else:
            return None

    @robots.setter
    def robots(self, value):
        if self.lock.acquire():
            try:
                pass # update stuff
            finally:
                self.lock.release()
        else:
            raise Exception('Somehow failed to acquire lock in robots setter')
