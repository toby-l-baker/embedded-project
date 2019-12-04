#include "read_angle_filt.h"
#include "slist.h"

#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979
#define G 	9.80665


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


const float TAPS[LEN_BUFFER] = {-1.1390375091067432E-18,0.00035259598715081734,0.001477766783703178,0.00355893500529667,
0.006843359038215591,0.01156690107151631,0.01787102176489292,0.025729190352121574,
0.03489853523601125,0.044908166421298065,0.05508897695608773,0.06464209335547222,
0.07273590538888708,0.07861610666014734,0.08171044597919938,0.08171044597919938,
0.07861610666014733,0.07273590538888705,0.06464209335547222,0.05508897695608773,
0.04490816642129805,0.03489853523601124,0.025729190352121564,0.017871021764892907,
0.011566901071516297,0.006843359038215583,0.003558935005296659,0.001477766783703178,
0.00035259598715081734,-1.1390375091067432E-18};

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
void update_angles() {
  /* Filtering */
  // x_ch->g = (x_ch->filtered_voltage - x_ch->bias) / x_ch->sensitivity;
  y_ch->g = (y_ch->filtered_voltage - y_ch->bias) / y_ch->sensitivity;
  // z_ch->g = (z_ch->filtered_voltage - z_ch->bias) / z_ch->sensitivity;

/*  x_ch->theta = atan(x_ch->g / sqrt(y_ch->g*y_ch->g + z_ch->g*z_ch->g))/PI*180;
  y_ch->theta = atan(y_ch->g / sqrt(x_ch->g*x_ch->g + z_ch->g*z_ch->g))/PI*180;
  z_ch->theta = atan(z_ch->g / sqrt(x_ch->g*x_ch->g + y_ch->g*y_ch->g))/PI*180;*/
  y_ch->theta = asin(y_ch->g / G) * 180.0 / PI;
}

// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
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


  // x_ch = channel_create(X_SENSITIVITY, X_BIAS, X_CHANNEL);
  // x_list = slist_create();

  y_ch = channel_create(Y_SENSITIVITY, Y_BIAS, Y_CHANNEL);
  y_list = slist_create();

  // z_ch = channel_create(Z_SENSITIVITY, Z_BIAS, Z_CHANNEL);
  // z_list = slist_create();
  // fill up the circular buffers of adc readings
  // adc_buf_init(x_ch, x_list);
  adc_buf_init(y_ch, y_list);
  // adc_buf_init(z_ch, z_list);

  // init_clock_time();
}

void update_angles_struct(struct angles_t* angles) {
  // USing filetering
  //read_and_filter_voltage(x_ch, x_list);
  read_and_filter_voltage(y_ch, y_list);
  //read_and_filter_voltage(z_ch, z_list);

  update_angles();
  
  // angles->x = x_ch->theta;
  angles->y = y_ch->theta;
  // angles->z = z_ch->theta;

}