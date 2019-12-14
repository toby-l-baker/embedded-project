#ifndef BOARD_PARAMS_H
#define BOARD_PARAMS_H

#include "setup.h"

// Comment out to remove adc filtering
#define FILTER


// Defines for directions used in set_dc_motor_direction()
#define FORWARD				1
#define REVERSE				-1
#define STOP				0

// // Pins used forthe dc motor driving the flywheel
// #define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,12)
// #define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,23)
// #define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,2)

// //Pins used for the dc motor driving the drive wheel
// #define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,3)
// #define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,4)
// #define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,28)

#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,11)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,12) // 26
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,13) //2

#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,3)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,28)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,4)

// Pins used for servo motor turning the front fork
#define SERVO_PIN NRF_GPIO_PIN_MAP(0,29)

// Timer used for both dc motors and servo motor
#define DC_MOTOR_TIMER			1
#define DC_MOTOR_FREQ			1000L
#define FlYWHEEL_MOTOR_CHANNEL	1
#define DRIVE_MOTOR_CHANNEL		0

#define SERVO_MOTOR_TIMER		1
#define SERVO_MOTOR_CHANNEL		0
#define SERVO_MOTOR_FREQ		20000L

//Buckler 32
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

#define IMU_TIMER_REFRESH_RATE 1.0



/*************** PID torque values **************/

//Proportional control
#define Kp_torque 		-.3

//Derivative constant
#define Kd_torque 		-0

//Integral constant
#define Ki_torque 		0

/*************** PID PWM values **************/

//Proportional control
#define Kp_PWM 			-30.0

//Derivative constant
#define Kd_PWM 			-0.0

//Integral constant
#define Ki_PWM 			0.0

/************** Proportionnal torque value **************/

#define Kp_torque_prop 	-0.2

/************** This anti-windup constant prevents the integral
from becoming to big and improves system stability ************/

#define Kw 				0.95 //It should be less that 1





#define MIN_DUTY_CYCLE 10
#define MAX_DUTY_CYCLE 100


#endif
