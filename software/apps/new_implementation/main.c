
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

    init_timer();
    init_mpu9250();
    init_mpu9250_timer(IMU_TIMER_REFRESH_RATE); 
  
    init_tail_lights();






    angles_t * angles = malloc(sizeof(angles_t));
    float duty_cycle = 0;
    int8_t direction = STOP;
    // init_timer();
    
    /*typedef struct angles_t {
  float theta_x;
  float theta_y;
  float theta_z;
  float timestamp;
} angles_t;

*/
    uint32_t i =0;
    // while(i++ < NUM_LOOPS){
    while(true){

        // waste_time();
        // start_timer();

        update_angles(angles);
        
        update_lights(angles);
        // duty_cycle_PWM_PID(angles->theta_x, angles->time_stamp, &duty_cycle, &direction);
        // printf("Timestamp %f\n", angles->time_stamp);
        // printf("Angle %f\n", angles->theta_x );
        // printf("Direction %d\n", direction);
        // printf("Duty cycle %f\n\n", duty_cycle);

        // printf("X: %f, Y: %f, Z: %f\n", angles->theta_x, angles->theta_y, angles->theta_z);
        // end_timer();
        // if (! ((i++) % 100) )
            // 
       
        // nrf_delay_ms(1);
       
        nrf_delay_ms(1);
        // if (i++ % 10 == 0){
        //     printf("Angle %f\n", angles->theta_x ); 
        //     printf("Timestamp %f\n\n", angles->time_stamp);
        // }
        // if (i % 250 == 0)
        //     blinker_toggle();
    }
    // print_timer_vals();

 //    printf("Setting up sensors...\n");
 //    struct angles_t* angles = malloc(sizeof(struct angles_t));
 //    init_accelerometers();
    
 //    float min = 180.0;
 //    float max = -180.0;
 //    int i = 0;

 //    float angle_y = 0;

 //    while(i++ < 10000){
        
 //        update_angles_struct(angles);
 //        if (angles->y > max)
 //            max = angles->y;
 //        if (angles->y < min){
 //            min = angles->y;
 //        }
 //        printf("X: %f, Y: %f, Z: %f", angles->x, angles->y, angles->z);
 //        angle_y = read_angle_y();
 //    	nrf_delay_ms(1);
 //    }
	// printf("Min: %f, Max: %f", min, max);
	return 0;
}

