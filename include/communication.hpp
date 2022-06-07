#ifndef communication_hpp
#define communication_hpp

#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>

#include <string.h>
#include <vector>

const String host = "http://13.41.77.188:80/";


void InitWifi();

// function to check whether or not the WiFi connection has been interrupted
// will try to reconnect to the WiFi if the connection failed
void CheckWifi();

void get();

// fetch the set of instructions from the server that haven't been executed yet
String FetchInstruction();

void PostSensorReadings();

#endif