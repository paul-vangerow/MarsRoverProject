#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>
#include <gyro.hpp>


TaskHandle_t drive_core;
Instruction_queue instrq;
int session_id;

// init variables used to send data to the server
Orientation rover_orientation;
Colour *colour_of_object = new Colour;
int distance_to_object;


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
  
  xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);
  //xTaskCreate(angle_core_code, "gyro", 1000, &instrq, tskIDLE_PRIORITY, NULL);
  
}

void loop() {


  gyroRead();
  Serial.print(robotAngle); Serial.println("---");

  //gyroRead();
  //read_values();
  //instrq.update();


  // PostRadarValue(5, 5, 10.3);
  // delay(60000);

}