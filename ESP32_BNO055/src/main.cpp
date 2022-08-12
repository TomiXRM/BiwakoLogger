#include "Arduino.h"
#include <Ticker.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <timer.h>

#define LED_PIN 2
#define FILTER_VALUE 0.8

Ticker tick;
Ticker calVelocityTick;

timer deltaT;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
typedef struct {
    float x;
    float y;
    float z;
} XYZ;

float velocity_whole = 0;

XYZ accelerBias;
XYZ accelerBiasThreshold;
XYZ accelerBiasCancel[2];
XYZ accelerFiltered[2];

float threshold;

XYZ velocity[2];
XYZ velocity_filterd;

void calVelocity();

void setup(void) {
    pinMode(LED_PIN, OUTPUT);
    tick.attach_ms(650, []() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    });
    // pinMode(21, INPUT_PULLUP); // SDA 21番ピンのプルアップ(念のため)
    // pinMode(22, INPUT_PULLUP); // SDA 22番ピンのプルアップ(念のため)

    Serial.begin(2000000);
    Serial.println("");

    if (!bno.begin()) // センサの初期化
    {
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1)
            ;
    }
    deltaT.reset();
    delay(1000);

    /* Display the current temperature */
    int8_t temp = bno.getTemp();
    Serial.print("Current Temperature: ");
    Serial.print(temp);
    Serial.println(" C");
    Serial.println("");

    bno.setExtCrystalUse(false);

    Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
}

void calVelocity() {
    imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    accelerBiasCancel[0] = accelerBiasCancel[1]; //前の値を移動させる

    // 0が古い方 1が新しい方
    if (abs(accelermetor.x()) > accelerBiasThreshold.x) {
        accelerBiasCancel[1].x = accelermetor.x() - accelerBias.x;
    } else {
        accelerBiasCancel[1].x = 0;
    }
    if (abs(accelermetor.y()) > accelerBiasThreshold.y) {
        accelerBiasCancel[1].y = accelermetor.y() - accelerBias.y;
    } else {
        accelerBiasCancel[1].y = 0;
    }
    if (abs(accelermetor.z()) > accelerBiasThreshold.z) {
        accelerBiasCancel[1].z = accelermetor.z() - accelerBias.z;
    } else {
        accelerBiasCancel[1].z = 0;
    }

    accelerFiltered[1].x = FILTER_VALUE * accelerFiltered[0].x + (1.0 - FILTER_VALUE) * accelerBiasCancel[1].x;
    accelerFiltered[1].y = FILTER_VALUE * accelerFiltered[0].y + (1.0 - FILTER_VALUE) * accelerBiasCancel[1].y;
    accelerFiltered[1].z = FILTER_VALUE * accelerFiltered[0].z + (1.0 - FILTER_VALUE) * accelerBiasCancel[1].z;

    float time = (float)deltaT.read_ms() * 0.001;

    velocity[1].x = (accelerFiltered[1].x + accelerFiltered[0].x) / 2 * time;
    velocity[1].y = (accelerFiltered[1].y + accelerFiltered[0].y) / 2 * time;
    velocity[1].z = (accelerFiltered[1].z + accelerFiltered[0].z) / 2 * time;
    velocity_whole = sqrt(velocity[1].x * velocity[1].x + velocity[1].y * velocity[1].y + velocity[1].z * velocity[1].z);
    velocity[0] = velocity[1];
    deltaT.reset();
}

void get_biase_acceler(void) {
    // 定常誤差を計算する
    for (size_t i = 0; i < 50; i++) {
        imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    }
    for (size_t i = 0; i < 50; i++) {
        imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        accelerBias.x += accelermetor.x();
        accelerBias.y += accelermetor.y();
        accelerBias.z += accelermetor.z();
    }

    accelerBias.x /= 50;
    accelerBias.y /= 50;
    accelerBias.z /= 50;
    accelerBiasThreshold.x = abs(accelerBias.x * 1.2);
    accelerBiasThreshold.y = abs(accelerBias.y * 1.2);
    accelerBiasThreshold.z = abs(accelerBias.z * 1.2);
    // threshold = sqrt(accelerBiasThreshold.x * accelerBiasThreshold.x + accelerBiasThreshold.y * accelerBiasThreshold.y + accelerBiasThreshold.z * accelerBiasThreshold.z);
    Serial.printf("　Bias_xyz:%f,%f,%f,%f\r\n", accelerBias.x, accelerBias.y, accelerBias.z, threshold);
    delay(2000);
}

void get_bno055_data(void) {
    // Possible vector values can be:
    // - VECTOR_ACCELEROMETER - m/s^2
    // - VECTOR_MAGNETOMETER  - uT
    // - VECTOR_GYROSCOPE     - rad/s
    // - VECTOR_EULER         - degrees
    // - VECTOR_LINEARACCEL   - m/s^2
    // - VECTOR_GRAVITY       - m/s^2

    // キャリブレーションのステータスの取得と表示
    // uint8_t system, gyro, accel, mag = 0;
    // bno.getCalibration(&system, &gyro, &accel, &mag);
    // Serial.print("CALIB Sys:");
    // Serial.print(system, DEC);
    // Serial.print(", Gy");
    // Serial.print(gyro, DEC);
    // Serial.print(", Ac");
    // Serial.print(accel, DEC);
    // Serial.print(", Mg");
    // Serial.print(mag, DEC);

    /*
    // ジャイロセンサ値の取得と表示
    imu::Vector<3> gyroscope = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    Serial.print(" 　Gy_xyz:");
    Serial.print(gyroscope.x());
    Serial.print(", ");
    Serial.print(gyroscope.y());
    Serial.print(", ");
    Serial.print(gyroscope.z());
    */

    // 加速度センサ値の取得と表示

    /*
    // 磁力センサ値の取得と表示
    imu::Vector<3> magnetmetor = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    Serial.print(" 　Mg_xyz:");
    Serial.print(magnetmetor .x());
    Serial.print(", ");
    Serial.print(magnetmetor .y());
    Serial.print(", ");
    Serial.print(magnetmetor .z());
    */

    // センサフュージョンによる方向推定値の取得と表示
    // imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    // Serial.print(" 　DIR_xyz:");
    // Serial.print(euler.x());
    // Serial.print(", ");
    // Serial.print(euler.y());
    // Serial.print(", ");
    // Serial.print(euler.z());

    /*
      // センサフュージョンの方向推定値のクオータニオン
      imu::Quaternion quat = bno.getQuat();
      Serial.print("qW: ");
      Serial.print(quat.w(), 4);
      Serial.print(" qX: ");
      Serial.print(quat.x(), 4);
      Serial.print(" qY: ");
      Serial.print(quat.y(), 4);
      Serial.print(" qZ: ");
      Serial.print(quat.z(), 4);
      Serial.print("\t\t");
    */
    // imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    // accelerBiasCancel.x = accelermetor.x() - accelerBias.x;
    // accelerBiasCancel.y = accelermetor.y() - accelerBias.y;
    // accelerBiasCancel.z = accelermetor.z() - accelerBias.z;

    // Serial.printf("　Acceler_xyz:%f,%f,%f\r\n", accelerBiasCancel.x, accelerBiasCancel.y, accelerBiasCancel.z);
}

void loop(void) {
    get_biase_acceler();
    calVelocityTick.attach_ms(10, calVelocity);
    while (1) {
        Serial.printf("velocity:%f,%f,%f \t accelerRC:%f,%f,%f \t VelWhole:%f\r\n", velocity[1].x, velocity[1].y, velocity[1].z, accelerFiltered[1].x, accelerFiltered[1].y, accelerFiltered[1].z, velocity_whole);
        delay(10);
    }
}