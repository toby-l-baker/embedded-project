#include "pathplanning.h"

float* poly_x(float x_f, float y_f, float theta_f, float K, uint8_t N) {
	float* points_x = malloc(N*sizeof(float));
	float N_float = (float) N;
	float alpha_x = K * cos(theta_f) - 3.0*x_f;
	float beta_x = K;

	for(int i = 0; i < N; i++) {
		float s = ((float) (i + 1)) / ((float) N);
		points_x[i] = s*s*s*x_f + alpha_x*s*s*(s-1.0)+beta_x*s*(s-1.0)*(s-1.0); 
	}

	return points_x;
}

float* poly_y(float x_f, float y_f, float theta_f, float K, uint8_t N) {
	float* points_y = malloc(N*sizeof(float));
	float alpha_y = K*sin(theta_f) -3.0 * y_f;
	float beta_y = 0.0;

	for(int i = 0; i < N; i++) {
		float s = ((float) (i + 1)) / ((float) N);
		points_y[i] = s*s*s*y_f + alpha_y*s*s*(s-1.0)+beta_y*s*(s-1.0)*(s-1.0); 
	}


	return points_y;

}


point_t* plan_path(float x_f, float y_f, float theta_f, uint8_t N) {
	float* points_x = poly_x(x_f, y_f, theta_f, K_path, N);
	float* points_y = poly_y(x_f, y_f, theta_f, K_path, N);

	/*
	for(int i = 0; i < N; i++) {
		printf("(%f,%f)\n", points_x[i], points_y[i]);
	} 
	*/


	point_t* points = malloc(N*sizeof(points));
	for(int i = 0; i < N; i++) {
		points[i].x = points_x[i];
		points[i].y = points_y[i];
	}
	//free(points_x);
	//free(points_y);

	return points;
}