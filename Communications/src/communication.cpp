#include <communication.hpp>
#include <instructions.hpp>
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
        }
    }
    else{
        Serial.println("Error in HTTP");
    }
}

int InitDB(){
     // initialise the http connection
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "create_database.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);

    // dealing with the recieved payload
    int httpCode = http.GET();
    Serial.printf("HTTP code: %i\n", httpCode);
    if(httpCode > 0){
        if(httpCode == HTTP_CODE_OK){
            String payload = http.getString();
            return std::stoi(payload.c_str());
        }
        return 0;
    }
    return 0;
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
            Serial.println(payload);
            return payload;
        }
        return "";
    }
    else{
        Serial.println("Error in HTTP");
        return "";
    }
}

void PostInstruction(Mouvement mouv){
    // initialise the http connection
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "put_sensor_data.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // dealing with the post
    String s_instr = String(mouv.get_instruction());
    String s_val = String(mouv.get_value());
    String http_post_data = "instr=" + s_instr + "&val=" + s_val + "&executed=1";
    int httpCode = http.POST(http_post_data);
    if(!httpCode == HTTP_CODE_OK){
        fprintf(stderr, "HTTP code for post instruction: %i", httpCode);
    }
}

void PostRadarValues(val_t x, val_t y, double value){
    // initialise the http connection
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "put_sensor_data.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // dealing with the post
    String s_x = String(x), s_y = String(y), s_val = String(value);
    String http_post_data = "x=" + s_x + "&y=" + s_y + "&val=" + s_val;
    int httpCode = http.POST(http_post_data);
    if(!httpCode == HTTP_CODE_OK){
        fprintf(stderr,"HTTP code for Post radar value: %i", httpCode);
    }
}

void PostAlienLocation(val_t x, val_t y, double distance, Orientation orientation, Colour c){
    // initialise the http connection
    HTTPClient http;
    String GetAddress, LinkGet, GetData;
    GetAddress = "put_alien_location.php";
    LinkGet = host + GetAddress;
    http.begin(LinkGet);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    //putting colour into string
    String s_c;
    switch(c){
        case red:
            s_c = String("red");
            break;
        case blue:
            s_c = String("blue");
            break;
        case green:
            s_c = String("green");
            break;
        case teal:
            s_c = String("teal");
            break;
        case pink:
            s_c = String("pink");
            break;
        case yellow:
            s_c = String("yellow");
            break;
    }
   
    //dealing with the post
    String s_x = String(x), s_y = String(y), s_distance = String(distance), s_orientation = String(orientation);
    String http_post_data = "x=" + s_x + "&y=" + s_y + "&distance=" + s_distance + "&orientation=" + s_orientation + "&colour=" + s_c;
    int httpCode = http.POST(http_post_data);
    std::cout << httpCode << std::endl;
    if(!httpCode == HTTP_CODE_OK){
        fprintf(stderr, "HTTP code for post alien location: %i", httpCode);
    }

}