#include "Arduino.h"
#include "./setup/setup.hpp"

#include "mode/m.hpp"

sensor1_t sensor1(10, "temp", "℃");

void setup() {
    Serial.begin(2000000);
    SerialBT.begin(HOSTNAME);

    systemManager.addMode(mode_m);
    systemManager.setMode('M');
}

void loop() {
    systemManager.run();
}
