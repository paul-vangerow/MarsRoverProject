#include <Arduino.h>
#include <communication.hpp>
#include <instructions.hpp>
#include <SPI.h>
#include <string>
#include <stdio.h>
#include <cstdlib>

// init variables used to send data to the server
int session_id;

HardwareSerial Reciever(1);

#define Reciever_Txd_pin 27
#define Reciever_Rxd_pin 14

float GetTop(String &str){
  int index = std::string(str.c_str()).find_first_of('\t');
  float val = str.substring(0,index).toFloat();
  str = str.substring(index+1);
  return val;
}

void setup() {
  Serial.begin(115200);

  //-------
  Reciever.begin(115200, SERIAL_8N1, Reciever_Txd_pin, Reciever_Rxd_pin);
  Reciever.setTimeout(10);
  //-------

  delay(1000);

  InitWifi();
  session_id = InitDB();
  // session_id = 2;

}


void loop() {
  String str, str_other;
  Orientation orientation;
  float x, y, radar_value;
  int robot_angle;
  float red_distance, teal_distance, blue_distance, pink_distance, yellow_distance, green_distance;

  bool transmit=false;

  String instruction = FetchInstruction();
  Serial.println(instruction);
  Reciever.println(instruction);



  str = Reciever.readStringUntil('\n');
  

  /*
  x y orientation radar_value color distance
  */


  if(!(str=="")){
    x = GetTop(str);
    y = GetTop(str);
    robot_angle = GetTop(str);
    radar_value = int(GetTop(str))%360;
    red_distance = GetTop(str);
    yellow_distance = GetTop(str);
    teal_distance = GetTop(str);
    pink_distance = GetTop(str);
    blue_distance = GetTop(str);
    green_distance = GetTop(str);

    if(robot_angle<=2){
      orientation = up;
      transmit = true;
    }
    if(robot_angle<=92 && robot_angle >= 88){
      orientation = left;
      transmit = true;
    }
    if(robot_angle<=182 && robot_angle >= 178){
      orientation = down;
      transmit = true;
    }
    if(robot_angle<=272 && robot_angle >= 268){
      orientation = right;
      transmit = true;
    }

    if(transmit){
      if(!red_distance==0){
        PostAlienLocation(x,y, red_distance, orientation, red);
      }
      if(!blue_distance==0){
        PostAlienLocation(x,y, blue_distance, orientation, blue);
      }
      if(!yellow_distance==0){
        PostAlienLocation(x,y, yellow_distance, orientation, yellow);
      }
      if(!teal_distance==0){
        PostAlienLocation(x,y, teal_distance, orientation, teal);
      }
      if(!pink_distance==0){
        PostAlienLocation(x,y, pink_distance, orientation, pink);
      }
      if(!green_distance==0){
        PostAlienLocation(x,y, green_distance, orientation, green);
      }
      
      
      PostRadarValues(x,y,radar_value);
    }
  }

  transmit = false;


}