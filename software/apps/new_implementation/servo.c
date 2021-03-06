#include "servo.h"


struct servo_motor * create_servo(uint8_t pin_nb, uint8_t pwm_channel)
{
    struct servo_motor * servo = malloc(sizeof(struct servo_motor));
    if (servo) {
        servo->duty_cycle = 0;
        servo->pin_nb = pin_nb;
        servo->pwm_channel = pwm_channel;
        nrf_gpio_cfg_output(pin_nb);
    }
    return servo;
}

void set_servo_angle(struct servo_motor * servo, float angle) {
    float duty_cycle = ((angle+85.0)*12.0/170.0 + 2.0); //shift to 90 being centre and map to a 180 range, RANGE 2-13 PWM values
    // printf("%f\n", duty_cycle);
    set_servo_pwm(servo, duty_cycle);
}

void set_servo_pwm(struct servo_motor * servo, float duty_cycle){
    servo->duty_cycle = duty_cycle;
    return app_pwm_channel_duty_set(servo->pwm_inst, servo->pwm_channel, servo->duty_cycle);
}