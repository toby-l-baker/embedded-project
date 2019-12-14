#include "timer_module.h"

#define MICROSECONDS 1000000.0

static uint32_t recorded_vals[NUM_LOOPS] = {0};
static int current_idx = 0;

static uint32_t start_val = 0;
static uint32_t end_val = 0;


// nrfx_timer_t TIMER = NRFX_TIMER_INSTANCE(4);
float delta_time(float t1, float t2){
	// returns time difference in seconds
	return (t1 - t2);
}

void start_timer(){
	start_val = get_timer_value();
	// printf("%u\n", start_val);
}

void end_timer(){
	end_val = get_timer_value();
	// printf("%u\n", end_val);
	recorded_vals[current_idx++] = end_val - start_val;
	
	start_val = end_val = 0;
}

float timestamp(){
	// printf("In timestamp: %f\n", get_timer_value());

	return (float)(convert_to_secs(get_timer_value())) ;
}

uint32_t get_timer_value() {
	NRF_TIMER2->TASKS_CAPTURE[1] = 1;
	uint32_t timer_value = NRF_TIMER2->CC[1];
	// NRF_TIMER4->TASKS_CAPTURE[1] ;
	return timer_value;
}

float convert_to_secs(uint32_t time){
	return (float)(time /CLOCK_SPEED*PRESCALER_VALUE);

}

void print_timer_vals(){
	for (int i =0; i < NUM_LOOPS; i++){
		printf("%f, ", (float)(recorded_vals[i]/CLOCK_SPEED*PRESCALER_VALUE));
	}


}

void init_timer() {

	NRF_TIMER2->MODE          = 0; //Timer not counter
	NRF_TIMER2->BITMODE       = 3; //32 bits
	NRF_TIMER2->PRESCALER     = 9; //31 250 Hz
	NRF_TIMER2->TASKS_CLEAR   = 1; //Reset value to 0
	NRF_TIMER2->TASKS_START   = 1;
// NVIC_EnableIRQ(TIMER4_IRQn);
}
