#include "pid.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>



int main() {

	float thetas[6] = {5.0, 4.9, 4.2, 3.1, 1.1, -0.4};
	int time_stamps[6] = {0, 1, 2, 3, 4, 5};

	for(int i = 0; i < 6; i++) {

		printf("Theta = %f\n", thetas[i]);
		printf("Timestamp = %d\n", time_stamps[i]);
		printf("Duty cycle = %f\n\n", duty_cycle_PID(thetas[i], time_stamps[i]));
	}
}