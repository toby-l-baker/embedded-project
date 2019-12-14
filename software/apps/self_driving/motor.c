#include "motor.h"
#include "setup.h"


// struct servo_motor * create_servo_motor(uint8_t ctrl){
//     struct servo_motor * motor = malloc(sizeof(struct servo_motor));
//     if (motor){
//         nrf_gpio_pin_clear(ctrl);
//         motor->duty_cycle = 0;
//         motor->ctrl = ctrl;
//         nrf_gpio_cfg_output(ctrl);
//     }
//     return motor;
// }

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
            printf("Setting direction to reverse.\n");
            nrf_gpio_pin_set(motor->in2);
            nrf_gpio_pin_clear(motor->in1);
            break;
        case STOP:
            printf("Setting direction to stop.\n");
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


ret_code_t set_drive_PWM(struct dc_motor* motor, int8_t PWM) {

    ret_code_t error;

    if (PWM > 100) {PWM = 100;}
    if (PWM < -100) {PWM = - 100;}

    printf("Setting PWM to %d\n", PWM);

    if (PWM == 0) {
        set_dc_motor_direction(motor, STOP);
        error = set_dc_motor_pwm(motor, 0);
    }

    if (PWM < 0) {
        set_dc_motor_direction(motor, REVERSE);
        error = set_dc_motor_pwm(motor, -PWM);
    }

    if (PWM > 0) {
        set_dc_motor_direction(motor, FORWARD);
        error = set_dc_motor_pwm(motor, PWM);
    }

    return error;
}

ret_code_t set_drive_speed(struct dc_motor* motor, float speed) {
    float PWM_float = speed/WHEEL_RADIUS_CM * 60.0 / (2.0*3.14*DRIVE_RPM_PER_PWM);

    if (PWM_float > 100.0 || PWM_float < -100.0) {
        printf("Required speed (%f cm/s) is more than max speed\n", speed);
    }

    int8_t PWM = (int8_t) PWM_float;

    if (PWM > 100) {PWM = 100;}
    if (PWM < -100) {PWM = - 100;}

    printf("Setting PWM to %d\n", PWM);

    ret_code_t error;

    if (PWM == 0) {
        set_dc_motor_direction(motor, STOP);
        error = set_dc_motor_pwm(motor, 0);
    }

    if (PWM < 0) {
        set_dc_motor_direction(motor, REVERSE);
        error = set_dc_motor_pwm(motor, -PWM);
    }

    if (PWM > 0) {
        set_dc_motor_direction(motor, FORWARD);
        error = set_dc_motor_pwm(motor, PWM);
    }

    return error;
}

// ret_code_t set_servo_motor_pwm(struct servo_motor * motor, uint8_t duty_cycle){
//     motor->duty_cycle = duty_cycle;
//     return app_pwm_channel_duty_set(motor->pwm_inst, SERVO_MOTOR_CHANNEL, motor->duty_cycle);
//     // while (app_pwm_channel_duty_set(motor->pwm_inst, SERVO_MOTOR_CHANNEL, motor->duty_cycle) == NRF_ERROR_BUSY);
// }
