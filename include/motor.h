#ifndef motor
#define motor

#include <Robojax_L298N_DC_motor.h>

// Motor Functions

void motorInit();

void stp();

void move(float distance);

void rotCW(int angle);

void rotCCW(int angle);


#endif