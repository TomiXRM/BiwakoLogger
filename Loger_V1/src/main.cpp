// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const int LED_PIN = 2;
const int ONE_WIRE_BUS = 33;

Ticker tick;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSens(&oneWire);

void setup() {
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(1000, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    Serial.begin(2000000);
    temperatureSens.begin();
    Serial.println("CAN Loger");

    // start the CAN bus at 500 kbps
    CAN.setPins(26, 25);
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }
}

void loop() {
    sensors.requestTemperatures();
}
