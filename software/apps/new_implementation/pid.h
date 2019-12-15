#ifndef PID_H
#define PID_H

#include "setup.h"
#include "board_params.h"


void duty_cycle_torque_PID(float theta, float time_stamp, float* duty_cyle, int8_t* direction);

void duty_cycle_torque_proportional(float theta, float* duty_cyle, int8_t* direction);

void duty_cycle_PWM_PID(float theta, float time_stamp, float* duty_cycle, int8_t* direction);

//Resets the integral used in the PID. Takes the current clock time in clock cycles
void reset_integral(int time_stamp);

//Resets the derivative used in the PID. Takes current time & current angle
void reset_derivative(float theta, int time_stamp);

float map_duty_cycle(float);




#endif