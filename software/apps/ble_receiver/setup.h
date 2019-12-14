#ifndef SETUP_H
#define SETUP_H

//Comment out to not filter data
/*
#ifdef NRFX_TIMER0_ENABLED
#undef NRFX_TIMER0_ENABLED
#define NRFX_TIMER0_ENABLED 1
#endif
#ifdef TIMER0_ENABLED
#undef TIMER0_ENABLED
#define TIMER0_ENABLED 1
#endif

#ifdef NRFX_TIMER1_ENABLED
#undef NRFX_TIMER1_ENABLED
#define NRFX_TIMER1_ENABLED 1
#endif
#ifdef TIMER1_ENABLED
#undef TIMER1_ENABLED
#define TIMER1_ENABLED 1
#endif

#ifdef NRFX_TIMER2_ENABLED
#undef NRFX_TIMER2_ENABLED
#define NRFX_TIMER2_ENABLED 1
#endif
#ifdef TIMER2_ENABLED
#undef TIMER2_ENABLED
#define TIMER2_ENABLED 1
#endif

#ifdef NRFX_TIMER3_ENABLED
#undef NRFX_TIMER3_ENABLED
#define NRFX_TIMER3_ENABLED 1
#endif
#ifdef TIMER3_ENABLED
#undef TIMER3_ENABLED
#define TIME3_ENABLED 1
#endif

#ifdef NRFX_TIMER4_ENABLED
#undef NRFX_TIMER4_ENABLED
#define NRFX_TIMER4_ENABLED 1
#endif
#ifdef TIMER4_ENABLED
#undef TIMER4_ENABLED
#define TIMER4_ENABLED 1
#endif

*/

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

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "motor.h"


void initialize_buckler();
void initialize_dc_motor_pwm(struct dc_motor* motor_1, struct dc_motor* motor_2);
void initialize_servo_motor_pwm(struct servo_motor* motor);
void pwm_ready_callback();


#endif