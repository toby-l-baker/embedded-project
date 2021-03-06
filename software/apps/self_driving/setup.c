#include "setup.h"
#include "motor.h"
#include "board_params.h"


APP_PWM_INSTANCE(PWM0,4); 

void initialize_buckler(){
	ret_code_t error_code = NRF_SUCCESS;
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    printf("Buckler initialized.\n");

}


void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2){
	ret_code_t error_code = NRF_SUCCESS;
	error_code = NRF_LOG_INIT(NULL);

	
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
    error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);
    
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);
    set_dc_motor_pwm(motor_1, 0);
    set_dc_motor_pwm(motor_2, 0);
    printf("DC Motors initialized.\n");
    

}



void pwm_ready_callback(uint32_t pwm_num){
}