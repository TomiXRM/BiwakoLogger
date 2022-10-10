#include "Arduino.h"
#include "./setup/setup.hpp"

// #include "mode/m.hpp"

void setup() {
    // initiate serial communication
    Serial.begin(2000000);
    SerialBT.begin(HOSTNAME);
}

void loop() {
    Serial.println("loop");
    delay(1000);
}
