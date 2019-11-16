#include "pid.h"

int main() {
	float[1] thetas = {0.0f};
	int[1] time_stamps = {0};

	for(int i = 0; i < 1; i++) {
		printf("Theta = %f\n", thetas[i]);
		printf("Timestamp = %d\n", time_stamps[i]);
		printf("Duty cycle = %f\n", duty_cycle_PID(thetas[i], time_stamps[i]));
	}
}