#ifndef READ_ANGLE_H
#define READ_ANGLE_H

#include "setup.h"
#include "board_params.h"

extern nrf_saadc_channel_config_t channel_config;
extern nrfx_saadc_config_t saadc_config ;

void init_accelerometers();

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

//Contains accelerometer readings in DEGREES
struct angles_t {
	float x;
	float y;
	float z;
	float time_stamp;
};

void update_angles_struct(struct angles_t* angles);

#endif
