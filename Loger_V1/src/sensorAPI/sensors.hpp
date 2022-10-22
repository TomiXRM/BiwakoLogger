#ifndef SENSORS_HPP
#define SENSORS_HPP
#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <BluetoothSerial.h>
#include <./setup/dataDefs.hpp>
#include <./setup/pinDefs.hpp>

static void Core0a(void *args);
static void Core1a(void *args);
static TaskHandle_t thp[2];
static QueueHandle_t xQueue_1;
static OneWire oneWire;
static DallasTemperature waterTemp;
static float ___temp;
static float ___press;

extern void Core1a(void *args) {
    float tmp;
    while (1) {
        waterTemp.requestTemperatures();
        tmp = waterTemp.getTempCByIndex(0);
        xQueueSend(xQueue_1, &tmp, 0);
    }
}

// task2 (Core0) : put water___temperature to global variable
extern void Core0a(void *args) {
    float tmp = 0;
    while (1) {
        // wait for queue to be filled
        xQueueReceive(xQueue_1, &tmp, portMAX_DELAY);
        ___temp = tmp; // put to global variable
        ___press = analogRead(PRESSURE_SENSOR_PIN);
    }
}

class Sensors {
  public:
    // Sensors();
    Sensors(BluetoothSerial *_SerialBT);
    void begin();
    // void setBluetoothSerial(BluetoothSerial *_SerialBT);
    void printTest() {
        SerialBT->println("Sensors class is working");
    }
    void readIMU(Sensor3_t &acc, Sensor3_t &mag, Sensor3_t &gyro, Sensor3_t &grav, Sensor3_t &euler, Sensor4_t &quat);

    void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData);
    void imuCalib();

  private:
    BluetoothSerial *SerialBT;

    Adafruit_BNO055 bno;
};

#endif