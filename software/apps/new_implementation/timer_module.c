#include "timer_module.h"


uint32_t recorded_vals[NUM_LOOPS] = {0};
int current_idx = 0;

uint32_t start_val = 0;
uint32_t end_val = 0;


// nrfx_timer_t TIMER = NRFX_TIMER_INSTANCE(4);

void start_timer(){
	start_val = get_timer_value();
}

void end_timer(){
	end_val = get_timer_value();
	recorded_vals[current_idx++] = end_val - start_val;
}

uint32_t get_timer_value() {
	NRF_TIMER4->TASKS_CAPTURE[1] |= 1;
	uint32_t timer_value = NRF_TIMER4->CC[1];
	return timer_value;
}

void print_timer_vals(){
	for (int i =0; i < NUM_LOOPS; i++){
		printf("%u, ", recorded_vals[i]);
	}


}

void init_timer(uint32_t num_times) {
	Num_Loops = num_times;
	NRF_TIMER4->BITMODE |= 3;
	NRF_TIMER4->PRESCALER |= 4; // Read at 16 Mhz / 16 = 1MHz
	NRF_TIMER4->TASKS_CLEAR |= 1; //Set TASKS_CLEAR
	NRF_TIMER4->TASKS_START |= 1; //Set TASKS_START
	NRF_TIMER4->INTENSET |= 1<<16; //Enable interrupts for CC[0]
	NVIC_EnableIRQ(TIMER4_IRQn);
}
