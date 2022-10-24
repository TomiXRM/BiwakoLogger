// read waterTemperature with multitasking and display it on the screen
#include "setup.hpp"
#include "dataDefs.hpp"
#include <CAN.h>

int canId;

Sensor1_t temp(10, "temp", "℃");
Sensor1_t press(20, "press", "Pa");
Sensor3_t acc(30, "acc", "m/s^2");
Sensor3_t mag(40, "mag", "uT");
Sensor3_t gyro(50, "gyro", "rad/s");
Sensor3_t grav(60, "grav", "m/s^2");
Sensor3_t euler(70, "euler", "rad");
Sensor4_t quat(80, "quat", "");

Sensor1_t *sensors[] = {&temp, &press, &acc.x, &acc.y, &acc.z, &mag.x, &mag.y, &mag.z, &gyro.x, &gyro.y, &gyro.z, &grav.x, &grav.y, &grav.z, &euler.x, &euler.y, &euler.z, &quat.w, &quat.x, &quat.y, &quat.z};
Sensor1_t *sensors1[] = {&temp, &press};
Sensor3_t *sensors3[] = {&acc, &mag, &gyro, &grav, &euler};
Sensor4_t *sensors4[] = {&quat};

long canIdList[20] = {0};
int canIdQty = 0;

bool isExtended;
bool isRemote;
bool isRtr;
int requestBytes;
long receivedCanId;
long requestedCanId;

void makeCanIdList(long *canIdList, int *canIdQty) {
    int i = 0;
    for (size_t j = 0; j < sizeof(sensors1) / sizeof(sensors1[0]); j++) {
        canIdList[i] = sensors1[j]->id;
        i++;
    }
    for (size_t j = 0; j < sizeof(sensors3) / sizeof(sensors3[0]); j++) {
        canIdList[i] = sensors3[j]->id;
        i++;
    }
    for (size_t j = 0; j < sizeof(sensors4) / sizeof(sensors4[0]); j++) {
        canIdList[i] = sensors4[j]->id;
        i++;
    }
    *canIdQty = i;
}

void read(uint8_t packetSize, Sensor1_t &s1) {
    if (isRtr) {
        requestBytes = CAN.packetDlc();
        Serial.printf(" and requested length %d", requestBytes);
    } else {
        // Serial.printf(" and length %d ", packetSize);
        // only print packet data for non-RTR packets
        uint8_t i = 0;
        uint8_t buf[8]{0};
        while (CAN.available()) {
            buf[i] = CAN.read();
            // Serial.printf("%d ", buf[i]);
            i++;
        }
        s1.u8[0] = buf[0];
        s1.u8[1] = buf[1];
        s1.u8[2] = buf[2];
        s1.u8[3] = buf[3];
        Serial.printf(" %.2f\n", s1.f);
    }
}

// CAN受信時に呼び出される関数
void onReceive(int packetSize) {
    Serial.print("Receive: ");

    isExtended = CAN.packetExtended();
    if (isExtended) Serial.print(" extended ");

    isRtr = CAN.packetRtr();
    if (isRtr) Serial.print("RTR ");

    receivedCanId = CAN.packetId();
    // Serial.printf("packet with id 0x%x", receivedCanId);
    Serial.printf("%d ", receivedCanId);
    // check match canId
    for (size_t i = 0; i < sizeof(sensors1) / sizeof(sensors1[0]); i++) {
        if (sensors1[i]->id == receivedCanId) {
            Serial.printf("%s", sensors1[i]->name);
            read(packetSize, *sensors1[i]);
            break;
        }
    }
    for (size_t i = 0; i < sizeof(sensors3) / sizeof(sensors3[0]); i++) {
        if (sensors3[i]->x.id == receivedCanId) {
            Serial.printf("%s%s", sensors3[i]->name, sensors3[i]->x.name);
            read(packetSize, sensors3[i]->x);
            break;
        }
        if (sensors3[i]->y.id == receivedCanId) {
            Serial.printf("%s%s", sensors3[i]->name, sensors3[i]->y.name);
            read(packetSize, sensors3[i]->y);
            break;
        }
        if (sensors3[i]->z.id == receivedCanId) {
            Serial.printf("%s%s", sensors3[i]->name, sensors3[i]->z.name);
            read(packetSize, sensors3[i]->z);
            break;
        }
    }
    for (size_t i = 0; i < sizeof(sensors4) / sizeof(sensors4[0]); i++) {
        if (sensors4[i]->w.id == receivedCanId) {
            Serial.printf("%s%s", sensors4[i]->name, sensors4[i]->w.name);
            read(packetSize, sensors4[i]->w);
            break;
        }
        if (sensors4[i]->x.id == receivedCanId) {
            Serial.printf("%s%s", sensors4[i]->name, sensors4[i]->x.name);
            read(packetSize, sensors4[i]->x);
            break;
        }
        if (sensors4[i]->y.id == receivedCanId) {
            Serial.printf("%s%s", sensors4[i]->name, sensors4[i]->y.name);
            read(packetSize, sensors4[i]->y);
            break;
        }
        if (sensors4[i]->z.id == receivedCanId) {
            Serial.printf("%s%s", sensors4[i]->name, sensors4[i]->z.name);
            read(packetSize, sensors4[i]->z);
            break;
        }
    }
    Serial.println();
}

void sendRequest(long id) {
    CAN.beginPacket(id, 4, true);
    CAN.endPacket();
    requestedCanId = id;
}

void setup() {
    Serial.begin(1000000);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    Serial2.setRxBufferSize(2048);
    SerialBT.begin(hostname);
    pinMode(LED_PIN, OUTPUT);
    // alive LED initialization
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    uint8_t v = 0;
    while (!SD.begin(4) && v < 10) {
        v++;
        delay(1000);
        Serial.println("SD Card Mount Failed");
    }

    Serial.println("Ready");
    SerialBT.println("Ready");

    // CAN通信を初期化
    CAN.setPins(25, 26);      // CAN_RX, CAN_TX
    if (!CAN.begin(1000E3)) { // 500kbpsで初期化
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }
    // CAN受信割り込みコールバック関数を設定
    CAN.onReceive(onReceive);

    makeCanIdList(canIdList, &canIdQty);
    Serial.println("--canIdList--");
    for (long canId : canIdList) {
        Serial.printf("%d ", canId);
    }
    Serial.println();
}

void loop() {
    // readGPS();
    CAN.beginPacket(10, 4, true);
    CAN.endPacket();
    delay(100);
    CAN.beginPacket(20, 4, true);
    CAN.endPacket();
    delay(100);
    CAN.beginPacket(30, 4, true);
    CAN.endPacket();
    delay(100);
    CAN.beginPacket(40, 4, true);
    CAN.endPacket();
    delay(100);
    CAN.beginPacket(50, 4, true);
    CAN.endPacket();
    delay(100);
    CAN.beginPacket(60, 4, true);
    CAN.endPacket();
    delay(100);
    CAN.beginPacket(70, 4, true); // Slaveにデータ送信のリクエスト(RTR設定)
    CAN.endPacket();              // Slaveにデータ送信のリクエスト(送信)
    delay(100);
    CAN.beginPacket(80, 4, true);
    CAN.endPacket();
    delay(100);
}
