#include "setup.h"



APP_PWM_INSTANCE(DC_MOTOR_PWM, DC_MOTOR_TIMER); 
APP_PWM_INSTANCE(SERV_MOTOR_PWM, SERVO_MOTOR_TIMER);

void print_angles(angles_t* angles, float duty_cycle){
    printf("X angle: %f\nY angle: %f\nZ angle%f\nTimeStamp: %f\nDuty Cycle: %f\n\n", 
        angles->theta_x, angles->theta_y, angles->theta_z, angles->time_stamp, duty_cycle);
}

void initialize_buckler(){
	
	ret_code_t error_code = NRF_SUCCESS;
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    printf("\n\nBuckler initialized.\n");

}




void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2){
	ret_code_t error_code = NRF_SUCCESS;
	error_code = NRF_LOG_INIT(NULL);

	
	motor_1->pwm_inst = motor_2->pwm_inst = &DC_MOTOR_PWM;


	
	// nrf_gpio_pin_clear(FLYWHEEL_PIN_ENABLE);
	// nrf_gpio_pin_clear(DRIVE_PIN_ENABLE);
	nrf_gpio_cfg_output(motor_1->enable);
	nrf_gpio_cfg_output(motor_2->enable);
	nrf_gpio_pin_clear(motor_1->enable);
	nrf_gpio_pin_clear(motor_2->enable);

	app_pwm_config_t pwm_cfg = APP_PWM_DEFAULT_CONFIG_2CH(DC_MOTOR_FREQ, motor_1->enable, motor_2->enable);

	pwm_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
    error_code = app_pwm_init(&DC_MOTOR_PWM, &pwm_cfg, NULL);
    
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&DC_MOTOR_PWM);
    set_dc_motor_pwm(motor_1, 0);
    set_dc_motor_pwm(motor_2, 0);
    printf("DC Motors initialized. PWM uses TIMER%i.\n", DC_MOTOR_TIMER);
    

}
void initialize_servo_motor_pwm(struct servo_motor* motor){
	ret_code_t error_code = NRF_SUCCESS;
	error_code = NRF_LOG_INIT(NULL);

	motor->pwm_inst = &SERV_MOTOR_PWM;

	nrf_gpio_cfg_output(motor->pin_nb);
	nrf_gpio_pin_clear(motor->pin_nb);
  /* Initialize 1 CH PWM, 50 Hz */
	app_pwm_config_t pwm_cfg = APP_PWM_DEFAULT_CONFIG_1CH(SERVO_MOTOR_FREQ, motor->pin_nb);

	pwm_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
	error_code = app_pwm_init(&SERV_MOTOR_PWM, &pwm_cfg, NULL);
	APP_ERROR_CHECK(error_code);
	app_pwm_enable(&SERV_MOTOR_PWM);
  // set_servo_pwm(servo_motor, 0);
  printf("Servo Motors initialized. PWM uses TIMER%i.\n", SERVO_MOTOR_TIMER);
}


