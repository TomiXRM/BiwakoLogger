#ifndef CANSENDER_HPP
#define CANSENDER_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>
#include <BluetoothSerial.h>
#include <CAN.h>

class CanSender {
  public:
    CanSender(BluetoothSerial *_SerialBT);
    void begin(long canBaud);
    void baud(long canBaud);
    void sendSensor1(Sensor1_t &sensor);
    void sendSensor3(Sensor3_t &sensor);
    void sendSensor4(Sensor4_t &sensor);

  private:
    BluetoothSerial *SerialBT;
};
#endif