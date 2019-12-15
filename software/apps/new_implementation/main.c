
#include "setup.h"
#include "motor.h"
#include "board_params.h"
#include "timer_module.h"
#include "mpu.h"
#include "tail_lights.h"



int main(){


	initialize_buckler();

	struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
    struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
    initialize_dc_motor_pwm(flywheel, drive);

    struct servo_motor * front = create_servo(SERVO_PIN, SERVO_MOTOR_CHANNEL);
    initialize_servo_motor_pwm(front);

    init_timer_module();

    init_mpu9250();
    init_mpu9250_timer(IMU_TIMER_REFRESH_RATE); 
  

    init_tail_lights();






    angles_t * angles = malloc(sizeof(angles_t));
    float duty_cycle = 0;
    int8_t direction = STOP;

    while(true){

     

        update_angles(angles);
        print_angles(angles, duty_cycle);
        
        update_lights(angles);

       
        nrf_delay_ms(MS_DELAY);
      
    }
   
	return 0;
}

