#include "control.h"
#include "timer_module.h"
#include "math.h"

struct dc_motor* drive;
struct servo* front;
struct angles_t*  angle;

bool nav_complete = false;

float x = 0.0;
float y = 0.0;
float heading = 0;

float x_dest = 0.0;
float y_dest = 0.0;

float last_update_timestamp = 0;
float last_update_front_PWM = 0;
int last_update_back_PWM = 0;

APP_TIMER_DEF(m_repeated_timer_id);

void update_state(float delta_t) {
	float v_b = 0.01*last_update_back_PWM * 6.28* WHEEL_RADIUS_CM*DRIVE_RPM_PER_PWM/60.0;
	float l = BIKE_LENGTH_M;
	// float nu = 3.14/180*((last_update_front_PWM-2)*170/12.0 - 85.0);

	x += v_b * cos(heading * TO_RAD) * delta_t;
	y += v_b * sin(heading * TO_RAD) * delta_t;
	// heading += v_b * tan(nu)/l * delta_t;;
}


static void tracking_handler(void * p_context) {
	float current_timestamp = angle->time_stamp;
	// printf("%f\n", current_timestamp);
	float delta_t = (current_timestamp - last_update_timestamp) * PRESCALE_CONV;
	// printf("DELTA_T: %f\n", delta_t);

	update_state(delta_t);

	//Update old PWM
	int pwm = (int) drive->duty_cycle;
	int direction = (int) drive->direction;
	last_update_back_PWM = pwm*direction;

	//Update old timestamp
	last_update_timestamp = current_timestamp;
	//Update old front heading
	heading = angle->heading;//front->duty_cycle;

  return;
}

ret_code_t init_tracking(struct dc_motor* drive_motor, struct servo* front_servo, struct angles_t * angles) {
	drive = drive_motor;
	front = front_servo;
	angle = angles;
	//Get a repeating timer running
	init_timer_module();
	ret_code_t err_code;

	// err_code = nrf_drv_clock_init();
  // APP_ERROR_CHECK(err_code);
  // nrf_drv_clock_lfclk_request(NULL);

	// app_timer_init();

	err_code = app_timer_create(&m_repeated_timer_id, APP_TIMER_MODE_REPEATED, tracking_handler);
	APP_ERROR_CHECK(err_code);
	return err_code;
}

ret_code_t start_tracking() {

	ret_code_t err_code;

	start_timer();

	err_code = app_timer_start(m_repeated_timer_id, APP_TIMER_TICKS(100.0), NULL);
	APP_ERROR_CHECK(err_code);
	return err_code;
}

ret_code_t stop_tracking() {
	ret_code_t err_code;
	end_timer();
	err_code = app_timer_stop(m_repeated_timer_id);
	APP_ERROR_CHECK(err_code);
	return err_code;
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
	float path_angle_deg = path_angle * 360 / 255;
	float path_angle_rad = path_angle_deg * TO_RAD;
	float x_d = path_len / 100 * sin(path_angle_rad);
	float y_d = path_len / 100 * cos(path_angle_rad);
	// Get the x_dest and y_dest in terms of the global coordinate frame
	float heading_rad = heading * TO_RAD;
	x_dest = x_d * cos(heading_rad) - y_d * sin(heading_rad) + x;
	y_dest = x_d * sin(heading_rad) + y_d * cos(heading_rad) + y;
	printf("X_GOAL: %f, Y_GOAL: %f\n", x_dest, y_dest);
}

// void get_dest(){

// }

float calc_alpha() {
	float angle2dest = atan2f((x-x_dest),(y-y_dest));
	float alpha = heading - angle2dest;
	return alpha;
}

float calc_steering() {
	float k = 1; // Gain
	float vx = 1; // Constant longitutanal velocity
	float L = 1; // Bike Length

	float alpha = calc_alpha();
	float steering = atanf(2*L*sin(alpha)/(k*vx)) * (180.0/3.14) ;

	if (steering > 45.0) {
		steering = 45.0;
	}

	else if (steering < -45.0) {
		steering = -45.0;
	}
	float euclidean = sqrt((x_dest - x)*(x_dest - x) + (y_dest - y)*(y_dest - y));
	if (euclidean < 0.2) {
		nav_complete = true;
	}

	return steering;
}
