#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>

/*

------- CURRENT PIN SETUP -------

OPTICS >>

PIN_SS        5 <-- CONFLICT --> IO21
PIN_MISO      19 <-- CONFLICT --> IO22
PIN_MOSI      23 
PIN_SCK       18 <-- CONFLICT --> IO14
PIN_MOUSECAM_RESET     35
PIN_MOUSECAM_CS        5 <-- CONFLICT --> IO21

MOTOR >>

Motor (1)
CHA 0
ENA 19 // this pin must be PWM enabled pin if Arduino board is used
IN1 18 
IN2 5

Motor (2)
IN3 17
IN4 16
ENB 4 // this pin must be PWM enabled pin if Arduino board is used
CHB 1

*/

TaskHandle_t drive_core;
Instruction_queue instrq;

void drive_core_code( void * parameter){
  motorInit();
  delay(4000);
  move(500);
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
    //delay(100);
  }
}

void setup(){

  Serial.begin(115200);

  //InitWifi();
  cam_init();
  
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
>>>>>>> fe7195e3830d4282fd8eb83a351df311ad0a485f
}