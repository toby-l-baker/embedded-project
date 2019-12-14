#ifndef MPU9250_INTERFACE
#define MPU9250_INTERFACE

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
#include "app_timer.h"

#include "buckler.h"
#include "display.h"
#include "mpu9250.h"
#include "mpu9250_interface.h"
#include "math.h"
// #include "MadgwickAHRS.h"
#include "MadgwickAHRS.h"

//Contains accelerometer readings in DEGREES
typedef struct angles_t {
  float theta_x;
	float theta_y;
	float theta_z;
  float timestamp;
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
