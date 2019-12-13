#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"

#define NUM_LOOPS 100

void init_timer();

void start_timer();
void end_timer();

uint32_t get_timer_value();

void print_timer_vals();

#endif 