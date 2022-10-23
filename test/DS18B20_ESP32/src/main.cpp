
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>
const int LED_PIN = 2;

Ticker tick;
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 33

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void) {
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(1000, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    // start serial port
    Serial.begin(1000000);
    pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
    Serial.println("Dallas Temperature IC Control Library Demo");

    // Start up the library
    sensors.begin();
}

void loop(void) {
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");

    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(sensors.getTempCByIndex(0));

    delay(500);
}
