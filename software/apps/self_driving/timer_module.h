 
#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"

#define NUM_LOOPS 100

#define CLOCK_SPEED 16000000.0
#define PRESCALER_VALUE 512

void init_timer();

void start_timer();
void end_timer();

uint32_t get_timer_value();
float convert_to_secs(uint32_t time);

void print_timer_vals();

float delta_time(float t1, float t2);

#endif 