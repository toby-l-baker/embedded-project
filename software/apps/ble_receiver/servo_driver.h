#ifndef MOTOR_DRIVER
#define MOTOR_DRIVER

#include "buckler.h"
#include "app_pwm.h"

#define FORWARD				1
#define BACKWARD			-1
#define STOP				0

#define PWM_CHANNEL_0 		0
#define PWM_CHANNEL_1		1

#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,19)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,20)
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,22)

#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,23)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,24)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,25)

#define ENCODER_CH_A NRF_GPIO_PIN_MAP(0,20) //using BUCKLER_SENSORS_SDA
#define ENCODER_CH_B NRF_GPIO_PIN_MAP(0,7) //using BUCKLER_IMU_INTERUPT



/* Struct for storing information about each ADC Channel */
struct servo
{
  int8_t direction;
  app_pwm_duty_t duty_cycle; // between 0 and 100
  uint8_t pin_nb;
  uint8_t pwm_channel;
  app_pwm_t* pwm_inst;
};


struct servo * create_servo(uint8_t pin_nb, uint8_t pwm_channel, app_pwm_t const * const pwm_inst);

void set_servo_angle(struct servo * servo, float angle);

void set_servo_pwm(struct servo * servo, float duty_cycle);

void pwm_ready_callback(uint32_t pwm_id) ;

#endif
