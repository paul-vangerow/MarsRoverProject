#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>
#include <gyro.hpp>


TaskHandle_t drive_core;
Instruction_queue instrq;
int session_id;

HardwareSerial Sender(1);
#define Sender_Txd_pin 2
#define Sender_Rxd_pin 32


float prev_elapsed = 1000;
int rotations = 0;

// init variables used to send data to the server
Server_info s_info;

void drive_core_code( void * parameter){
  motorInit();
  // delay(2000);
  // rot(90);
  // delay(1000);
  //rot(360);
  
  
  for(;;){
    // delay(200);
    // rot(10);
    // rotations++;
    // delay(200);
    // rot(-10);
    // rotations++;
    //Serial.println(instrq.isEmpty());
    // delay(100);
    // move(36);
    // delay(100);
    // rot(90);
    // delay(100);
    // move(44);
    // delay(100);
    // rot(90);
    // if (!instrq.isEmpty()){
    //   Serial.println("succ");
    //   Mouvement instr = instrq.get_instruction();

    //   if (instr.get_instruction() == forward){
    //     move(instr.get_value());
    //   } else if (instr.get_instruction() == spinCW ) {
    //     rot(instr.get_value());
    //   } else if (instr.get_instruction() == spinCCW ) {
    //     rot(instr.get_value());
    //   }

    // }
    //delay(1000);
  }
  
}

void setup(){

  Serial.begin(115200);
  Sender.begin(115200, SERIAL_8N1, Sender_Txd_pin, Sender_Rxd_pin);

  cam_init();
  gyroInit();

  delay(1000);
  
  xTaskCreate(drive_core_code, "drive", 1000, &drive_core, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  float start = millis();

  // -- Data Reading -- 
  read_values(); // Optical flow data <-- Location_Scaled (X, Y), Camera dx, dy, Total Change in dx, dy
  gyroRead(); // Gyro angle data <-- Robot_Angle

  // Read Camera Data
  // Read Radar Data

  /* Variables available: 
    - Location_Scaled(X, Y)
    - Camera (dx, dy)
    - Total change (x, y)
    - Robot Angle (Gyro) - degrees
    - kill_motion <-- stop any current instruction
    - collision <-- 1 when robot got stuck on a wall and retreated

  */

  // -- Send Sensor Data to Server --

  // float val = 5.3;
  Sender.println("5.3\t8.9");

    // PostRadarValue(5, 5, 10.3);

  // -- Recieve Server Instructions -- 



  // -- Control Algorithm --

  //instrq.update();

  // -- Next Cycle --   

  delay(100); // Main loop Delay
  elapsed_time = millis() - start; // Gyro Callibration
}