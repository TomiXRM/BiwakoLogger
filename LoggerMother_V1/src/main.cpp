// read waterTemperature with multitasking and display it on the screen
#include "setup.hpp"
// #include <ArduinoLog.h>

void onReceive(int packetSize) {
    long packetId = CAN.packetId();
    long num = packetId;
    long num_;
    unsigned digit = 0;
    while (num != 0) {
        num_ = num;
        num /= 10;
        digit++;
    }
    long packetIdForLogger = num_ * pow(10, digit - 1);
    Log.notice("packet:%d \n", packetIdForLogger);
    for (size_t i = 0; i < sizeof(Logger) / sizeof(Logger[0]); i++) {
        long loggerId = Logger[i].getId();
        Log.notice("matchId:%d \n", loggerId);
        if (loggerId == packetIdForLogger) {
            Logger[i].onReceive(packetSize, packetId);
            break;
        }
    }
}

void setup() {
    Serial.begin(1000000);
    Serial2.setRxBufferSize(1024);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    SerialBT.begin(hostname);
    //  LOG_LEVEL_SILENT 0
    //  LOG_LEVEL_FATAL 1
    //  LOG_LEVEL_ERROR 2
    //  LOG_LEVEL_WARNING 3
    //  LOG_LEVEL_INFO 4
    //  LOG_LEVEL_NOTICE 4
    //  LOG_LEVEL_TRACE 5
    //  LOG_LEVEL_VERBOSE 6
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    pinMode(LED_PIN, OUTPUT);
    // alive LED initialization
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    for (size_t i = 0; i < sizeof(Logger) / sizeof(Logger[0]); i++) {
        Logger[i].init();
    }

    initSD();
    initCan();
    CAN.onReceive(onReceive);
    Log.notice("Ready");
    SerialBT.println("Ready");
}

void loop() {
    // readGPS();
    // Log.noticeln("CAN:");
    for (Logger_V1 &logger : Logger) {
        logger.sendRequest(logger.temp.id, 30);
        logger.sendRequest(logger.press.id, 30);
        logger.sendRequest(logger.acc.id, 30);
        logger.sendRequest(logger.mag.id, 30);
        logger.sendRequest(logger.gyro.id, 30);
        logger.sendRequest(logger.grav.id, 30);
        logger.sendRequest(logger.euler.id, 30);
        logger.sendRequest(logger.quat.id, 30);
    }
}