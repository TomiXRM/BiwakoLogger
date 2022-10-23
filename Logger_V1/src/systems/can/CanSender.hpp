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
    void send(Sensor1_t &sensor);
    void send(Sensor3_t &sensor);
    void send(Sensor4_t &sensor);
    void onReceive(int packetSize);

    long getReceivedCanId();
    bool getIsExtended();
    bool getIsRtr();
    int getRequestBytes();

    long chechMatch(long *canId, int canIdQty);

  private:
    BluetoothSerial *SerialBT;
    long receivedCanId;
    bool isRtr;
    bool isExtended;
    int requestBytes;
};
#endif