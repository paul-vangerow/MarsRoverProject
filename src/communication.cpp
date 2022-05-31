#include "communication.hpp"
#include "instructions.hpp"
#include <iostream>

unsigned long previous_timer = 0;
unsigned long timeout_interval = 3000;


void InitWifi(){
    auto ssid = "Luc phone"; // access point name
    auto password="yoloswag"; // access point password
        // IPAddress static_addi(172, 20, 10, 4); // IP address for the shield
        // IPAddress gateway_addi(172,20,20,1);
        // IPAddress subnet(255,255,255,240);
        // WiFi.config(static_addi, gateway_addi, subnet);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    delay(10000);
    Serial.println("Connecting to WiFi...");
    delay(1000);
    while(WiFi.status() != WL_CONNECTED){
        Serial.println('.');
        delay(1000);
    }
}

void CheckWifi(){
    unsigned long current_timer = millis();
    if((WiFi.status()!=WL_CONNECTED)&&(current_timer-previous_timer >= timeout_interval)){
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
        previous_timer = current_timer;
    }
}

void get(){
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "get_instruction.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);
    int httpCode = http.GET();
    if(httpCode > 0){
        Serial.printf("HTTP GET code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK){
            String payload = http.getString();
            Serial.println(payload);
        }
    }
    else{
        Serial.println("Error in HTTP");
    }
}

String FetchInstruction(){
    // initialise the http connection
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "get_instruction.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);

    // dealing with the recieved payload of the get request
    int httpCode = http.GET();
    if(httpCode > 0){
        Serial.printf("HTTP GET code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK){
            String payload = http.getString();
            http.end();
            return payload;
        }
        http.end();
        return "";
    }
    else{
        Serial.println("Error in HTTP");
        http.end();
        return "";
    }
}

void PostSensorReadings(){
    // initialise the http connection
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "put_sensor_data.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // dealing with the post
    String http_post_data = "x=2&y=3";
    int httpCode = http.POST(http_post_data);
    std::cout << httpCode << std::endl;
}
