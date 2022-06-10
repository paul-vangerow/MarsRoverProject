#ifndef optics
#define optics

// Pass Values onto other components
extern float robot_angle;
extern float d_optics[2];
extern float total_optics[2];
extern float straight_factor;

extern int ROBOT_STATE;

void cam_init();

void getAVG();

void read_values();



#endif