#ifndef CONTROL_H
#define CONTROL_H

#include "setup.h"

extern bool nav_complete; // set to true when we acheive our goal

void init_tracking(struct dc_motor* drive_motor, struct servo* front_servo, struct angles_t * angles);

void start_tracking(); //Starts a timer that updates the bike state
void stop_tracking(); //Stops the timer that updates the bike state
void reset_tracking(); // resets heading and (x, y) pos

void get_bike_state(float * x_coo, float * y_coo, float * heading_coo);

void set_dest(float x_d, float y_d);
float calc_steering();

#endif
