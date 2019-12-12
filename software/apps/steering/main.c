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
#include "app_pwm.h"

#include "buckler.h"
#include "servo_driver.h"


#define PIN_NB 27

/* Initialize PWM2 on TIMER 1 */
APP_PWM_INSTANCE(PWM2,1);

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
    // printf("%d\n", (uint8_t) ((-50.0+90.0)*9.0/180.0 + 1.0));
    /* Create Motor Structs */

	// nrf_gpio_pin_set(drive->enable);

	// while(1){
	// 	nrf_delay_ms(25);
	// }



    struct servo * front = create_servo(PIN_NB, PWM_CHANNEL_0, &PWM2);

    /* Initialize 1 CH PWM, 50 Hz */
    app_pwm_config_t pwm_cfg = APP_PWM_DEFAULT_CONFIG_1CH(20000L, front->pin_nb);

    /* Switch the polarity of the first channel. */
    pwm_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    error_code = app_pwm_init(&PWM2, &pwm_cfg, pwm_ready_callback);
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM2);

    /* Set both duty cycles to zero initially, wait until channels are ready */
    //while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
    printf("Enter main loop");

    uint8_t duty_cycle;
    // loop forever
    while(1) {
      for(int i=-50; i < 50; i++) {
        set_servo_angle(front, i);
        printf("%d\n", i);
        nrf_delay_ms(50);
      }
      for(int i=50; i > -50; i--) {
        set_servo_angle(front, i);
        printf("%d\n", i);
        nrf_delay_ms(50);
      }
    }
}
