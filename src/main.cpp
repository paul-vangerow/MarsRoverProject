#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>
#include <gyro.hpp>


TaskHandle_t drive_core;
Instruction_queue instrq;
int session_id;

void drive_core_code( void * parameter){
  motorInit();
  //move(25);
  
  for(;;){
    Serial.println(instrq.isEmpty());
    if (!instrq.isEmpty()){
      Serial.println("succ");
      Mouvement instr = instrq.get_instruction();

      if (instr.get_instruction() == forward){
        move(instr.get_value());
      } else if (instr.get_instruction() == spinCW ) {
        rot(instr.get_value());
      } else if (instr.get_instruction() == spinCCW ) {
        rot(instr.get_value());
      }

    }
    delay(1000);
  }
}

void angle_core_code( void * parameter){
  gyroInit();
  for(;;){
    gyroRead();
    Serial.println(robotAngle);
  }
}

void setup(){

  Serial.begin(115200);

  cam_init();
  gyroInit();

  InitWifi();
  delay(1000);
  // session_id = InitDB();
  session_id = 2;
  
<<<<<<< HEAD
  // xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);
=======
  xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);
  //xTaskCreate(angle_core_code, "gyro", 1000, &instrq, tskIDLE_PRIORITY, NULL);
>>>>>>> bd579a496eacc5930e02275dd512f948d0f25a8e
  
}

void loop() {
<<<<<<< HEAD
  // read_values();
=======

  gyroRead();
  Serial.print(robotAngle); Serial.println("---");

  //gyroRead();
  //read_values();
>>>>>>> bd579a496eacc5930e02275dd512f948d0f25a8e
  //instrq.update();


  PostSensorReadings(5, 5, 10.3);
  delay(60000);
  // for(int i=0; i<4; i++){
  //   delay(10000);
  //   Orientation orient = Orientation(i);
  //   Colour c = pink;
  //   PostAlienLocation(10, 10, 2, orient, c);
  // }
}