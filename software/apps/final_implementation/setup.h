#ifndef SETUP_H
#define SETUP_H

// NRF Libraries
#include "app_error.h"
#include "app_timer.h"
#include "app_pwm.h"
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
#include "nrf_drv_spi.h"

// Buckler library
#include "buckler.h"
#include "display.h"
#include "mpu9250.h"
#include "simple_ble.h"

// Our libraries
#include "accel_list.h"
#include "ble_receiver.h"
#include "board_params.h"
#include "control.h"
#include "display.h"
#include "motor.h"
#include "mpu.h"
#include "servo_driver.h"
#include "states.h"
#include "tail_lights.h"
#include "timer_module.h"

// Standard C libraries
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

void initialize_buckler();
void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2);
void initialize_servo_motor_pwm(struct servo * servo_motor);
void pwm_ready_callback();

#endif
