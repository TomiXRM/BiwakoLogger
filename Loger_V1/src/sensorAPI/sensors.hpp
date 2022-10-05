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
class sensors {
  public:
    sensors(BluetoothSerial *_SerialBT);
    void read(uint8_t sensor);

    void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData);
    void imuCalib();

    static float temp;
    static float press;

  private:
    static void Core0a(void *args);
    static void Core1a(void *args);
    static TaskHandle_t thp[2];
    static QueueHandle_t xQueue_1;
    static OneWire oneWire;
    static DallasTemperature waterTemp;

    BluetoothSerial *SerialBT;

    Adafruit_BNO055 bno;
    imu::Vector<3> accel, mag, gyro, euler, grav;
};

#endif