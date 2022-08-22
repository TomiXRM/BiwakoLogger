// read waterTemperature with multitasking and display it on the screen
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>
#include <CAN.h>

// pin definitions
#define LED_PIN 2
#define ONE_WIRE_BUS 33

// create instance
Ticker tick;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);

// multiTask
TaskHandle_t thp[2];
QueueHandle_t xQueue_1;

// global variables
volatile struct {
    float temp;
    float pressur;
    uint32_t time;
} data;

// prototypes
void Core0a(void *args);
void Core1a(void *args);

void setup() {
    Serial.begin(115200);

    CAN.setPins(25, 26);
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }

    // alive LED initialization
    tick.attach_ms(10, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    // create tasks
    xQueue_1 = xQueueCreate(10, 16);
    xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 2, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, NULL, 1, &thp[1], 1);
}

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

// task1 (Core1) : read temperature and pressure
void Core1a(void *args) {
    float tmp;
    while (1) {
        waterTemp.requestTemperatures();
        tmp = waterTemp.getTempCByIndex(0);
        xQueueSend(xQueue_1, &tmp, 0);
    }
}

// task2 (Core0) : put water temperature to global variable
void Core0a(void *args) {
    float tmp = 0;
    while (1) {
        // wait for queue to be filled
        xQueueReceive(xQueue_1, &tmp, portMAX_DELAY);
        data.temp = tmp; // put to global variable
    }
}

void loop() {
    int packetSize = CAN.parsePacket(); //パケットサイズの確認
    if (packetSize) {                   // CANバスからデータを受信したら
        onReceive(packetSize);          //受信時に呼び出される関数を呼び出す
    }
    Serial.println(data.temp);
}
