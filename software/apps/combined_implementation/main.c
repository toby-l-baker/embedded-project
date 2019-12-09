// Analog accelerometer app
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

//Equilibrium perturbation
#define EQ_ANGLE 3.35

//Minimum angle, below which no control is applied, should be removed
#define MIN_ANGLE 0

//Used to print state and control values
#define DEBUG 1 

/* CODE RESOURCES */
// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.0.0%2Flib_pwm.html
// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.0.0%2Fgroup__app__pwm.html&anchor=ga0ab3501072119588eb8bea06efe10350
// https://github.com/xueliu/nRF52/blob/master/nRF52_SDK_0.9.1_3639cc9/components/libraries/pwm/app_pwm.h

    
int main (void) {
    ret_code_t error_code = NRF_SUCCESS;

    // initialize RTT library
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // initialization complete
    printf("Buckler initialized!\n");

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
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);



   
    printf("Setting up sensors...\n");
    init_mpu9250_timer(1.953125); 
    init_clock_time();

    printf("Initializing MPU\n");
    init_mpu9250();




    /* Set both duty cycles to zero initially, wait until channels are ready */
    set_motor_pwm(flywheel, 0);
    set_motor_pwm(drive, 0);
    angles_t* angles = malloc(sizeof(angles_t));


    float duty_cycle = 0;
    int8_t direction = STOP;
    set_motor_direction(flywheel, direction);


    printf("Going into main....\n");
    while(1) {
        

        update_angles(angles);

        if (abs(angles->theta_y - EQ_ANGLE) > MIN_ANGLE) {
            duty_cycle_torque_PID(angles->theta_x - EQ_ANGLE, angles->time_stamp, &duty_cycle, &direction);
        }
        
        #if DEBUG
            printf("Timestamp %f\n", angles->time_stamp);
            printf("Angle %f\n", angles->theta_x - EQ_ANGLE);
            printf("Direction %d\n", direction);
            printf("Duty cycle %f\n\n", duty_cycle);
        #endif

        set_motor_direction(flywheel, direction);
        set_motor_pwm(flywheel, (uint8_t) map_duty_cycle(duty_cycle));

    	nrf_delay_ms(1);

      
    }
}
