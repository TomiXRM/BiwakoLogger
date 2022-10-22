#include "Arduino.h"
#include "./setup/setup.hpp"

#include "mode/m.hpp"

void setup() {
    // initiate serial communication
    sensors.begin();
    Serial.begin(115200);
    SerialBT.begin(HOSTNAME);
    sysMan.begin();
    canSender.begin(1000E3);

    sysMan.addMode(mode_m);
}

void loop() {
    sysMan.run();
}
