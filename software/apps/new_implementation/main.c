

// #include "motors.h"
// #include "accelerometer_reading.h"
// #include "pid.h"

#include "setup.h"
#include "motor.h"
#include "board_params.h"
extern ret_code_t error_code;

int main(){


	initialize_buckler();
	struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
    struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
    initialize_dc_motor_pwm(flywheel, drive);
    // while (app_pwm_channel_duty_set(flywheel->pwm_inst, flywheel->pwm_channel, 50) == NRF_ERROR_BUSY);
    // set_dc_motor_pwm(flywheel, 50);
    // set_dc_motor_direction(flywheel, FORWARD);


	// nrf_gpio_pin_set(flywheel->enable);
	// set_dc_motor_direction(flywheel, FORWARD);

    nrf_gpio_pin_set(flywheel->in1);
	nrf_gpio_pin_clear(flywheel->in2);
    while(1){
    	// set_dc_motor_pwm(flywheel, 100);
    	nrf_delay_ms(50);
    }
	// app_pwm_t dc_motor_pwm = initialize_pwm();

	//struct motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, 0, &PWM0);

	return 0;
}