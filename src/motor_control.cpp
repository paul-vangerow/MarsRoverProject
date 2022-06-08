#include <Robojax_L298N_DC_motor.h>
#include <motor.h>
#include <optics.h>

// motor 1 settings
#define CHA 0
#define ENA 12 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 14
#define IN2 15

// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 4// this pin must be PWM enabled pin if Arduino board is used
#define CHB 1

const int CCW = 2; // do not change
const int CW  = 1; // do not change

#define motor1 1 // do not change
#define motor2 2 // do not change

#define ANGLE_COEFFICIENT 40
#define DISTANCE_COEFFICIENT 20

// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);
// for two motors with debug information
//Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, IN3, IN4, ENB, CHB, true);

void motorInit(){
  robot.begin();
}

void stp(){
  robot.brake(1);
  robot.brake(2);
}

void move(float distance){
  stp();
  robot.rotate(motor1, 39, CW);
  robot.rotate(motor2, 43, CCW);
  delay(distance * DISTANCE_COEFFICIENT);
  stp();
}

void rotCW(int angle){
  stp();
  direction -= angle;
  read_data= false;
  robot.rotate(motor1, 20, CW);
  robot.rotate(motor2, 20, CW);
  delay(angle * ANGLE_COEFFICIENT);
  read_data = true;
  stp();
  
}

void rotCCW(int angle){
  stp();
  direction += angle;
  read_data = false;
  robot.rotate(motor1, 20, CCW);
  robot.rotate(motor2, 20, CCW);
  delay(angle * ANGLE_COEFFICIENT);
  read_data = true;
  stp();  
}


