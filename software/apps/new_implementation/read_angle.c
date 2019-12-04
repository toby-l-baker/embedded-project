#include "read_angle.h"

#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979



/*
struct adc_channel{
	uint8_t channel;
	float bias;
	float sensitivity;
	float voltage;
	float theta;
	float g;
};*/



/*
struct adc_channel * create_channel(float sensitivity, float bias, uint8_t channel){
	struct adc_channel * ch = malloc(sizeof(struct adc_channel));
	if (ch){
		ch->channel = channel;
		ch->bias = bias;
		ch->sensitivity = sensitivity;
	}
	return channel;
}
*/
nrf_saadc_value_t sample_adc_value (uint8_t channel) {
	nrf_saadc_value_t val;
	ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
	APP_ERROR_CHECK(error_code);
	return val;
}


float read_angle_y(){
	float voltage_x = sample_adc_value(X_CHANNEL);
	voltage_x *= LSB_SIZE;
	float voltage_y = sample_adc_value(Y_CHANNEL);
	voltage_y *= LSB_SIZE;
	float voltage_z = sample_adc_value(Z_CHANNEL);
	voltage_z *= LSB_SIZE;

	float x_g = (voltage_x - X_BIAS) / X_SENSITIVITY;
	float y_g = (voltage_y - Y_BIAS) / Y_SENSITIVITY;
	float z_g = (voltage_z - Z_BIAS) / Z_SENSITIVITY;

	printf("\tUnfilterd: %f\n", atan(y_g/  sqrt(x_g * x_g + z_g * z_g)    ) / PI * 180);
	return 0.0;

}


// void init_accelerometers(){
// 	ret_code_t error_code = NRF_SUCCESS;

// 	// initialize analog to digital converter
// 	saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
// 	error_code = nrfx_saadc_init(&saadc_config, saadc_callback);
// 	APP_ERROR_CHECK(error_code);

// 	// initialize analog inputs
// 	// configure with 0 as input pin for now
// 	channel_config.gain = NRF_SAADC_GAIN1_6; // input gain of 1/6 Volts/Volt, multiply incoming signal by (1/6)
// 	channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6 Volt reference, input after gain can be 0 to 0.6 Volts

// 	channel_config.pin_p = BUCKLER_ANALOG_ACCEL_X;
// 	error_code = nrfx_saadc_channel_init(X_CHANNEL, &channel_config);
// 	APP_ERROR_CHECK(error_code);


// 	channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Y;
// 	error_code = nrfx_saadc_channel_init(Y_CHANNEL, &channel_config);
// 	APP_ERROR_CHECK(error_code);

// 	channel_config.pin_p = BUCKLER_ANALOG_ACCEL_Z;
// 	error_code = nrfx_saadc_channel_init(Z_CHANNEL, &channel_config);
// 	APP_ERROR_CHECK(error_code);


// 	printf("Accelerometer initialized.\n");





// }