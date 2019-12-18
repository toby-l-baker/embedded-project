/*** Final Implementation of the Semi-Autonomous Bike Demo ***/

#include "setup.h" // includes all libraries we use

/* Function for printing the state that the bike is in for debugging ***/
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
	float x, y, heading, turn_auto = 0.0; // variables for the current x, y and heading of the bike
	bool destination_set = false;

  // setup the board
  initialize_buckler();
  // setup BLE
  init_ble_receiver();

	// setup the servo
	struct servo * front = create_servo(SERVO_PIN, PWM_CHANNEL_0);
  initialize_servo_motor_pwm(front);

  // setup the drive motor and flywheel
	struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
  int8_t direction = STOP; // direction for the drive motor
  struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
	initialize_dc_motor_pwm(drive, flywheel);

  // setup the mpu9250 software interrupts and the sensor (I2C etc.)
  bool ble = true; // to avoid double initializing app_timer
  init_mpu9250_timer(IMU_TIMER_REFRESH_RATE, ble);
  init_mpu9250();

  // setup the angles struct - struct in mpu.h
  struct angles_t * angles = malloc(sizeof(angles_t));
  angles->heading = 0.0;

  // setup tail lights (initializes the display)
  init_tail_lights();

  // initialise the tracking by giving it a pointer to drive motor, servo motor and the angles struct
  init_tracking(drive, front, angles);

  // begin tracking the bikes position
  start_tracking();

	// loop forever, running state machine
  printf("Entering Main\n");

  // used to print less often
  int i = 0;

	while (1) {
    // update the angles struct (internally checks if the timer has fired)
    update_angles(angles);

    // update the blinkers on the LCD screen
    update_lights(angles);

    /*** STATE MACHINE ***/
    switch(bike_state) { // state transitions handled by the callback function for BLE events
      /*** BIKE OFF ***/
      case OFF: {
        print_state(bike_state);
        break; // each case needs to end with break!
      }

      /*** MANUAL CONTROL OF THE BIKE ***/
      case MANUAL: {
        print_state(bike_state);

        // set servo angle to the user defined angle (from joystick)
        set_servo_angle(front, (float) turn_angle);

        // set the bikes direction based off the drive_speed
        if (drive_speed == 0){
          direction = STOP;
        } else if (drive_speed > 0) {
        	direction = FORWARD;
        } else {
          direction = REVERSE;
        }
        // send values to the drive_motor
        set_dc_motor_direction(drive, direction);
    	  set_dc_motor_pwm(drive, drive_speed);

        // print debug info every 20 loops
        if (i++ % 20 == 0) {
          printf("%f\n", angles->time_stamp);
        }
        break;
      }

      /*** AUTONOMOUS CONTROL OF THE BIKE ***/
      case AUTONOMOUS: {
        print_state(bike_state);
        // Wait until our path is defined or we have accomplished our goal
        if ((path_angle_received == false) || (path_len_received == false) || (nav_complete == true)) {
          direction = STOP;
          drive_speed = 0;
          turn_auto = 0;
          // printf("IM DOING NOTHING: nav %d, len: %d, ang: %d\n", nav_complete, path_len_received, path_angle_received);
          // ensures the gyro is reset before we start navigation
          mpu9250_stop_gyro_integration();
        } else {
          /*** GET TO OUR GOAL LOCATION ***/
          if (destination_set == false) { // only set our destination once
            reset_tracking(); // reset our initial position (function in control.c)
            mpu9250_start_gyro_integration(); // begin integrating gyro to track our heading
            printf("Angle:%d, Length:%d\n", path_angle, path_len);  // print the polar coords the bike was given
            set_dest(path_len, path_angle); // set the bikes destination
            destination_set = true;
          }

          angles->heading = mpu9250_read_gyro_integration().z_axis; // update the heading
          direction = FORWARD;
          drive_speed = 100; // set PWM to 100
          turn_auto = calc_steering(); // find the steering angle determined by control

          // every 20 loops print current state and turn command
          if (i++ % 20 == 0) {
            printf("x: %f, y %f, heading: %f, turn angle: %f\n", x, y, heading, turn_auto);
          }
        }
        // set drive motor and sero angle to desired angles and speed
        set_dc_motor_direction(drive, direction);
        set_dc_motor_pwm(drive, drive_speed); // always drive at a constant speed in auto mode
        set_servo_angle(front, turn_auto);
        break;
      }
    }
    nrf_delay_ms(1);
	}
}
