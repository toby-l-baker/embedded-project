#ifndef SERVO_H
#define SERVO_H

#include "setup.h"
#include "board_params.h"

/* Struct for storing information about each ADC Channel */
struct servo_motor
{
	int8_t direction;
	app_pwm_duty_t duty_cycle; // between 0 and 100
	uint8_t pin_nb;
	uint8_t pwm_channel;
	app_pwm_t* pwm_inst;
};


struct servo_motor * create_servo(uint8_t pin_nb, uint8_t pwm_channel);

void set_servo_angle(struct servo_motor * servo, float angle);

void set_servo_pwm(struct servo_motor * servo, float duty_cycle);

void pwm_ready_callback(uint32_t pwm_id) ;

#endif 