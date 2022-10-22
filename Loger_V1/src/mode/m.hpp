#ifndef M_HPP
#define M_HPP

#include <Arduino.h>
#include "./setup/pinDefs.hpp"
#include "./setup/dataDefs.hpp"
#include "./setup/setup.hpp"

sensor1_t temp(10, "temp", "℃");
sensor1_t press(20, "press", "Pa");
sensor3_t acc(30, "acc", "m/s^2");
sensor3_t mag(40, "mag", "uT");
sensor3_t gyro(50, "gyro", "rad/s");
sensor3_t grav(60, "grav", "m/s^2");
sensor3_t euler(70, "euler", "rad");
sensor4_t quat(80, "quat", "q");

timer ttt;

void print() {
    acc.print();
    mag.print();
    gyro.print();
    grav.print();
    euler.print();
    quat.print();
    Serial.println(ttt.read_ms());
    Serial.print("\033[H");
    Serial.print("\033[2J");
    ttt.reset();
}

static void m_before() {
    Serial.println("m_before");
}

static void m_body() {
    // Serial.println("m_body");
    sensors.readIMU(acc, mag, gyro, grav, euler, quat);
    temp.f = ___temp;
    press.f = ___press;

    print();
}

static void m_after() {
    Serial.println("m_after");
}

static Mode_t mode_m = {
    "MainMode",
    'M',
    m_before,
    m_body,
    m_after,
};

#endif