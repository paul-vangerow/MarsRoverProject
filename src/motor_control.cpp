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
#define TURN_CONTROL 26

// P Control Values (Turning)
#define P_T 0.1

// P Control Values (Driving)
#define P_D 0.2

#define ROT_ERROR_TOL 1

// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);
// for two motors with debug information
//Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, IN3, IN4, ENB, CHB, true);

int sign(int val){
  if (val > 0){
    return 2;
  } else {
    return 1;
  }
  
}

void motorInit(){
  robot.begin();
}

void stp(){
  robot.brake(1);
  robot.brake(2);
}

void move(float distance){
  stp();
  int speed_d = 0;
  while (1){
    speed_d = straight_factor * P_D;
    
    if (speed_d > 40){
      speed_d = 40;
    } else if (speed_d < -40){
      speed_d = -40;
    }

    Serial.println(straight_factor);

    robot.rotate(motor1, 25 + speed_d, CW);
    robot.rotate(motor2, 25 - speed_d, CCW); 
    delay(100); 
  }
  
  stp();
}

void rot(int angle){

  int target = robot_angle + angle;
  int error = 2;

  stp();
  while (abs(error) > ROT_ERROR_TOL){
    error = target - robot_angle;

    robot.rotate(motor1, 20, sign(error));
    robot.rotate(motor2, 20, sign(error));

    delay(10);
    stp();
  }
  stp();
}


