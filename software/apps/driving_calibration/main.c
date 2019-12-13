// Analog accelerometer app
//
#define PWM 100
#define DRIVE_TIME_SECS 10.0
//
//
//
// Reads data from the ADXL327 analog accelerometer

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
#include "mpu9250_interface.h"

#include "buckler.h"

#include "motors.h"
#include "pid.h"

//Used to print state and control values
#define DEBUG 0 


APP_TIMER_DEF(m_single_shot_timer_id); 

int int_flag = 0;
static void callback(void * p_context) {int_flag = 1;}
    
int main (void) {
    ret_code_t error_code = NRF_SUCCESS;

    // initialize RTT library
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // initialization complete
    printf("Buckler initialized!\n");

    int_flag = 0;

    /* Create Motor Structs */
    
    /* Initialize PWM0 on TIMER 2 */
    APP_PWM_INSTANCE(PWM0,2);

    struct motor * flywheel = create_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, PWM_CHANNEL_0, &PWM0);
    struct motor * drive = create_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, PWM_CHANNEL_1, &PWM0);

    printf("Stucts made...\n");



    /* Initialize 2 CH PWM, 200 Hz */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, flywheel->enable, drive->enable);
    
    /* Switch the polarity of the second channel. */
    pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;

    printf("About to call app_pwm_init.\n");
    error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);
    nrf_delay_ms(1000);
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);



   
    printf("Setting up sensors...\n");
    init_mpu9250_timer(1.953125); 
    init_clock_time();

    printf("Initializing MPU\n");
    init_mpu9250();

    printf("Initializing vitrual timer\n\n\n");
    app_timer_init();
    error_code = app_timer_create(&m_single_shot_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                &callback);
    APP_ERROR_CHECK(error_code);

    printf("Driving at %d PWM for %f secs...\n", PWM, DRIVE_TIME_SECS);
    error_code = app_timer_start(m_single_shot_timer_id, APP_TIMER_TICKS(1000*DRIVE_TIME_SECS), NULL);
    APP_ERROR_CHECK(error_code);

    set_motor_direction(drive, FORWARD);
    set_motor_pwm(drive, PWM);

    
    while(1) {
        
        if (int_flag == 1) {break;}
        nrf_delay_ms(100);
      
    }

    set_motor_direction(drive, STOP);
    set_motor_pwm(drive, 0);
    printf("Finished driving\n");
    nrf_delay_ms(100);
}
