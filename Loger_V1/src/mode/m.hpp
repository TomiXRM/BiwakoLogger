#ifndef M_HPP
#define M_HPP

#include <Arduino.h>
#include <./setup/pinDefs.hpp>
#include <./setup/dataDefs.hpp>

sensor1_t temp(10, "temp", "℃");
sensor1_t press(20, "press", "Pa");
sensor3_t acc(30, "acc", "m/s^2");
sensor3_t mag(40, "mag", "uT");
sensor3_t gyro(50, "gyro", "rad/s");
sensor3_t grav(60, "grav", "m/s^2");
sensor3_t euler(70, "euler", "rad");
sensor4_t quat(80, "quat", "q");

static void m_before() {
    Serial.println("m_before");
}

static void m_body() {
    Serial.println("m_body");
    sensors.readIMU(acc, mag, gyro, grav, euler, quat);
    temp.value.f = ___temp;
    press.value.f = ___press;

    Serial.printf("temp: %f, press: %f\n", temp.value.f, press.value.f);
    Serial.printf("acc: %f, %f, %f %s\n", (float)acc.x, (float)acc.y, (float)acc.z, acc.x.unit);
    Serial.printf("mag: %f, %f, %f %s\n", (float)mag.x, (float)mag.y, (float)mag.z, mag.x.unit);
    Serial.printf("gyro: %f, %f, %f %s\n", (float)gyro.x, (float)gyro.y, (float)gyro.z, gyro.x.unit);
    Serial.printf("grav: %f, %f, %f %s\n", (float)grav.x, (float)grav.y, (float)grav.z, grav.x.unit);
    Serial.printf("euler: %f, %f, %f %s\n", (float)euler.x, (float)euler.y, (float)euler.z, euler.x.unit);
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