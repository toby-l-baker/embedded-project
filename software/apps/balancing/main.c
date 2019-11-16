#include "../pid/pid.h"
#include "../sensors_reading/accelerometer_reading.h"

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





int main() {
	angles_t angles;
	angles.theta_x = 1;

	init_accelerometers();

	while(1) {
		update_angles_struct(&angles);
		nrf_delay_ms(50);
		printf("%f    ",angles.theta_x);

		printf("%f\n", duty_cycle_proportionnal(angles.theta_x));
	}
	return 0;
}