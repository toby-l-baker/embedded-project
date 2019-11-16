#ifndef ACC_READING
#define ACC_READING

void init_accelerometers();

//Contains accelerometer readings in DEGREES
typedef struct angles_t {
	float theta_x;
	float theta_y;
	float theta_z;
} angles_t;

void update_angles_struct(angles_t* angles);


#endif