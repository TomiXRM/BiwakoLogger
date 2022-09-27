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
#include <BluetoothSerial.h>

// pin definitions
#define LED_PIN 2
#define ONE_WIRE_BUS 33
#define PRESSURE_SENSOR_PIN 32
#define HOSTNAME "ESP32-1"

// create instance
Ticker tick;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28); // BNO055のインスタンス
BluetoothSerial SerialBT;

// multiTask
TaskHandle_t thp[2];
QueueHandle_t xQueue_1;

char mode = 'Z';

// global variables
volatile struct {
    float temp;
    uint32_t pressure;
    struct {
        float x;
        float y;
        float z;
    } accr;
    struct {
        float x;
        float y;
        float z;
    } gyro;
    struct {
        float x;
        float y;
        float z;
    } mag;
    struct {
        float x;
        float y;
        float z;
    } eulr;

    struct {
        float x;
        float y;
        float z;
    } grav;
    struct {
        float w;
        float x;
        float y;
        float z;
    } quat;
} data;

typedef union {
    float f;
    uint8_t b[4];
} u;

imu::Vector<3> accr, mag, gyro, eulr, grav;

// prototypes
void Core0a(void *args);
void Core1a(void *args);

// task1 (Core1) : read temperature
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
        data.pressure = analogRead(PRESSURE_SENSOR_PIN);
    }
}

void getIMU() {
    // accels
    accr = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    data.accr.x = (float)accr.x();
    data.accr.y = (float)accr.y();
    data.accr.z = (float)accr.z();

    imu::Quaternion quat = bno.getQuat();
    data.quat.w = (float)quat.w();
    data.quat.x = (float)quat.x();
    data.quat.y = (float)quat.y();
    data.quat.z = (float)quat.z();

    // gravity
    grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    data.grav.x = (float)grav.x();
    data.grav.y = (float)grav.y();
    data.grav.z = (float)grav.z();
    // eulr
    eulr = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    data.eulr.x = (float)eulr.x();
    data.eulr.y = (float)eulr.y();
    data.eulr.z = (float)eulr.z();
    // mag
    mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    data.mag.x = (float)mag.x();
    data.mag.y = (float)mag.y();
    data.mag.z = (float)mag.z();
    // gyro
    gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    data.gyro.x = (float)gyro.x();
    data.gyro.y = (float)gyro.y();
    data.gyro.z = (float)gyro.z();
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
    SerialBT.begin(HOSTNAME);
    CAN.setPins(25, 26);
    uint8_t c = 0;
    while (!CAN.begin(1000E3)) {
        c++;
        Serial.println("Starting CAN failed!");
        SerialBT.println("Starting CAN failed!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : CAN failed");
            SerialBT.println("Reboot reason : CAN failed");
            ESP.restart();
        }
    }
    c = 0;
    while (!bno.begin()) {
        c++;
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        SerialBT.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : BNO055 failed");
            SerialBT.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
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
        SerialBT.println("\nNo Calibration Data for this sensor exists in EEPROM");
        delay(500);
    } else {
        Serial.println("\nFound Calibration for this sensor in EEPROM.");
        SerialBT.println("\nFound Calibration for this sensor in EEPROM.");
        eeAddress += sizeof(long);
        EEPROM.get(eeAddress, calibrationData);

        displaySensorOffsets(calibrationData);

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        SerialBT.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        Serial.println("\n\nCalibration data loaded into BNO055");
        SerialBT.println("\n\nCalibration data loaded into BNO055");
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
    uint16_t serialAvailable = Serial.available();
    uint16_t serialBTAvailable = SerialBT.available();
    if (serialAvailable > 0 || serialBTAvailable > 0) {
        char buf[256]{NULL};
        if (serialAvailable > 0) {
            size_t bufSize = serialAvailable;
            Serial.readBytesUntil('\n', buf, bufSize);
            Serial.printf(" --read:%s,", buf);
            mode = buf[0];
        } else if (serialBTAvailable > 0) {
            size_t bufSize = serialBTAvailable;
            SerialBT.readBytesUntil('\n', buf, bufSize);
            SerialBT.printf(" --read:%s,", buf);
            mode = buf[0];
        }
        delay(500);
    } else {
        getIMU();
        switch (mode) {
        case 'C':
            imuCalib();
            break;
        case 'A':
            Serial.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", data.accr.x, data.accr.y, data.accr.z);
            SerialBT.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", data.accr.x, data.accr.y, data.accr.z);
            break;
        case 'V':
            Serial.printf("GRV:%.4f\t,%.4f\t,%.4f\r\n", data.grav.x, data.grav.y, data.grav.z);
            SerialBT.printf("GRV:%.4f\t,%.4f\t,%.4f\r\n", data.grav.x, data.grav.y, data.grav.z);
            break;
        case 'G':
            Serial.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", data.gyro.x, data.gyro.y, data.gyro.z);
            SerialBT.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", data.gyro.x, data.gyro.y, data.gyro.z);
            break;
        case 'M':
            Serial.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", data.mag.x, data.mag.y, data.mag.z);
            SerialBT.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", data.mag.x, data.mag.y, data.mag.z);
            break;
        case 'E':
            Serial.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", data.eulr.x, data.eulr.y, data.eulr.z);
            SerialBT.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", data.eulr.x, data.eulr.y, data.eulr.z);
            break;
        case 'Q':
            Serial.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", data.quat.w, data.quat.x, data.quat.y, data.quat.z);
            SerialBT.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", data.quat.w, data.quat.x, data.quat.y, data.quat.z);
            break;
        case 'W':
            Serial.printf("WP,%d,WT,%.2f\r\n", data.pressure, data.temp);
            SerialBT.printf("WP,%d,WT,%.2f\r\n", data.pressure, data.temp);
            break;
        case 'Z':
        default:
            Serial.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", data.accr.x, data.accr.y, data.accr.z);
            Serial.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", data.eulr.x, data.eulr.y, data.eulr.z);
            Serial.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", data.gyro.x, data.gyro.y, data.gyro.z);
            Serial.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", data.mag.x, data.mag.y, data.mag.z);
            Serial.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", data.eulr.x, data.eulr.y, data.eulr.z);
            Serial.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", data.quat.w, data.quat.x, data.quat.y, data.quat.z);
            Serial.printf("WP,%d,WT,%.2f\r\n", data.pressure, data.temp);

            SerialBT.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", data.accr.x, data.accr.y, data.accr.z);
            SerialBT.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", data.eulr.x, data.eulr.y, data.eulr.z);
            SerialBT.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", data.gyro.x, data.gyro.y, data.gyro.z);
            SerialBT.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", data.mag.x, data.mag.y, data.mag.z);
            SerialBT.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", data.eulr.x, data.eulr.y, data.eulr.z);
            SerialBT.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", data.quat.w, data.quat.x, data.quat.y, data.quat.z);
            SerialBT.printf("WP,%d,WT,%.2f\r\n", data.pressure, data.temp);
            break;
        }
        Serial.print("\033[2J");
        Serial.print("\033[H");
        delay(10);
    }
}
