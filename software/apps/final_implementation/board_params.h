#ifndef BOARD_PARAMS_H
#define BOARD_PARAMS_H

#include "setup.h"

/*** Angle Conversion Parameters ***/
#define TO_RAD 0.0174532925 // PI / 180
#define TO_DEG 57.2957795131 // 180 / PI
#define PI 3.1415926535

/*** Estimation Parameters ***/
#define TO_M 0.01
#define VX 0.285 // velocity of the bike at 100 PWM

/***Servo Parameters***/
#define SERVO_RANGE 170
#define MAX_TURN_ANGLE 45
#define SERVO_PIN NRF_GPIO_PIN_MAP(0,27)
#define SERVO_MOTOR_TIMER		1
#define SERVO_MOTOR_CHANNEL		0
#define SERVO_MOTOR_FREQ		20000L

/*** BLINKER PARAMETERS ***/
#define RIGHT 1
#define LEFT -1
#define RIGHT_BLINKER_STRING	"              >>"
#define LEFT_BLINKER_STRING		"<<              "
#define BRAKE_STRING			"|||          |||"
#define BOTH_BLINKER_STRING		"<<            >>"
#define EMPTY_STRING			"                "
#define ACCEL_POSITIVE 1
#define ACCEL_NEGATIVE -1
#define ACCEL_ZERO		0
#define BLINKER_SAMPLE_FREQ 50
#define MICROSECONDS 1000000.0

/***BLINKER PARAMS***/
#define BRAKE_ACCELERATION_USECOND_THRESH 60000.0
#define BRAKE_ACCEL_THRESHOLD_VAL_G			0.12
#define BLINKER_PERIOD	0.25
#define MINIMUM_BLINKER_ANGLE_PER_USECOND 0.000003

/*** Defines for directions used in set_dc_motor_direction() ***/
#define FORWARD				1
#define REVERSE				-1
#define STOP				0

/*** CONTROL PARAMETERS ***/
#define CONTROL_GAIN 4.0

/*** ESTIMATION PARAMETERS ***/
#define DRIVE_RPM_PER_PWM 0.420 //The number of RPM per unit of PWM, assuming a linear relationship
#define WHEEL_RADIUS_M 0.045
#define TRACKING_UPDATE_MS 10.0
#define BIKE_LENGTH_M 0.30

/*** TIMER PARAMETERS ***/
#define TIMER_MODULE_TIMER 		4

/*** GPIO PINS USED FOR MOTORS ***/
#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,11)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,12) // 26
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,13) //2
#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,3)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,4)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,28)

/***DC MOTOR PARAMETERS***/
#define DC_MOTOR_TIMER			1
#define DC_MOTOR_FREQ			5000L
#define FLYWHEEL_MOTOR_CHANNEL	1
#define DRIVE_MOTOR_CHANNEL

/*** IMU PARAMS ***/
#define IMU_TIMER_REFRESH_RATE 1.9

#define NUM_LOOPS 100
#define PRESCALE_CONV 0.000001

#endif
