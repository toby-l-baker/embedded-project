#ifndef PID_HEADER
#define PID_HEADER

//Proportional control
extern float Kp;

//Derivative constant
extern float Kd;

//Integral constant
extern float Ki;

//Antiwindup constant, not used yet
extern float Kw;

//Takes the angle theta the bike makes with the vertical axis, return the motor
//duty cycle required to correct the error using PID
//Returns a number between -100 and 100 to account for direction of rotation
float duty_cycle_PID(float theta, float time_stamp);

//Takes and angle theta and outputs a simple proportionnal control duty cycle
//between -100 and 100
float duty_cycle_proportionnal(float theta);

//Resets the integral used in the PID. Takes the current clock time in clock cycles
void reset_integral(int time_stamp);

//Resets the derivative used in the PID. Takes current time & current angle
void reset_derivative(float theta, int time_stamp);


#endif
