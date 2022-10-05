#include "./setup/setup.hpp"
sensor1_t sensor1(10, "temp", "℃");
void setup() {
    Serial.begin(2000000);
    SerialBT.begin(HOSTNAME);
}

void loop() {
}
