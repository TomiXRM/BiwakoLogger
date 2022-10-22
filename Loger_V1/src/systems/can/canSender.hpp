#ifndef CANSENDER_HPP
#define CANSENDER_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>
#include <BluetoothSerial.h>
#include <CAN.h>

class canSender {
  public:
    canSender(BluetoothSerial *_SerialBT);
    void begin(long canBaud);
    void baud(long canBaud);
    void sendSensor1(sensor1_t &sensor);
    void sendSensor3(sensor3_t &sensor);
    void sendSensor4(sensor4_t &sensor);

  private:
    BluetoothSerial *SerialBT;
};
#endif