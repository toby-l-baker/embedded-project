#include "pid.h"

float map_duty_cycle(float dc){
	return dc+MIN_DUTY_CYCLE*atan(2*dc);
	if (dc >= 0.0)
		return MIN_DUTY_CYCLE + dc*(MAX_DUTY_CYCLE - MIN_DUTY_CYCLE)/100;
	else
		return -MIN_DUTY_CYCLE + dc*(MAX_DUTY_CYCLE - MIN_DUTY_CYCLE)/100;

}


//Computation intermediaries for the PIDs
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
	integral = integral*Kw +  new_theta * delta_t;
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


/********************* These functions control torque ************************/
/*
This means that the PID outputs control the torque as a function of the inputs
It then transforms that torque into a new PWM by integrating the torque
Basically : new_dc = old_dc + torque * dt
*/


float signed_torque_PID(float theta, float time_stamp) {

	//Check that integral, derivative are initialized
	if (last_timestamp < 0.0) {
		printf("Integral and derivates not set\n");
		reset_derivative(theta, time_stamp);
		reset_integral(time_stamp);
	}

	//Compute duty cycle update
	float signed_torque = Kp_torque*theta;
	signed_torque += Kd_torque*compute_derivative(theta, time_stamp);
	signed_torque += Ki_torque*update_integral(theta, time_stamp);

	//Update last values
	last_theta = theta;
	last_timestamp = time_stamp;


	return signed_torque;
}

void duty_cycle_torque_PID(float theta, float time_stamp, float* duty_cycle, int8_t* direction) {
	
	//Udpate unbounded duty cycle
	float signed_current_dc = ((float)*direction) * ((float)*duty_cycle);
	float signed_torque = signed_torque_PID(theta, time_stamp);
	float signed_duty_cycle = signed_current_dc + signed_torque;

	//CLip to to -100 / 100
	if (signed_duty_cycle > 100.0f)  {signed_duty_cycle = 100.0f;}
	if (signed_duty_cycle < -100.0f) {signed_duty_cycle = -100.0f;}

	//Return motor-usable parameters
	if (signed_duty_cycle > 0.0) {
		*duty_cycle = signed_duty_cycle;
		*direction = FORWARD;
		return;
	} else {
		*duty_cycle = (-signed_duty_cycle);
		*direction = REVERSE;
		return;
	}


}


/********************* These functions control torque ************************/
/*
This is a simple torque controller similar to the previous PID, except it is
simply proportional
Formula used to convert torque to dc is still : new_dc = old_dc + torque * dt
*/

float signed_torque_proportional(float theta) {

	float torque = Kp_torque_prop*theta;
	return torque;

}

void duty_cycle_torque_proportional(float theta, float* duty_cycle, int8_t* direction) {

	//Udpate unbounded duty cycle
	float signed_current_dc = ((float)*direction) * ((float)*duty_cycle);
	float signed_torque = signed_torque_proportional(theta);
	float signed_duty_cycle = signed_current_dc + signed_torque;

	//CLip to to -100 / 100
	if (signed_duty_cycle > 100.0f)  {signed_duty_cycle = 100.0f;}
	if (signed_duty_cycle < -100.0f) {signed_duty_cycle = -100.0f;}

	//Return motor-usable parameters
	if (signed_duty_cycle > 0.0) {
		*duty_cycle = signed_duty_cycle;
		*direction = FORWARD;
		return;
	} else {
		*duty_cycle = (-signed_duty_cycle);
		*direction = REVERSE;
		return;
	}

}


/********************* These functions control PWM ************************/
/*
This means that the PID outputs control the PWM as a function of the inputs
It then returns that PWM directly. It has the advantag of being very simple
to use, but will probably be very hard to tune, because the proportionnal
control alone cannot work
*/

float signed_PWM_PID(float theta, float time_stamp) {

	//Check that integral, derivative are initialized
	if (last_timestamp < 0.0) {
		printf("Integral and derivates not set\n");
		reset_derivative(theta, time_stamp);
		reset_integral(time_stamp);
	}

	//Compute duty cycle update
	float signed_duty_cycle = Kp_PWM*theta;
	signed_duty_cycle += Kd_PWM*compute_derivative(theta, time_stamp);
	signed_duty_cycle += Ki_PWM*update_integral(theta, time_stamp);

	//Update last values
	last_theta = theta;
	last_timestamp = time_stamp;

	return signed_duty_cycle;

}

void duty_cycle_PWM_PID(float theta, float time_stamp, float* duty_cycle, int8_t* direction) {

	//Udpate unbounded duty cycle
	float signed_duty_cycle = signed_PWM_PID(theta, time_stamp);

	//CLip to to -100 / 100
	if (signed_duty_cycle > 100.0f)  {signed_duty_cycle = 100.0f;}
	if (signed_duty_cycle < -100.0f) {signed_duty_cycle = -100.0f;}

	//Return motor-usable parameters
	if (signed_duty_cycle > 0.0) {
		*duty_cycle = signed_duty_cycle;
		*direction = FORWARD;
		return;
	} else {
		*duty_cycle = (-signed_duty_cycle);
		*direction = REVERSE;
		return;
	}
}