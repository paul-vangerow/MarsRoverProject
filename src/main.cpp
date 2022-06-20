#include <Arduino.h>
#include <motor.h>
#include <optics.h>
#include <instructions.hpp>
#include <communication.hpp>
#include <gyro.hpp>

#define Sender_Txd_pin 2
#define Sender_Rxd_pin 32

#define FPGA_UART_Rx_PIN 1
#define FPGA_UART_Tx_PIN 3

TaskHandle_t drive_core;
Instruction_queue instrq;

HardwareSerial Sender(1);
HardwareSerial FPGA(2);

int session_id;
float prev_elapsed = 1000;
int rotations = 0;

// init variables used to send data to the server
Server_info s_info;

void createInstruction(){

  // Figure out what the next instruction needed is (Based on sensor data, e.t.c)

}

void drive_core_code( void * parameter){
  motorInit();
  
  delay(1000);
  //move(100);

  for(;;){

    move(10);
    rot(90);
    delay(10000);

    if (!instrq.isEmpty()){
      Serial.println("Fetching Instruction");
      Mouvement instr = instrq.get_instruction();

      if (instr.get_instruction() == forward){
        move(instr.get_value());
      } else if (instr.get_instruction() == rotate ) {
        rot(instr.get_value());
      } else if (instr.get_instruction() == end) {
        // Prevent Further Operation (When it reaches Homebase)
        vTaskDelete(drive_core);
      }

    } else {
      // If no instructions are left, create new instructions for the queue.
      createInstruction();
    }
    delay(100);
  }
}

void setup(){

  Serial.begin(115200);
  Sender.begin(115200, SERIAL_8N1, Sender_Txd_pin, Sender_Rxd_pin);
  Sender.begin(115200, SERIAL_8N1, FPGA_UART_Tx_PIN, FPGA_UART_Rx_PIN);

  cam_init();
  gyroInit();

  delay(1000);
  
  xTaskCreate(drive_core_code, "drive", 1000, &drive_core, tskIDLE_PRIORITY, NULL);
  
}

void loop() {
  float start = millis();

  // -- Data Reading -- 
  read_values(); // Optical flow data <-- Location_Scaled (X, Y), Camera dx, dy, Total Change in dx, dy
  gyroRead(); // Gyro angle data <-- Robot_Angle

  // Read Camera Data
  // Read Radar Data

  Serial.print(location_scaled[0]); Serial.print(" "); Serial.println(location_scaled[1]);

  Sender.print(String(location_scaled[0])+"\t"); 
  Sender.print(String(location_scaled[1])+"\t"); 
  Sender.print(String(robotAngle)+"\t"); 
  Sender.println("2\t");

  /* Variables available: 
    - Location_Scaled(X, Y)
    - Camera (dx, dy)
    - Total change (x, y)
    - Robot Angle (Gyro) - degrees
    - kill_motion <-- stop any current instruction
    - collision <-- 1 when robot got stuck on a wall and retreated

  */

  // -- Send Sensor Data to Server --

  // Sender.println("5.3\t8.9");

  // -- Recieve Server Instructions -- 

  //instrq.update(); <--- For recieving Instructions from the server

  // -- Next Cycle --   

  delay(100); // Main loop Delay
  elapsed_time = millis() - start; // Gyro Callibration
}