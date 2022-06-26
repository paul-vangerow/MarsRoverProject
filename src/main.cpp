#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>
#include <gyro.hpp>
#include <sonic.hpp>
#include <list>
#include <random>

#define Sender_Txd_pin 2
#define Sender_Rxd_pin 32

#define FPGA_UART_Rx_PIN 17
#define FPGA_UART_Tx_PIN 16 

#define RADAR_READ_PIN 26

TaskHandle_t drive_core;
Instruction_queue instrq;

HardwareSerial Sender(1);
HardwareSerial FPGA(2);

int session_id;
float prev_elapsed = 1000;
int rotations = 0;
bool automated = false;

uint8_t buffer[36];
float alien_distances[7];

struct point{
  float x;
  float y;
};

std::list<point> points;

int point_val = 0;
int radar_spotted = 0;
bool proximity = false;

// init variables used to send data to the server
Server_info s_info;

void create_points(){
  point val;
  val.x=0; val.y=0;
  points.push_back( val );
  val.x=100; val.y=0;
  points.push_back( val );
  val.x=100; val.y=100;
  points.push_back( val );
  val.x=0; val.y=100;
  points.push_back( val );

}

float getTan(float v1, float v2){

  if (v2 == 0){
    if (v1 < 0){
      return -90;
    } else {
      return 90;
    }
  }

  float angleValue = atan(v1/v2) * (180/PI);

  if (v2 < 0 && v1 > 0){
    return -180 - angleValue;
  } else if (v2 < 0 && v1 < 0){
    return 180 - angleValue;
  } else {
    return angleValue;
  }

}

void createInstruction(float x, float y){

  if (kill_motion == 0){
    float dx = x - location_scaled[0];
    float dy = y - location_scaled[1];
    float new_angle = getTan(dy, dx) - correct_angle;

    Serial.print(dx);Serial.print(" ");Serial.print(dy); Serial.print(" ");Serial.print(getTan(dy, dx)); Serial.print(" ");Serial.println(new_angle);

    instrq.add_instruction( Mouvement(1, new_angle) );

    float new_distance( sqrt( pow(dx, 2) + pow(dy, 2) ) );

    instrq.add_instruction( Mouvement(0, new_distance) );
  } else {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,90);
    float angle = -1*distribution(generator); 
    
    kill_motion = 0;
    instrq.add_instruction( Mouvement(1, angle) );
    instrq.add_instruction( Mouvement(0, 300) );

  }

  

}

void drive_core_code( void * parameter){
  motorInit();

  delay(100);

  automated = 0;

  if (automated){
    instrq.add_instruction( Mouvement(0, 300) );
  }

  for(;;){

    if (!instrq.isEmpty()){
      Serial.println("Fetching Instruction");
      Mouvement instr = instrq.get_instruction();

      Serial.print(instr.get_instruction());Serial.print(" "); Serial.println(instr.get_value());

      if (instr.get_instruction() == forward){
        move(instr.get_value());
      } else if (instr.get_instruction() == rotate ) {
        rot(instr.get_value());
      } else if (instr.get_instruction() == explore ) {
        automated = true;
      } else if (instr.get_instruction() == end_explore ) {
        automated = false;
      }else if (instr.get_instruction() == end) {
        // Prevent Further Operation (When it reaches Homebase)
        vTaskDelete(drive_core);
      }

    } else {
      // If no instructions are left, create new instructions for the queue.
      if (automated){

        point val = points.front();
        points.pop_front();

        if (abs(val.x-location_scaled[0]) > 2 || abs(val.y-location_scaled[1]) > 2){
          createInstruction(val.x, val.y);
        }
        
        if (point_val == points.size()){
          create_points();
          point_val = 0;
        } else {
          point_val++;
        }
      }  
    }
    delay(100);
  }
}

void setup(){

  Serial.begin(115200);

  create_points();

  Sender.begin(115200, SERIAL_8N1, Sender_Txd_pin, Sender_Rxd_pin);
  FPGA.begin(115200, SERIAL_8N1, FPGA_UART_Tx_PIN, FPGA_UART_Rx_PIN);
  //pinMode(RADAR_READ_PIN, INPUT);

  Sender.setTimeout(10);
  FPGA.setTimeout(10);

  Serial.println("Initialising Camera...");
  cam_init();
  
  Serial.println("Initialising Gyro...");
  gyroInit();

  xTaskCreate(drive_core_code, "drive", 1000, &drive_core, tskIDLE_PRIORITY, NULL);

}

void loop() {
  float start = millis();

  // -- Data Reading -- 
  read_values(); // Optical flow data <-- Location_Scaled (X, Y), Camera dx, dy, Total Change in dx, dy
  gyroRead(); // Gyro angle data <-- Robot_Angle
  proximity = ping();
  
  if (FPGA.available() >= 36){
    
    int optics_reading = FPGA.readBytes(buffer, 36); // Read Camera Data
    for (int i = 1; i < 9; i++){
      alien_distances[i-1]=((buffer[4*i+1]<<8) + (buffer[4*i]));
    }
  }
  while (FPGA.available()) {
    FPGA.read();
  }

  for (int i = 0 ; i < 7; i++){
    if ((alien_distances[i] < 10 && alien_distances[i] > 5) || proximity){
      if (kill_motion == 0){
        kill_motion = 1;
        while (!instrq.isEmpty()){
          instrq.get_instruction();
        }
      }
    }
  }

  // DATA FORMAT d1 d2 d3 d4 d5 d6 d7

  // Read Radar Data

  //radar_spotted = digitalRead(RADAR_READ_PIN);
  //Serial.println(radar_spotted);

  // -- Send Sensor Data to Server --

  Sender.println(String(location_scaled[0])+
                          "\t"+String(location_scaled[1])+
                          "\t"+String(robotAngle)+
                          "\t"+String(radar_spotted)+ 
                          "\t"+String(alien_distances[0])+
                          "\t"+String(alien_distances[1])+
                          "\t"+String(alien_distances[2])+
                          "\t"+String(alien_distances[3])+
                          "\t"+String(alien_distances[4])+
                          "\t"+String(alien_distances[5])+
                          "\t"+String(alien_distances[6])+
                          "\t"+String(alien_distances[7])+
                          "\t");

  // -- Recieve Server Instructions -- 

  String str = Sender.readStringUntil('\n');
  str = "";
  str = Sender.readStringUntil('\n');
  if (str.length() > 1){
    int index = std::string(str.c_str()).find_first_of('\t');
    int instruction = str.substring(0, index).toInt();
    float value = str.substring(index+1).toFloat();
    Mouvement n(instruction, value);
    instrq.add_instruction(n);
  }

  // -- Next Cycle --   

  delay(50); // Main loop Delay
  elapsed_time = millis() - start; // Gyro Callibration

}