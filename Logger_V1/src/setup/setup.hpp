#ifndef SETUP_HPP
#define SETUP_HPP

#include <Arduino.h>
#include "dataDefs.hpp"
#include "pinDefs.hpp"
#include "./systems/System.hpp"
#include "./sensorAPI/Sensors.hpp"
#include "systems/can/CanSender.hpp"
#include "time.h"

#define HOSTNAME "ESP32-1"
#define MY_ID 0x01

#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <BluetoothSerial.h>

static BluetoothSerial SerialBT;
static Sensors sensors(&SerialBT);
static System sysManager(HOSTNAME, MY_ID, &SerialBT);
static CanSender canSender(&SerialBT);

static TaskHandle_t thp[2];
static QueueHandle_t xQueue_1 = xQueueCreate(1, sizeof(Sensor1_t));
static OneWire oneWire = OneWire(ONE_WIRE_BUS);
static DallasTemperature waterTemp = DallasTemperature(&oneWire);
static float ___temp;
static float ___press;

void Core1a(void *args) {
    float tmp;
    while (1) {
        waterTemp.requestTemperatures();
        tmp = waterTemp.getTempCByIndex(0);
        xQueueSend(xQueue_1, &tmp, 0);
    }
}

// task2 (Core0) : put water___temperature to global variable
void Core0a(void *args) {
    float tmp = 0;
    while (1) {
        // wait for queue to be filled
        xQueueReceive(xQueue_1, &tmp, portMAX_DELAY);
        ___temp = tmp; // put to global variable
        ___press = (float)analogRead(PRESSURE_SENSOR_PIN);
    }
}
#endif