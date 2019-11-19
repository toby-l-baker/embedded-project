#include "motor_driver.h"

struct motor * create_motor(uint8_t enable, uint8_t in1, uint8_t in2, uint8_t pwm_channel, app_pwm_t const * const pwm_inst)
{
    struct motor * motor = malloc(sizeof(struct motor));
    if (motor) {
        motor->duty_cycle = 0;
        motor->enable = enable;
        motor->in1 = in1;
        motor->in2 = in2;
        motor->pwm_channel = pwm_channel;
        motor->pwm_inst = pwm_inst;
        nrf_gpio_cfg_output(enable);
        nrf_gpio_cfg_output(in1);
        nrf_gpio_cfg_output(in2);
        
    }
    return motor;
    
}


void set_motor_direction(struct motor * motor, int8_t direction){
    switch(direction){
        case FORWARD:
            motor->direction = FORWARD;
            nrf_gpio_pin_set(motor->in1);
            nrf_gpio_pin_clear(motor->in2);
            break;
        case BACKWARD:
            motor->direction = BACKWARD;
            nrf_gpio_pin_set(motor->in2);
            nrf_gpio_pin_clear(motor->in1);
            break;
        case STOP:
            motor->direction = STOP;
            nrf_gpio_pin_clear(motor->in1);
            nrf_gpio_pin_clear(motor->in2);
            break;
        default:
            printf("ERROR: incorrect motor direction\n");
            break;
    }
}

void set_motor_pwm(struct motor * motor, uint8_t duty_cycle){
    motor->duty_cycle = duty_cycle;
    while (app_pwm_channel_duty_set(motor->pwm_inst, motor->pwm_channel, motor->duty_cycle) == NRF_ERROR_BUSY);
}

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    printf("PWM Initialized\n");
}