#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h>

/*
GPIO pinmap:
D2  - GPS 1PPS
D4  - SD_CS
D5  - as GPS receive
D6  - as GPS transmit
D11 - MOSI
D12 - as MISO
D13 - as SCK
*/

#define interruptPin 2
#define chipSelect 4

SoftwareSerial gps(5, 6); // RX, TX
TinyGPSPlus tinyGPS;

typedef struct {
    double latitude;
    double longitude;
} gpsData;

gpsData GPSData;

/* ----- prototype functions ----- */
void saveGPSData(gpsData &data);
void triger();
void initSD();

void saveGPSData(gpsData &data) {
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
        String dataString = String(data.latitude) + ',' + String(data.longitude) + '\n';
        dataFile.println(dataString);
    } else {
        Serial.println(F("error opening datalog.txt"));
    }
    dataFile.close();
}

void triger() {
    GPSData = {tinyGPS.location.lat(), tinyGPS.location.lng()};
    saveGPSData(GPSData);

    Serial.print(GPSData.latitude, 6);
    Serial.print(", ");
    Serial.println(GPSData.latitude, 6);
}

void initSD() {
    Serial.print(F("Initializing SD card..."));
    if (!SD.begin(chipSelect)) {
        while (1) {
            delay(1000);
            Serial.println(F("Card failed, or not present"));
        }
    }
    Serial.println("SD init done");
}

void setup() {
    pinMode(interruptPin, INPUT_PULLUP);
    Serial.begin(115200);
    gps.begin(9600);
    initSD();
    attachInterrupt(digitalPinToInterrupt(interruptPin), triger, FALLING);
}

void loop() {
    if (gps.available())
        tinyGPS.encode(gps.read());
}