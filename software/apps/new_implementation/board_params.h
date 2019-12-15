#ifndef BOARD_PARAMS_H
#define BOARD_PARAMS_H

#include "setup.h"

// Delay introduced in the loop
#define MS_DELAY		1

// Defines for directions used in set_dc_motor_direction() 
#define FORWARD				1
#define REVERSE				-1
#define STOP				0

// Timers
#define DC_MOTOR_TIMER			3
#define SERVO_MOTOR_TIMER		2
#define TIMER_MODULE_TIMER 		4
 



// Pins used to drive the motors
#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,11)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,12) 
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,13) 

#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,3)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,28)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,4)

#define SERVO_PIN NRF_GPIO_PIN_MAP(0,27)

#define DC_MOTOR_FREQ			1000L
#define FlYWHEEL_MOTOR_CHANNEL	0
#define DRIVE_MOTOR_CHANNEL		1


#define SERVO_MOTOR_CHANNEL		0
#define SERVO_MOTOR_FREQ		20000L

// Calibration for Buckler 32 
#define X_SENSITIVITY 0.3908
#define X_BIAS 1.4009

#define Y_SENSITIVITY 0.3860
#define Y_BIAS 1.403

#define Z_SENSITIVITY 0.3799
#define Z_BIAS 1.4041


// ADC channels
#define X_CHANNEL 0
#define Y_CHANNEL 1
#define Z_CHANNEL 2
#define NUM_SAMPLES 4096
#define LEN_BUFFER 30

#define IMU_TIMER_REFRESH_RATE 1.953125

/*************** Self driving ******************/

#define DRIVE_RPM_PER_PWM 0.420 //The number of RPM per unit of PWM, assuming a linear relationship

#define WHEEL_RADIUS_CM 4.50

#define TRACKING_UPDATE_MS 100.0

#define BIKE_LENGTH_M 0.30

/*************** PID torque values **************/

//Proportional control
#define Kp_torque 		-0.3

//Derivative constant
#define Kd_torque 		-0

//Integral constant
#define Ki_torque 		8.5

//
#define Kpwm_torque		-0.3

/*************** PID PWM values **************/

//Proportional control
#define Kp_PWM 			-24.58

//Derivative constant
#define Kd_PWM 			-1.0

//Integral constant
#define Ki_PWM 			100 //397.0

#define Kpwm_PWM		-10.1

/************** Proportionnal torque value **************/

#define Kp_torque_prop 	-0.2

/************** This anti-windup constant prevents the integral 
from becoming to big and improves system stability ************/

#define Kw 				0.95 //It should be less that 1





#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 100



#define BRAKE_ACCELERATION_USECOND_THRESH 50000.0
#define BRAKE_ACCEL_THRESHOLD_VAL_G			0.04

#define BLINKER_PERIOD	0.25
#define MINIMUM_BLINKER_ANGLE_PER_USECOND 0.000003





#endif