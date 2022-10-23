// read waterTemperature with multitasking and display it on the screen
#include <setup.hpp>

#include <CAN.h>

typedef union {
    float value;
    uint8_t bytes[4];
} sensorData;
int canId;

sensorData euler;
void onReceive(int packetSize); // CAN受信時に呼び出される関数(プロトタイプ宣言)

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

    canId = CAN.packetId();
    Serial.print("packet with id 0x");
    Serial.print(canId, HEX);

    if (CAN.packetRtr()) {
        Serial.print(" and requested length ");
        Serial.println(CAN.packetDlc());
    } else {
        Serial.print(" and length ");
        Serial.println(packetSize);
        // only print packet data for non-RTR packets
        uint8_t i = 0;
        while (CAN.available()) {
            euler.bytes[i] = CAN.read();
            Serial.printf("%d ", euler.bytes[i]);
            i++;
        }
        Serial.println(euler.value);
    }

    Serial.println();
}

void loop() {
    // readGPS();

    CAN.beginPacket(10, 4, true);
    CAN.endPacket();

    CAN.beginPacket(20, 4, true);
    CAN.endPacket();

    CAN.beginPacket(30, 4, true);
    CAN.endPacket();

    CAN.beginPacket(40, 4, true);
    CAN.endPacket();

    CAN.beginPacket(50, 4, true);
    CAN.endPacket();

    CAN.beginPacket(60, 4, true);
    CAN.endPacket();

    CAN.beginPacket(70, 4, true); // Slaveにデータ送信のリクエスト(RTR設定)
    CAN.endPacket();              // Slaveにデータ送信のリクエスト(送信)

    CAN.beginPacket(80, 4, true);
    CAN.endPacket();
    delay(100);
}
