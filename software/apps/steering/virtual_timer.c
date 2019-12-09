// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"

// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void) {
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[0] = 0;

  // Place your interrupt handler code here

  node_t* first = list_remove_first();

  if (first != NULL) {
  	first->cb();
  	if (first->repeated == true) {
  		first->timer_value = first->timer_value + first->microseconds;
  		list_insert_sorted(first);
  	} else {
  		free(first);
  	}
  }

  uint32_t cur = read_timer();
  node_t* new_time = list_get_first();
  while ((new_time != NULL) && (new_time->timer_value <= cur)) {
  	new_time = list_remove_first();
  	new_time->cb();
  	if (new_time->repeated == true) {
  		new_time->timer_value = new_time->timer_value + new_time->microseconds;
  		list_insert_sorted(new_time);
  	} else {
  		free(new_time);
  	}
  	new_time = list_get_first();
  }

  if (new_time != NULL) {
  	NRF_TIMER4->CC[0] =  new_time->timer_value;
  }

  //printf("Timer Fired!\n");

}

// Read the current value of the timer counter
uint32_t read_timer(void) {
	NRF_TIMER4->TASKS_CAPTURE[1] |= 1;
	uint32_t timer_value = NRF_TIMER4->CC[1];

  // Should return the value of the internal counter for TIMER4
  return timer_value;
}

// Initialize TIMER4 as a free running timer
// 1) Set to be a 32 bit timer
// 2) Set to count at 1MHz
// 3) Enable the timer peripheral interrupt (look carefully at the INTENSET register!)
// 4) Clear the timer
// 5) Start the timer
void virtual_timer_init(void) {
  // Place your timer initialization code here
	NRF_TIMER4->BITMODE |= 3;
	NRF_TIMER4->PRESCALER |= 4;
	NRF_TIMER4->TASKS_CLEAR |= 1; //Set TASKS_CLEAR
	NRF_TIMER4->TASKS_START |= 1; //Set TASKS_START

	NRF_TIMER4->INTENSET |= 1<<16; //Enable interrupts for CC[0]
	NVIC_EnableIRQ(TIMER4_IRQn);
}

// Start a timer. This function is called for both one-shot and repeated timers
// To start a timer:
// 1) Create a linked list node (This requires `malloc()`. Don't forget to free later)
// 2) Setup the linked list node with the correct information
//      - You will need to modify the `node_t` struct in "virtual_timer_linked_list.h"!
// 3) Place the node in the linked list
// 4) Setup the compare register so that the timer fires at the right time
// 5) Return a timer ID
//
// Your implementation will also have to take special precautions to make sure that
//  - You do not miss any timers
//  - You do not cause consistency issues in the linked list (hint: you may need the `__disable_irq()` and `__enable_irq()` functions).
//
// Follow the lab manual and start with simple cases first, building complexity and
// testing it over time.
static uint32_t timer_start(uint32_t microseconds, virtual_timer_callback_t cb, bool repeated) {
	//define a new node and store the
	node_t* timer = malloc(sizeof(node_t));
	timer->cb = cb;
	timer->repeated = repeated;
	timer->microseconds = microseconds;
	//get start time and store value of timer
	uint32_t start_time = read_timer();
	timer->id = &timer;
	timer->timer_value = microseconds + start_time;
	__disable_irq();
	list_insert_sorted(timer);
	NRF_TIMER4->CC[0] =  list_get_first()->timer_value;
	__enable_irq();
  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
  return timer->id;
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, false);
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start_repeated(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, true);
}

// Remove a timer by ID.
// Make sure you don't cause linked list consistency issues!
// Do not forget to free removed timers.
void virtual_timer_cancel(uint32_t timer_id) {
	__disable_irq();
	node_t* first = list_get_first();
	while(first != NULL){
		if (first->id == timer_id) {
			break;
		}
		first = first->next;
	}
	if (first == NULL) {
		printf("Timer ID does not exist\n");
	} else {
		printf("Timer with ID: %d cancelled\n", timer_id);
		list_remove(first);
		free(first);
	}
	node_t* new_time = list_get_first();
	if (new_time != NULL) {
	  NRF_TIMER4->CC[0] =  new_time->timer_value;
	} else {
		NRF_TIMER4->CC[0] = 0;
	}
	__enable_irq();

}
