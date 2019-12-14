// Robot Template app
//
// Framework for creating applications that control the Kobuki robot

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_spi.h"

#include "buckler.h"
#include "display.h"
#include "kobukiActuator.h"
#include "kobukiSensorPoll.h"
#include "kobukiSensorTypes.h"
#include "kobukiUtilities.h"
#include "mpu9250.h"
#include "simple_ble.h"

#include "states.h"
#include "servo_driver.h"
#include "motor.h"

// I2C manager
// NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

/* Create PWM instance for Servo. */
APP_PWM_INSTANCE(PWM2, 3);

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
        // c0:98:e5:49:xx:xx
        .platform_id       = 0x49,    // used as 4th octect in device BLE address
        .device_id         = 0x000b, // TODO: replace with your lab bench number
        .adv_name          = "KOBUKI", // used in advertisements if there is room
        .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS),
};

//4607eda0-f65e-4d59-a9ff-84420d87a4ca
//stored in little endian
static simple_ble_service_t bike_srv = {{
    .uuid128 = {0xca,0xa4,0x87,0x0d,0x42,0x84,0xff,0xA9,
                0x59,0x4D,0x5e,0xf6,0xa0,0xed,0x07,0x46}
}};

static simple_ble_char_t manual_char = {.uuid16 = 0xeda1};
bool manual_control=0;

static simple_ble_char_t power_char = {.uuid16 = 0xeda2};
bool power=0;

static simple_ble_char_t drive_char = {.uuid16 = 0xeda3};
int8_t drive_speed=0;

static simple_ble_char_t turn_char = {.uuid16 = 0xeda4};
int8_t turn_angle=0;

static simple_ble_char_t path_char = {.uuid16 = 0xeda5};
float* path_plan;

simple_ble_app_t* simple_ble_app;

bike_states bike_state;
autonomous_states auto_state;


void ble_evt_write(ble_evt_t const* p_ble_evt) {
    // TODO: logic for each characteristic and related state changes

    if (simple_ble_is_char_event(p_ble_evt, &manual_char)) {
      if (manual_control) {
        bike_state = MANUAL;
      } else {
        bike_state = AUTONOMOUS;
      }

    } else if (simple_ble_is_char_event(p_ble_evt, &power_char)) {
      if (power) {
        bike_state = MANUAL;
      } else {
        bike_state = OFF;
      }
    } else if (simple_ble_is_char_event(p_ble_evt, &turn_char)) {
      auto_state = DRIVE;
    }
}

void print_state(bike_states current_state){
	switch(current_state){
  	case OFF: {
  		printf("Off\n");
  		break;
    }

    case MANUAL: {
  		printf("Manual\n");
  		break;
    }

    case AUTONOMOUS: {
  		printf("Autonomous\n");
  		break;
    }
  }
}

int main(void) {
    ret_code_t error_code = NRF_SUCCESS;

	// initialize RTT library
	error_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(error_code);
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	printf("Log initialized!\n");

	// Setup BLE
	simple_ble_app = simple_ble_init(&ble_config);

	simple_ble_add_service(&bike_srv);

	// Start Advertising
	simple_ble_adv_only_name();

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(manual_control), (uint8_t*)&manual_control,
	  &bike_srv, &manual_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(power), (uint8_t*)&power,
	  &bike_srv, &power_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(drive_speed), (uint8_t*)&drive_speed,
	  &bike_srv, &drive_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(turn_angle), (uint8_t*)&turn_angle,
	  &bike_srv, &turn_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(*path_plan), (float*)path_plan,
	  &bike_srv, &path_char);

	/* Initialize servo. */
	struct servo * front = create_servo(SERVO_PIN, PWM_CHANNEL_0, &PWM2);
	/* Initialize 1 CH PWM, 50 Hz */
	app_pwm_config_t pwm_cfg = APP_PWM_DEFAULT_CONFIG_1CH(20000L, front->pin_nb);
	/* Switch the polarity of the first channel. */
	pwm_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
	error_code = app_pwm_init(&PWM2, &pwm_cfg, pwm_ready_callback);
	APP_ERROR_CHECK(error_code);
	app_pwm_enable(&PWM2);

	struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
  struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
	initialize_dc_motor_pwm(flywheel, drive);

	float duty_cycle = 0;
  int8_t direction = STOP;
	// loop forever, running state machine
	while (1) {
    char print_string[16];
    // read sensors from robot
    // TODO: complete state machine
    switch(bike_state) {
      /*** BIKE OFF, ONLY BALANCING ***/
      case OFF: {
        print_state(bike_state);
        // transition logic handled by ble_evt_write
        break; // each case needs to end with break!
      }

      /*** MANUAL CONTROL OF THE BIKE ***/
      case MANUAL: {
        sprintf(print_string, "Spd:%d  Ang:%d", drive_speed, turn_angle);
        print_state(bike_state);

        //set servo angle
        printf("Turning Angle:%d\n", turn_angle);
        set_servo_angle(front, (float) turn_angle);
        nrf_delay_ms(50);

        if (drive_speed < 0){
        	// if speed is negative reverse
        	set_dc_motor_direction(drive, REVERSE);
          set_dc_motor_direction(flywheel, REVERSE);
        } else {
        	// if speed is positive move forward
        	set_dc_motor_direction(drive, FORWARD);
          set_dc_motor_direction(flywheel, FORWARD);
        }
        // set the PWM to the speed TODO: Map speed to PWM
        printf("Drive Speed PWM:%d\n", drive_speed);
    	  set_dc_motor_pwm(drive, drive_speed);
        //set_dc_motor_pwm(flywheel, drive_speed);
        nrf_delay_ms(50);

        break;
      }

      /*** AUTOOMOUS CONTROL OF THE BIKE ***/
      case AUTONOMOUS: {
        print_state(bike_state);
        switch(auto_state){
          /*** GET THE PATH FOR THE BIKE TO AUTONOMOUSLY FOLLOW ***/
          float actual_path_plan;
          case GET_PATH: {
            //Integrate the joystick to get a vector
            printf("Getting path\n");
            // actual_path_plan = (float)  path_plan;
            sprintf(print_string, "r:%f  theta:%f", path_plan[0], path_plan[1]);
            break;
          }
          /*** FOLLOW THE SET PATH ***/
          case DRIVE: {
            //Get to our goal!!
            printf("Navigating to path\n");
            //Keep navigating to our goal pos
            break;
          }
        }
        break;
      }
    }
    nrf_delay_ms(5);
	}
}
