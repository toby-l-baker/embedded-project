#include "control.h"
#include "timer_module.h"
#include "math.h"

struct dc_motor* drive;
struct servo* front;
struct angles_t*  angle;


float x = 0.0;
float y = 0.0;
float heading = 0;

float last_update_timestamp = 0;
float last_update_front_PWM = 0;
int last_update_back_PWM = 0;

APP_TIMER_DEF(m_repeated_timer_id);

void update_state(float delta_t) {
	float v_b = 0.01*last_update_back_PWM * 6.28* WHEEL_RADIUS_CM*DRIVE_RPM_PER_PWM/60.0;
	float l = BIKE_LENGTH_M;
	// float nu = 3.14/180*((last_update_front_PWM-2)*170/12.0 - 85.0);

	x += v_b * cos(heading) * delta_t;
	y += v_b * sin(heading) * delta_t;
	// heading += v_b * tan(nu)/l * delta_t;;
}


static void tracking_handler(void * p_context) {
	float current_timestamp = convert_to_secs(get_timer_value());
	float delta_t = current_timestamp - last_update_timestamp;
	// printf("DELTA_T: %f\n", delta_t);

	update_state(delta_t);

	//Update old PWM
	int pwm = (int) drive->duty_cycle;
	int direction = (int) drive->direction;
	last_update_back_PWM = pwm*direction;

	//Update old timestamp
	last_update_timestamp = current_timestamp;
	//Update old front heading
	heading = angle->theta_z;// front->duty_cycle;

    return;
}

ret_code_t init_tracking(struct dc_motor* drive_motor, struct servo* front_servo, struct angles_t * angles) {
	drive = drive_motor;
	front = front_servo;
	angle = angles;
	//Get a repeating timer running
	init_timer_module();
	ret_code_t err_code;

	// ret_code_t err_code = nrf_drv_clock_init();
  //   APP_ERROR_CHECK(err_code);
  //   nrf_drv_clock_lfclk_request(NULL);

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
	last_update_timestamp = convert_to_secs(get_timer_value());
	last_update_back_PWM = (int) ((int) (drive->duty_cycle)) * drive->direction;
}

void get_bike_state(float* x_coo, float* y_coo, float* heading_coo) {
	*x_coo = x;
	*y_coo = y;
	*heading_coo = heading;
	return;
}
