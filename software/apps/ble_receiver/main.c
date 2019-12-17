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
#include "mpu9250.h"
#include "simple_ble.h"
#include "timer_module.h"
#include "states.h"
#include "servo_driver.h"
#include "motor.h"
#include "mpu.h"

// I2C manager
// NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

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

static simple_ble_char_t path_len_char = {.uuid16 = 0xeda5};
uint8_t path_len; // will be range of 0 to 255 cm

static simple_ble_char_t path_angle_char = {.uuid16 = 0xeda6};
int8_t path_angle; //will be 0 to 255, need to do 360/255 * path_angle to get degrees

bool path_len_received = false;
bool path_angle_received = false;
bool destination_set = false;
float turn_auto = 0.0;

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
    } else if (simple_ble_is_char_event(p_ble_evt, &path_len_char)) {
      nav_complete = false;
      path_len_received = true;
      destination_set = false;
    } else if (simple_ble_is_char_event(p_ble_evt, &path_angle_char)) {
      nav_complete = false;
      path_angle_received = true;
      destination_set = false;
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

float x, y, heading;

int main(void) {
  // setup the board
  initialize_buckler();
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
	  sizeof(path_len), (uint8_t*)&path_len,
	  &bike_srv, &path_len_char);

  simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(path_angle), (uint8_t*)&path_angle,
	  &bike_srv, &path_angle_char);

	/* Initialize servo. */
	struct servo * front = create_servo(SERVO_PIN, PWM_CHANNEL_0);
  initialize_servo_motor_pwm(front);

	struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
  struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
	initialize_dc_motor_pwm(drive, flywheel);
  // init_timer();
  bool ble = true;
  init_mpu9250_timer(IMU_TIMER_REFRESH_RATE, ble);
  init_mpu9250();
  int8_t direction = STOP;

  // /* TAIL LIGHTS SETUP */
  struct angles_t * angles = malloc(sizeof(angles_t));
  angles->heading = 0.0;

  init_tail_lights();

  init_buzzer();

  init_tracking(drive, front, angles);
  //
  start_tracking();
	// loop forever, running state machine
  printf("Entering Main\n");
  bool first_time = false;
  float first_timestamp = 0;
  int i = 0;

  mpu9250_start_gyro_integration();
	while (1) {
    char print_string[16];
    // read sensors from robot
    // Update angles and lights
    update_angles(angles);
    update_lights(angles);
    if (direction == REVERSE) {
      buzzer_on();
      // printf("BUZZ\n");
    } else {
      buzzer_off();
    }
    switch(bike_state) {
      /*** BIKE OFF, ONLY BALANCING ***/
      case OFF: {
        // print_state(bike_state);
        // transition logic handled by ble_evt_write
        break; // each case needs to end with break!
      }

      /*** MANUAL CONTROL OF THE BIKE ***/
      case MANUAL: {
        sprintf(print_string, "Spd:%d  Ang:%d", drive_speed, turn_angle);
        print_state(bike_state);
        //set servo angle
        // printf("Drive Speed: %d", drive_speed);
        set_servo_angle(front, (float) turn_angle);
        if (drive_speed == 0){
        	// if speed is negative reverse
          direction = STOP;
        } else if (drive_speed > 0) {
        	// if speed is positive move forward
        	direction = FORWARD;
        } else {
          direction = REVERSE;
        }
        angles->heading = mpu9250_read_gyro_integration().z_axis;
        set_dc_motor_direction(drive, direction);
    	  set_dc_motor_pwm(drive, drive_speed);
        get_bike_state(&x, &y, &heading);
        // if (i++ % 20 == 0) {
        //   // printf("%f\n", angles->time_stamp);
        //   // printf("x: %f, y %f, heading: %f\n", x, y, heading);
        // }
        //set_dc_motor_pwm(flywheel, drive_speed);
        break;
      }

      /*** AUTOOMOUS CONTROL OF THE BIKE ***/
      case AUTONOMOUS: {
        // print_state(bike_state);
        /*** Wait until our path is non-zero then follow ***/
        if ((path_angle_received == false) || (path_len_received == false) || (nav_complete == true)) {
          direction = STOP;
          drive_speed = 0;
          turn_auto = 0;
          // printf("IM DOING NOTHING: nav %d, len: %d, ang: %d\n", nav_complete, path_len_received, path_angle_received);
          mpu9250_stop_gyro_integration();
        } else {
          /*** FOLLOW THE PATH ***/
          if (destination_set == false) {
            reset_tracking();
            mpu9250_start_gyro_integration();
            printf("Angle:%d, Length:%d\n", path_angle, path_len);
            set_dest(path_len, path_angle);
            destination_set = true;
          }
          angles->heading = mpu9250_read_gyro_integration().z_axis;
          direction = FORWARD;
          drive_speed = 100;
          turn_auto = calc_steering();
          // printf("%d\n", turn_angle);
          // if (i++ % 20 == 0) {
            // printf("x: %f, y %f, heading: %f, turn angle: %f\n", x, y, heading, turn_auto);
          // }
        }
        set_dc_motor_direction(drive, direction);
        set_dc_motor_pwm(drive, drive_speed); //always drive at a constant speed in auto mode
        set_servo_angle(front, turn_auto);
        break;
      }
    }
    nrf_delay_ms(1);
	}
}
