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

      Mouvement instr = instrq.get_instruction();

      if (instr.get_instruction() == forward){
        move(instr.get_value());
      } else if (instr.get_instruction() == spinCW ) {
        rotCW(instr.get_value());
      } else if (instr.get_instruction() == spinCCW ) {
        rotCCW(instr.get_value());
      }

    }
  }
}

void setup(){

  Serial.begin(115200);

  InitWifi();
  
  xTaskCreatePinnedToCore(drive_core_code, "drive", 1000, NULL, 0, &drive_core, 0);

  cam_init();
}

void loop() {
  //read_values();
  instrq.update();
  Serial.println(instrq.isEmpty());
  delay(1000);
}