## Overview
File that reads Joystick data and advertises messages over BLE to be received by
the self-driving bike.

## Inputs and Outputs
### Outputs
1. manual_control (boolean): moves the bike from manual control to autonomous control and vice versa
2. power (boolean): turns the bike on and off (toggle)
3. drive_speed (uint8_t): the drive speed for the bike represented as a duty cycle between 0 and 100
4. turn_angle (uint8_t): the turning angle for the wheel, to be between -45 and 45 degrees

### Inputs
1. joy_angle (float): the angle of the joystick in degrees (used to derive turn_angle)
2. joy_dir (float): how far the joystick is pressed (used to derive drive_speed)
3. switch (boolean): used to determine the manual_control output
4. button (boolean): used to toggle the bike on and off.

#### Author: Toby Baker
