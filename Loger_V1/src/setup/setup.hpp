#ifndef SETUP_HPP
#define SETUP_HPP

#include <Arduino.h>
#include "dataDefs.hpp"
#include "pinDefs.hpp"
#include "./systems/System.hpp"
#include "./sensorAPI/Sensors.hpp"
#include "systems/can/CanSender.hpp"
#include "time.h"

#define HOSTNAME "ESP32-1"
#define MY_ID 0x01

#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <BluetoothSerial.h>

static BluetoothSerial SerialBT;
static Sensors sensors(&SerialBT);
static System sysManager(HOSTNAME, MY_ID, &SerialBT);
static CanSender canSender(&SerialBT);
#endif