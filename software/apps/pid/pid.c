#include "pid.h"

float integral = 0.0f;
float derivative = 0.0f;

float last_theta = -1.0f;
int last_timestamp = -1.0f;

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
	integral += new_theta * delta_t;
	return integral;
}

float compute_derivative(float theta, int time_stamp) {
	float delta_t = time_stamp - last_timestamp;
	float delta_theta = theta - last_theta;
	return derivative;
}

float duty_cycle_PID(float theta, float time_stamp) {

	//Check that integral, derivative are initialized
	if (last_timestamp < 0.0) {
		reset_derivative(theta, time_stamp);
		reset_integral(time_stamp);
	}

	//Compute unbounded duty cycle
	float duty_cycle = Kp*theta;
	duty_cycle += Kd*compute_derivative(theta, time_stamp);
	duty_cycle += Ki*update_integral(theta, time_stamp);

	//Clip dury cycle to -100 -- 100 interval
	if (duty_cycle > 100.0f) {duty_cycle = 100.0f;}
	if (duty_cycle < -100.0f) {duty_cycle = -100.0f;}

	return duty_cycle;

}