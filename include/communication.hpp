#ifndef communication_hpp
#define communication_hpp

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <instructions.hpp>

#include <string.h>
#include <vector>
#include <stdlib.h>

const String host = "http://13.41.77.188:80/";


void InitWifi();

// function to check whether or not the WiFi connection has been interrupted
// will try to reconnect to the WiFi if the connection failed
void CheckWifi();

void get();

// fetch the set of instructions from the server that haven't been executed yet
String FetchInstruction();

// initialise the database for the session
int InitDB();

// post position of the rover
void PostSensorReadings(val_t x, val_t y, double value);

//post the position of an alien
void PostAlienLocation(val_t x, val_t y, double distance, Orientation orientation, Colour c);

#endif