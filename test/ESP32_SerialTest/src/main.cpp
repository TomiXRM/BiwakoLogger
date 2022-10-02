// // Copyright (c) Sandeep Mistry. All rights reserved.
// // Licensed under the MIT license. See LICENSE file in the project root for full license information.

// #include <CAN.h>
// #include <Ticker.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

// const int LED_PIN = 2;
// const int ONE_WIRE_BUS = 33;
// const int AIN_PIN = 27;

// Ticker tick;
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature temperatureSens(&oneWire);

// struct {
//     int temp;
//     int pressure;
// } sensors

//     void
//     onReceive(int packetSize); // CAN受信時に呼び出される関数(プロトタイプ宣言)

// void setup() {
//     pinMode(LED_PIN, OUTPUT);
//     tick.attach_ms(1000, []() {
//         digitalWrite(LED_PIN, !digitalRead(LED_PIN));
//     });
//     Serial.begin(2000000);
//     temperatureSens.begin();
//     Serial.println("CAN Loger");

//     // start the CAN bus at 500 kbps
//     CAN.setPins(25, 26);
//     if (!CAN.begin(500E3)) {
//         Serial.println("Starting CAN failed!");
//         while (1)
//             ;
//     }
//     // CAN.onReceive(onReceive);
// }

// // CAN受信時に呼び出される関数
// void onReceive(int packetSize) {
//     Serial.print("Received ");

//     Serial.print("filter:");
//     Serial.print(CAN.filter(MODULE_ID)); // フィルター設定

//     if (CAN.packetExtended()) {
//         Serial.print(" extended ");
//     }

//     if (CAN.packetRtr()) {
//         // Remote transmission request, packet contains no data
//         Serial.print("RTR ");
//     }

//     Serial.print("packet with id 0x");
//     Serial.print(CAN.packetId(), HEX);

//     if (CAN.packetRtr()) { // RTRパケットの場合(送信リクエストが来た場合)
//         Serial.print(" and requested length ");
//         Serial.println(CAN.packetDlc());   //送信依頼の来ているデータのバイト数の表示
//         if (CAN.packetId() == MODULE_ID) { //パケットIDが自分のIDに一致した場合
//             Serial.println("Received packet is correct");
//             CAN.beginPacket(0x44); // パケットを送信するためにbeginPacket()を呼び出す
//             CAN.write('H');        //データ(1byte目)
//             CAN.write('E');        //データ(2byte目)
//             CAN.write('L');        //データ(3byte目)
//             CAN.write('L');        //データ(4byte目)
//             CAN.write('O');        //データ(5byte目)
//             CAN.endPacket();       // パケットを送信
//             Serial.println("sent packet");
//         } else {
//             Serial.println("Received packet is incorrect");
//         }
//     } else {
//         Serial.print(" and length ");
//         Serial.println(packetSize);

//         // only print packet data for non-RTR packets
//         while (CAN.available()) {
//             Serial.print((char)CAN.read());
//         }
//         Serial.println();
//     }

//     Serial.println();
// }

// void loop() {
//     int packetSize = CAN.parsePacket(); //パケットサイズの確認
//     sensors.requestTemperatures();

//     if (packetSize) {          // CANバスからデータを受信したら
//         onReceive(packetSize); //受信時に呼び出される関数を呼び出す
//     }
//     Serial.print("Temperature: ");
//     Serial.println();
// }

#include "Arduino.h"

// HardwareSerial Serial1(2);

char data = 0;
void setup() {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 12, 13);
}

void loop() {
    data ++;
    Serial1.write(data);
    delay(100);
    if (Serial1.available()) {
        Serial.println(Serial1.read());
    }
}