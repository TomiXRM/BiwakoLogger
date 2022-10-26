// read waterTemperature with multitasking and display it on the screen
#include "setup.hpp"
#include "dataDefs.hpp"
#include <CAN.h>
#include <./Logger/Logger_V1.hpp>
#define CAN_DEBUG
int canId;

Logger_V1 Logger[] = {
    Logger_V1(0, LOG_LEVEL_VERBOSE, true),
    Logger_V1(100, LOG_LEVEL_VERBOSE, true),
    Logger_V1(200, LOG_LEVEL_VERBOSE, true),
    Logger_V1(300, LOG_LEVEL_VERBOSE, true),
    Logger_V1(400, LOG_LEVEL_VERBOSE, true),
    Logger_V1(500, LOG_LEVEL_VERBOSE, true),
    Logger_V1(600, LOG_LEVEL_VERBOSE, true),
    Logger_V1(700, LOG_LEVEL_VERBOSE, true),
    Logger_V1(800, LOG_LEVEL_VERBOSE, true),
    Logger_V1(900, LOG_LEVEL_VERBOSE, true),
};

void onReceive(int packetSize) {
    long packetId = CAN.packetId();
    int num = packetId;
    int num_;
    unsigned digit = 0;
    while (num != 0) {
        num_ = num;
        num /= 10;
        digit++;
    }
    int packetIdForLogger = num_ * pow(10, digit - 1);
    for (size_t i = 0; i < sizeof(Logger) / sizeof(Logger[0]); i++) {
        if (Logger[i].getId() == packetIdForLogger) {
            Logger[i].onReceive(packetSize);
            break;
        }
    }
}

void setup() {
    Serial.begin(1000000);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    Serial2.setRxBufferSize(2048);
    SerialBT.begin(hostname);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    pinMode(LED_PIN, OUTPUT);
    // alive LED initialization
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    uint8_t v = 0;
    while (!SD.begin(4) && v < 10) {
        v++;
        delay(1000);
        Log.errorln("SD Card Mount Failed");
    }

    Log.notice("Ready");
    SerialBT.println("Ready");

    // CAN通信を初期化
    CAN.setPins(25, 26);      // CAN_RX, CAN_TX
    if (!CAN.begin(1000E3)) { // 500kbpsで初期化
        Log.errorln("Starting CAN failed!");
        while (1)
            ;
    }

    for (size_t i = 0; i < sizeof(Logger) / sizeof(Logger[0]); i++) {
        Logger[i].init();
    }
    // CAN受信割り込みコールバック関数を設定
    CAN.onReceive(onReceive);
}

void loop() {
    // readGPS();
    for (Logger_V1 &logger : Logger) {
        logger.sendRequest(logger.temp.id, 10);
        logger.sendRequest(logger.press.id, 10);
        logger.sendRequest(logger.acc.id, 30);
        logger.sendRequest(logger.mag.id, 30);
        logger.sendRequest(logger.gyro.id, 30);
        logger.sendRequest(logger.grav.id, 30);
        logger.sendRequest(logger.euler.id, 30);
        logger.sendRequest(logger.quat.id, 30);
    }
}
