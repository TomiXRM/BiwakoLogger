#ifndef LOGEGR_V1_HPP
#define LOGEGR_V1_HPP
#include <Arduino.h>
#include <ArduinoLog.h>
#include "./dataDefs.hpp"
#include <CAN.h>

class Logger_V1 {
  public:
    Logger_V1(int id, int logLevel, bool debug, ESP32SJA1000Class *can);

    void init();
    void appendSensor(Sensor1_t *s1);
    void appendSensor(Sensor3_t *s3);
    void appendSensor(Sensor4_t *s4);

    void makeCanIdList();

    void sendRequest(long id, int interval);

    void read(uint8_t packetSize, Sensor1_t &s1);

    void onReceive(int packetSize, long receivedCanId);

    int getId() { return id; }
    Sensor1_t temp;
    Sensor1_t press;
    Sensor3_t acc;
    Sensor3_t mag;
    Sensor3_t gyro;
    Sensor3_t grav;
    Sensor3_t euler;
    Sensor4_t quat;

    Sensor1_t *sensors1[20]; // change manually
    Sensor3_t *sensors3[20]; // change manually
    Sensor4_t *sensors4[20]; // change manually
  private:
    int s1Qty = 0;
    int s3Qty = 0;
    int s4Qty = 0;

    bool isExtended;
    bool isRemote;
    bool isRtr;
    int requestBytes;
    long receivedCanId;
    long requestedCanId;

    long canIdList[32] = {0};
    int canIdQty = 0;

    long id;
    ESP32SJA1000Class *can;
};
#endif