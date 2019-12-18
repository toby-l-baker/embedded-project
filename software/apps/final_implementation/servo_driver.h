#ifndef SERVO_DRIVER
#define SERVO_DRIVER

#include "setup.h"

/* Struct for storing information about each ADC Channel */
struct servo
{
  int8_t direction;
  app_pwm_duty_t duty_cycle; // between 0 and 100
  uint8_t pin_nb;
  uint8_t pwm_channel;
  app_pwm_t* pwm_inst;
};


struct servo * create_servo(uint8_t pin_nb, uint8_t pwm_channel);

void set_servo_angle(struct servo * servo, float angle);

void set_servo_pwm(struct servo * servo, float duty_cycle);

void pwm_ready_callback(uint32_t pwm_id) ;

#endif
