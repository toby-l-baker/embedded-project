#ifndef MPU_H
#define MPU_H


#include "setup.h"
#include "mpu9250.h"

#include "MadgwickAHRS.h"

//Contains accelerometer readings in DEGREES
typedef struct angles_t {
  float theta_x;
  float theta_y;
  float theta_z;
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

void init_mpu9250_timer(float ms);
void init_mpu9250();
void update_angles(angles_t* angles);


#endif