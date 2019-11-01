// Analog accelerometer app
//
// Reads data from the ADXL327 analog accelerometer

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "nrfx_gpiote.h"
#include "nrfx_saadc.h"

#include "buckler.h"

// ADC channels
#define X_CHANNEL 0
#define Y_CHANNEL 1
#define Z_CHANNEL 2
#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979

/* Struct for storing information about each ADC Channel */
struct adc_channel
{
  // TODO - account for Vcc not being 3.3 as in datasheet (changes expected bias and sensitivity)
  char* name;
  float bias;
  float sensitivity;
  float voltage;
  float theta;
  float g;
};

/* Function to write to a log file to be able to filter accelerometer data */
int write_to_file(struct adc_channel x, struct adc_channel y, struct adc_channel z, char const *fileName)
{
  FILE *f=fopen(fileName,"a");
  if(f==NULL) return -1;
  fprintf(f,"%f,%f,%f\n", x.voltage, y.voltage, z.voltage);
  fclose(f);
  return 0;
}

/* Converts a given voltage reading into units of gravity */
float vtog(struct adc_channel channel) {
  float g = (channel.voltage - channel.bias) / channel.sensitivity;
  return g;
}

// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
}

// sample a particular analog channel in blocking mode
nrf_saadc_value_t sample_value (uint8_t channel) {
  nrf_saadc_value_t val;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
  APP_ERROR_CHECK(error_code);
  return val;
}

/* Reads ADC code and converts it to a voltage */
float read_adc_voltage(uint8_t channel) {
  nrf_saadc_value_t val = sample_value(channel);
  float voltage = val*LSB_SIZE;
  return voltage;
}

int main (void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // initialize analog to digital converter
  nrfx_saadc_config_t saadc_config = NRFX_SAADC_DEFAULT_CONFIG;
  saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
  error_code = nrfx_saadc_init(&saadc_config, saadc_callback);
  APP_ERROR_CHECK(error_code);

  // initialize analog inputs
  // configure with 0 as input pin for now
  nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(0);
  channel_config.gain = NRF_SAADC_GAIN1_6; // input gain of 1/6 Volts/Volt, multiply incoming signal by (1/6)
  channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6 Volt reference, input after gain can be 0 to 0.6 Volts

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_X;
  error_code = nrfx_saadc_channel_init(X_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Y;
  error_code = nrfx_saadc_channel_init(Y_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Z;
  error_code = nrfx_saadc_channel_init(Z_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // initialization complete
  printf("Buckler initialized!\n");


  struct adc_channel x_ch;
  struct adc_channel y_ch;
  struct adc_channel z_ch;

  bool log_ = false;
  char const * fileName = "X.txt";

  // Write headers
  if (log_) {
    FILE *f=fopen(fileName,"w");
    if(f==NULL) return -1;
    fprintf(f,"%s,%s,%s\n","X (V)", "Y (V)", "Z (V)");
    fclose(f);
  }

  // loop forever
  while (1) {
    // read ADC voltages
    x_ch.voltage = read_adc_voltage(X_CHANNEL);
    y_ch.voltage = read_adc_voltage(Y_CHANNEL);
    z_ch.voltage = read_adc_voltage(Z_CHANNEL);

    // converts voltages to units of gravity
    x_ch.g = vtog(x_ch);
    y_ch.g = vtog(y_ch);
    z_ch.g = vtog(z_ch);

    // find rotation angles about the x, y and z axes.
    x_ch.theta = atan(x_ch.g / sqrt(y_ch.g*y_ch.g + z_ch.g*z_ch.g))/PI*180;
    y_ch.theta = atan(y_ch.g / sqrt(x_ch.g*x_ch.g + z_ch.g*z_ch.g))/PI*180;
    z_ch.theta = atan(z_ch.g / sqrt(y_ch.g*y_ch.g + x_ch.g*x_ch.g))/PI*180;

    // print voltages from ADC
    printf("x: %.2f\ny: %.2f\nz: %.2f\n", x_ch.voltage, y_ch.voltage, z_ch.voltage);
    printf("theta_x: %.2f\ntheta_y: %.2f\ntheta_z: %.2f\n", x_ch.theta, y_ch.theta, z_ch.theta);

    if (log_) {
      write_to_file(x_ch, y_ch, z_ch, fileName);
    }

    nrf_delay_ms(100);
  }
}
