// Basic demo for accelerometer readings from Adafruit MPU6050

// ESP32 Guide: https://RandomNerdTutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/
// ESP8266 Guide: https://RandomNerdTutorials.com/esp8266-nodemcu-mpu-6050-accelerometer-gyroscope-arduino/
// Arduino Guide: https://RandomNerdTutorials.com/arduino-mpu-6050-accelerometer-gyroscope/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <optics.h>

#define DRIFT_MODIFIER -0.005

Adafruit_MPU6050 mpu;

float time_delays[100];

float robotAngle = 0;
float elapsed_time = 0;

void gyroInit(void) {

if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

}

void gyroRead() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //Serial.print("--"); Serial.println(g.gyro.z);

  if (abs(g.gyro.z * 0.1 *(180/PI)) > 0.1){
    robotAngle += g.gyro.z *(180/PI) * (elapsed_time/1000.0);
  }
  // if (ROBOT_STATE == 2){
  //   robotAngle += DRIFT_MODIFIER;
  // }
  
  
  
}