#include "servo_driver.h"

struct servo * create_servo(uint8_t pin_nb, uint8_t pwm_channel, app_pwm_t const * const pwm_inst)
{
    struct servo * servo = malloc(sizeof(struct servo));
    if (servo) {
        servo->duty_cycle = 0;
        servo->pin_nb = pin_nb;
        servo->pwm_channel = pwm_channel;
        servo->pwm_inst = pwm_inst;
        nrf_gpio_cfg_output(pin_nb);
        
    }
    return servo;
    
}

void set_servo_angle(struct servo * servo, float angle) {
    uint8_t duty_cycle = (uint8_t) ((angle+90.0)*9.0/180.0 + 1.0);
    printf("%d\n", duty_cycle);
    set_servo_pwm(servo, duty_cycle);
}

void set_servo_pwm(struct servo * servo, uint8_t duty_cycle){
    servo->duty_cycle = duty_cycle;
    while (app_pwm_channel_duty_set(servo->pwm_inst, servo->pwm_channel, servo->duty_cycle) == NRF_ERROR_BUSY);
}

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    // printf("PWM Initialized\n");
}