#include "setup.h"
#include "motor.h"
#include "board_params.h"

ret_code_t error_code = NULL;

void initialize_buckler(){
	ret_code_t error_code = NRF_SUCCESS;
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    printf("Buckler initialized.\n");
    return error_code;
}



void initialize_dc_motor_pwm_hold(struct dc_motor* motor_1, struct dc_motor* motor_2){
	APP_PWM_INSTANCE(PWM0, DC_MOTOR_TIMER);
	motor_1->pwm_inst = motor_2->pwm_inst = &PWM0;

	app_pwm_config_t cfg = APP_PWM_DEFAULT_CONFIG_2CH(DC_MOTOR_FREQ, motor_1->enable, motor_2->enable);
	cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;

    error_code = app_pwm_init(&PWM0, &cfg, pwm_ready_callback);
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);

    return;
}

void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2){
	error_code = NRF_SUCCESS;
	error_code = NRF_LOG_INIT(NULL);

	APP_PWM_INSTANCE(PWM0,1);
	motor_1->pwm_inst = motor_2->pwm_inst = &PWM0;


	
	// nrf_gpio_pin_clear(FLYWHEEL_PIN_ENABLE);
	// nrf_gpio_pin_clear(DRIVE_PIN_ENABLE);
	nrf_gpio_cfg_output(motor_1->enable);
	nrf_gpio_cfg_output(motor_2->enable);
	nrf_gpio_pin_clear(motor_1->enable);
	nrf_gpio_pin_clear(motor_2->enable);

	app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, motor_1->enable, motor_2->enable);

	pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
    printf("About to call app_pwm_init.\n");
    error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);
    
    printf("Chekcing error code.\n");
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);
    printf("PWM enabled....\n");
    while (app_pwm_channel_duty_set(&PWM0, 0, 0) == NRF_ERROR_BUSY);
    printf("hannel 0 is set to 0\n");
    while (app_pwm_channel_duty_set(&PWM0, 1, 0) == NRF_ERROR_BUSY);
    nrf_delay_ms(500);
    printf("hannel 1 is set to 0\n");

    // while (app_pwm_channel_duty_set(&PWM0, 0, 50) == NRF_ERROR_BUSY);
    // while (app_pwm_channel_duty_set(&PWM0, 1, 50) == NRF_ERROR_BUSY);

}

void initialize_servo_motor_pwm(struct servo_motor* motor){
	APP_PWM_INSTANCE(PWM1, SERVO_MOTOR_TIMER);
	motor->pwm_inst = &PWM1;

	app_pwm_config_t cfg = APP_PWM_DEFAULT_CONFIG_1CH(SERVO_MOTOR_FREQ, motor->ctrl);
	cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

	error_code = app_pwm_init(&PWM1, &cfg, pwm_ready_callback);
	APP_ERROR_CHECK(error_code);
	app_pwm_enable(&PWM1);
}


void pwm_ready_callback(){
	printf("Ready\n");
	return;
}