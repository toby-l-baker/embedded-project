# Instructions for fixing timer conflict: 
`ERROR 8 [NRF_ERROR_INVALID_STATE]`
### Edit line 25 of embedded-project/software/buckler/software/libraries/mpu9250/mpu9250.c to a new timer:
`static const nrf_drv_timer_t gyro_timer = NRFX_TIMER_INSTANCE(<X>);`
### Add the following definitions for the timer X in embedded-project/software/buckler/software/boards/buckler_revB/app_config.h:
`#define NRFX_TIMER<X>_ENABLED 1`<br />
`#define TIMER<X>_ENABLED 1`
