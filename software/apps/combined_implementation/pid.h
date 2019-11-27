#ifndef PID_HEADER
#define PID_HEADER

#include "buckler.h"

//Proportional control
extern float Kp;

//Derivative constant
extern float Kd;

//Integral constant
extern float Ki;

//Antiwindup constant, not used yet
extern float Kw;

//Minimal duty cycle to have the motor turn
extern float MIN_DUTY_CYCLE;

#define FORWARD				1
#define BACKWARD			-1
#define STOP				0

//Takes the angle theta the bike makes with the vertical axis, return the motor
//duty cycle required to correct the error using PID
//Returns a number between 0 and 100 and a rotation direction
void duty_cycle_PID(float theta, float time_stamp, uint8_t* duty_cyle, int8_t* direction);

//Takes and angle theta and outputs a simple proportionnal control duty cycle
//between 0 and 100 and a direction
void duty_cycle_proportionnal(float theta, uint8_t* duty_cyle, int8_t* direction);

//Resets the integral used in the PID. Takes the current clock time in clock cycles
void reset_integral(int time_stamp);

//Resets the derivative used in the PID. Takes current time & current angle
void reset_derivative(float theta, int time_stamp);

float map_duty_cycle(float);

#endif
