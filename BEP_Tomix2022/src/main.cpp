#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h>

#define interruptPin 2

SoftwareSerial gps(10, 11); // RX, TX
TinyGPSPlus tinyGPS;

void triger() {
    Serial.print(tinyGPS.location.lat(), 6);
    Serial.print(", ");
    Serial.println(tinyGPS.location.lng(), 6);
}

void setup() {
    pinMode(interruptPin, INPUT_PULLUP);
    Serial.begin(115200);
    gps.begin(9600);
    attachInterrupt(digitalPinToInterrupt(interruptPin), triger, FALLING);
}

void loop() {
    while (gps.available())
        tinyGPS.encode(gps.read());
}