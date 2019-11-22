/*
 * states.h
 *
 *  Created on: Sep 22, 2018
 *      Author: shromonaghosh
 */

#ifndef STATES_H_
#define STATES_H_

#include <stdio.h>

typedef enum {
    MANUAL=0,
    AUTONOMOUS,
} bike_states;

typedef enum {
    GET_PATH=0,
    DRIVE,
} autonomous_states;

#endif /* STATES_H_ */
