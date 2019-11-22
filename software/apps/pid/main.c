#include "pid.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "buckler.h"
#include "nrf_delay.h"


int main() {

	uint8_t duty_cycle = 0;
	int8_t direction = STOP;

	for(int i = -10; i < 10; i++) {
		float theta = (float) i;
		printf("Theta = %f\n", (float) i);
		duty_cycle_proportionnal(theta,  &duty_cycle, &direction);
		printf("Duty cycle = %d\n", duty_cycle);
		printf("Direction = %d\n\n", direction);
		nrf_delay_ms(100);
	}
	

	return 1;
}
