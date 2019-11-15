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

/* Struct for storing information about each ADC Channel */
struct motor
{
  bool forward;
  bool backward;
  uint8_t duty_cycle; // between 0 and 100
  uint8_t enable_pin; //most recent voltage reading
};

struct motor * channel_create(uint8_t pin)
{
    struct motor * motor = malloc(sizeof(struct motor));
    if (motor) {
        motor->duty_cycle = 0;
        motor->enable_pin = pin;
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

/* Converts gravity to an angle */
void update_angles(struct adc_channel * x_ch, struct adc_channel * y_ch, struct adc_channel * z_ch) {
  x_ch->g = (x_ch->filtered_voltage - x_ch->bias) / x_ch->sensitivity;
  y_ch->g = (y_ch->filtered_voltage - y_ch->bias) / y_ch->sensitivity;
  z_ch->g = (z_ch->filtered_voltage - z_ch->bias) / z_ch->sensitivity;

  x_ch->theta = atan(x_ch->g / sqrt(y_ch->g*y_ch->g + z_ch->g*z_ch->g))/PI*180;
  y_ch->theta = atan(y_ch->g / sqrt(x_ch->g*x_ch->g + z_ch->g*z_ch->g))/PI*180;
  z_ch->theta = atan(z_ch->g / sqrt(x_ch->g*x_ch->g + y_ch->g*y_ch->g))/PI*180;
}

// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
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

  error_code = nrfx_saadc_channel_init(Z_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // initialization complete
  printf("Buckler initialized!\n");

  // Don't forget to initialize your timer library
  virtual_timer_init();
  nrf_delay_ms(3000);

  struct adc_channel * x_ch = channel_create(0.3908, 1.4132, X_CHANNEL);

  struct adc_channel * y_ch = channel_create(0.3869, 1.4009, Y_CHANNEL);

  struct adc_channel * z_ch = channel_create(0.3799, 1.4041, Z_CHANNEL);

  // fill up the circular buffers of adc readings
  adc_buf_init(x_ch, x_list);
  adc_buf_init(y_ch, y_list);
  adc_buf_init(z_ch, z_list);


  virtual_timer_start_repeated(125, adc_callback);

  // loop forever
  while(1) {
    if (complete) { //For collecting data for filter design
      regular_sampling(); //NUM_SAMPLES defined as a global
      complete = false;
    } else { //For calibration
      // read_adc_voltage(x_ch);
      // read_adc_voltage(y_ch);
      // read_adc_voltage(z_ch);
      read_and_filter_voltage(x_ch, x_list);
      read_and_filter_voltage(y_ch, y_list);
      read_and_filter_voltage(z_ch, z_list);

      update_angles(x_ch, y_ch, z_ch);

      printf("Roll: %.4f, Pitch:%.4f, Yaw: %.4f\n", (x_ch->theta), (y_ch->theta), (z_ch->theta));
      //printf("X:%.4f\n", (x_ch->filtered_voltage)); //(x_ch->filtered_voltage), (y_ch->filtered_voltage),
      nrf_delay_ms(50);
    }
  }
}
