#!/usr/bin/env python3
import struct, time, keyboard, argparse
import numpy as np
from getpass import getpass
from enum import Enum
from bluepy.btle import Peripheral, DefaultDelegate
from evdev import InputDevice, ecodes, categorize

# parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
# parser.add_argument('addr', metavar='A', type=str, help='Address of the form XX:XX:XX:XX:XX:XX')
# args = parser.parse_args()
# addr = args.addr.lower()
addr = "c0:98:e5:49:00:0b"
BLE = True
event = "event7"

if len(addr) != 17:
    raise ValueError("Invalid address supplied")

SERVICE_UUID = "4607eda0-f65e-4d59-a9ff-84420d87a4ca"
CHAR_UUIDS = ["4607eda1-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda2-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda3-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda4-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda5-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda6-f65e-4d59-a9ff-84420d87a4ca"]

class States(Enum):
    IDLE=0
    AUTONOMOUS=1
    MANUAL=2

# TO RUN THIS CODE sudo python3 bike_control_remote.py

class BikeController():

    def __init__(self, address):
        if BLE:
            self.bike = Peripheral(addr)
            self.sv = self.bike.getServiceByUUID(SERVICE_UUID)
            self.manual_char = self.sv.getCharacteristics(CHAR_UUIDS[0])[0]
            self.power_char = self.sv.getCharacteristics(CHAR_UUIDS[1])[0]
            self.drive_char = self.sv.getCharacteristics(CHAR_UUIDS[2])[0]
            self.turn_char = self.sv.getCharacteristics(CHAR_UUIDS[3])[0]
            self.path_len_char = self.sv.getCharacteristics(CHAR_UUIDS[4])[0]
            self.path_angle_char = self.sv.getCharacteristics(CHAR_UUIDS[5])[0]

        print("connected")
        # Setup the gamepad and print its information
        self.gamepad = InputDevice('/dev/input/'+event)
        print(self.gamepad)
        self.count = 0

        # keep state for button presses
        self.buttons = {"A": 305, "B": 306, "START": 316, "UP": 313, "JOY_X": 0, "JOY_Y": 1}
        self.state = States.IDLE
        # dicts for mapping angles and speeds to new ranges
        self.turn_map = {"new_min": -45, "new_max": 45, "min": -90, "max": 90}
        self.drive_map = {"new_min": -100, "new_max": 100, "min": -128, "max": 128}
        #initial values for x and y
        self.x = 0
        self.y = 0

        # integral time constant for auonomous path plan
        self.int_const = 0.006 # A somewhat magic number that gives the resolution we want
        self.path_length = 0
        self.path_angles = []

    def __enter__(self):
        return self
    def __exit__(self, exc_type, exc_value, traceback):
        self.bike.disconnect()

    '''Maps the speed of the bike to a range of turn_map["new_min"] to turn_map["new_max"]'''
    def map_angle(self):
        if self.y < 0:
            angle = (np.arctan2(self.y, self.x) * 180 / np.pi + 90)
            angle = -angle # account for RH rule convention
        else:
            angle = (np.arctan2(self.x, self.y) * 180 / np.pi)
            angle = -angle # account for RH rule convention

        val = np.interp(angle, (self.turn_map["min"], self.turn_map["max"]), (self.turn_map["new_min"], self.turn_map["new_max"]))
        self.angle = int(val)

    '''Maps the speed of the bike to a range of drive_map["new_min"] to drive_map["new_max"]'''
    def map_speed(self):
        if self.y > 0:
            speed = -np.maximum(abs(self.x), abs(self.y))
        else:
            speed = np.maximum(abs(self.x), abs(self.y))
        val = np.interp(speed, (self.drive_map["min"], self.drive_map["max"]), (self.drive_map["new_min"], self.drive_map["new_max"]))
        self.speed = int(val)

    '''Maps an 8 bit signed integer to a uint for sending over BLE'''
    def convert_to_uint(self, num):
        if num < 0:
            return np.uint8(256 + num)
        else:
            return np.uint8(num)

    '''Maps a -180 to 180 degree angle to the range 0 to 255 for BLE - converted to int on buckler'''
    def map_180_to_uint(self, list):
        average = np.average(list)
        val = np.interp(average, (-180, 180), (-128, 127))
        if val < 0:
            return np.uint8(256 + val)
        else:
            return np.uint8(val)

    '''Maps the path angle to a range of -180 to 180 degrees, where 0 is straight ahead'''
    def map_angle_path(self):
        ang = np.arctan2(-self.x, self.y)*180/np.pi - 180
        if ang < -180:
            ang = 360+ang
        return ang

    def run(self):
        for event in self.gamepad.read_loop():
        #filters by event type
            '''Update the state of the bike'''
            if event.type == ecodes.EV_KEY:
                # print(event)
                '''Set the state to be Autonomous when pressing A'''
                if event.code == self.buttons["A"] and event.value == 1:
                    self.state = States.AUTONOMOUS
                    print(self.state)
                    if BLE:
                        self.manual_char.write(bytes([False]))
                    time.sleep(0.1)

                '''Set the state to be Manual when pressing B'''
                if event.code == self.buttons["B"] and event.value == 1:
                    self.state = States.MANUAL
                    print(self.state)
                    if BLE:
                        self.manual_char.write(bytes([True]))
                    time.sleep(0.1)

                '''Toggle state when pressing start'''
                if event.code == self.buttons["START"] and event.value == 1:
                    if self.state == States.IDLE:
                        self.state = States.MANUAL
                        print(self.state)
                        if BLE:
                            self.manual_char.write(bytes([True]))
                        time.sleep(0.1)
                    else:
                        self.state = States.IDLE
                        print(self.state)
                        if BLE:
                            self.power_char.write(bytes([False]))
                        time.sleep(0.1)

                '''Send path to follow'''
                if event.code == self.buttons["UP"] and event.value == 1:
                    if self.state == States.AUTONOMOUS:
                        '''Print desired path and send to buckler'''
                        print("(r (cm), theta (deg)): ({},{})".format(np.uint8(self.path_length), self.path_angle * 360/255))
                        if BLE:
                            self.path_len_char.write(bytes([np.uint8(self.path_length)])) #np.uint8(self.path_length)
                            self.path_angle_char.write(bytes([self.path_angle])) #self.path_angle
                        '''Reset path lengths and angles'''
                        self.path_length = 0
                        self.path_angles = []

            '''Update instructions for bike'''
            if event.type == ecodes.EV_ABS:
                if event.code == self.buttons["JOY_X"]:
                    self.x = event.value - 128

                if event.code == self.buttons["JOY_Y"]:
                    self.y = event.value - 128

                if self.state == States.MANUAL:
                    '''Print 8-bit Values'''
                    # print("(X,Y): ({},{})".format(self.x, self.y))

                    '''Update Angle'''
                    self.map_angle()
                    angle = self.convert_to_uint(self.angle)

                    '''Update Speed'''
                    self.map_speed()
                    speed = self.convert_to_uint(self.speed)
                    if self.count == 7 or (speed == 0 and angle == 0):
                        print("Drive Speed: {}".format(speed))
                        print("Turn Angle: {}".format(angle))
                        if BLE:
                            self.drive_char.write(bytes([speed]))
                            self.turn_char.write(bytes([angle]))
                        self.count = 0

                    self.count += 1

                if self.state == States.AUTONOMOUS:
                    self.path_angles.append(self.map_angle_path())
                    self.map_speed()
                    self.path_length += self.int_const * self.speed # self.speed updated in self.map_speed()
                    '''Clip path length to be in range of 0 to 255 cm '''
                    if self.path_length <= 0:
                        self.path_length = 0
                    elif self.path_length >= 255:
                        self.path_length = 255
                    self.path_angle = self.map_180_to_uint(self.path_angles)
                    print("Current Path Plan (r, theta_int, theta_float): ({},{}, {})".format(self.path_length, self.path_angle * (360/255), np.average(self.path_angles)))

with BikeController(addr) as bike:
    print('Use Start, A, B and Joystick to control the bike')
    bike.run()
