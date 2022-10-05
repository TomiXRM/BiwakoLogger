#ifndef CANSENDER_HPP
#define CANSENDER_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>
#include <BluetoothSerial.h>
#include <CAN.h>

class canSender {
  public:
    canSender(long canBaud, BluetoothSerial *_SerialBT);
    void baud(long canBaud);
    void sendSensor1(sensor1_t &sensor);
    void sendSensor3(sensors3_t &sensor);
    void sendSensor4(sensors4_t &sensor);

  private:
    BluetoothSerial *SerialBT;
};
#endif