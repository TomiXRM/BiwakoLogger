#include "Arduino.h"
#include "./setup/setup.hpp"

// #include "mode/m.hpp"

bool tl = 0;

void setup() {
    // initiate serial communication
    sensors.begin();
    Serial.begin(115200);
    SerialBT.begin(HOSTNAME);
    sysMan.begin();
    // sensors = Sensors(&SerialBT);
}

void loop() {
    Serial.println("loop");
    sensors.printTest();
    delay(100);
}
