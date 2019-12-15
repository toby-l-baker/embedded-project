#ifndef CONTROL_H
#define CONTROL_H


#include "setup.h"
#include "board_params.h"
#include "motor.h"
#include "servo_driver.h"
#include "mpu.h"


//Starts a fast timer that updates
ret_code_t init_tracking(struct dc_motor* drive_motor, struct servo* front_servo, struct angles_t * angles);
ret_code_t start_tracking();
ret_code_t stop_tracking();

void reset_tracking();

float get_distance();
float get_heading();
void get_bike_state(float * x_coo, float * y_coo, float * heading_coo);

float calc_steering();
void set_dest(float x_d, float y_d);

#endif
