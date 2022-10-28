#include "Logger_V1.hpp"

Logger_V1::Logger_V1(int id, int logLevel, bool debug, ESP32SJA1000Class *can)
    : temp(id + 10, "temp", "C"),
      press(id + 20, "press", "Pa"),
      acc(id + 30, "acc", "m/s^2"),
      mag(id + 40, "mag", "uT"),
      gyro(id + 50, "gyro", "deg/s"),
      grav(id + 60, "grav", "m/s^2"),
      euler(id + 70, "euler", "deg"),
      quat(id + 80, "quat", "") {
    this->id = id;
    this->can = can;
    Log.begin(logLevel, &Serial, debug);
    Log.notice("Logger_V1 %d created", id);
    s4Qty = s1Qty = s3Qty = 0;
}

void Logger_V1::init() {
    appendSensor(&temp);
    appendSensor(&press);
    appendSensor(&acc);
    appendSensor(&mag);
    appendSensor(&gyro);
    appendSensor(&grav);
    appendSensor(&euler);
    appendSensor(&quat);

    uint8_t k = 0;
    for (size_t i = 0; i < s1Qty; i++) {
        canIdList[k] = sensors1[i]->id;
        k++;
    }
    for (size_t i = 0; i < s3Qty; i++) {
        canIdList[k] = sensors3[i]->id;
        k++;
    }
    for (size_t i = 0; i < s4Qty; i++) {
        canIdList[k] = sensors4[i]->id;
        k++;
    }
    // Log.noticeln("Logger_V1 %d initialized", id);
    for (size_t i = 0; i < k; i++) {
        Serial.printf("canIdList[%d] = %d\n", i, canIdList[i]);
    }
}

void Logger_V1::appendSensor(Sensor1_t *s1) {
    sensors1[s1Qty] = s1;
    s1Qty++;
}

void Logger_V1::appendSensor(Sensor3_t *s3) {
    sensors3[s3Qty] = s3;
    s3Qty++;
}
void Logger_V1::appendSensor(Sensor4_t *s4) {
    sensors4[s4Qty] = s4;
    s4Qty++;
}
void Logger_V1::sendRequest(long id, int interval) {
    // Log.noticeln("sendRequest %d", id);
    can->beginPacket(id, 4, true);
    can->endPacket();
    requestedCanId = id;
    delay(interval);
}

void Logger_V1::read(uint8_t packetSize, Sensor1_t &s1) {
    if (isRtr) {
        requestBytes = can->packetDlc();
        Log.trace(" and requested length %d", requestBytes);
    } else {
        // Serial.printf(" and length %d ", packetSize);
        // only print packet data for non-RTR packets
        uint8_t i = 0;
        uint8_t buf[8] = {0};
        while (can->available()) {
            buf[i] = can->read();
            // Serial.printf("%d ", buf[i]);
            i++;
        }
        s1.u8[0] = buf[0];
        s1.u8[1] = buf[1];
        s1.u8[2] = buf[2];
        s1.u8[3] = buf[3];
        // ArduinoLog CANNOT PRINT FLOATS
        // Serial.printf("VALUE:%f", s1.f);
    }
}

void Logger_V1::onReceive(int packetSize, long receivedCanId) {
    // Log.trace("Receive: ");
    isExtended = can->packetExtended();
    // if (isExtended) Log.trace(" extended ");

    isRtr = can->packetRtr();
    // if (isRtr)Log.trace("RTR ");
    // Serial.printf("packet with id 0x%x", receivedCanId);
    // Log.trace("Id:%d ", receivedCanId);
    // check match canId
    for (size_t i = 0; i < s1Qty; i++) {
        if (sensors1[i]->id == receivedCanId && receivedCanId == requestedCanId) {
            // Log.trace("%s", sensors1[i]->name);
            read(packetSize, *sensors1[i]);
            break;
        }
    }
    for (size_t i = 0; i < s3Qty; i++) {
        if (sensors3[i]->x.id == receivedCanId && receivedCanId == requestedCanId + 1) {
            // Log.trace("%s%s", sensors3[i]->name, sensors3[i]->x.name);
            read(packetSize, sensors3[i]->x);
            break;
        }
        if (sensors3[i]->y.id == receivedCanId && receivedCanId == requestedCanId + 2) {
            // Log.trace("%s%s", sensors3[i]->name, sensors3[i]->y.name);
            read(packetSize, sensors3[i]->y);
            break;
        }
        if (sensors3[i]->z.id == receivedCanId && receivedCanId == requestedCanId + 3) {
            // Log.trace("%s%s", sensors3[i]->name, sensors3[i]->z.name);
            read(packetSize, sensors3[i]->z);
            break;
        }
    }
    for (size_t i = 0; i < s4Qty; i++) {
        if (sensors4[i]->w.id == receivedCanId && receivedCanId == requestedCanId + 1) {
            // Log.trace("%s%s", sensors4[i]->name, sensors4[i]->w.name);
            read(packetSize, sensors4[i]->w);
            break;
        }
        if (sensors4[i]->x.id == receivedCanId && receivedCanId == requestedCanId + 2) {
            // Log.trace("%s%s", sensors4[i]->name, sensors4[i]->x.name);
            read(packetSize, sensors4[i]->x);
            break;
        }
        if (sensors4[i]->y.id == receivedCanId && receivedCanId == requestedCanId + 3) {
            // Log.trace("%s%s", sensors4[i]->name, sensors4[i]->y.name);
            read(packetSize, sensors4[i]->y);
            break;
        }
        if (sensors4[i]->z.id == receivedCanId && receivedCanId == requestedCanId + 4) {
            // Log.trace("%s%s", sensors4[i]->name, sensors4[i]->z.name);
            read(packetSize, sensors4[i]->z);
            break;
        }
    }
    // Log.trace("\n");
}