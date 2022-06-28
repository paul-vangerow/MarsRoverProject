#include <NewPing.h>

#define TRIGGER_PIN 26
#define ECHO_PIN 25
#define MAX_DISTANCE 10

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int ping(){
    return sonar.ping_cm();
}
