#ifndef SYSTEM_HPP
#define SYSTEM_HPP

// #include "../setup/setup.hpp"
#include <Arduino.h>
#include <Ticker.h>
#include <timer.h>
#include <BluetoothSerial.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>
#define MODE_UNMATCH -1
// using funcionPointer = void (*)();
class systemManager {
  public:
    systemManager(const char *name, int id, BluetoothSerial *_SerialBT);
    void begin();
    void setMode(char mode);
    void checkSerial();
    void run();
    void addMode(Mode_t mode);
    uint8_t checkModeMatch(char &m);

  private:
    const char *name;
    int id;
    char mode;
    Ticker tick;
    uint8_t modeQty;
    char modePrev;
    Mode_t modes[32];
    Mode_t modeRunning;
    Mode_t modePrevRunning;
    uint8_t runningModeIndex;
    uint8_t runningModeIndexPrev;
    BluetoothSerial *SerialBT;
};
#endif