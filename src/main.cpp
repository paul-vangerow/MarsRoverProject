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

  for(;;){

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
  FPGA.begin(9600, SERIAL_8N1, FPGA_UART_Tx_PIN, FPGA_UART_Rx_PIN);
  pinMode(RADAR_READ_PIN, INPUT);

  Sender.setTimeout(10);
  FPGA.setTimeout(10);

  cam_init();
  gyroInit();

  delay(1000);
  
  xTaskCreate(drive_core_code, "drive", 1000, &drive_core, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  float start = millis();

<<<<<<< HEAD
  read_values();
  gyroRead();
  float val = 23.5;
  Sender.println("5\t6\t" + String(val)+"\t");
=======
  // -- Data Reading -- 
  read_values(); // Optical flow data <-- Location_Scaled (X, Y), Camera dx, dy, Total Change in dx, dy
  gyroRead(); // Gyro angle data <-- Robot_Angle

  String c = FPGA.readStringUntil('\n'); // Read Camera Data
  Serial.println(c);

  // Read Radar Data

  radar_spotted = digitalRead(RADAR_READ_PIN);

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
                          "\t"+String(alien_distances[6])+"\t");

  // -- Recieve Server Instructions -- 
>>>>>>> 205176aad307b961a2ee4d2a416fb2062df99663

  String str = Sender.readStringUntil('\n');
  if (!str.isEmpty()){
    int index = std::string(str.c_str()).find_first_of('\t');
    int instruction = str.substring(0, index).toInt();
    float value = str.substring(index+1).toFloat();
    Mouvement n(instruction, value);
    instrq.add_instruction(n);
  }

  // -- Next Cycle --   

  delay(100); // Main loop Delay
  elapsed_time = millis() - start; // Gyro Callibration
  Serial.println(elapsed_time);
}