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
#include "virtual_timer.h"

// ADC channels
#define X_CHANNEL 0
#define Y_CHANNEL 1
#define Z_CHANNEL 2
#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979
#define NUM_SAMPLES 1024

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

bool sample = false;

void adc_callback() {
  sample = true;
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


  // Don't forget to initialize your timer library
  virtual_timer_init();
  nrf_delay_ms(3000);

  virtual_timer_start_repeated(125, adc_callback);

  struct adc_channel x_ch;
  struct adc_channel y_ch;
  struct adc_channel z_ch;

  // Write headers
  printf("%s,%s,%s\n","X (V)", "Y (V)", "Z (V)");
  uint32_t i = 0;
  uint32_t j = 0;

  float x[NUM_SAMPLES] = {0};
  float y[NUM_SAMPLES] = {0};
  float z[NUM_SAMPLES] = {0};
  bool complete = true;
  // loop forever
  while(1) {
    while (i < NUM_SAMPLES) {
      if (sample && complete) { // && complete
        // read ADC voltages
        x[i] = read_adc_voltage(X_CHANNEL);
        y[i] = read_adc_voltage(Y_CHANNEL);
        z[i] = read_adc_voltage(Z_CHANNEL);

        //x_ch.voltage = read_adc_voltage(X_CHANNEL);
        //y_ch.voltage = read_adc_voltage(Y_CHANNEL);
        //z_ch.voltage = read_adc_voltage(Z_CHANNEL);

        sample = false;
        i++;
      }
    }
    i = 0;
    while (j < NUM_SAMPLES) {
      // print voltages from ADC
      //printf("%.8f,%.8f,%.8f\n", x_ch.voltage, y_ch.voltage, z_ch.voltage);
      if (complete) {
        printf("%.8f,%.8f,%.8f\n", x[j], y[j], z[j]);
        nrf_delay_ms(10);
        j++;
      }
    }
    complete = false;
    j = 0;
    printf("DONE");
    nrf_delay_ms(100);
  }
}
