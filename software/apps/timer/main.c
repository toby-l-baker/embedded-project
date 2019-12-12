// Virtual timers
//
// Uses a single hardware timer to create an unlimited supply of virtual
//  software timers

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
#include "nrf_serial.h"

#include "buckler.h"
#include "virtual_timer.h"

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Board initialized!\n");


  // Don't forget to initialize your timer library
  virtual_timer_init();
  nrf_delay_ms(3000);

  // loop forever
  while (1) {
    uint32_t timer_value = read_timer();
    float time_s = timer_value * 2.5e-7;
    printf("%f\n", time_s);

    nrf_delay_ms(1);
  }
}
