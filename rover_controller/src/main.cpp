#include <Arduino.h>

#define LED 2
void setup(){
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
}

void loop() {
  digitalWrite(LED,HIGH);
  Serial.println("LED ison");
  delay(1000);
  digitalWrite(LED,LOW);
  Serial.println("LED isoff");
  delay(1000);
}