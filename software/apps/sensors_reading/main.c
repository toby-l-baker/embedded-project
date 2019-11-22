#include "accelerometer_reading.h"
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
#include "virtual_timer.h"
#include "slist.h"


int main() {
	printf("Test\n");
	init_accelerometers();
	angles_t angles;
	printf("Clock timer initialized ! \n");
	while(1) {
		update_angles_struct(&angles);
		printf("%f\n", angles.time_stamp);
		nrf_delay_ms(50);
	}
	return 1;
}