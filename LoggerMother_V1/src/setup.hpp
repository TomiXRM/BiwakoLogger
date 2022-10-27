#ifndef SETUP
#define SETUP

#include <Arduino.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
#include <CAN.h>
#include <TinyGPS++.h>
#include <Ticker.h>

#include <BluetoothSerial.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "dataDefs.hpp"
#include <./Logger/Logger_V1.hpp>

// pin definitions
#define LED_PIN 2
#define PRESSURE_SENSOR_PIN 32
#define ONE_WIRE_BUS_PIN 33

const char *ssid = "GL-SFT1200-3a3";     //自分のSSIDを設定する
const char *password = "goodlife";       // SSIDのパスワードを設定する
const char *hostname = "BEP_ESP_MOTHER"; //ホスト名を設定する

// create instance
Ticker tick;
BluetoothSerial SerialBT;
TinyGPSPlus tinyGPS;
File myfile; // SDカードの状態を格納

Logger_V1 Logger[] = {
    // Logger_V1(0, LOG_LEVEL_VERBOSE, true),
    Logger_V1(100, LOG_LEVEL_VERBOSE, true,&CAN),
    // Logger_V1(200, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(300, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(400, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(500, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(600, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(700, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(800, LOG_LEVEL_VERBOSE, true),
    // Logger_V1(900, LOG_LEVEL_VERBOSE, true),
};

// global variables
volatile struct {
    float temp;
    int32_t pressure;
    int32_t _pressure[2];
    uint32_t time;
    struct {
        double latitude;
        double longitude;
    } gps;
} data;

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

void initSD() {
    uint8_t v = 0;
    while (!SD.begin(4) && v < 10) {
        v++;
        delay(1000);
        Log.errorln("SD Card Mount Failed");
    }
}

void initCan() {
    // CAN通信を初期化
    CAN.setPins(25, 26);      // CAN_RX, CAN_TX
    if (!CAN.begin(1000E3)) { // 500kbpsで初期化
        Log.errorln("Starting CAN failed!");
        while (1)
            ;
    }
}

#endif