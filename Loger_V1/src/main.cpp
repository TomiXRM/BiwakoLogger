// read waterTemperature with multitasking and display it on the screen
#include "./setup/setup.hpp"

struct {
    sensor1_t temp{100, "Temp", "°C"};
    sensor1_t press{101, "press", ""};
    sensors3_t accel{102, "accel", "m/s²"};
    sensors3_t gyro{103, "gyro", "°/s"};
    sensors3_t mag{104, "mag", "uT"};
    sensors3_t grav{105, "grav", "m/s²"};
    sensors3_t euler{106, "euler", "°"};
    sensors4_t quat{107, "quat", ""};
} data;
// create instance
Ticker tick;
BluetoothSerial SerialBT;
char mode = 'Z';
// multiTask
TaskHandle_t thp[2];
QueueHandle_t xQueue_1;

// sensors
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28); // BNO055のインスタンス

typedef union {
    float f;
    uint32_t i;
    uint8_t b[4];
} uBytes;

imu::Vector<3> accel, mag, gyro, euler, grav;

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
        data.press = analogRead(PRESSURE_SENSOR_PIN);
    }
}

void getIMU() {
    // accels
    accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    data.accel.x = (float)accel.x();
    data.accel.y = (float)accel.y();
    data.accel.z = (float)accel.z();

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
    // euler
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    data.euler.x = (float)euler.x();
    data.euler.y = (float)euler.y();
    data.euler.z = (float)euler.z();
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

// void sendFloat(int id, float data) {
//     uBytes data_;
//     data_.f = data;
//     if (id == MY_ID) {
//         CAN.beginPacket(MY_ID);
//     } else {
//         CAN.beginExtendedPacket(id);
//     }
//     CAN.write(data_.b, 4);
//     CAN.endPacket();
// }

// void sendUint32(int id, uint32_t data) {
//     uBytes data_;
//     data_.i = data;
//     if (id == MY_ID) {
//         CAN.beginPacket(MY_ID);
//     } else {
//         CAN.beginExtendedPacket(id);
//     }
//     CAN.write(data_.b, 4);
//     CAN.endPacket();
// }

void sendData() {
    // sendFloat(MY_ID,(float)data.temp);
    // sendUint32(MY_ID + 1,(float)data.pressプレス);
    // sendFloat(MY_ID + 2,(float)data.grav.x);
    // sendFloat(MY_ID + 3,(float)data.grav.y);
    // sendFloat(MY_ID + 4,(float)data.grav.z);
    // sendFloat(MY_ID + 5,(float)data.accel.x);
    // sendFloat(MY_ID + 6,(float)data.accel.y);
    // sendFloat(MY_ID + 7,(float)data.accel.z);
    // sendFloat(MY_ID + 8,(float)data.mag.x);
    // sendFloat(MY_ID + 9,(float)data.mag.y);
    // sendFloat(MY_ID + 10,(float)data.mag.z);
    // sendFloat(MY_ID + 11,(float)data.euler.x);
    // sendFloat(MY_ID + 12,(float)data.euler.y);
    // sendFloat(MY_ID + 13,(float)data.euler.z);
    // sendFloat(MY_ID + 14,(float)data.quat.w);
    // sendFloat(MY_ID + 15,(float)data.quat.x);
    // sendFloat(MY_ID + 16,(float)data.quat.y);
    // sendFloat(MY_ID + 17,(float)data.quat.z);
    // sendFloat(MY_ID + 18,(float)data.gyro.x);
    // sendFloat(MY_ID + 19,(float)data.gyro.y);
    // sendFloat(MY_ID + 20,(float)data.gyro.z);
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
            Serial.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", (float)data.accel.x, (float)data.accel.y, (float)data.accel.z);
            SerialBT.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", (float)data.accel.x, (float)data.accel.y, (float)data.accel.z);
            break;
        case 'V':
            Serial.printf("GRV:%.4f\t,%.4f\t,%.4f\r\n", (float)data.grav.x, (float)data.grav.y, (float)data.grav.z);
            SerialBT.printf("GRV:%.4f\t,%.4f\t,%.4f\r\n", (float)data.grav.x, (float)data.grav.y, (float)data.grav.z);
            break;
        case 'G':
            Serial.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.gyro.x, (float)data.gyro.y, (float)data.gyro.z);
            SerialBT.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.gyro.x, (float)data.gyro.y, (float)data.gyro.z);
            break;
        case 'M':
            Serial.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", (float)data.mag.x, (float)data.mag.y, (float)data.mag.z);
            SerialBT.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", (float)data.mag.x, (float)data.mag.y, (float)data.mag.z);
            break;
        case 'E':
            Serial.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.euler.x, (float)data.euler.y, (float)data.euler.z);
            SerialBT.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.euler.x, (float)data.euler.y, (float)data.euler.z);
            break;
        case 'Q':
            Serial.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", (float)data.quat.w, (float)data.quat.x, (float)data.quat.y, (float)data.quat.z);
            SerialBT.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", (float)data.quat.w, (float)data.quat.x, (float)data.quat.y, (float)data.quat.z);
            break;
        case 'W':
            Serial.printf("WP,%d,WT,%.2f\r\n", (float)data.press, (float)data.temp);
            SerialBT.printf("WP,%d,WT,%.2f\r\n", (float)data.press, (float)data.temp);
            break;
        case 'Z':
        default:
            Serial.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", (float)data.accel.x, (float)data.accel.y, (float)data.accel.z);
            Serial.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.euler.x, (float)data.euler.y, (float)data.euler.z);
            Serial.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.gyro.x, (float)data.gyro.y, (float)data.gyro.z);
            Serial.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", (float)data.mag.x, (float)data.mag.y, (float)data.mag.z);
            Serial.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.euler.x, (float)data.euler.y, (float)data.euler.z);
            Serial.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", (float)data.quat.w, (float)data.quat.x, (float)data.quat.y, (float)data.quat.z);
            Serial.printf("WP,%d,WT,%.2f\r\n", (float)data.press, (float)data.temp);

            SerialBT.printf("ACC:%.4f\t,%.4f\t,%.4f\r\n", (float)data.accel.x, (float)data.accel.y, (float)data.accel.z);
            SerialBT.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.euler.x, (float)data.euler.y, (float)data.euler.z);
            SerialBT.printf("GYR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.gyro.x, (float)data.gyro.y, (float)data.gyro.z);
            SerialBT.printf("MAG:%.4f\t,%.4f\t,%.4f\r\n", (float)data.mag.x, (float)data.mag.y, (float)data.mag.z);
            SerialBT.printf("EUR:%.4f\t,%.4f\t,%.4f\r\n", (float)data.euler.x, (float)data.euler.y, (float)data.euler.z);
            SerialBT.printf("QUAT:%.4f\t,%.4f\t,%.4f\t,%.4f\r\n", (float)data.quat.w, (float)data.quat.x, (float)data.quat.y, (float)data.quat.z);
            SerialBT.printf("WP,%d,WT,%.2f\r\n", (float)data.press, (float)data.temp);
            break;
        }
        Serial.print("\033[2J");
        Serial.print("\033[H");

        delay(10);
    }
}
