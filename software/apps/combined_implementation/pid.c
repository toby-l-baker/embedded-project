#include "pid.h"
#include <stdio.h>
#include "math.h"

//Proportional control
float Kp = -.3;

//Derivative constant
float Kd = -0;

//Integral constant
float Ki = 0;

//Antiwindup constant (use it?)
float Kw = 2.0;

#define MIN_DUTY_CYCLE 10
#define MAX_DUTY_CYCLE 100

float map_duty_cycle(float dc){
	return dc+MIN_DUTY_CYCLE*atan(2*dc);
	if (dc >= 0.0)
		return MIN_DUTY_CYCLE + dc*(MAX_DUTY_CYCLE - MIN_DUTY_CYCLE)/100;
	else
		return -MIN_DUTY_CYCLE + dc*(MAX_DUTY_CYCLE - MIN_DUTY_CYCLE)/100;

}

/* //Hardcoded
float map_duty_cycle(float dc){
  if (dc >= 0.0)
    return 15.0 + dc*0.850;
  else
    return -15.0 + dc*0.850;

}*/


//Computation intermediaries
float integral = 0.0f;
float derivative = 0.0f;

float last_theta = -1.0f;
float last_timestamp = -1.0f;


void reset_integral(int time_stamp) {
	integral = 0.0f;
	last_timestamp = time_stamp;
}

void reset_derivative(float theta, int time_stamp) {
	derivative = 0.0f;
	last_theta = theta;
	last_timestamp = time_stamp;
}

float update_integral(float new_theta, int time_stamp) {
	float delta_t = time_stamp - last_timestamp;
	integral = integral/Kw +  new_theta * delta_t;
	return integral;
}

float compute_derivative(float theta, int time_stamp) {
	int delta_t = time_stamp - last_timestamp;
	float delta_theta = theta - last_theta;

	//If delta_t is 0, the we only have one sample
	if (delta_t == 0) {return 0;}

	derivative = delta_theta/delta_t;

	return derivative;
}

float signed_torque_PID(float theta, float time_stamp) {

	//Check that integral, derivative are initialized
	if (last_timestamp < 0.0) {
		printf("Integral and derivates not set\n");
		reset_derivative(theta, time_stamp);
		reset_integral(time_stamp);
	}

	//Compute unbounded duty cycle
	float signed_torque = Kp*theta;
	signed_torque += Kd*compute_derivative(theta, time_stamp);
	signed_torque += Ki*update_integral(theta, time_stamp);


	//Update last values
	last_theta = theta;
	last_timestamp = time_stamp;


	return signed_torque;
}


void duty_cycle_PID(float theta, float time_stamp, float* duty_cycle, int8_t* direction) {
	
	float signed_current_dc = ((float)*direction) * ((float)*duty_cycle);
	float signed_torque = signed_torque_PID(theta, time_stamp);
	float signed_duty_cycle = signed_current_dc + signed_torque;

	if (signed_duty_cycle > 100.0f)  {signed_duty_cycle = 100.0f;}
	if (signed_duty_cycle < -100.0f) {signed_duty_cycle = -100.0f;}

	if (signed_duty_cycle > 0.0) {
		*duty_cycle = signed_duty_cycle;
		*direction = FORWARD;
		return;
	} else {
		*duty_cycle = (-signed_duty_cycle);
		*direction = BACKWARD;
		return;
	}


}


float signed_torque_proportionnal(float theta) {

	//Compute unbounded duty cycle
	float torque = Kp*theta;
	return torque;

}

void duty_cycle_proportionnal(float theta, float* duty_cycle, int8_t* direction) {

	float signed_current_dc = ((float)*direction) * ((float)*duty_cycle);
	float signed_torque = signed_torque_proportionnal(theta);

	float signed_duty_cycle = signed_current_dc + signed_torque;

	if (signed_duty_cycle > 100.0f)  {signed_duty_cycle = 100.0f;}
	if (signed_duty_cycle < -100.0f) {signed_duty_cycle = -100.0f;}

	if (signed_duty_cycle > 0.0) {
		*duty_cycle = signed_duty_cycle;
		*direction = FORWARD;
		return;
	} else {
		*duty_cycle = (-signed_duty_cycle);
		*direction = BACKWARD;
		return;
	}

}





//////////////////////////////// THESE CONTROL PWM DIRECTLY ////////////////


float signed_PWM_PID(float theta, float time_stamp) {

	//Check that integral, derivative are initialized
	if (last_timestamp < 0.0) {
		printf("Integral and derivates not set\n");
		reset_derivative(theta, time_stamp);
		reset_integral(time_stamp);
	}

	//Compute unbounded duty cycle
	float signed_duty_cycle = Kp*theta;
	signed_duty_cycle += Kd*compute_derivative(theta, time_stamp);
	signed_duty_cycle += Ki*update_integral(theta, time_stamp);


	//Update last values
	last_theta = theta;
	last_timestamp = time_stamp;

	return signed_duty_cycle;

}

void duty_cycle_PWM_PID(float theta, float time_stamp, float* duty_cycle, int8_t* direction) {

	float signed_duty_cycle = signed_PWM_PID(theta, time_stamp);

	if (signed_duty_cycle > 100.0f)  {signed_duty_cycle = 100.0f;}
	if (signed_duty_cycle < -100.0f) {signed_duty_cycle = -100.0f;}

	if (signed_duty_cycle > 0.0) {
		*duty_cycle = signed_duty_cycle;
		*direction = FORWARD;
		return;
	} else {
		*duty_cycle = (-signed_duty_cycle);
		*direction = BACKWARD;
		return;
	}
}