# Final Implementation that was used on Demo Day

## Contents
1. accel_list: an adaptation of a linked list library found online used to store accelerometer readings to turn on brake lights
    * Author: Garey
2. bike_control_remote.py: bike_control is the file used from lab and bike_control_remote is an adaptation that uses the N64 remote joystick and various buttons to control the bike.
    * Author: Toby
3. board_params.h: contains all macros used throughout the code including pin numbers, constants etc.
4. control*: contains both the code used for estimation and control of the bike to it's desired location (app folder: self_driving)
   * Authors: Leo (Estimation), Carl (Control)
5. Madgwick*: a sensor fusion algorithm for the gyro and accelerometer of the mpu9250, this was sourced online and gave us reliable roll, pitch and yaw readings.
6. main: contains the state machine on the bike consisting of OFF, MANUAL and AUTONOMOUS modes as well as the BLE setup.
    * Author: Toby Baker
7. motor*: contains the functions to setup DC motors and write PWM values to them (app folder: motor_driver)
    * Author: Garey
8. mpu*: Contains all code relating to the mpu9250 including reading, filtering, SW interrupts.
    * Authors: Toby, Garey
9. servo_driver*: interface setting up and writing angles to the servo motor used for steering.
    * Authors: Carl, Leo
10. setup*: setup code for PWM channels used by each motors
    * Author: Garey
11. tail_lights*: code that controls the brake light and indicator lights on the bike, an essential safety feature of the sem-autonomous bike
    * Author: Garey
12. timer_module*: code for interfacing with the timer peripheral so that we could timestamp our angles, used for blinkers and estimation (as well as PID in balancing mode)
    * Author: Garey
