// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"

// Read the current value of the timer counter
uint32_t read_timer(void) {
	NRF_TIMER4->TASKS_CAPTURE[1] |= 1;
	uint32_t timer_value = NRF_TIMER4->CC[1];

  // Should return the value of the internal counter for TIMER4
  return timer_value;
}


void virtual_timer_init(void) {
  // Place your timer initialization code here
	NRF_TIMER4->BITMODE |= 3;
	NRF_TIMER4->PRESCALER |= 4;
	NRF_TIMER4->TASKS_CLEAR |= 1; //Set TASKS_CLEAR
	NRF_TIMER4->TASKS_START |= 1; //Set TASKS_START

	NRF_TIMER4->INTENSET |= 1<<16; //Enable interrupts for CC[0]
	NVIC_EnableIRQ(TIMER4_IRQn);
}
