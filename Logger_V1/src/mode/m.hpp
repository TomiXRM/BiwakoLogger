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
Sensor3_t euler(70, "euler", "deg");
Sensor4_t quat(80, "quat", "");

timer ttt;

Sensor1_t *sensors1[] = {&temp, &press};
Sensor3_t *sensors3[] = {&acc, &mag, &gyro, &grav, &euler};
Sensor4_t *sensors4[] = {&quat};

// make canId list from sensors list

long canIdList[20] = {0};
int canIdQty = 0;

void makeCanIdList(long *canIdList, int *canIdQty) {
    int i = 0;
    for (size_t j = 0; j < sizeof(sensors1) / sizeof(sensors1[0]); j++) {
        canIdList[i] = sensors1[j]->id;
        i++;
    }
    for (size_t j = 0; j < sizeof(sensors3) / sizeof(sensors3[0]); j++) {
        canIdList[i] = sensors3[j]->id;
        i++;
    }
    for (size_t j = 0; j < sizeof(sensors4) / sizeof(sensors4[0]); j++) {
        canIdList[i] = sensors4[j]->id;
        i++;
    }
    *canIdQty = i;
}

void print() {
    temp.print();
    press.print();
    acc.print();
    mag.print();
    gyro.print();
    grav.print();
    euler.print();
    quat.print();

    // Serial.print("\033[H");
    // Serial.print("\033[2J");
}

void canSend() {
    canSender.send(temp);
    canSender.send(press);
    canSender.send(acc);
    canSender.send(mag);
    canSender.send(gyro);
    canSender.send(grav);
    canSender.send(euler);
    canSender.send(quat);
}

static void m_before() {
    Serial.println("m_before");
    makeCanIdList(canIdList, &canIdQty);
    // print canIdList
    Serial.println("--canIdList--");
    for (size_t i = 0; i < canIdQty; i++) {
        Serial.printf("%d ", canIdList[i]);
    }
    Serial.println();
}

static void m_body() {
    ttt.reset();
    sensors.readIMU(acc, mag, gyro, grav, euler, quat);
    temp.f = ___temp;
    press.f = ___press;

    // print();
    // checkCan();
    int packetSize = CAN.parsePacket();  //パケットサイズの確認
    if (packetSize) {                    // CANバスからデータを受信したら
        canSender.onReceive(packetSize); //受信時に呼び出される関数を呼び出す
        long matchId = canSender.chechMatch((long *)canIdList, canIdQty);
        Serial.printf("matchId is %d ", matchId);
        // send data
        for (size_t i = 0; i < sizeof(sensors1) / sizeof(sensors1[0]); i++) {
            // Serial.printf("%d ==%d ? \r\n", matchId, sensors1[i]->id);
            if (sensors1[i]->id == matchId) {
                Serial.printf("%s\r\n", sensors1[i]->name);
                canSender.send(*sensors1[i]);
                return;
            }
        }
        for (size_t i = 0; i < sizeof(sensors3) / sizeof(sensors3[0]); i++) {
            // Serial.printf("%d ==%d ? \r\n", matchId, sensors3[i]->id);
            if (sensors3[i]->id == matchId) {
                Serial.printf("%s\r\n", sensors3[i]->name);
                canSender.send(*sensors3[i]);
                return;
            }
        }
        for (size_t i = 0; i < sizeof(sensors4) / sizeof(sensors4[0]); i++) {
            // Serial.printf("%d ==%d ? \r\n", matchId, sensors4[i]->id);
            if (sensors4[i]->id == matchId) {
                Serial.printf("%s\r\n", sensors4[i]->name);
                canSender.send(*sensors4[i]);
                return;
            }
        }
    }
    // Serial.println(ttt.read_ms());
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