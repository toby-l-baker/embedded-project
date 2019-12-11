// Analog accelerometer app
//
// Reads data from the ADXL327 analog accelerometer

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrfx_gpiote.h"
#include "nrfx_saadc.h"


#include "buckler.h"

#include "pathplanning.h"

/* CODE RESOURCES */
// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.0.0%2Flib_pwm.html
// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.0.0%2Fgroup__app__pwm.html&anchor=ga0ab3501072119588eb8bea06efe10350
// https://github.com/xueliu/nRF52/blob/master/nRF52_SDK_0.9.1_3639cc9/components/libraries/pwm/app_pwm.h


int main (void) {
	float x_f = 1.0;
	float y_f = 0.0;
	float theta_f = 0.0;
	int N = 10;
	point_t* points = plan_path(x_f, y_f, theta_f, N);

	for(int i = 0; i < N; i++) {
		printf("(%f,%f)\n", points[i].x, points[i].y);
	} 
	printf("\n\n");
	return 1;
}
