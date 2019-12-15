#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "board_params.h"

#define NUM_LOOPS 100
#define PRESCALE_CONV 62.5

void init_timer_module();

void start_timer();
void end_timer();

uint32_t get_timer_value();

void print_timer_vals();

float delta_time(float t1, float t2);

#endif
