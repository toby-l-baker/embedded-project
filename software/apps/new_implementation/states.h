#ifndef STATES_H
#define STATES_H

#include "setup.h"
#include "board_params.h"

typedef enum {
    OFF=0,
    MANUAL,
    AUTONOMOUS,
} bike_states;

extern bike_states bike_state;


#endif