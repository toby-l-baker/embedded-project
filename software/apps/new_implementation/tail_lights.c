#include "tail_lights.h"

nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);

#define RIGHT 1
#define LEFT -1


#define RIGHT_BLINKER_STRING	"              >>"
#define LEFT_BLINKER_STRING		"<<              "
#define BRAKE_STRING			"|||          |||"
#define BOTH_BLINKER_STRING		"<<            >>"
#define EMPTY_STRING			"                "

#define ACCEL_POSITIVE 1
#define ACCEL_NEGATIVE -1
#define ACCEL_ZERO		0


#define BLINKER_SAMPLE_FREQ 50



bool Blink_State = false;


void init_tail_lights(){
	
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
	ret_code_t error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
	APP_ERROR_CHECK(error_code);
	display_init(&spi_instance);
	printf("Tail Light Display initialized.\n");

	return;
}

void update_blinkers(int8_t side, float time_stamp){
	bool change_blinker = false;
	static int8_t prev_side = 0;


	
	static float Blink_Update_Time = 0.0;
	// printf("time_stamp: %f, Blink_Update_Time: %f\n", time_stamp, Blink_Update_Time);
	
	if (delta_time(time_stamp, Blink_Update_Time) >= BLINKER_PERIOD){
	
		change_blinker = true;
		Blink_Update_Time = time_stamp;
		if (side != prev_side){
			Blink_State = true;
		}
		else{
			Blink_State = !Blink_State;
		}

		if (Blink_State == false){
			display_write(EMPTY_STRING, DISPLAY_LINE_1);
		}
		else{
			if (side==RIGHT)
				display_write(RIGHT_BLINKER_STRING, DISPLAY_LINE_1);
			else if (side == LEFT)
				display_write(LEFT_BLINKER_STRING, DISPLAY_LINE_1);
		}
	}




	prev_side = side;
}
void turn_check(angles_t * angles){
	static float prev_angle = 0.0;
	static float prev_timestamp = 0.0;
	// printf("New: %f, Old: %f\n", new_timestamp, prev_timestamp);
	float time_delta = delta_time(angles->time_stamp, prev_timestamp);
	// printf("\tDelta: %f\n", time_delta);
	if (angles->theta_z/time_delta >= prev_angle/time_delta + MINIMUM_BLINKER_ANGLE_PER_USECOND){
		// printf("Left New Angle: %f, Prev Angle: %f, Time Delta: %f\n", new_angle, prev_angle, time_delta);
		update_blinkers(LEFT, angles->time_stamp);
	}
	else if (angles->theta_z/time_delta <= prev_angle/time_delta - MINIMUM_BLINKER_ANGLE_PER_USECOND){
		// printf("Left New Angle: %f, Prev Angle: %f, Time Delta: %f\n", new_angle, prev_angle, time_delta);
		update_blinkers(RIGHT, angles->time_stamp);
	}
	else{
		display_write(EMPTY_STRING, DISPLAY_LINE_1);
	}
	prev_angle = angles->theta_z;
	prev_timestamp = angles->time_stamp;
}

void update_lights(angles_t * angles){
	static int8_t angle_ct = 0;
	if (angle_ct++ >= BLINKER_SAMPLE_FREQ){
		angle_ct = 0;
		turn_check(angles);
		// printf("Timestamp: %f, Accel %f\n", angles->time_stamp, angles->raw_accel_x);
		
	}
	
	brake_check(angles);
	
	
	

}


float average_accel_list(slist* list){
	snode* node;
	float average = 0;
	for (node = list->head; node != NULL; node = node->next){
		average += node->accel;
	}
	return average / (float)(slist_get_count(list));
}
// BRAKE_ACCELERATION_TIME_THRESH
void brake_check(angles_t * angles){
	
	
	static bool list_empty = true;
	static slist*  accel_list;
	static float first_data_time = 0.0;

	if (list_empty){
		accel_list = slist_create();
		list_empty = false;
		first_data_time = angles->time_stamp;
		slist_add_head(accel_list, first_data_time, angles->raw_accel_x);
	}

	float time_delta = delta_time(angles->time_stamp, first_data_time);
	slist_add_tail(accel_list, angles->time_stamp, angles->raw_accel_x);
	// printf("Time delta: %f, Accel: %f\n", time_delta,angles->raw_accel_x);
	while (time_delta > BRAKE_ACCELERATION_USECOND_THRESH && accel_list->count > 0){
		
		first_data_time = slist_remove_head(accel_list);
		time_delta = delta_time(angles->time_stamp, first_data_time);

	}
	float accel_avg = average_accel_list(accel_list);
	// printf("Average: %f, Count: %i\n\n", accel_avg, accel_list->count);
	if (accel_avg <= -BRAKE_ACCEL_THRESHOLD_VAL_G){
		display_write(BRAKE_STRING, DISPLAY_LINE_0);
	}
	else{
		display_write(EMPTY_STRING, DISPLAY_LINE_0);
	}

}

