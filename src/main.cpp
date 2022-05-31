#include <Arduino.h>
#include <motor.h>
#include <optics.h>

#define LED 2

void setup(){
  Serial.begin(115200);
  motorInit();
  cam_init();
}

void loop() {
  //rotCW(360);
  //delay(1000);
  read_values();
}