#include "control.h"
#include "timer_module.h"
#include "math.h"

// to act as local pointers to drive, front and angle structs
struct dc_motor* drive;
struct servo* front;
struct angles_t* angle;

// variable set when we reach our goal
bool nav_complete = false;

// the bikes state
float x = 0.0;
float y = 0.0;
float heading = 0;

// variables used by the controller
float x_dest = 0.0;
float y_dest = 0.0;
float euclidean = 0.0;

// used for state estimations
float last_update_timestamp = 0;
int last_update_back_PWM = 0;

// timer for triggering state updates
APP_TIMER_DEF(m_repeated_timer_id);

void update_state(float delta_t) {
	float rad_s = (2 * PI * last_update_back_PWM * DRIVE_RPM_PER_PWM) / 60.0;
	float v_b = WHEEL_RADIUS_M * rad_s; // linear velocity of the bike in m/s

	x += v_b * cos(heading * TO_RAD) * delta_t; // update global x
	y += v_b * sin(heading * TO_RAD) * delta_t; // update global y
}


static void tracking_handler(void * p_context) {
	float current_timestamp = angle->time_stamp;
	float delta_t = (current_timestamp - last_update_timestamp) * PRESCALE_CONV; // get delta_t in seconds

	update_state(delta_t);

	//  Update old PWM
	int pwm = (int) drive->duty_cycle;
	int direction = (int) drive->direction;
	last_update_back_PWM = pwm*direction;

	//Update old timestamp
	last_update_timestamp = current_timestamp;

  return;
}

void init_tracking(struct dc_motor* drive_motor, struct servo* front_servo, struct angles_t * angles) {
	drive = drive_motor;
	front = front_servo;
	angle = angles;

	// Get a repeating timer running
	init_timer_module();
	ret_code_t err_code;

	err_code = app_timer_create(&m_repeated_timer_id, APP_TIMER_MODE_REPEATED, tracking_handler);
	APP_ERROR_CHECK(err_code);
}

void start_tracking() {
	ret_code_t err_code;
	start_timer();
	err_code = app_timer_start(m_repeated_timer_id, APP_TIMER_TICKS(100.0), NULL);
	APP_ERROR_CHECK(err_code);
}

void stop_tracking() {
	ret_code_t err_code;
	end_timer();
	err_code = app_timer_stop(m_repeated_timer_id);
	APP_ERROR_CHECK(err_code);
}

void reset_tracking() {
	x = 0.0;
	y = 0.0;
	heading = 0.0;
	last_update_timestamp = angle->time_stamp;
	last_update_back_PWM = (int) ((int) (drive->duty_cycle)) * drive->direction;
}

void get_bike_state(float* x_coo, float* y_coo, float* heading_coo) {
	*x_coo = x;
	*y_coo = y;
	*heading_coo = heading;
	return;
}

void set_dest(float path_len, float path_angle){
	float path_angle_deg = path_angle * 360 / 255; // convert 0 to 255 range to 0 to 360
	float path_angle_rad = path_angle_deg * TO_RAD;
	x_dest = path_len * TO_M * cos(path_angle_rad); // convert to metres
	y_dest = path_len * TO_M * sin(path_angle_rad); // convert to metres
	// Get the x_dest and y_dest in terms of the global coordinate frame
	// float heading_rad = heading * TO_RAD;
	// x_dest = x_d * cos(heading_rad) - y_d * sin(heading_rad) + x;
	// y_dest = x_d * sin(heading_rad) + y_d * cos(heading_rad) + y;
	printf("X_GOAL: %f, Y_GOAL: %f, HEADING: %f\n", x_dest, y_dest, heading);
}

float calc_alpha() {
	float angle2dest = atan2f((y_dest-y), (x_dest-x));
	float alpha = angle2dest - heading * TO_RAD; // error between heading and angle to destination
	return alpha;
}

float calc_steering() {
	float alpha = calc_alpha();
	float steering = atan2f(2*BIKE_LENGTH_M*sin(alpha),(CONTROL_GAIN*VX)) * (TO_DEG);

	if (steering > MAX_TURN_ANGLE) {
		steering = MAX_TURN_ANGLE;
	}

	else if (steering < -MAX_TURN_ANGLE) {
		steering = -MAX_TURN_ANGLE;
	}

	euclidean = sqrt((x_dest - x)*(x_dest - x) + (y_dest - y)*(y_dest - y)); // euclidean distance to goal

	if (euclidean < 0.2) {
		nav_complete = true;
		printf("GOAL REACHED\n");
	}

	return steering;
}
