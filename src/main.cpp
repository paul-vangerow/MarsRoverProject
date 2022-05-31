#include <Arduino.h>
#include <motor.h>
#include <optics.h>

#define LED 2

TaskHandle_t drive_core;

void drive_core_code( void * parameter){
  motorInit();
  for(;;){
    rotCW(360);
    delay(1000);
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