#include "motor.h"
#include "setup.h"


struct dc_motor * create_dc_motor(uint8_t enable, uint8_t in1, uint8_t in2, uint8_t pwm_channel){
    struct dc_motor * motor = malloc(sizeof(struct dc_motor));
    if (motor) {
        // nrf_gpio_cfg_output(enable);
        nrf_gpio_cfg_output(in1);
        nrf_gpio_cfg_output(in2);
        // nrf_gpio_pin_clear(enable);
        nrf_gpio_pin_clear(in1);
        nrf_gpio_pin_clear(in2);
        motor->duty_cycle = 0;
        motor->enable = enable;
        motor->in1 = in1;
        motor->in2 = in2;
        motor->pwm_channel = pwm_channel;
  
        
    }
    return motor;
    
}

void set_dc_motor_direction(struct dc_motor * motor, int8_t direction){
    motor->direction = direction;
    switch(direction){
        case FORWARD:
            printf("Setting direction to forward.\n");
            nrf_gpio_pin_set(motor->in1);
            nrf_gpio_pin_clear(motor->in2);
            break;
        case REVERSE:
            nrf_gpio_pin_set(motor->in2);
            nrf_gpio_pin_clear(motor->in1);
            break;
        case STOP:
            nrf_gpio_pin_clear(motor->in1);
            nrf_gpio_pin_clear(motor->in2);
            break;
        default:
            printf("ERROR: incorrect motor direction\n");
            break;
    }
}

ret_code_t set_dc_motor_pwm(struct dc_motor * motor, uint8_t duty_cycle){
    motor->duty_cycle = duty_cycle;
    return app_pwm_channel_duty_set(motor->pwm_inst, motor->pwm_channel, motor->duty_cycle);
    // while (app_pwm_channel_duty_set(motor->pwm_inst, motor->pwm_channel, motor->duty_cycle) == NRF_ERROR_BUSY);
}
