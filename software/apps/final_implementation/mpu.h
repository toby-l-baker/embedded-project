#ifndef MPU_H
#define MPU_H

#include "MadgwickAHRS.h"
#include "setup.h"

//Contains accelerometer readings in DEGREES
typedef struct angles_t {
  float theta_x;
  float theta_y;
  float theta_z;
  float raw_accel_x;
  float raw_accel_y;
  float raw_accel_z;
  float raw_imu_theta_x;
  float raw_imu_theta_y;
  float raw_imu_theta_z;
  float heading;
  float time_stamp;
} angles_t;

typedef struct Quaternion {
    float w;
    float x;
    float y;
    float z;
} Quaternion;

typedef struct EulerAngles {
    float roll, pitch, yaw;
} EulerAngles;

void init_mpu9250_timer(float ms, bool ble);
void init_mpu9250();
void update_angles(angles_t* angles);


#endif
