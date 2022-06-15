#ifndef gyro
#define gyro

// Motor Functions

//extern bool kill_motion;

extern float_t robotAngle;
extern float elapsed_time;

void gyroInit();

void gyroRead();

#endif