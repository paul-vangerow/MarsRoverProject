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
    delay(200);
    rot(10);
    rotations++;
    delay(200);
    rot(-10);
    rotations++;
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
  //Sender.begin(115200, SERIAL_8N1, 33, 32);

  cam_init();
  gyroInit();

  delay(1000);
  
  xTaskCreate(drive_core_code, "drive", 1000, &drive_core, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  float start = millis();

  read_values();
  gyroRead();

  float val = 5.1;
  //Sender.print(val);

  Serial.print(robotAngle); Serial.print(" , "); Serial.print(rotations); Serial.print(" , ");
  //instrq.update();
  delay(100);

  // PostRadarValue(5, 5, 10.3);
  elapsed_time = millis() - start;

  Serial.println(elapsed_time);
  // delay(60000);
}