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


#include "buckler.h"

#include "motors.h"
#include "accelerometer_reading.h"
#include "pid.h"

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
    
	// nrf_gpio_pin_set(drive->enable);
	
	// while(1){
	// 	nrf_delay_ms(25);
	// }
    /* Initialize PWM0 on TIMER 1 */
    APP_PWM_INSTANCE(PWM0,1);

    struct motor * flywheel = create_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, PWM_CHANNEL_0, &PWM0);
    struct motor * drive = create_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, PWM_CHANNEL_1, &PWM0);

    printf("Stucts made...\n");
    // nrf_gpio_pin_set(drive->in1);
    // nrf_gpio_pin_clear(drive->in2);
    /* Initialize 2 CH PWM, 200 Hz */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, flywheel->enable, drive->enable);
    /* Switch the polarity of the second channel. */
    pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;

 //    app_pwm_disable(&PWM0);
	// error_code = app_pwm_uninit(&PWM0);

	printf("About to call app_pwm_init.\n");
    error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);
    
    
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);

    /* Set both duty cycles to zero initially, wait until channels are ready */
    set_motor_pwm(flywheel, 0);
    set_motor_pwm(drive, 0);
   
    // while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
    // while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);


    // int temp_pwm = 50;
    // flywheel->duty_cycle = drive->duty_cycle = temp_pwm;
    // while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
    // while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);
    // while(1)
    // 	nrf_delay_ms(200);

    // loop forever
    // while(1){

	    
    // }
 
	
	// nrf_delay_ms(2000);
    printf("Setting up sensors...\n");
    angles_t* angles = malloc(sizeof(angles_t));
    init_accelerometers(); 
    set_motor_direction(flywheel, FORWARD);

    
    uint8_t duty_cycle = 0;
    int8_t direction = STOP;

    printf("Going into main....\n");
    while(1) {
      // for (int i = 0; i<100; i++) {
      //     flywheel->duty_cycle = i;
      //     drive->duty_cycle = i;
      //     set_motor_pwm(flywheel, i);
      //     set_motor_pwm(drive, i);
      //     // while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
      //     // while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);
      //     nrf_delay_ms(100);
      // }
    	update_angles_struct(angles);
        if (abs(angles->theta_y) > 15) {
    	   duty_cycle_proportionnal(angles->theta_y, &duty_cycle, &direction);
        }

        printf("Direction %d\n", direction);
    	set_motor_direction(flywheel, direction);
    	set_motor_pwm(flywheel, duty_cycle);
    	
    	nrf_delay_ms(1);

      
    }
}
