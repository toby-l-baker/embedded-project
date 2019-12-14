// Robot Template app
//
// Framework for creating applications that control the Kobuki robot

#include "mpu9250_interface.h"

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  init_mpu9250();
  init_mpu9250_timer(1.953125); // period for 512 HZ 
  angles_t * angles = malloc(sizeof(angles_t));
  int i = 0;
  while (1) {
    // Note: removing this delay will make responses quicker, but will result
    //  in printf's in this loop breaking JTAG
    nrf_delay_ms(1);
    update_angles(angles);
    if (i++ % 50 == 0)
      printf("Roll: %.4f, Pitch: %.4f, Yaw: %.4f\n", angles->theta_x, angles->theta_y, angles->theta_z);
  }
}
