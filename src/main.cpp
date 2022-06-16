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
<<<<<<< HEAD
Server_info s_info;
=======
Orientation rover_orientation;
Colour *colour_of_object = new Colour;
int distance_to_object;
>>>>>>> 9c0906250fc705bba990c25ffa491eb19b0a9e1f

void drive_core_code( void * parameter){
  motorInit();
  // delay(2000);
  // rot(90);
  // delay(1000);
  // rot(-90);
  delay(2000);
  move(50);
  /*
  for(;;){
    //Serial.println(instrq.isEmpty());
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
  */
}

void send_to_server(void * parameters){
  InitWifi();
  // session_id = InitDB();
  session_id = 2;
  double radar_val = 10;

  for(;;){
    PostRadarValues(s_info.x, s_info.y, radar_val);
    delay(1000);
  }
}

void setup(){

  Serial.begin(115200);

  cam_init();
  gyroInit();

  delay(1000);
  
  xTaskCreate(drive_core_code, "drive", 1000, &instrq, tskIDLE_PRIORITY, NULL);
  xTaskCreate(send_to_server, "server", 1000, &s_info, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  float start = millis();

  read_values();
  gyroRead();

  Serial.println(robotAngle); 
  //instrq.update();
  delay(100);

  // PostRadarValue(5, 5, 10.3);
  elapsed_time = millis() - start;
  // delay(60000);
}