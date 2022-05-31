#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>

#define LED 2

TaskHandle_t drive_core;
Instruction_queue instrq;

void drive_core_code( void * parameter){
  motorInit();
  for(;;){
    if (!instrq.isEmpty()){

      Mouvement instr = instrq.get_instruction();

      if (instr.get_instruction() == )

    }
  }
}

void setup(){

  Serial.begin(115200);
  
  xTaskCreatePinnedToCore(drive_core_code, "drive", 1000, NULL, 0, &drive_core, 0);

  cam_init();
}

void loop() {
  read_values();
  Serial.println(xPortGetCoreID());
}