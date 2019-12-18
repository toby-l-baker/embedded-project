#!/usr/bin/env python3
import time
import numpy as np
from enum import Enum
from bluepy.btle import Peripheral, DefaultDelegate
from evdev import InputDevice, ecodes, categorize

# TO RUN THIS CODE sudo python3 bike_control_remote.py

'''Global parameters'''
addr = "c0:98:e5:49:00:0b" # BLE Address
BLE = False # Are we using BLE?
event = "event5" # what event the USB controller is connected to

if len(addr) != 17:
    raise ValueError("Invalid address supplied")

'''Constants for BLE Service and Characteristics'''
SERVICE_UUID = "4607eda0-f65e-4d59-a9ff-84420d87a4ca"
CHAR_UUIDS = ["4607eda1-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda2-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda3-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda4-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda5-f65e-4d59-a9ff-84420d87a4ca",
            "4607eda6-f65e-4d59-a9ff-84420d87a4ca"]

'''Bike States'''
class States(Enum):
    IDLE=0
    AUTONOMOUS=1
    MANUAL=2

class BikeController():

    def __init__(self, address):
        if BLE:
            '''Setup BLE Characteristics'''
            self.bike = Peripheral(addr)
            self.sv = self.bike.getServiceByUUID(SERVICE_UUID)
            self.manual_char = self.sv.getCharacteristics(CHAR_UUIDS[0])[0]
            self.power_char = self.sv.getCharacteristics(CHAR_UUIDS[1])[0]
            self.drive_char = self.sv.getCharacteristics(CHAR_UUIDS[2])[0]
            self.turn_char = self.sv.getCharacteristics(CHAR_UUIDS[3])[0]
            self.path_len_char = self.sv.getCharacteristics(CHAR_UUIDS[4])[0]
            self.path_angle_char = self.sv.getCharacteristics(CHAR_UUIDS[5])[0]

        print("connected")
        '''Setup the gamepad and print its information'''
        self.gamepad = InputDevice('/dev/input/'+event)
        print(self.gamepad)
        self.count = 0 # counnt for downsampling the amount of data we send over BLE

        # Dictionary for buttons on the remote with the values as the code
        self.buttons = {"A": 305, "B": 306, "START": 316, "UP": 313, "LEFT": 307, "RIGHT": 312, "JOY_X": 0, "JOY_Y": 1}
        # Initial state
        self.state = States.IDLE
        # dicts for mapping angles and speeds to new ranges
        self.turn_map = {"new_min": -35, "new_max": 35, "min": -90, "max": 90}
        self.drive_map = {"new_min": -100, "new_max": 100, "min": -128, "max": 128}
        #initial values for x and y values of the joystick
        self.x = 0
        self.y = 0
        # initialise variables used when generating the path plan
        self.int_const = 0.006 # A somewhat magic number that gives the resolution we want when setting distance for the bike path
        self.path_length = 0
        self.path_angle = 0
        self.angle_increment = 5

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
        self.angle = np.uint8(val)

    '''Maps the speed of the bike to a range of drive_map["new_min"] to drive_map["new_max"]'''
    def map_speed(self):
        if self.y > 0:
            speed = -np.maximum(abs(self.x), abs(self.y))
        else:
            speed = np.maximum(abs(self.x), abs(self.y))
        val = np.interp(speed, (self.drive_map["min"], self.drive_map["max"]), (self.drive_map["new_min"], self.drive_map["new_max"]))
        self.speed = np.uint8(val)

    '''Maps an 8 bit signed integer to a uint for sending over BLE'''
    def convert_to_uint(self, num):
        if num < 0:
            return np.uint8(256 + num)
        else:
            return np.uint8(num)

    '''Maps a -180 to 180 degree angle to the range 0 to 255 for BLE - converted back to an angle on buckler'''
    def map_180_to_uint(self, val):
        val = np.interp(val, (-180, 180), (-128, 127))
        if val < 0:
            return np.uint8(256 + val)
        else:
            return np.uint8(val)

    def run(self):
        for event in self.gamepad.read_loop():
            '''Update the state of the bike'''
            if event.type == ecodes.EV_KEY:
                # print(event)
                '''Set the state to be Autonomous when pressing A'''
                if event.code == self.buttons["A"] and event.value == 1:
                    self.state = States.AUTONOMOUS
                    # reset the path angle and length
                    self.path_angle = 0
                    self.path_length = 0
                    print(self.state)
                    if BLE: # write state and path variables to buckler
                        self.manual_char.write(bytes([False]))
                        self.path_len_char.write(bytes([self.path_length]))
                        self.path_angle_char.write(bytes([self.path_angle]))

                '''Set the state to be Manual when pressing B'''
                if event.code == self.buttons["B"] and event.value == 1:
                    self.state = States.MANUAL
                    print(self.state)
                    if BLE:
                        self.manual_char.write(bytes([True]))

                '''Toggle state when pressing start'''
                if event.code == self.buttons["START"] and event.value == 1:
                    if self.state == States.IDLE:
                        self.state = States.MANUAL
                        print(self.state)
                        if BLE:
                            self.manual_char.write(bytes([True]))
                    else:
                        self.state = States.IDLE
                        print(self.state)
                        if BLE:
                            self.power_char.write(bytes([False]))

                if self.state == States.AUTONOMOUS:
                    '''Send path to follow'''
                    if event.code == self.buttons["UP"] and event.value == 1:
                        '''Print desired path and send to buckler'''
                        if BLE:
                            self.path_len_char.write(bytes([np.uint8(self.path_length)]))
                            self.path_angle_char.write(bytes([np.uint8(self.map_180_to_uint(self.path_angle))]))
                        '''Reset path lengths and angles'''
                        self.path_length = 0
                        self.path_angle = 0

                    if event.code == self.buttons["RIGHT"] and event.value == 1:
                        '''Increment angle by 5'''
                        self.path_angle -= self.angle_increment
                        if self.path_angle < -180:
                            self.path_angle = -180
                        print("Current Path Plan (r, theta_int): ({},{})".format(np.uint8(self.path_length), self.path_angle))

                    if event.code == self.buttons["LEFT"] and event.value == 1:
                        '''Decrement angle'''
                        self.path_angle += self.angle_increment
                        if self.path_angle > 180:
                            self.path_angle = 180
                        print("Current Path Plan (r, theta_int): ({},{})".format(np.uint8(self.path_length), self.path_angle))

            '''Update instructions for bike'''
            if event.type == ecodes.EV_ABS:
                if event.code == self.buttons["JOY_X"]:
                    self.x = event.value - 128 # centre the value at 0

                if event.code == self.buttons["JOY_Y"]:
                    self.y = event.value - 128  # centre the value at 0

                if self.state == States.MANUAL:
                    '''Update Angle'''
                    self.map_angle()
                    angle = self.convert_to_uint(self.angle)

                    '''Update Speed'''
                    self.map_speed()
                    speed = self.convert_to_uint(self.speed)
                    if self.count == 7 or (speed == 0 and angle == 0): #send every 7th data point
                        print("Drive Speed: {}".format(speed))
                        print("Turn Angle: {}".format(angle))
                        if BLE:
                            self.drive_char.write(bytes([speed]))
                            self.turn_char.write(bytes([angle]))
                        self.count = 0

                    self.count += 1

                if self.state == States.AUTONOMOUS:
                    self.map_speed()
                    self.path_length += self.int_const * self.speed
                    '''Clip path length to be in range of 20 to 255 cm'''
                    if self.path_length <= 20:
                        self.path_length = 20
                    elif self.path_length >= 255:
                        self.path_length = 255
                    print("Current Path Plan (r, theta_int): ({},{})".format(np.uint8(self.path_length), self.path_angle))

with BikeController(addr) as bike:
    print('Use Start, A, B and Joystick to control the bike')
    bike.run()
