// read waterTemperature with multitasking and display it on the screen
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>

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

    // alive LED initialization
    tick.attach_ms(10, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });

    // create tasks
    xQueue_1 = xQueueCreate(10, 16);
    xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 2, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, NULL, 1, &thp[1], 1);
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
    Serial.println(data.temp);
    delay(100);
}
