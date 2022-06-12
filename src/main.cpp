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
<<<<<<< HEAD
=======
  //delay(4000);
  move(25);
>>>>>>> e43cd112ec7adaf25a9a683f7c94947ebd640f00
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

void setup(){

  Serial.begin(115200);

  InitWifi();
  
<<<<<<< HEAD
  // xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);

  cam_init();
}

void loop() {
  //read_values();
  instrq.update();

  delay(10000);
=======
  xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  read_values();
  //instrq.update();
  //delay(1000);
>>>>>>> e43cd112ec7adaf25a9a683f7c94947ebd640f00
}