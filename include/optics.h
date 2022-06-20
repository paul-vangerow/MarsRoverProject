#ifndef optics
#define optics

// Pass Values onto other components
//extern float robot_angle;
extern float d_optics[2]; // Change in X and Y
extern float total_optics[2]; // Total Change in X and Y
//extern float straight_factor;
extern float location_scaled[2];

extern int ROBOT_STATE;

void cam_init();

void getAVG();

void read_values();



#endif