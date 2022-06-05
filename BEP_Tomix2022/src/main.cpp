#include <Arduino.h>
#include <SoftwareSerial.h>

#define interruptPin 2
SoftwareSerial mySerial(10, 11); // RX, TX

void triger() {
    Serial.print("triger\n");
}

void setup() {
    pinMode(interruptPin, INPUT_PULLUP);
    Serial.begin(115200);
    mySerial.begin(9600);
    attachInterrupt(digitalPinToInterrupt(interruptPin), triger, FALLING);
}

void loop() {
    if (mySerial.available()) Serial.write(mySerial.read());
}