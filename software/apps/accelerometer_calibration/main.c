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
#include "slist.h"

// ADC channels
#define X_CHANNEL 0
#define Y_CHANNEL 1
#define Z_CHANNEL 2
#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979
#define NUM_SAMPLES 4096
#define LEN_BUFFER 50

float TAPS[50] = {-6.74184202E-19, 7.23168838E-05, 2.94312721E-04, 6.80624788e-04,
   1.25404475E-03, 2.04323985E-03, 3.07979211E-03, 4.39476698E-03,
   6.01505838E-03, 7.95977580E-03, 1.02369404E-02, 1.28407397E-02,
   1.57495575E-02, 1.89249456E-02, 2.23116458E-02, 2.58387011E-02,
   2.94216243E-02, 3.29655233E-02, 3.63690183E-02, 3.95287309E-02,
   4.23440888E-02, 4.47221621E-02, 4.65822437E-02, 4.78599001E-02,
   4.85102462E-02, 4.85102462E-02, 4.78599001E-02, 4.65822437E-02,
   4.47221621E-02, 4.23440888E-02, 3.95287309E-02, 3.63690183E-02,
   3.29655233E-02, 2.94216243E-02, 2.58387011E-02, 2.23116458E-02,
   1.89249456E-02, 1.57495575E-02, 1.28407397E-02, 1.02369404E-02,
   7.95977580E-03, 6.01505838E-03, 4.39476698E-03, 3.07979211E-03,
   2.04323985E-03, 1.25404475E-03, 6.80624788E-04, 2.94312721E-04,
   7.23168838E-05, -6.74184202E-19};

/* Struct for storing information about each ADC Channel */
struct adc_channel
{
  // TODO - account for Vcc not being 3.3 as in datasheet (changes expected bias and sensitivity)
  uint8_t channel;
  float bias;
  float sensitivity;
  float voltage; //most recent voltage reading
  float filtered_voltage;
  float theta; //most recent angle
  float g; //most recent reading in units of g
};

struct adc_channel * channel_create(float sensitivity, float bias, uint8_t channel)
{
    struct adc_channel * ch = malloc(sizeof(struct adc_channel));
    if (ch) {
        ch->channel = channel;
        ch->bias = bias;
        ch->sensitivity = sensitivity;
    }
    return ch;
}


// sample a particular analog channel in blocking mode
nrf_saadc_value_t sample_value (uint8_t channel) {
  nrf_saadc_value_t val;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
  APP_ERROR_CHECK(error_code);
  return val;
}

/* Reads ADC code and converts it to a voltage */
void read_adc_voltage(struct adc_channel * ch) {
  nrf_saadc_value_t val = sample_value(ch->channel);
  ch->voltage = val*LSB_SIZE;
}

void adc_buf_init(struct adc_channel * ch, struct slist * list) {
  printf("Entering ADC Buf Init\n");
  for (int i = 0; i < LEN_BUFFER; i++) {
    read_adc_voltage(ch);
    printf("%f\n", ch->voltage);
    slist_add_head(list, (ch->voltage));
  }
  printf("Leaving ADC init\n");
}

void read_and_filter_voltage(struct adc_channel * ch, struct slist * list) {
  ch->filtered_voltage = 0;
  // pop a data point off the buffer and read in a new voltage
  slist_remove_tail(list);
  read_adc_voltage(ch);
  slist_add_head(list, (ch->voltage));
  snode* point = list->head; //variable to keep track of our place in the circ buffer
  int i = 0;
  while (point->next != NULL) { //iterate through the whole buffer
    (ch->filtered_voltage) += TAPS[i] * (point->data);
    point = point->next;
    i++;
  }
}

/* Converts a given voltage reading into units of gravity */
float vtog(struct adc_channel * channel) {
  float g = (channel->voltage - channel->bias) / channel->sensitivity;
  return g;
}

// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
}

bool sample = false;

void adc_callback() {
  sample = true;
}

void regular_sampling() {
  float x[NUM_SAMPLES] = {0};
  float y[NUM_SAMPLES] = {0};
  float z[NUM_SAMPLES] = {0};

  uint32_t i = 0;
  uint32_t j = 0;

  while (i < NUM_SAMPLES) {
    if (sample) { // && complete
      // read ADC voltages
      x[i] = sample_value(X_CHANNEL)*LSB_SIZE;
      y[i] = sample_value(Y_CHANNEL)*LSB_SIZE;
      z[i] = sample_value(Z_CHANNEL)*LSB_SIZE;

      sample = false;
      i++;
    }
  }

  printf("Completed Sampling\n");

  while (j < NUM_SAMPLES) {
    // print voltages from ADC
    printf("%.8f,%.8f,%.8f\n", x[j], y[j], z[j]);
    //printf("%.8f\n", x[j]);
    nrf_delay_ms(10);
    j++;
  }

  printf("Completed Printing Samples\n");
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

  struct adc_channel * x_ch = channel_create(0.0, 0.0, X_CHANNEL);
  slist * x_list = slist_create();

  struct adc_channel * y_ch = channel_create(0.0, 0.0, Y_CHANNEL);
  slist * y_list = slist_create();

  struct adc_channel * z_ch = channel_create(0.0, 0.0, Z_CHANNEL);
  slist * z_list = slist_create();
  // fill up the circular buffers of adc readings
  adc_buf_init(x_ch, x_list);
  adc_buf_init(y_ch, y_list);
  adc_buf_init(z_ch, z_list);

  // Write headers
  printf("%s\n","X (V), Y (V), Z (V), X_F (V), Y_F (V), Z_F (V)");


  bool complete = false;
  // Start timer if we want to sample accelerometer at regular intervals
  if (complete) {
    virtual_timer_start_repeated(125, adc_callback);
  }

  // loop forever
  while(1) {
    if (complete) { //For collecting data for filter design
      regular_sampling(); //NUM_SAMPLES defined as a global
      complete = false;
    } else { //For calibration
      read_adc_voltage(x_ch);
      read_adc_voltage(y_ch);
      read_adc_voltage(z_ch);
      read_and_filter_voltage(x_ch, x_list);
      read_and_filter_voltage(y_ch, y_list);
      read_and_filter_voltage(z_ch, z_list);

      printf("%.8f,%.8f,%.8f,", (x_ch->voltage), (y_ch->voltage), (z_ch->voltage));
      printf("%.8f,%.8f,%.8f\n", (x_ch->filtered_voltage), (y_ch->filtered_voltage), (z_ch->filtered_voltage));
      nrf_delay_ms(100);
    }
  }
}
