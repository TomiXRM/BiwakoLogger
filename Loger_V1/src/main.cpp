#include "Arduino.h"
#include "./setup/setup.hpp"

#include "mode/m.hpp"
#include "mode/calibrationMode.hpp"

void setup() {
    // initialize serial communication
    Serial.begin(115200);
    SerialBT.begin(HOSTNAME);

    // initialize CAN communication
    canSender.begin(1000E3);

    // initialize system Manager
    sysManager.begin();

    // initialize sensors
    sensors.begin();
    // add modes
    sysManager.addMode(mode_m);
    sysManager.addMode(mode_c);
}

void loop() {
    sysManager.run();
}
