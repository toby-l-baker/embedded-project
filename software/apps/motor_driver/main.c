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
#include "app_pwm.h"

#include "buckler.h"

#define FLYWHEEL_PIN NRF_GPIO_PIN_MAP(0,19) //using BUCKLER_SENSORS_SCL
#define ENCODER_CH_A NRF_GPIO_PIN_MAP(0,20) //using BUCKLER_SENSORS_SDA
#define ENCODER_CH_B NRF_GPIO_PIN_MAP(0,7) //using BUCKLER_IMU_INTERUPT
#define DRIVE_PIN NRF_GPIO_PIN_MAP(0,27) //using BUCKLER_LIGHT_INTERRUPT

/* Struct for storing information about each ADC Channel */
struct motor
{
  bool forward;
  bool backward;
  app_pwm_duty_t duty_cycle; // between 0 and 100
  uint8_t pin; //most recent voltage reading
};

struct motor * create_motor(uint8_t pin)
{
    struct motor * motor = malloc(sizeof(struct motor));
    if (motor) {
        motor->duty_cycle = 0;
        motor->pin = pin;
    }
    //enable output pin
    nrf_gpio_cfg_output(pin);
    return motor;
}

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    printf("PWM Initialized\n");
}

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
    struct motor * flywheel = create_motor(FLYWHEEL_PIN);
    struct motor * drive = create_motor(DRIVE_PIN);

    /* Initialize PWM0 on TIMER 1 */
    APP_PWM_INSTANCE(PWM0,1);
    /* Initialize 2 CH PWM, 200 Hz */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, flywheel->pin, drive->pin);
    /* Switch the polarity of the second channel. */
    // pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;

    error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);

    /* Set both duty cycles to zero initially, wait until channels are ready */
    while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
    while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);


    // loop forever
    while(1) {
      for (int i = 0; i<90; i++) {
          flywheel->duty_cycle = i;
          drive->duty_cycle = i;
          while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
          while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);
          nrf_delay_ms(25);
      }

      nrf_delay_ms(100);
    }
}
