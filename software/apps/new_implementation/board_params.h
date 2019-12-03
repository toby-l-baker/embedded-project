#ifndef BOARD_PARAMS_H
#define BOARD_PARAMS_H

#include "setup.h"


// Defines for directions used in set_dc_motor_direction() 
#define FORWARD				1
#define REVERSE				-1
#define STOP				0

// Pins used forthe dc motor driving the flywheel
#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,31)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,30)
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,29)

//Pins used for the dc motor driving the drive wheel
#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,3)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,4)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,28)

// Pins used for servo motor turning the front fork
#define SERVO_PIN NRF_GPIO_PIN_MAP(0,20)

// Timer used for both dc motors and servo motor
#define DC_MOTOR_TIMER			1
#define DC_MOTOR_FREQ			5000L
#define FlYWHEEL_MOTOR_CHANNEL	0
#define DRIVE_MOTOR_CHANNEL		1


#define SERVO_MOTOR_TIMER		1
#define SERVO_MOTOR_CHANNEL		0
#define SERVO_MOTOR_FREQ		20000L



#endif