// Robot Template app
//
// Framework for creating applications that control the Kobuki robot

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

#include "buckler.h"
#include "display.h"
#include "mpu9250.h"
#include "math.h"
// #include "MadgwickAHRS.h"
#include "MahonyAHRS.h"


#define PI 3.1415926535
#define TO_RAD 3.1415926535 / 180

// I2C manager
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

typedef struct Quaternion {
    float w;
    float x;
    float y;
    float z;
} Quaternion;

typedef struct EulerAngles {
    float roll, pitch, yaw;
}EulerAngles;

EulerAngles ToEulerAngles(Quaternion q) {
    EulerAngles angles;

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = (180 / PI) *atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = 2 * (q.w * q.y - q.z * q.x);
    // if (abs(sinp) >= 1)
    //     angles.pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    // else
    angles.pitch = asin(sinp) * (180 / PI);

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = atan2(siny_cosp, cosy_cosp) * (180 / PI);

    return angles;
}



int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  // initialize LEDs
  nrf_gpio_pin_dir_set(23, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(24, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(25, NRF_GPIO_PIN_DIR_OUTPUT);

  /* initialize display */
  nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
  nrf_drv_spi_config_t spi_config = {
    .sck_pin = BUCKLER_LCD_SCLK,
    .mosi_pin = BUCKLER_LCD_MOSI,
    .miso_pin = BUCKLER_LCD_MISO,
    .ss_pin = BUCKLER_LCD_CS,
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0,
    .frequency = NRF_DRV_SPI_FREQ_4M,
    .mode = NRF_DRV_SPI_MODE_2,
    .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  };
  error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(error_code);
  display_init(&spi_instance);
  display_write("Hello, Human!", DISPLAY_LINE_0);
  printf("Display initialized!\n");

  // initialize i2c master (two wire interface)
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);
  mpu9250_init(&twi_mngr_instance);
  printf("IMU initialized!\n");

  /* Initialize values for each measurement unit */
  // float gyro_ang;
  // float mag_ang;
  // float accel_ang;
  mpu9250_measurement_t mag;
  mpu9250_measurement_t accel;
  mpu9250_measurement_t gyro;

  Quaternion quat;
  EulerAngles euler;

  // loop forever, running state machine
  uint32_t i = 0;
  float min = 180;
  float max = -180;
  while (1) {
    // Note: removing this delay will make responses quicker, but will result
    //  in printf's in this loop breaking JTAG
    nrf_delay_ms(1);

    gyro = mpu9250_read_gyro();
    mag = mpu9250_read_magnetometer();
    accel = mpu9250_read_accelerometer();
    // angle_mag = (180 / PI) * atan2(mag_reading.y_axis, mag_reading.x_axis);
    // angle_accel = (180 / PI) * asin(mpu9250_read_accelerometer().y_axis);
    //MadgwickAHRSupdateIMU(gyro.x_axis * TO_RAD, gyro.y_axis * TO_RAD, gyro.z_axis * TO_RAD, accel.x_axis* 9.81, accel.y_axis * 9.81, accel.z_axis * 9.81);
    MahonyAHRSupdateIMU(gyro.x_axis * TO_RAD, gyro.y_axis * TO_RAD, gyro.z_axis * TO_RAD, accel.x_axis* 9.81, accel.y_axis * 9.81, accel.z_axis * 9.81);
    quat.w = q0;
    quat.x = q1;
    quat.y = q2;
    quat.z = q3;
    euler = ToEulerAngles(quat);
    if (euler.roll < min) {
      min = euler.roll;
    } else if (euler.roll > max) {
      max = euler.roll;
    }
    printf("Roll: %.4f, Pitch: %.4f, Yaw: %.4f\n", euler.roll, euler.pitch, euler.yaw);
    // printf("Gyro_X: %.4f, Gyro_Y: %.4f, Gyro_z: %.4f\n", gyro.x_axis, gyro.y_axis, gyro.z_axis);
    // printf("Accel_X: %.4f, Accel_Y: %.4f, Accel_Z: %.4f\n", accel.x_axis, accel.y_axis, accel.z_axis);
    printf("Min Roll: %f, Max Roll: %f\n", min, max);
    i++;
  }
}
