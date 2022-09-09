// Slave
// #define CHECK2
#ifdef CHECK2
#include <CAN.h>
#include <Ticker.h>

const int MODULE_ID = 0x100; // Slaｖeモジュール(このESP32)のユニークID
const int LED_PIN = 2;       //生存確認用LED
Ticker tick;                 //生存確認用Ticker

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
    CAN.setPins(25, 26);     // CAN_RX, CAN_TX
    if (!CAN.begin(500E3)) { // 500kbpsで初期化
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }

    //↓なぜか受信割り込み中にCAN.endPacket()するとメモリエラーを起こすので使わないことにしています。
    // CAN.onReceive(onReceive);
}

// CAN受信時に呼び出される関数
void onReceive(int packetSize) {
    Serial.print("Received ");

    Serial.print("filter:");
    Serial.print(CAN.filter(MODULE_ID)); // フィルター設定

    if (CAN.packetExtended()) {
        Serial.print(" extended ");
    }

    if (CAN.packetRtr()) {
        // Remote transmission request, packet contains no data
        Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) { // RTRパケットの場合(送信リクエストが来た場合)
        Serial.print(" and requested length ");
        Serial.println(CAN.packetDlc());   //送信依頼の来ているデータのバイト数の表示
        if (CAN.packetId() == MODULE_ID) { //パケットIDが自分のIDに一致した場合
            Serial.println("Received packet is correct");
            CAN.beginPacket(0x44); // パケットを送信するためにbeginPacket()を呼び出す
            CAN.write('H');        //データ(1byte目)
            CAN.write('E');        //データ(2byte目)
            CAN.write('L');        //データ(3byte目)
            CAN.write('L');        //データ(4byte目)
            CAN.write('O');        //データ(5byte目)
            CAN.endPacket();       // パケットを送信
            Serial.println("sent packet");
        } else {
            Serial.println("Received packet is incorrect");
        }
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

    int packetSize = CAN.parsePacket(); //パケットサイズの確認
    if (packetSize) {                   // CANバスからデータを受信したら
        onReceive(packetSize);          //受信時に呼び出される関数を呼び出す
    } else {
        Serial.println("loop");
        delay(100);
    }
}
#endif
#ifndef CHECK2
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
#include <Ticker.h>

const int LED_PIN = 2;

Ticker tick;
void setup() {
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(1000, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    Serial.begin(2000000);
    while (!Serial)
        ;

    Serial.println("CAN Sender");

    // start the CAN bus at 500 kbps
    CAN.setPins(25, 26);
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }
}

void loop() {
    // send packet: id is 11 bits, packet can contain up to 8 bytes of data
    Serial.print("Sending packet ... ");

    CAN.beginPacket(0x12);
    CAN.write('h');
    CAN.write('e');
    CAN.write('l');
    CAN.write('l');
    CAN.write('o');
    CAN.endPacket();

    Serial.println("done");

    delay(1000);

    // send extended packet: id is 29 bits, packet can contain up to 8 bytes of data
    Serial.print("Sending extended packet ... ");

    CAN.beginExtendedPacket(0xabcdef);
    CAN.write('w');
    CAN.write('o');
    CAN.write('r');
    CAN.write('l');
    CAN.write('d');
    CAN.endPacket();

    Serial.println("done");

    delay(1000);
}
#endif