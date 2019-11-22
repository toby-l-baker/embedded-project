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

#include "accelerometer_reading.h"

// ADC channels
#define X_CHANNEL 0
#define Y_CHANNEL 1
#define Z_CHANNEL 2
#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979
#define NUM_SAMPLES 4096
#define LEN_BUFFER 30

#define CLOCK_SPEED 16000000.0
#define PRESCALER_VALUE 512.0


//Buckler stuff
nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(0);
nrfx_saadc_config_t saadc_config = NRFX_SAADC_DEFAULT_CONFIG;

//Reading structs
struct adc_channel * x_ch;
struct adc_channel * y_ch;
struct adc_channel * z_ch;

slist * x_list; 
slist * y_list; 
slist * z_list; 


float TAPS[LEN_BUFFER] = {-1.1390375091067432E-18,0.00035259598715081734,0.001477766783703178,0.00355893500529667,
0.006843359038215591,0.01156690107151631,0.01787102176489292,0.025729190352121574,
0.03489853523601125,0.044908166421298065,0.05508897695608773,0.06464209335547222,
0.07273590538888708,0.07861610666014734,0.08171044597919938,0.08171044597919938,
0.07861610666014733,0.07273590538888705,0.06464209335547222,0.05508897695608773,
0.04490816642129805,0.03489853523601124,0.025729190352121564,0.017871021764892907,
0.011566901071516297,0.006843359038215583,0.003558935005296659,0.001477766783703178,
0.00035259598715081734,-1.1390375091067432E-18};

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
    //printf("%f\n", ch->voltage);
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

/* Converts gravity to an angle */
void update_angles(struct adc_channel * x, struct adc_channel * y, struct adc_channel * z) {
  x->g = (x->filtered_voltage - x->bias) / x->sensitivity;
  y->g = (y->filtered_voltage - y->bias) / y->sensitivity;
  z->g = (z->filtered_voltage - z->bias) / z->sensitivity;

  x->theta = atan(x->g / sqrt(y->g*y->g + z->g*z->g))/PI*180;
  y->theta = atan(y->g / sqrt(x->g*x->g + z->g*z->g))/PI*180;
  z->theta = atan(z->g / sqrt(x->g*x->g + y->g*y->g))/PI*180;
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


void init_clock_time() {
  NRF_TIMER4->MODE          = 0;    //Timer not counter
  NRF_TIMER4->BITMODE       = 3; //32 bits
  NRF_TIMER4->PRESCALER     = 9; //31 250 Hz
  NRF_TIMER4->TASKS_CLEAR   = 1; //Reset value to 0
  NRF_TIMER4->TASKS_START   = 1;
  printf("Clock time initialized\n");
  return;
}

void init_accelerometers() {

  ret_code_t error_code = NRF_SUCCESS;

  // initialize analog to digital converter
  saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
  error_code = nrfx_saadc_init(&saadc_config, saadc_callback);
  APP_ERROR_CHECK(error_code);

  // initialize analog inputs
  // configure with 0 as input pin for now
  channel_config.gain = NRF_SAADC_GAIN1_6; // input gain of 1/6 Volts/Volt, multiply incoming signal by (1/6)
  channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6 Volt reference, input after gain can be 0 to 0.6 Volts

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_X;
  error_code = nrfx_saadc_channel_init(X_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and ini  ret_code_t error_code = NRF_SUCCESS;tialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Y;
  error_code = nrfx_saadc_channel_init(Y_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Z;
  error_code = nrfx_saadc_channel_init(Z_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // initialization complete
  printf("Accelerometer initialized!\n");


  x_ch = channel_create(0.3908, 1.4132, X_CHANNEL);
  x_list = slist_create();

  y_ch = channel_create(0.3869, 1.4009, Y_CHANNEL);
  y_list = slist_create();

  z_ch = channel_create(0.3799, 1.4041, Z_CHANNEL);
  z_list = slist_create();
  // fill up the circular buffers of adc readings
  adc_buf_init(x_ch, x_list);
  adc_buf_init(y_ch, y_list);
  adc_buf_init(z_ch, z_list);

  init_clock_time();
}

float read_clock_time() {
  NRF_TIMER4->TASKS_CAPTURE[1] = 1;
  uint32_t time_stamp_int = NRF_TIMER4->CC[1];
  float time_stamp = ((float) time_stamp_int) / (CLOCK_SPEED / PRESCALER_VALUE);
  return time_stamp;

}

void update_angles_struct(angles_t* angles) {
  read_and_filter_voltage(x_ch, x_list);
  read_and_filter_voltage(y_ch, y_list);
  read_and_filter_voltage(z_ch, z_list);

  update_angles(x_ch, y_ch, z_ch);
  
  angles->theta_x = x_ch->theta;
  angles->theta_y = y_ch->theta;
  angles->theta_z = z_ch->theta;
  angles->time_stamp = read_clock_time();

  return;

}
