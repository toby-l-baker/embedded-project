## Overview
File that reads Joystick data and advertises messages over BLE to be received by
another buckler.

## Inputs and Outputs
### Outputs
manual_control (boolean): moves the bike from manual control to autonomous control and vice versa
power (boolean): turns the bike on and off (toggle)
drive_speed (uint8_t): the drive speed for the bike represented as a duty cycle between 0 and 100
turn_angle (uint8_t): the turning angle for the wheel, to be between -45 and 45 degrees

### Inputs
joy_angle (float): the angle of the joystick in degrees (used to derive turn_angle)
joy_dir (float): how far the joystick is pressed (used to derive drive_speed)
switch (boolean): used to determine the manual_control output
button (boolean): used to toggle the bike on and off.

#### Author: Toby Baker
