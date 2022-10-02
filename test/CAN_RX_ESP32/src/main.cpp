// Master
#define CHECK2
#ifdef CHECK2
#include <CAN.h>
#include <Ticker.h>
const int LED_PIN = 2; //生存確認用LED
Ticker tick;           //生存確認用Ticker

void onReceive(int packetSize); // CAN受信時に呼び出される関数(プロトタイプ宣言)

void setup() {
    // 生存確認のタイマー割り込み設定
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(1000, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    // シリアル初期化
    Serial.begin(2000000);
    while (!Serial)
        ;

    Serial.println("CAN Receiver Callback");

    // CAN通信を初期化
    CAN.setPins(25, 26);      // CAN_RX, CAN_TX
    if (!CAN.begin(1000E3)) { // 500kbpsで初期化
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }

    // CAN受信割り込みコールバック関数を設定
    CAN.onReceive(onReceive);
}

// CAN受信時に呼び出される関数
void onReceive(int packetSize) {

    Serial.print("Received ");

    if (CAN.packetExtended()) {
        Serial.print(" extended ");
    }

    if (CAN.packetRtr()) {
        // Remote transmission request, packet contains no data
        Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
        Serial.print(" and requested length ");
        Serial.println(CAN.packetDlc());
    } else {
        Serial.print(" and length ");
        Serial.println(packetSize);
        // only print packet data for non-RTR packets
        while (CAN.available()) {
            Serial.print((char)CAN.read());
        }
        Serial.println();
    }

    Serial.println();
}

void loop() {
    Serial.println("loop");
    CAN.beginPacket(0x100, 5, true); // Slaveにデータ送信のリクエスト(RTR設定)
    CAN.endPacket();                  // Slaveにデータ送信のリクエスト(送信)
    delay(100);
}
#endif
#ifndef CHECK2
// Copyright(c) Sandeep Mistry.All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "Arduino.h"
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
void onReceive(int packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
        Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
        // Remote transmission request, packet contains no data
        Serial.print("RTR ");
    }

    Serial.print("packet with id ");
    Serial.print(CAN.packetId());

    if (CAN.packetRtr()) {
        Serial.print(" and requested length ");
        Serial.println(CAN.packetDlc());
    } else {
        Serial.print(" and length ");
        Serial.println(packetSize);

        // only print packet data for non-RTR packets
        while (CAN.available()) {
            Serial.print((char)CAN.read());
        }
        Serial.println();
    }

    Serial.println();
}

void setup() {
    Serial.begin(2000000);
    while (!Serial)
        ;

    Serial.println("CAN Receiver Callback");

    // start the CAN bus at 500 kbps
    CAN.setPins(25, 26); // CAN_RX, CAN_TX
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }

    // register the receive callback
    CAN.onReceive(onReceive);
}

void loop() {
    // do nothing
}

#endif