#include "canSender.hpp"

canSender::canSender(BluetoothSerial *_SerialBT) {
    SerialBT = _SerialBT;
}

void canSender::begin(long canBaud) {
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
    CAN.write(sensor.u8, 4);
    CAN.endPacket();
}

void canSender::sendSensor3(sensor3_t &sensor) {
    sendSensor1(sensor.x);
    sendSensor1(sensor.y);
    sendSensor1(sensor.z);
}

void canSender::sendSensor4(sensor4_t &sensor) {
    sendSensor1(sensor.w);
    sendSensor1(sensor.x);
    sendSensor1(sensor.y);
    sendSensor1(sensor.z);
}