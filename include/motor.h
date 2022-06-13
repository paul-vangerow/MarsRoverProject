#ifndef motor
#define motor

#include <Robojax_L298N_DC_motor.h>

// Motor Functions

//extern bool kill_motion;

void motorInit();

void stp();

void move(float distance);

void rot(int angle);


#endif