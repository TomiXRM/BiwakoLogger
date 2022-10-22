#ifndef M_HPP
#define M_HPP

#include <Arduino.h>
#include "./setup/pinDefs.hpp"
#include "./setup/dataDefs.hpp"
#include "./setup/setup.hpp"

Sensor1_t temp(10, "temp", "℃");
Sensor1_t press(20, "press", "Pa");
Sensor3_t acc(30, "acc", "m/s^2");
Sensor3_t mag(40, "mag", "uT");
Sensor3_t gyro(50, "gyro", "rad/s");
Sensor3_t grav(60, "grav", "m/s^2");
Sensor3_t euler(70, "euler", "rad");
Sensor4_t quat(80, "quat", "q");

timer ttt;

void print() {
    temp.print();
    press.print();
    acc.print();
    mag.print();
    gyro.print();
    grav.print();
    euler.print();
    quat.print();

    Serial.print("\033[H");
    Serial.print("\033[2J");
}

void canSend() {
    canSender.sendSensor1(temp);
    canSender.sendSensor1(press);
    canSender.sendSensor3(acc);
    canSender.sendSensor3(mag);
    canSender.sendSensor3(gyro);
    canSender.sendSensor3(grav);
    canSender.sendSensor3(euler);
    canSender.sendSensor4(quat);
}

static void m_before() {
    Serial.println("m_before");
}

static void m_body() {
    // Serial.println("m_body");
    ttt.reset();
    sensors.readIMU(acc, mag, gyro, grav, euler, quat);
    temp.f = ___temp;
    press.f = ___press;

    print();
    canSend();
    Serial.println(ttt.read_ms());
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