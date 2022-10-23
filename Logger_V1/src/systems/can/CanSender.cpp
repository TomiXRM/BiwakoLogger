#include "CanSender.hpp"

CanSender::CanSender(BluetoothSerial *_SerialBT) {
    SerialBT = _SerialBT;
}

void CanSender::begin(long canBaud) {
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

void CanSender::onReceive(int packetSize) {
    Serial.print("Received ");

    isExtended = CAN.packetExtended();
    if (isExtended) Serial.print(" extended ");

    isRtr = CAN.packetRtr();
    if (isRtr) Serial.print("RTR ");

    receivedCanId = CAN.packetId();
    Serial.printf("packet with id 0x%x", receivedCanId);

    if (isRtr) {
        requestBytes = CAN.packetDlc();
        Serial.printf(" and requested length %d", requestBytes);
    } else {
        Serial.printf(" and length %d", packetSize);
        // only print packet data for non-RTR packets
        while (CAN.available()) {
            Serial.print((char)CAN.read());
        }
        Serial.println();
    }

    Serial.println();
}

void CanSender::baud(long canBaud) {
    CAN.begin(canBaud);
}

long CanSender::chechMatch(long *canId, int canIdQty) {
    for (size_t i = 0; i < canIdQty; i++) {
        if (receivedCanId == canId[i]) {
            return receivedCanId;
        }
    }
    return -1;
}

long CanSender::getReceivedCanId() {
    return receivedCanId;
}
bool CanSender::getIsExtended() {
    return isExtended;
}
bool CanSender::getIsRtr() {
    return isRtr;
}
int CanSender::getRequestBytes() {
    return requestBytes;
}

void CanSender::send(Sensor1_t &sensor) {
    Serial.printf(" - Sending id 0x%x %f\r\n", sensor.id, sensor.f);
    CAN.beginPacket(sensor.id);
    CAN.write(sensor.u8, 4);
    CAN.endPacket();
}

void CanSender::send(Sensor3_t &sensor) {
    send(sensor.x);
    send(sensor.y);
    send(sensor.z);
}

void CanSender::send(Sensor4_t &sensor) {
    send(sensor.w);
    send(sensor.x);
    send(sensor.y);
    send(sensor.z);
}