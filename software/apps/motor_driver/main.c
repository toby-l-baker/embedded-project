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



#define FORWARD				1
#define BACKWARD			-1
#define STOP				0

#define PWM_CHANNEL_0 		0
#define PWM_CHANNEL_1		1

#define FLYWHEEL_PIN_ENABLE NRF_GPIO_PIN_MAP(0,19)
#define FLYWHEEL_PIN_IN1 NRF_GPIO_PIN_MAP(0,20)
#define FLYWHEEL_PIN_IN2 NRF_GPIO_PIN_MAP(0,22)

#define DRIVE_PIN_ENABLE NRF_GPIO_PIN_MAP(0,23)
#define DRIVE_PIN_IN1 NRF_GPIO_PIN_MAP(0,24)
#define DRIVE_PIN_IN2 NRF_GPIO_PIN_MAP(0,25)

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

struct motor * create_motor(uint8_t enable, uint8_t in1, uint8_t in2, uint8_t pwm_channel, app_pwm_t const * const pwm_inst)
{
    struct motor * motor = malloc(sizeof(struct motor));
    if (motor) {
        motor->duty_cycle = 0;
        motor->enable = enable;
        motor->in1 = in1;
        motor->in2 = in2;
        motor->pwm_channel = pwm_channel;
        motor->pwm_inst = pwm_inst;
        nrf_gpio_cfg_output(enable);
        nrf_gpio_cfg_output(in1);
        nrf_gpio_cfg_output(in2);
        
    }
    return motor;
    
}

void set_motor_direction(struct motor * motor, int8_t direction){
	switch(direction){
		case FORWARD:
			motor->direction = FORWARD;
			nrf_gpio_pin_set(motor->in1);
			nrf_gpio_pin_clear(motor->in2);
			break;
		case BACKWARD:
			motor->direction = BACKWARD;
			nrf_gpio_pin_set(motor->in2);
			nrf_gpio_pin_clear(motor->in1);
			break;
		case STOP:
			motor->direction = STOP;
			nrf_gpio_pin_clear(motor->in1);
			nrf_gpio_pin_clear(motor->in2);
			break;
		default:
			printf("ERROR: incorrect motor direction\n");
			break;
	}
}


void set_motor_pwm(struct motor * motor, uint8_t duty_cycle){
	motor->duty_cycle = duty_cycle;
	while (app_pwm_channel_duty_set(motor->pwm_inst, motor->pwm_channel, motor->duty_cycle) == NRF_ERROR_BUSY);
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
    
	// nrf_gpio_pin_set(drive->enable);
	
	// while(1){
	// 	nrf_delay_ms(25);
	// }
    /* Initialize PWM0 on TIMER 1 */
    APP_PWM_INSTANCE(PWM0,1);

    struct motor * flywheel = create_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, PWM_CHANNEL_0, &PWM0);
    struct motor * drive = create_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, PWM_CHANNEL_1, &PWM0);
    nrf_gpio_pin_set(drive->in1);
    nrf_gpio_pin_clear(drive->in2);
    /* Initialize 2 CH PWM, 200 Hz */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(5000L, flywheel->enable, drive->enable);
    /* Switch the polarity of the second channel. */
    //pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    error_code = app_pwm_init(&PWM0, &pwm1_cfg, pwm_ready_callback);
    APP_ERROR_CHECK(error_code);
    app_pwm_enable(&PWM0);

    /* Set both duty cycles to zero initially, wait until channels are ready */
    while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
    while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);
    set_motor_direction(drive, STOP);
    set_motor_direction(drive, BACKWARD);


    // loop forever
    while(1) {
      for (int i = 0; i<100; i++) {
          flywheel->duty_cycle = i;
          drive->duty_cycle = i;
          set_motor_pwm(flywheel, i);
          set_motor_pwm(drive, i);
          // while (app_pwm_channel_duty_set(&PWM0, 0, flywheel->duty_cycle) == NRF_ERROR_BUSY);
          // while (app_pwm_channel_duty_set(&PWM0, 1, drive->duty_cycle) == NRF_ERROR_BUSY);
          nrf_delay_ms(100);
      }

      nrf_delay_ms(100);

      
    }
}
