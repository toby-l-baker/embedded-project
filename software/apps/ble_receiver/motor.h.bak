#ifndef MOTOR_H
#define MOTOR_H

#include "setup.h"
#include "board_params.h"

struct dc_motor{
	int8_t direction;
	app_pwm_duty_t duty_cycle; // between 0 and 100
	uint8_t enable; //most recent voltage reading
	uint8_t in1;
	uint8_t in2;
	uint8_t pwm_channel;
	app_pwm_t* pwm_inst;
};

// struct servo_motor * create_servo_motor(uint8_t ctrl);
struct dc_motor * create_dc_motor(uint8_t enable, uint8_t in1, uint8_t in2, uint8_t pwm_channel);

void set_dc_motor_direction(struct dc_motor * motor, int8_t direction);
ret_code_t set_dc_motor_pwm(struct dc_motor * motor, uint8_t duty_cycle);
// ret_code_t set_servo_motor_pwm(struct servo_motor * motor, uint8_t duty_cycle);


#endif
