#ifndef MOTOR_DRIVER
#define MOTOR_DRIVER

#define FORWARD				1
#define BACKWARD			-1
#define STOP				0

#define CW				1
#define CCW			-1

#define PWM_CHANNEL_0 		0
#define PWM_CHANNEL_1		1

/*
#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,19)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,20)
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,22)
*/

#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,23)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,24)
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,25)

#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,23)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,24)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,25)

#define ENCODER_CH_A NRF_GPIO_PIN_MAP(0,20) //using BUCKLER_SENSORS_SDA
#define ENCODER_CH_B NRF_GPIO_PIN_MAP(0,7) //using BUCKLER_IMU_INTERUPT

//Imports uint and pwm types
#include "app_pwm.h"



struct motor
{
  int8_t direction;
  app_pwm_duty_t duty_cycle; // between 0 and 100
  uint8_t enable; //most recent voltage reading
  uint8_t in1;
  uint8_t in2;
  uint8_t pwm_channel;
  app_pwm_t* pwm_inst;
};

struct motor * create_motor(uint8_t enable, uint8_t in1, uint8_t in2, uint8_t pwm_channel, app_pwm_t const * const pwm_inst);

void set_motor_direction(struct motor * motor, int8_t direction);

void set_motor_pwm(struct motor * motor, uint8_t duty_cycle);

void pwm_ready_callback(uint32_t pwm_id);


#endif