// Basic demo for accelerometer readings from Adafruit MPU6050

// ESP32 Guide: https://RandomNerdTutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/
// ESP8266 Guide: https://RandomNerdTutorials.com/esp8266-nodemcu-mpu-6050-accelerometer-gyroscope-arduino/
// Arduino Guide: https://RandomNerdTutorials.com/arduino-mpu-6050-accelerometer-gyroscope/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <optics.h>

float DRIFT_MODIFIER = 0;

Adafruit_MPU6050 mpu;
sensors_event_t acc, g, temp;

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

  Serial.println("Callibrating Gyroscope...");

  for (int i = 0; i < 1000; i++){
    mpu.getEvent(&acc, &g, &temp);
    DRIFT_MODIFIER += g.gyro.z * (180/PI);
  }
  DRIFT_MODIFIER = DRIFT_MODIFIER / 1000;
  
  Serial.println("Done Callibrating, drift value = " + String(DRIFT_MODIFIER));

}

void gyroRead() {
  /* Get new sensor events with the readings */
  
  mpu.getEvent(&acc, &g, &temp);

  robotAngle += (g.gyro.z *(180/PI) - DRIFT_MODIFIER) * (elapsed_time/1000.0);
}