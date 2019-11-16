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