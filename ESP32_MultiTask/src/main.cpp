// read waterTemperature with multitasking and display it on the screen
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>

#include <BluetoothSerial.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

// pin definitions
#define LED_PIN 2
#define PRESSURE_SENSOR_PIN 32
#define ONE_WIRE_BUS_PIN 33

const char *ssid = "GL-SFT1200-3a3";     //自分のSSIDを設定する
const char *password = "goodlife";       // SSIDのパスワードを設定する
const char *hostname = "BEP_ESP_MOTHER"; //ホスト名を設定する

// create instance
Ticker tick;
OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature waterTemp(&oneWire);
BluetoothSerial SerialBT;
File myfile; // SDカードの状態を格納

// multiTask
TaskHandle_t thp[2];
QueueHandle_t xQueue_1;

// global variables
volatile struct {
    float temp;
    int32_t pressure;
    int32_t _pressure[2];
    uint32_t time;
} data;

// prototypes
void Core0a(void *args);
void Core1a(void *args);

void setup() {
    Serial.begin(115200);
    SerialBT.begin(hostname);
    pinMode(LED_PIN, OUTPUT);
    // alive LED initialization
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    // create tasks
    xQueue_1 = xQueueCreate(10, 16);
    xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 2, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, NULL, 1, &thp[1], 1);
    uint8_t v = 0;
    while (!SD.begin(4) && v < 10) {
        v++;
        delay(1000);
        Serial.println("SD Card Mount Failed");
        //初期化失敗の処理
    }

    Serial.println("Ready");
    SerialBT.println("Ready");
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

void read_pressure() {
    data._pressure[0] = data._pressure[1];
    data._pressure[1] = analogRead(PRESSURE_SENSOR_PIN);
    data._pressure[1] = data._pressure[1] * 0.5 + data._pressure[0] * 0.5;
    data.pressure = data._pressure[1];
    // Serial.printf("pressure:%d\r\n", data.pressure);
    // SerialBT.printf("pressure:%d\r\n", data.pressure);
}

void loop() {
    char text[128];

    read_pressure();

    sprintf(text, "wp,%d,wt,%.2f", data.pressure, data.temp);
    Serial.printf("%s\r\n", text);
    SerialBT.printf("%s\r\n", text);
    writeSD("test", text);
    delay(100);
}
