// BLE Service Template
//
// Creates a service for changing LED state over BLE

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_util.h"
#include "nrf_twi_mngr.h"
#include "nrf_gpio.h"
#include "nrfx_saadc.h"
#include "nrf_delay.h"
#include "display.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "math.h"

#include "simple_ble.h"
#include "buckler.h"
#include "states.h"

#include "max44009.h"

// ADC channels for joystick
#define X_CHANNEL 0
#define X_CHANNEL_ADC NRF_SAADC_INPUT_AIN0
#define Y_CHANNEL 1
#define Y_CHANNEL_ADC NRF_SAADC_INPUT_AIN1
#define LSB_SIZE 3.6 / 4096
#define PI 3.14159265358979
#define MAX_TURN 45
#define MAX_DRIVE 100
#define MAX_ADC_OUT 4095

// Create a timer
APP_TIMER_DEF(bike_timer);

// I2C manager
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

//global variable for the bike state
bike_states bike_state;

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
        // c0:98:e5:49:xx:xx
        .platform_id       = 0x49,    // used as 4th octect in device BLE address
        .device_id         = 0x000b, // TODO: replace with your lab bench number
        .adv_name          = "EE149 LED", // used in advertisements if there is room
        .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

// 32e61089-2b22-4db5-a914-43ce41986c70
static simple_ble_service_t bike_srv = {{
    .uuid128 = {0xca,0xa4,0x87,0x0d,0x42,0x84,0xff,0xA9,
                0x59,0x4D,0x5e,0xf6,0xa0,0xed,0x07,0x46}
}};

static simple_ble_char_t manual_char = {.uuid16 = 0xeda1};
bool manual_control=0;

static simple_ble_char_t power_char = {.uuid16 = 0xeda2};
bool power=0;

static simple_ble_char_t drive_char = {.uuid16 = 0xeda3};
uint8_t drive_speed=0;

static simple_ble_char_t turn_char = {.uuid16 = 0xeda4};
uint8_t turn_angle=0;

simple_ble_app_t* simple_ble_app;

bool send_data = false;

void bike_timer_callback () {
  send_data = true;
}

// sample a particular analog channel in blocking mode
nrf_saadc_value_t sample_value (uint8_t channel) {
  nrf_saadc_value_t val;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &val);
  APP_ERROR_CHECK(error_code);
  return val;
}

// callback for SAADC events
void saadc_callback (nrfx_saadc_evt_t const * p_event) {
  // don't care about adc callbacks
}

void update_input(uint8_t pin, bool * old_val, bool * char_value, simple_ble_char_t * char_) {
  bool state = nrf_gpio_pin_read(pin);

  if (*old_val != state) {
    *char_value = state;
    simple_ble_notify_char(char_);
  }
  *old_val = state;
}

void update_angle(nrf_saadc_value_t x, nrf_saadc_value_t y, uint8_t * angle) {
  float joy_angle = 0;
  if (y != 0 ) {
    joy_angle = (180/PI) * atan(x/y); // angle in degrees will be 0 to 360
  }
  * (angle) = (uint8_t) ((MAX_TURN/360) * joy_angle); //map angle to be -45 to + 45
}

void update_speed(nrf_saadc_value_t x, nrf_saadc_value_t y, uint8_t * speed) {
  float joy_dir = (x*x + y*y) / MAX_ADC_OUT; // get normalized magnitude of joy vector
  * (speed) = (uint8_t) (100 * joy_dir); // convert magnitue to duty cycle
}

int main(void) {

  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

/*****************************************************************************/
/********************************** SETUP ADC ********************************/
/*****************************************************************************/

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
  channel_config.pin_p = X_CHANNEL_ADC;
  error_code = nrfx_saadc_channel_init(X_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  // specify input pin and initialize that ADC channel
  channel_config.pin_p = Y_CHANNEL_ADC;
  error_code = nrfx_saadc_channel_init(Y_CHANNEL, &channel_config);
  APP_ERROR_CHECK(error_code);

  /*****************************************************************************/
  /******************************** SETUP DIPSLAY ******************************/
  /*****************************************************************************/

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
  //display_write("Hello, Human!", DISPLAY_LINE_0);
  printf("Display initialized!\n");

  // initialize i2c master (two wire interface)
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);

  // Setup LED & GPIO
  nrf_gpio_cfg_input(BUCKLER_BUTTON0, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_input(BUCKLER_SWITCH0, NRF_GPIO_PIN_PULLDOWN);
  nrf_gpio_cfg_output(BUCKLER_LED2);
  nrf_gpio_cfg_output(BUCKLER_LED1);
  nrf_gpio_cfg_output(BUCKLER_LED0);

  /*****************************************************************************/
  /********************************** SETUP BLE ********************************/
  /*****************************************************************************/

  // Setup BLE
  simple_ble_app = simple_ble_init(&ble_config);

  simple_ble_add_service(&bike_srv);

  simple_ble_add_characteristic(1, 1, 1, 0,
      sizeof(manual_control), (uint8_t*)&manual_control,
      &bike_srv, &manual_char);

  simple_ble_add_characteristic(1, 1, 1, 0,
      sizeof(power), (uint8_t*)&power,
      &bike_srv, &power_char);

  simple_ble_add_characteristic(1, 1, 1, 0,
      sizeof(drive_speed), (uint8_t*)&drive_speed,
      &bike_srv, &drive_char);

  simple_ble_add_characteristic(1, 1, 1, 0,
      sizeof(turn_angle), (uint8_t*)&turn_angle,
      &bike_srv, &turn_char);

  // Start Advertising
  simple_ble_adv_only_name();

  // Set a timer to read the light sensor and notify readers
  app_timer_init();
  app_timer_create(&bike_timer, APP_TIMER_MODE_REPEATED, (app_timer_timeout_handler_t) bike_timer_callback);
  app_timer_start(bike_timer, APP_TIMER_TICKS(100), NULL); // 1000 milliseconds

  bool old_switch_state = false;
  bool old_button_state = false;

  while(1) {

    update_input(BUCKLER_SWITCH0, &old_switch_state, &manual_control, &manual_char);
    //update_input(BUCKLER_BUTTON0, &old_button_state, &power, &power_char);

    switch(bike_state) {
      case MANUAL: {
        // periodically send the data over BLE
        if (send_data) {
          // saadc read from x and y axis of joystick
          nrf_saadc_value_t x = sample_value(X_CHANNEL);
          nrf_saadc_value_t y = sample_value(Y_CHANNEL);
          // update drive speed and turning angle
          update_speed(x, y, &drive_speed);
          update_angle(x, y, &turn_angle);
          // notify the bike of its new instructions
          simple_ble_notify_char(&drive_char);
          simple_ble_notify_char(&turn_char);
          send_data = false;
        }
        break;
      }
      case AUTONOMOUS: {
        // wait for joystick to move out of neutral position then start integrating
        // w.r.t. time to get a vector dircetion and length. Once complete send the
        // message over BLE and wait til the joystick moves again before sending
        // another message
        break;
      }
    }

    //power_manage();
    nrf_delay_ms(10);
  }
}
