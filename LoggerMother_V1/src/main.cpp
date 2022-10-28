#include <Arduino.h>
#include <CAN.h>
#include "SPI.h"
#include "FS.h"
#include "SD.h"
#include <BluetoothSerial.h>
#include <ArduinoLog.h>
#include <TinyGPS++.h>
#include <Ticker.h>
#include "./Logger/dataDefs.hpp"
#include <./Logger/Logger_V1.hpp>

// pin definitions
#define LED_PIN 2
#define PRESSURE_SENSOR_PIN 32
#define ONE_WIRE_BUS_PIN 33

const char *ssid = "GL-SFT1200-3a3";     //自分のSSIDを設定する
const char *password = "goodlife";       // SSIDのパスワードを設定する
const char *hostname = "BEP_ESP_MOTHER"; //ホスト名を設定する

/*************************************** Objects ***************************************/
Ticker tick;
BluetoothSerial SerialBT;
TinyGPSPlus tinyGPS;
File myfile; // SDカードの状態を格納
int packetSize = 0;
Logger_V1 Logger[] = {
    // Logger_V1(0, LOG_LEVEL_VERBOSE, true),
    Logger_V1(100, LOG_LEVEL_VERBOSE, true, &CAN),
    // Logger_V1(200, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(300, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(400, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(500, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(600, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(700, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(800, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(900, LOG_LEVEL_VERBOSE, true),
};

struct {
    uint32_t time;
    struct {
        double latitude;
        double longitude;
    } gps;
} data;

void initLoggerInfo() {
    for (size_t i = 0; i < sizeof(Logger) / sizeof(Logger[0]); i++) {
        Logger[i].init();
    }
}

void initLed() {
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
}

/*************************************** GNSS ***************************************/
void readGPS() {
    while (Serial2.available()) {
        char buf[256] = {NULL};
        Serial.printf("\n\n -Avairable:%d\n\n", Serial2.available());
        Serial2.readBytes(buf, Serial2.available());
        for (size_t i = 0; i < 256; i++) {
            if (buf[i] == NULL) break;
            tinyGPS.encode(buf[i]);
            Serial.print(buf[i]);
        }
    }
    if (tinyGPS.location.isUpdated()) {
        data.gps.latitude = tinyGPS.location.lat();
        data.gps.longitude = tinyGPS.location.lng();
        Serial.printf("%f,%f\r\n", data.gps.latitude, data.gps.longitude);
    }

    // sprintf(text, "wp,%d,wt,%.2f", data.pressure, data.temp);
    // Serial.printf("%s\r\n", text);
    // SerialBT.printf("%s\r\n", text);
    // writeSD("test", text);
}

/*************************************** SD ***************************************/
void initSD() {
    uint8_t v = 0;
    while (!SD.begin(4) && v < 10) {
        v++;
        delay(1000);
        Log.errorln("SD Card Mount Failed");
    }
}

void writeSD(String fileName, String data) {
    String filePath = "/" + fileName + ".csv";

    myfile = SD.open(filePath, FILE_APPEND);
    if (myfile) {
        myfile.println(data);
        myfile.close(); //ファイルを閉じる
    } else {
        Serial.println("SD-ERR");
    }
}

/*************************************** CAN ***************************************/
void initCan() {
    // CAN通信を初期化
    CAN.setPins(25, 26);      // CAN_RX, CAN_TX
    if (!CAN.begin(1000E3)) { // 500kbpsで初期化
        Log.errorln("Starting CAN failed!");
        while (1)
            ;
    }
}
void onReceive(int packetSize) {
    long packetId = CAN.packetId();
    long num = packetId;
    long num_ = 0;
    unsigned digit = 0;
    while (num != 0) {
        num_ = num;
        num /= 10;
        digit++;
    }
    if (num_ == 0) digit = 1;
    long packetIdForLogger = num_ * pow(10, digit - 1);
    // Log.notice("packetIdForLogger:%d \n", packetIdForLogger);
    for (size_t i = 0; i < sizeof(Logger) / sizeof(Logger[0]); i++) {
        long loggerId = Logger[i].getId();
        if (loggerId == packetIdForLogger) {
            // Log.notice("matchId:%d at %d \n", loggerId, packetId);
            // Serial.printf("[%d] = %d\n", i, packetId);
            Logger[i].onReceive(packetSize, packetId);
            break;
        }
    }
}
/*************************************** Main ***************************************/
void setup() {
    Serial.begin(1000000);
    Serial2.setRxBufferSize(1024);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    SerialBT.begin(hostname);
    // LOG_LEVEL_SILENT 0 LOG_LEVEL_FATAL 1 LOG_LEVEL_ERROR 2 LOG_LEVEL_WARNING 3 LOG_LEVEL_INFO 4 LOG_LEVEL_NOTICE 4 LOG_LEVEL_TRACE 5 LOG_LEVEL_VERBOSE 6
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    initLed();
    initLoggerInfo();
    initSD();
    initCan();
    CAN.onReceive(onReceive);
    Log.notice("Ready");
    SerialBT.println("Ready");
}

void loop() {
    // readGPS();
    for (Logger_V1 &logger : Logger) {
        logger.sendRequest(logger.temp.id, 20);
        logger.sendRequest(logger.press.id, 20);
        logger.sendRequest(logger.grav.id, 20);
        logger.sendRequest(logger.euler.id, 20);
    }

    for (Logger_V1 &logger : Logger) {
        logger.temp.print();
        logger.press.print();
        logger.grav.print();
        logger.euler.print();
    }
}
