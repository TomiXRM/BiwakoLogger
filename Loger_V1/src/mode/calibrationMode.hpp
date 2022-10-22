#ifndef CALIBRATIONMODE_HPP
#define CALIBRATIONMODE_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>

static void c_before() {
    Serial.println("Starting Calibration Mode");
}

static void c_body() {
    Serial.println("Calibration Mode");
}

static void c_after() {
    Serial.println("Exit Calibration Mode");
}

static Mode_t mode_c = {
    "CalibrationMode",
    'C',
    c_before,
    c_body,
    c_after,
};

#endif