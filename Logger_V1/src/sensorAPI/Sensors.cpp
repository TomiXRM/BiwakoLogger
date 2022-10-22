#include "Sensors.hpp"

// Sensors::Sensors() : bno(55, 0x28) {
// }

Sensors::Sensors(BluetoothSerial *_SerialBT) : bno(55, 0x28) {
    SerialBT = _SerialBT;
}

void Sensors::begin() {
    xQueue_1 = xQueueCreate(1, sizeof(Sensor1_t));
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
    sensor_t Sensor;
    EEPROM.begin(64);
    EEPROM.get(eeAddress, bnoID);
    bno.getSensor(&Sensor);
    if (bnoID != Sensor.sensor_id) {
        Serial.println("\nNo Calibration Data for this Sensor exists in EEPROM");
        // SerialBT.println("\nNo Calibration Data for this Sensor exists in EEPROM");
        delay(500);
    } else {
        Serial.println("\nFound Calibration for this Sensor in EEPROM.");
        // SerialBT.println("\nFound Calibration for this Sensor in EEPROM.");
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

// void Sensors::setBluetoothSerial(BluetoothSerial *_SerialBT) {
//     SerialBT = _SerialBT;
//     SerialBT->print("Bluetooth Serial is set");
//     Serial.println("Bluetooth Serial is set");
// }

void Sensors::displaySensorOffsets(const adafruit_bno055_offsets_t &calibData) {
    Serial.printf("Accelerometer:%d %d %d\n", calibData.accel_offset_x, calibData.accel_offset_y, calibData.accel_offset_z);
    Serial.printf("Gyro:%d %d %d\n", calibData.gyro_offset_x, calibData.gyro_offset_y, calibData.gyro_offset_z);
    Serial.printf("Mag:%d %d %d\n", calibData.mag_offset_x, calibData.mag_offset_y, calibData.mag_offset_z);
    Serial.printf("Accel Radius:%d\n", calibData.accel_radius);
    Serial.printf("Mag Radius:%d\n\n", calibData.mag_radius);
}

void Sensors::imuCalib() {
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
        sensor_t Sensor;
        bno.getSensor(&Sensor);
        long bnoID = Sensor.sensor_id;
        EEPROM.put(eeAddress, bnoID);
        eeAddress += sizeof(long);
        EEPROM.put(eeAddress, newCalib);
        Serial.println("Data stored to EEPROM.");
        delay(1000);
        // //mode = 'Z';
    }
}

void Sensors::readIMU(Sensor3_t &acc, Sensor3_t &mag, Sensor3_t &gyro, Sensor3_t &grav, Sensor3_t &euler, Sensor4_t &quat) {
    // accels
    imu::Vector<3> _acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    acc.x = (float)_acc.x();
    acc.y = (float)_acc.y();
    acc.z = (float)_acc.z();

    // mag
    imu::Vector<3> _mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    mag.x = (float)_mag.x();
    mag.y = (float)_mag.y();
    mag.z = (float)_mag.z();

    // gyro
    imu::Vector<3> _gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    gyro.x = (float)_gyro.x();
    gyro.y = (float)_gyro.y();
    gyro.z = (float)_gyro.z();

    // gravity
    imu::Vector<3> _grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    grav.x = (float)_grav.x();
    grav.y = (float)_grav.y();
    grav.z = (float)_grav.z();
    // euler
    imu::Vector<3> _euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    euler.x = (float)_euler.x();
    euler.y = (float)_euler.y();
    euler.z = (float)_euler.z();

    imu::Quaternion _quat = bno.getQuat();
    quat.w = (float)_quat.w();
    quat.x = (float)_quat.x();
    quat.y = (float)_quat.y();
    quat.z = (float)_quat.z();
}