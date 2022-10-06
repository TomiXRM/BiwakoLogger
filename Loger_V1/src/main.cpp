#include "Arduino.h"
#include "./setup/setup.hpp"

#include "mode/m.hpp"


void setup() {
    // initiate serial communication
    Serial.begin(2000000);
    SerialBT.begin(HOSTNAME);

    // initalize modes
    systemManager.addMode(mode_m);

    // setup default mode
    systemManager.setMode('M');
}

void loop() {
    systemManager.run();
}
