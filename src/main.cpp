#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>

#define LED 2

TaskHandle_t drive_core;
Instruction_queue instrq;

void drive_core_code( void * parameter){
  motorInit();
  for(;;){
    
    if (!instrq.isEmpty()){
      Serial.println("succ");
      Mouvement instr = instrq.get_instruction();

      if (instr.get_instruction() == forward){
        move(instr.get_value());
      } else if (instr.get_instruction() == spinCW ) {
        rotCW(instr.get_value());
      } else if (instr.get_instruction() == spinCCW ) {
        rotCCW(instr.get_value());
      }

    }
    //move(100);
    delay(1000);
  }
}

void setup(){

  Serial.begin(115200);

  //InitWifi();
  cam_init();
  
  xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  read_values();
  //instrq.update();
  //delay(1000);
}