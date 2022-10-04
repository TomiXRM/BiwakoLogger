#ifndef SENSORS_HPP
#define SENSORS_HPP
#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <./setup/dataDefs.hpp>
#include <./setup/pinDefs.hpp>
class Sensors {
  public:
    Sensors();
    virtual ~Sensors();
    void read(uint8_t sensor);

    void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData);
    void imuCalib();

    float temp;
    float press;

  private:
    void Core0a(void *args);
    void Core1a(void *args);
    TaskHandle_t thp[2];
    QueueHandle_t xQueue_1;
    OneWire oneWire;
    DallasTemperature waterTemp;

    Adafruit_BNO055 bno;
    imu::Vector<3> accel, mag, gyro, euler, grav;
};

#endif