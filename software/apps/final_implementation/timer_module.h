#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include "setup.h"

void init_timer_module();

void start_timer();
void end_timer();

uint32_t get_timer_value();

void print_timer_vals();

float delta_time(float t1, float t2);

#endif
