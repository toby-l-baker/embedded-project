#include "../pid/pid.h"
#include "../sensors_reading/accelerometer_reading.h"
#include "../motor_driver/motor_driver.h"

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

	ret_code_t error_code = NRF_SUCCESS;

	// initialize RTT library
	error_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(error_code);
	NRF_LOG_DEFAULT_BACKENDS_INIT();

	angles_t angles;
	angles.theta_x = 1;

	init_accelerometers();
	/*
	
	APP_PWM_INSTANCE(PWM0,1);
	struct motor * flywheel = create_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, PWM_CHANNEL_0, &PWM0);
	struct motor * drive = create_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, PWM_CHANNEL_1, &PWM0);



	nrf_gpio_pin_set(flywheel->in1);
    nrf_gpio_pin_clear(flywheel->in2);

    

    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, flywheel->enable, drive->enable);

    printf("1\n");

	error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);


    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);

    printf("2\n");

	//while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
    set_motor_direction(flywheel, STOP);

	printf("Initialized!\n");

	set_motor_direction(flywheel, FORWARD);
	set_motor_pwm(flywheel, 50);

	*/

	while(1) {
		//set_motor_direction(flywheel, FORWARD);
		//set_motor_pwm(flywheel, 50);
		
		update_angles_struct(&angles);
		

		printf("%f\n",angles.theta_x);
		
		/*
		float duty_cycle = duty_cycle_proportionnal(angles.theta_x);
		uint8_t direction = STOP;
		uint8_t duty_cycle_int = 0;

		if (duty_cycle > 0) {
			direction = CW;
			duty_cycle_int = (uint8_t) duty_cycle;
		}

		else {
			direction = CCW;
			duty_cycle_int = (uint8_t) (-duty_cycle);
		}

		
		*/
		nrf_delay_ms(10);
	}
	return 0;
}