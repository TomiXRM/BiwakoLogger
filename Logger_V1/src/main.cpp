#include "Arduino.h"
#include "./setup/setup.hpp"

#include "mode/m.hpp"
#include "mode/calibrationMode.hpp"

extern void Core0a(void *args);
extern void Core1a(void *args);

void setup() {
    // initialize serial communication
    Serial.begin(1000000);
    SerialBT.begin(HOSTNAME);

    // initialize CAN communication
    canSender.begin(1000E3);
    // CAN.onReceive(onReceive);
    // initialize system Manager
    sysManager.begin();

    // initialize sensors
    sensors.begin();
    // add modes
    sysManager.addMode(mode_m);
    sysManager.addMode(mode_c);

    // initialize water temperature and pressue sensor
    xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, 0, 1, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, 0, 1, &thp[1], 1);
}

void loop() {
    sysManager.run();
}
