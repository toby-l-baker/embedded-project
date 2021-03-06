#ifndef SETUP_H
#define SETUP_H

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
#include "app_pwm.h"
#include "buckler.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "ble_receiver.h"
#include "simple_ble.h"

#include "display.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "motor.h"
#include "servo.h"
#include "mpu.h"
#include "board_params.h"
#include "states.h"


void initialize_buckler();

void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2);
void initialize_servo_motor_pwm(struct servo_motor* motor);



#endif