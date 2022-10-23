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