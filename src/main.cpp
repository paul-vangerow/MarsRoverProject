#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>
#include <gyro.hpp>

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

float main_time = 0;

uint8_t buffer[36];
uint32_t readings[9];

int radar_spotted = 0;
float alien_distances[7];

// init variables used to send data to the server
Server_info s_info;

void createInstruction(){

  // Figure out what the next instruction needed is (Based on sensor data, e.t.c)

  /* Variables available: 
    - Location_Scaled(X, Y)
    - Camera (dx, dy)
    - Total change (x, y)
    - Robot Angle (Gyro) - degrees
    - kill_motion <-- stop any current instruction
    - collision <-- 1 when robot got stuck on a wall and retreated

  */

}

void drive_core_code( void * parameter){
  motorInit();

  delay(100);

  for(;;){

    delay(1000);
    rot(90);
    delay(1000);
    rot(-90);

    //move(20);

    if (!instrq.isEmpty()){
      Serial.println("Fetching Instruction");
      Mouvement instr = instrq.get_instruction();

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
        createInstruction();
      }  
    }
    delay(100);
  }
}

void setup(){

  Serial.begin(115200);

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
  //Serial.print("Optical flow: "); Serial.print(millis() - start); Serial.print(" ");
  gyroRead(); // Gyro angle data <-- Robot_Angle
  //Serial.print("Gyro: "); Serial.print(millis() - start); Serial.print(" ");
  
  if (FPGA.available() >= 36){
    
    int optics_reading = FPGA.readBytes(buffer, 36); // Read Camera Data
    for (int i = 1; i < 9; i++){
      alien_distances[i-1]=((buffer[4*i+1]<<8) + (buffer[4*i]));
    }
  }
  while (FPGA.available()) {
    FPGA.read();
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
  String str = "";
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
  Serial.print(robotAngle);Serial.print(" "); Serial.print(correct_angle);Serial.print(" ");
  Serial.println(elapsed_time);

}