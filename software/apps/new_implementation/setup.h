#ifndef SETUP_H
#define SETUP_H

//Comment out to not filter data





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

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "motor.h"


void initialize_buckler();
void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2);
void initialize_servo_motor_pwm(struct servo_motor* motor);
void pwm_ready_callback();


#endif