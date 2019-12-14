// Robot Template app
//
// Framework for creating applications that control the Kobuki robot

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_spi.h"

#include "buckler.h"
#include "display.h"
#include "kobukiActuator.h"
#include "kobukiSensorPoll.h"
#include "kobukiSensorTypes.h"
#include "kobukiUtilities.h"
#include "mpu9250.h"
#include "simple_ble.h"

#include "states.h"
#include "servo_driver.h"
#include "motor.h"
#include "board_params.h"
#include "control.h"
#include "timer_module.h"

// I2C manager
// NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);


/* Create PWM instance for Servo. */
APP_PWM_INSTANCE(PWM2, 3);


int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

	// initialize RTT library
	error_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(error_code);
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	printf("Log initialized!\n");

	/* Initialize servo. */
	struct servo * front = create_servo(SERVO_PIN, PWM_CHANNEL_0, &PWM2);
	/* Initialize 1 CH PWM, 50 Hz */
	app_pwm_config_t pwm_cfg = APP_PWM_DEFAULT_CONFIG_1CH(20000L, front->pin_nb);
	/* Switch the polarity of the first channel. */
	pwm_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
	error_code = app_pwm_init(&PWM2, &pwm_cfg, pwm_ready_callback);
	APP_ERROR_CHECK(error_code);
	app_pwm_enable(&PWM2);


	struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
  struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
	initialize_dc_motor_pwm(drive, flywheel);

	float duty_cycle = 0;
  int8_t direction = STOP;

  set_dc_motor_pwm(drive, 100);
  set_dc_motor_direction(drive, FORWARD);

  set_servo_angle(front, 15);


  //Init tracking

  init_tracking(drive, front);
  start_tracking();

  float x,y,heading;
	// loop forever, running state machine
	while (1) {
    // TODO: complete state machine
    get_state(&x, &y, &heading);
    printf("State: (%f,%f,%f)\n", x,y,heading);
    nrf_delay_ms(1000);
	}
}
