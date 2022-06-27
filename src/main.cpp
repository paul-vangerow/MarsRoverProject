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

int ping_val = 0;

int ensure_const_echo = 0;
int echo_count = 0;

int ensure_const_alien[6];
int alien_count[6];

struct point{
  float x;
  float y;
};

struct intPoint{
  int x;
  int y;
};

std::list<point> points;

point recent_point;

int point_val = 0;
int radar_spotted = 0;
bool proximity = false;

bool pathable[280][280];

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
  val.x=0; val.y=0;
  points.push_back( val );
}

float angleConver(float angle){

  // Convert angle to be in range of -180 - 180

  int temp = angle/360;

  angle = angle - (360 * std::floor(temp));

  if (angle < -180){
    return 360 + angle;
  } else if (angle > 180) {
    return 360 - angle;
  } else {
    return angle;
  }
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

  // Allow tan values to be in correct quadrant

  if (v2 < 0 && v1 > 0){
    return 180 + angleValue;
  } else if (v2 < 0 && v1 < 0){
    return -180 + angleValue;
  } else {
    return angleValue;
  }

}

void createInstruction(float x, float y){

  float dx = x - location_scaled[0];
  float dy = y - location_scaled[1];
  float new_angle = getTan(dy, dx) - correct_angle;

  Serial.print("x: ");Serial.println(location_scaled[0]);
  Serial.print("y: ");Serial.println(location_scaled[1]);
  Serial.print("dx: ");Serial.println(dx);
  Serial.print("dy: ");Serial.println(dy); 
  Serial.print("current angle: ");Serial.println(robotAngle);
  Serial.print("angle: ");Serial.println(getTan(dy, dx)); 
  Serial.print("amount to turn: ");Serial.println(new_angle);

  instrq.add_instruction( Mouvement(1, angleConver(new_angle)) );

  float new_distance( sqrt( pow(dx, 2) + pow(dy, 2) ) );
  instrq.add_instruction( Mouvement(0, new_distance) );

}

void drive_core_code( void * parameter){
  motorInit();

  delay(100);

  automated = 0;

  for(;;){

    if (!instrq.isEmpty() && kill_motion == 0){
      kill_motion = 0;
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

        if (points.size() != 0 ){
          if (kill_motion == 0){
            recent_point = points.front();
            points.pop_front();

            if (abs(recent_point.x-location_scaled[0]) > 2 || abs(recent_point.y-location_scaled[1]) > 2){
              createInstruction(recent_point.x, recent_point.y);
            }
          } else {

            // Motion Was killed. Create new points 


          }
          
        } else {
          // create_points(); // <-- For looping
        }
        
      }  
    }
    delay(100);
  }
}

void disable_points(int distance){

  float ang = robotAngle * (PI / 180);

  int x = cos(ang)*distance + location_scaled[0];
  int y = sin(ang)*distance + location_scaled[1];

  Serial.print(x); Serial.print(" "); Serial.println(y);

  if (pathable[x][y]==0){
    for (int x_val = -10; x_val <10; x_val++){
        if (x+x_val > 0 && x+x_val < 280){
          for (int y_val = -10; y_val <10; y_val++){
            if (y+y_val > 0 && y+y_val < 280){
              pathable[x+x_val][y+y_val] == 1;
            }
          }
        }
    }
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

  xTaskCreate(drive_core_code, "drive", 3000, &drive_core, tskIDLE_PRIORITY, NULL);

}

void loop() {
  float start = millis();

  // -- Data Reading -- 

  read_values(); // Optical flow data <-- Location_Scaled (X, Y), Camera dx, dy, Total Change in dx, dy
  gyroRead(); // Gyro angle data <-- Robot_Angle
  ping_val = ping();
  
  if (FPGA.available() >= 36){
    
    int optics_reading = FPGA.readBytes(buffer, 36); // Read Camera Data
    for (int i = 1; i < 9; i++){
      alien_distances[i-1]=((buffer[4*i+1]<<8) + (buffer[4*i]));
    }
  }
  while (FPGA.available()) {
    FPGA.read();
  }

  if (ping_val < ensure_const_echo-1 || ping_val > ensure_const_echo+1){
    ensure_const_echo = ping_val;
    echo_count = 0;
  } else if (echo_count>=3){
    //Serial.print("Wall at: "); Serial.println(ensure_const_echo);
    disable_points(ensure_const_echo+10);
  } else {
    echo_count++;
  }

  for (int a = 0; a < 6; a++){
    if (alien_distances[a] > 5 && alien_distances[a] < 50){
      if (alien_distances[a] < ensure_const_alien[a]-1 || alien_distances[a] > ensure_const_alien[a]+1){
        ensure_const_alien[a] = alien_distances[a];
        alien_count[a] = 0;
      } else if (alien_count[a]>=2){
        //Serial.print("Alien! "); Serial.println(ensure_const_alien[a]);
        disable_points(ensure_const_alien[a]+10);
      }else {
        alien_count[a]++;
      }
    }
  }
  Serial.println();

  // -- Send Sensor Data to Server --

  // Serial.print(robotAngle); Serial.print(" "); Serial.print(correct_angle); Serial.print(" "); Serial.print(location_scaled[0]); Serial.print(" "); Serial.println(location_scaled[1]); 

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
  Serial.println(elapsed_time);
}