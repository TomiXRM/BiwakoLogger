#ifndef SETUP_HPP
#define SETUP_HPP

#include <Arduino.h>
#include "dataDefs.hpp"
#include "pinDefs.hpp"
#include <./systems/systemManager.hpp>
#include <./sensorAPI/sensors.hpp>

#include <systems/can/canSender.hpp>
#include <BluetoothSerial.h>

#define HOSTNAME "ESP32-1"
#define MY_ID 0x01

#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>

static BluetoothSerial SerialBT;
static canSender canSender(1000E3, &SerialBT);
static sensors sensors(&SerialBT);
static systemManager systemManager(HOSTNAME, MY_ID, &SerialBT);
#endif