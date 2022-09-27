// read waterTemperature with multitasking and display it on the screen
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>
#include <CAN.h>

#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <timer.h>
#include <EEPROM.h>

// pin definitions
#define LED_PIN 2
#define ONE_WIRE_BUS 33

// create instance
Ticker tick;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28); // BNO055のインスタンス

// multiTask
TaskHandle_t thp[2];
QueueHandle_t xQueue_1;

char mode = 'Z';

// global variables
volatile struct {
    float temp;
    float pressure;
    struct {
        double x;
        double y;
        double z;
    } accr;
    struct {
        double x;
        double y;
        double z;
    } gyro;
    struct {
        double x;
        double y;
        double z;
    } mag;
    struct {
        double x;
        double y;
        double z;
    } eulr;
} data;

typedef union {
    double d;
    float f;
    uint8_t b[4];
} u;

imu::Vector<3> accr, mag, gyro, eulr;

// prototypes
void Core0a(void *args);
void Core1a(void *args);

// task1 (Core1) : read temperature and pressure
void Core1a(void *args) {
    float tmp;
    while (1) {
        waterTemp.requestTemperatures();
        tmp = waterTemp.getTempCByIndex(0);
        xQueueSend(xQueue_1, &tmp, 0);
    }
}

// task2 (Core0) : put water temperature to global variable
void Core0a(void *args) {
    float tmp = 0;
    while (1) {
        // wait for queue to be filled
        xQueueReceive(xQueue_1, &tmp, portMAX_DELAY);
        data.temp = tmp; // put to global variable
    }
}

void getIMU() {
    mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    accr = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    eulr = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    // data.accr = {accr.x(), accr.y(), accr.z()};
    // data.accr = {accr.x(), accr.y(), accr.z()};
}
void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData) {
    Serial.printf("Accelerometer:%d %d %d\n", calibData.accel_offset_x, calibData.accel_offset_y, calibData.accel_offset_z);
    Serial.printf("Gyro:%d %d %d\n", calibData.gyro_offset_x, calibData.gyro_offset_y, calibData.gyro_offset_z);
    Serial.printf("Mag:%d %d %d\n", calibData.mag_offset_x, calibData.mag_offset_y, calibData.mag_offset_z);
    Serial.printf("Accel Radius:%d\n", calibData.accel_radius);
    Serial.printf("Mag Radius:%d\n\n", calibData.mag_radius);
}

void imuCalib() {
    if (!bno.isFullyCalibrated()) {
        uint8_t s, g, a, m = 0;
        bno.getCalibration(&s, &g, &a, &m);
        Serial.print("\t");
        if (!s) {
            Serial.print("! ");
        }
        Serial.print("CALIBRATION: Sys=");
        Serial.print(s, DEC);
        Serial.print(" Gyro=");
        Serial.print(g, DEC);
        Serial.print(" Accel=");
        Serial.print(a, DEC);
        Serial.print(" Mag=");
        Serial.println(m, DEC);
    } else {
        Serial.println("\nFully calibrated!");
        adafruit_bno055_offsets_t newCalib;
        bno.getSensorOffsets(newCalib);
        displaySensorOffsets(newCalib);
        int eeAddress = 0;
        sensor_t sensor;
        bno.getSensor(&sensor);
        long bnoID = sensor.sensor_id;
        EEPROM.put(eeAddress, bnoID);
        eeAddress += sizeof(long);
        EEPROM.put(eeAddress, newCalib);
        Serial.println("Data stored to EEPROM.");
        delay(1000);
        mode = 'Z';
    }
}

void setup() {
    Serial.begin(2000000);
    CAN.setPins(25, 26);
    uint8_t c = 0;
    while (!CAN.begin(500E3)) {
        c++;
        Serial.println("Starting CAN failed!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : CAN failed");
            ESP.restart();
        }
    }
    c = 0;
    while (!bno.begin()) {
        c++;
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : BNO055 failed");
            ESP.restart();
        }
    }

    int eeAddress = 0;
    long bnoID;
    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;
    EEPROM.begin(64);
    EEPROM.get(eeAddress, bnoID);

    bno.getSensor(&sensor);
    if (bnoID != sensor.sensor_id) {
        Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
        delay(500);
    } else {
        Serial.println("\nFound Calibration for this sensor in EEPROM.");
        eeAddress += sizeof(long);
        EEPROM.get(eeAddress, calibrationData);

        displaySensorOffsets(calibrationData);

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        Serial.println("\n\nCalibration data loaded into BNO055");
        // foundCalib = true;
    }

    // alive LED initialization
    tick.attach_ms(10, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    // create tasks
    xQueue_1 = xQueueCreate(10, 16);
    xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 2, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, NULL, 1, &thp[1], 1);
}

void sendFloat(float data) {
    u data_;
    data_.f = data;
    CAN.write(data_.b, 4);
}

void sendData() {
    CAN.beginPacket(0x100);
    sendFloat(data.temp);
    sendFloat(data.pressure);
    sendFloat(data.accr.x);
    sendFloat(data.accr.y);
    sendFloat(data.accr.z);
    sendFloat(data.mag.x);
    sendFloat(data.mag.y);
    sendFloat(data.mag.z);
    CAN.endPacket();
}

void loop() {
    if (Serial.available() > 0) {
        char buf[256]{NULL};
        size_t bufSize = Serial.available();
        Serial.readBytesUntil('\n', buf, bufSize);
        Serial.printf(" --read:%s,", buf);
        mode = buf[0];
        delay(500);
    } else {
        getIMU();
        switch (mode) {
        case 'A':
            Serial.printf("ACC:%f,%f%,%f\r\n", accr.x(), accr.y(), accr.z());
            break;
        case 'G':
            Serial.printf("GYR:%f,%f%,%f\r\n", gyro.x(), gyro.y(), gyro.z());
            break;
        case 'E':
            Serial.printf("EUR:%f,%f%,%f\r\n", eulr.x(), eulr.y(), eulr.z());
            break;
        case 'M':
            Serial.printf("MAG:%f,%f%,%f\r\n", mag.x(), mag.y(), mag.z());
            break;
        case 'C':
            imuCalib();
            break;
        case 'Z':
        default:
            Serial.printf("EUR:%f,%f%,%f\r\n", eulr.x(), eulr.y(), eulr.z());
            Serial.printf("MAG:%f,%f%,%f\r\n", mag.x(), mag.y(), mag.z());
            Serial.printf("ACC:%f,%f%,%f\r\n", accr.x(), accr.y(), accr.z());
            Serial.printf("GYR:%f,%f%,%f\r\n", gyro.x(), gyro.y(), gyro.z());
            break;
        }
        Serial.print("\033[2J");
        delay(10);
    }
    // Serial.println(data.temp);
    // Serial.printf("wp,%d,wt,%.2f", data.pressure, data.temp);
}
