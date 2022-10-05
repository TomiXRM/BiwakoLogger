#include "canSender.hpp"

canSender::canSender(long canBaud, BluetoothSerial *_SerialBT) {
    SerialBT = _SerialBT;
    CAN.setPins(25, 26);
    uint8_t c = 0;
    while (!CAN.begin(canBaud)) {
        c++;
        Serial.println("Starting CAN failed!");
        SerialBT->println("Starting CAN failed!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : CAN failed");
            SerialBT->println("Reboot reason : CAN failed");
            ESP.restart();
        }
    }
}

void canSender::baud(long canBaud) {
    CAN.begin(canBaud);
}

void canSender::sendSensor1(sensor1_t &sensor) {
    CAN.beginPacket(sensor.id);
    CAN.write(sensor.value.u8, 4);
    CAN.endPacket();
}

void canSender::sendSensor3(sensors3_t &sensor) {
    sendSensor1(sensor.x);
    sendSensor1(sensor.y);
    sendSensor1(sensor.z);
}

void canSender::sendSensor4(sensors4_t &sensor) {
    sendSensor1(sensor.w);
    sendSensor1(sensor.x);
    sendSensor1(sensor.y);
    sendSensor1(sensor.z);
}