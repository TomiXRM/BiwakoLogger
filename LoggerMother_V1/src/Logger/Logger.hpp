#ifndef LOGEGR_V1_HPP
#define LOGEGR_V1_HPP
#include <Arduino.h>
#include <ArduinoLog.h>
#include "dataDefs.hpp"

class Logger_V1 {
    Logger_V1(int id) {
    }

    void appendSensor(int id, const char *name, const char *unit, int type) {
        switch (type) {
        case 1:
            Sensor1_t s1(id, name, unit);
            break;
        case 3:
            Sensor3_t s3(id, name, unit);
            break;
        case 4:
            Sensor4_t s4(id, name, unit);
            break;
        default:
            Log.errorln("Invalid sensor type");
            break;
        }
        
    }
};
#endif