#ifndef MOTORS_H
#define MOTORS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrfx_gpiote.h"
#include "nrfx_saadc.h"


#include "buckler.h"
#include "app_pwm.h"

#define FORWARD				1
#define REVERSE				-1
#define STOP				0

#define PWM_CHANNEL_0 		0
#define PWM_CHANNEL_1		1

#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,19)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,20)
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,23)

#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,0)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,0)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,0)

#define ENCODER_CH_A NRF_GPIO_PIN_MAP(0,20) //using BUCKLER_SENSORS_SDA
#define ENCODER_CH_B NRF_GPIO_PIN_MAP(0,7) //using BUCKLER_IMU_INTERUPT



/* Struct for storing information about each ADC Channel */
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

void set_motor_direction(struct motor * motor, int8_t direction);
void set_motor_pwm(struct motor * motor, uint8_t duty_cycle);
void pwm_ready_callback(uint32_t pwm_id);

#endif