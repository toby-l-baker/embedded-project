
#include "setup.h"
#include "motor.h"
#include "board_params.h"
#include "read_angle_filt.h"
#include "read_angle.h"

int main(){


	initialize_buckler();
	struct dc_motor * flywheel = create_dc_motor(FLYWHEEL_PIN_ENABLE, FLYWHEEL_PIN_IN1, FLYWHEEL_PIN_IN2, FlYWHEEL_MOTOR_CHANNEL);
    struct dc_motor * drive = create_dc_motor(DRIVE_PIN_ENABLE, DRIVE_PIN_IN1, DRIVE_PIN_IN2, DRIVE_MOTOR_CHANNEL);
    initialize_dc_motor_pwm(flywheel, drive);

    printf("Setting up sensors...\n");
    struct angles_t* angles = malloc(sizeof(struct angles_t));
    init_accelerometers();
    
    float min = 180.0;
    float max = -180.0;
    int i = 0;

    float angle_y = 0;

    while(i++ < 10000){
        
        update_angles_struct(angles);
        if (angles->y > max)
            max = angles->y;
        if (angles->y < min){
            min = angles->y;
        }
        printf("X: %f, Y: %f, Z: %f", angles->x, angles->y, angles->z);
        angle_y = read_angle_y();
    	nrf_delay_ms(1);
    }
	printf("Min: %f, Max: %f", min, max);
	return 0;
}

