#include <Arduino.h>
#include <motor.h>

#define LED 2

void setup(){
  Serial.begin(115200);
  motorInit();
}

void loop() {
  rotCW(360);
  delay(1000);
}