#include "motors.h"

//This distance keeps track fo the integrated speed of the bike to get the total distance
//travelled since it was last reset
float driving_distance = 0; 

struct motor * create_motor(uint8_t enable, uint8_t in1, uint8_t in2, uint8_t pwm_channel, app_pwm_t const * const pwm_inst)
{
    struct motor * motor = malloc(sizeof(struct motor));
    if (motor) {
        nrf_gpio_pin_clear(in1);
        nrf_gpio_pin_clear(in2);
        motor->duty_cycle = 0;
        motor->enable = enable;
        motor->in1 = in1;
        motor->in2 = in2;
        motor->pwm_channel = pwm_channel;
        motor->pwm_inst = pwm_inst;
        nrf_gpio_cfg_output(enable);
        nrf_gpio_cfg_output(in1);
        nrf_gpio_cfg_output(in2);
        nrf_gpio_pin_clear(enable);
        
        
    }
    return motor;
    
}

void set_motor_direction(struct motor * motor, int8_t direction){
    motor->direction = direction;
    switch(direction){
        case FORWARD:
            motor->direction = FORWARD;
            nrf_gpio_pin_set(motor->in1);
            nrf_gpio_pin_clear(motor->in2);
            break;
        case REVERSE:
            motor->direction = REVERSE;
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
{ }

void move(struct motor* drive_motor, float speed) {
    float PWM_float = speed/WHEEL_RADIUS_CM * 60.0 / (2.0*3.14*DRIVE_RPM_PER_PWM);

    if (PWM_float > 100.0 || PWM_float < -100.0) {
        printf("Required speed (%f cm/s) is more than max speed\n", speed);
    }

    int8_t PWM = (int8_t) PWM_float;

    if (PWM > 100) {PWM = 100;}
    if (PWM < -100) {PWM = - 100;}

    printf("Setting PWM to %d\n", PWM);



    if (PWM == 0) {
        set_motor_direction(drive_motor, STOP);
        set_motor_pwm(drive_motor, 0);
        return;
    }

    if (PWM < 0) {
        set_motor_direction(drive_motor, REVERSE);
        set_motor_pwm(drive_motor, -PWM);
        return;
    }

    if (PWM > 0) {
        set_motor_direction(drive_motor, FORWARD);
        set_motor_pwm(drive_motor, PWM);
    }
}