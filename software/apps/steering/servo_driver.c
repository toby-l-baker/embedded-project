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
    return;
}

void set_servo_pwm(struct servo * servo, uint8_t duty_cycle){
    servo->duty_cycle = duty_cycle;
}

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    printf("PWM Initialized\n");
}