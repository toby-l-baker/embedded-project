#!/usr/bin/env python3
import struct
import time
import keyboard
import argparse
from getpass import getpass
from bluepy.btle import Peripheral, DefaultDelegate

parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
parser.add_argument('addr', metavar='A', type=str, help='Address of the form XX:XX:XX:XX:XX:XX')
args = parser.parse_args()
addr = args.addr.lower()
if len(addr) != 17:
    raise ValueError("Invalid address supplied")

SERVICE_UUID = "4607eda0-f65e-4d59-a9ff-84420d87a4ca"
CHAR_UUIDS = ["4607eda1-f65e-4d59-a9ff-84420d87a4ca", 
            "4607eda2-f65e-4d59-a9ff-84420d87a4ca", 
            "4607eda3-f65e-4d59-a9ff-84420d87a4ca", 
            "4607eda4-f65e-4d59-a9ff-84420d87a4ca"]# TODO: add your characteristics

class RobotController():

    def __init__(self, address):

        self.robot = Peripheral(addr)
        print("connected")

        # keep state for keypresses
        self.pressed = {"up": False, "down": False, "right": False, "left": False}
        # TODO get service from robot
        sv = self.robot.getServiceByUUID(SERVICE_UUID)

        # TODO get characteristic handles from service/robot
        self.forward = sv.getCharacteristics(CHAR_UUIDS[0])[0]
        self.backward = sv.getCharacteristics(CHAR_UUIDS[1])[0]
        self.left = sv.getCharacteristics(CHAR_UUIDS[2])[0]
        self.right = sv.getCharacteristics(CHAR_UUIDS[3])[0]
        
        # TODO enable notifications if using notifications

        keyboard.hook(self.on_key_event)

    def on_key_event(self, event):
        # print key name
        print(event.name)
        # if a key unrelated to direction keys is pressed, ignore
        if event.name not in self.pressed: return
        # if a key is pressed down
        if event.event_type == keyboard.KEY_DOWN:
            # if that key is already pressed down, ignore
            if event.name == "up":
                if self.pressed[event.name]: return
                # set state of key to pressed
                self.pressed[event.name] = True
                # TODO write to characteristic to change direction
                self.forward.write(bytes([True]))
            if event.name == "down":
                if self.pressed[event.name]: return
                # set state of key to pressed
                self.pressed[event.name] = True
                # TODO write to characteristic to change direction
                self.backward.write(bytes([True]))
            if event.name == "left":
                if self.pressed[event.name]: return
                # set state of key to pressed
                self.pressed[event.name] = True
                # TODO write to characteristic to change direction
                self.left.write(bytes([True]))
            if event.name == "right":
                if self.pressed[event.name]: return
                # set state of key to pressed
                self.pressed[event.name] = True
                # TODO write to characteristic to change direction
                self.right.write(bytes([True]))
        else:
            # set state of key to released
            self.pressed[event.name] = False
            # TODO write to characteristic to stop moving in this direction
            self.forward.write(bytes([False]))
            self.backward.write(bytes([False]))
            self.left.write(bytes([False]))
            self.right.write(bytes([False]))


    def __enter__(self):
        return self
    def __exit__(self, exc_type, exc_value, traceback):
        self.robot.disconnect()

with RobotController(addr) as robot:
    getpass('Use arrow keys to control robot')
