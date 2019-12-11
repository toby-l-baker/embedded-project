#ifndef PATHPLANNING_H
#define PATHPLANNING_H

#include "math.h"
#include "buckler.h"

#define K_path 1.0


typedef struct point_t {
	float x;
	float y;
	float theta;
} point_t;

float* poly_x(float x_f, float y_f, float theta_f, float K, uint8_t N);

float* poly_y(float x_f, float y_f, float theta_f, float K, uint8_t N);

point_t* plan_path(float x_f, float y_f, float theta_f, uint8_t N);


#endif 