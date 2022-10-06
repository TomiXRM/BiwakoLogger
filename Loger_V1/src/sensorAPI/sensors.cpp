#include "sensors.hpp"

sensors::sensors(BluetoothSerial *_SerialBT) : bno(55, 0x28) {
    SerialBT = _SerialBT;
    xQueue_1 = xQueueCreate(1, sizeof(sensor1_t));
    oneWire = OneWire(ONE_WIRE_BUS);
    waterTemp = DallasTemperature(&oneWire);
    xTaskCreatePinnedToCore(Core0a, "Core0a", 10000, this, 1, &thp[0], 0);
    xTaskCreatePinnedToCore(Core1a, "Core1a", 10000, this, 1, &thp[1], 1);
    uint8_t c = 0;
    while (!bno.begin()) {
        c++;
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        // SerialBT.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        delay(1000);
        if (c > 20) {
            Serial.println("Reboot reason : BNO055 failed");
            // SerialBT.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
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
        // SerialBT.println("\nNo Calibration Data for this sensor exists in EEPROM");
        delay(500);
    } else {
        Serial.println("\nFound Calibration for this sensor in EEPROM.");
        // SerialBT.println("\nFound Calibration for this sensor in EEPROM.");
        eeAddress += sizeof(long);
        EEPROM.get(eeAddress, calibrationData);

        displaySensorOffsets(calibrationData);

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        // SerialBT.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        Serial.println("\n\nCalibration data loaded into BNO055");
        // SerialBT.println("\n\nCalibration data loaded into BNO055");
        //  foundCalib = true;
    }
}

void sensors::displaySensorOffsets(const adafruit_bno055_offsets_t &calibData) {
    Serial.printf("Accelerometer:%d %d %d\n", calibData.accel_offset_x, calibData.accel_offset_y, calibData.accel_offset_z);
    Serial.printf("Gyro:%d %d %d\n", calibData.gyro_offset_x, calibData.gyro_offset_y, calibData.gyro_offset_z);
    Serial.printf("Mag:%d %d %d\n", calibData.mag_offset_x, calibData.mag_offset_y, calibData.mag_offset_z);
    Serial.printf("Accel Radius:%d\n", calibData.accel_radius);
    Serial.printf("Mag Radius:%d\n\n", calibData.mag_radius);
}

void sensors::imuCalib() {
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
        // //mode = 'Z';
    }
}