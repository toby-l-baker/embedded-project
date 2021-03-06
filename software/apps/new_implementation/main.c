#include "setup.h"
#include "motor.h"
#include "board_params.h"
#include "timer_module.h"
#include "mpu.h"
#include "tail_lights.h"
#include "simple_ble.h"

#define BALANCING



int main(){
	initialize_buckler();

	struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
    struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
    initialize_dc_motor_pwm(flywheel, drive);

    struct servo_motor * front = create_servo(SERVO_PIN, SERVO_MOTOR_CHANNEL);
    initialize_servo_motor_pwm(front);

    init_timer_module();

    init_ble_receiver();
    init_mpu9250();
    init_mpu9250_timer(IMU_TIMER_REFRESH_RATE); 
  

    init_tail_lights();
    



   



  

    int loop_count = 0;


    angles_t * angles = malloc(sizeof(angles_t));
    float duty_cycle = 0.0;

    int8_t direction = STOP;

    while(true){

     

        update_angles(angles);


        
        // duty_cycle_PWM_PID(angles->theta_x, angles->)
        // duty_cycle_torque_proportional
        // duty_cycle_torque_PID(angles->theta_x, angles->time_stamp, &duty_cycle, &direction);
     
        if (loop_count++ %10 == 0){
            
            printf("State: %i\n", bike_state);
            printf("Z angle: %f\n", angles->raw_accel_x);
            print_angles(angles, duty_cycle);
        }
        update_lights(angles);

       


        nrf_delay_ms(MS_DELAY);
    }
   
	return 0;
}

