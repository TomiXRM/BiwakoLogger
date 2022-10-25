// read waterTemperature with multitasking and display it on the screen
#include "setup.hpp"
#include "dataDefs.hpp"
#include <CAN.h>
#include <canFunctions.hpp>
#define CAN_DEBUG
int canId;

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
    // CAN受信割り込みコールバック関数を設定
    CAN.onReceive(onReceive);

    makeCanIdList(canIdList, &canIdQty);
}

void loop() {
    // readGPS();
    int interval = 30;
    sendRequest(10);
    delay(interval);
    sendRequest(20);
    delay(interval);
    sendRequest(30);
    delay(interval);
    sendRequest(40);
    delay(interval);
    sendRequest(50);
    delay(interval);
    sendRequest(60);
    delay(interval);
    sendRequest(70);
    delay(interval);
    sendRequest(80);
    delay(interval);
    
}
