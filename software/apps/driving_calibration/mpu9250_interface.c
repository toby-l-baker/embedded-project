#include "mpu9250_interface.h"

#define TO_RAD 3.1415926535 / 180
#define TO_DEG 180 / 3.1415926535

mpu9250_measurement_t accel;
mpu9250_measurement_t gyro;

Quaternion quat;
EulerAngles euler;

bool sample_imu = false;

NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
    
}

EulerAngles ToEulerAngles(Quaternion q) {
    EulerAngles angles;

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = TO_DEG * atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = 2 * (q.w * q.y - q.z * q.x);
    // if (abs(sinp) >= 1)
    //     angles.pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    // else
    angles.pitch = TO_DEG * asin(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = TO_DEG * atan2(siny_cosp, cosy_cosp);

    return angles;
}

static void imu_callback(void * p_context)
{
  sample_imu = true;
}

void init_mpu9250() {
  ret_code_t error_code = NRF_SUCCESS;
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);
  mpu9250_init(&twi_mngr_instance);
  printf("IMU initialized!\n");
}

void init_mpu9250_timer(float ms) {
  ret_code_t error_code = NRF_SUCCESS;
  lfclk_request();
  //Initlize timer library
  error_code = app_timer_init();
  APP_ERROR_CHECK(error_code);
  // Create timer instance
  APP_TIMER_DEF(IMU_TIMER);
  error_code = app_timer_create(&IMU_TIMER, APP_TIMER_MODE_REPEATED, imu_callback);
  APP_ERROR_CHECK(error_code);
  uint32_t ticks = APP_TIMER_TICKS(ms); // period for 512Hz sampling
  printf("Timer Initialized!\n Num Ticks: %d\n", ticks);
  error_code = app_timer_start(IMU_TIMER, ticks, NULL);
  APP_ERROR_CHECK(error_code);
  printf("Timer Started!\n");
}

void init_clock_time() {
  NRF_TIMER4->MODE          = 0;    //Timer not counter
  NRF_TIMER4->BITMODE       = 3; //32 bits
  NRF_TIMER4->PRESCALER     = 9; //31 250 Hz
  NRF_TIMER4->TASKS_CLEAR   = 1; //Reset value to 0
  NRF_TIMER4->TASKS_START   = 1;
  printf("Clock time initialized\n");
  return;
}

float read_clock_time() {
  NRF_TIMER4->TASKS_CAPTURE[1] = 1;
  uint32_t time_stamp_int = NRF_TIMER4->CC[1];
  float time_stamp = ((float) time_stamp_int) / (CLOCK_SPEED / PRESCALER_VALUE);
  return time_stamp;

}


void update_angles(angles_t * angles) {
  if (sample_imu) {
    gyro = mpu9250_read_gyro();
    accel = mpu9250_read_accelerometer();
    //MadgwickAHRSupdateIMU(gyro.x_axis * TO_RAD, gyro.y_axis * TO_RAD, gyro.z_axis * TO_RAD, accel.x_axis* 9.81, accel.y_axis * 9.81, accel.z_axis * 9.81);
    MadgwickAHRSupdateIMU(gyro.x_axis * TO_RAD, gyro.y_axis * TO_RAD, gyro.z_axis * TO_RAD, accel.x_axis, accel.y_axis, accel.z_axis);
    quat.w = q0;
    quat.x = q1;
    quat.y = q2;
    quat.z = q3;
    euler = ToEulerAngles(quat);
    angles->theta_x = euler.roll;
    angles->theta_y = euler.pitch;
    angles->theta_z = euler.yaw;
    angles->time_stamp = read_clock_time();
    sample_imu = false;
  }
}
