#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Returns the IMU data as both a euler angles and quaternions as the WebSerial
   3D Model viewer at https://adafruit-3dmodel-viewer.glitch.me/ expects.

   This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.

   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground

   History
   =======
   2020/JUN/01  - First release (Melissa LeBlanc-Williams)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void) {
    sensor_t sensor;
    bno.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print("Sensor:       ");
    Serial.println(sensor.name);
    Serial.print("Driver Ver:   ");
    Serial.println(sensor.version);
    Serial.print("Unique ID:    ");
    Serial.println(sensor.sensor_id);
    Serial.print("Max Value:    ");
    Serial.print(sensor.max_value);
    Serial.println(" xxx");
    Serial.print("Min Value:    ");
    Serial.print(sensor.min_value);
    Serial.println(" xxx");
    Serial.print("Resolution:   ");
    Serial.print(sensor.resolution);
    Serial.println(" xxx");
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) {
    Serial.begin(9600);
    Serial.println("WebSerial 3D Firmware");
    Serial.println("");

    /* Initialise the sensor */
    if (!bno.begin()) {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1)
            ;
    }

    delay(1000);

    /* Use external crystal for better accuracy */
    bno.setExtCrystalUse(true);

    /* Display some basic information on this sensor */
    displaySensorDetails();
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) {
    /* Get a new sensor event */
    sensors_event_t event;
    bno.getEvent(&event);

    /* Board layout:
           +----------+
           |         *| RST   PITCH  ROLL  HEADING
       ADR |*        *| SCL
       INT |*        *| SDA     ^            /->
       PS1 |*        *| GND     |            |
       PS0 |*        *| 3VO     Y    Z-->    \-X
           |         *| VIN
           +----------+
    */

    /* The WebSerial 3D Model Viewer expects data as heading, pitch, roll */
    Serial.print(F("Orientation: "));
    Serial.print(360 - (float)event.orientation.x);
    Serial.print(F(", "));
    Serial.print((float)event.orientation.y);
    Serial.print(F(", "));
    Serial.print((float)event.orientation.z);
    Serial.println(F(""));

    /* The WebSerial 3D Model Viewer also expects data as roll, pitch, heading */
    imu::Quaternion quat = bno.getQuat();

    Serial.print(F("Quaternion: "));
    Serial.print((float)quat.w(), 4);
    Serial.print(F(", "));
    Serial.print((float)quat.x(), 4);
    Serial.print(F(", "));
    Serial.print((float)quat.y(), 4);
    Serial.print(F(", "));
    Serial.print((float)quat.z(), 4);
    Serial.println(F(""));

    /* Also send calibration data for each sensor. */
    uint8_t sys, gyro, accel, mag = 0;
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    Serial.print(F("Calibration: "));
    Serial.print(sys, DEC);
    Serial.print(F(", "));
    Serial.print(gyro, DEC);
    Serial.print(F(", "));
    Serial.print(accel, DEC);
    Serial.print(F(", "));
    Serial.print(mag, DEC);
    Serial.println(F(""));

    delay(BNO055_SAMPLERATE_DELAY_MS);
}
// #include <Ticker.h>
// #include <Wire.h>
// #include <Adafruit_BNO055.h>
// #include <timer.h>

// #define LED_PIN 2        //生きてるかわからる要にLEDを光らせる
// #define FILTER_VALUE 0.8 // RCフィルタの定数

// Ticker tick;            // 生きているかわかる様にLEDを光らせるタイマー割り込み
// Ticker calVelocityTick; // 10msに一回加速度を速度に変換するタイマー割り込み

// timer deltaT;                                    // 加速度積分様のタイマー
// Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28); // BNO055のインスタンス
// typedef struct {
//     float x; // x軸のデータ
//     float y; // x軸のデータ
//     float z; // x軸のデータ
// } XYZ;       // XYZの型定義(構造体)

// float velocity_whole = 0; // xyzの速度を三平方したやつ

// XYZ accelerBias;          //加速度バイアス(定常誤差の記録用)
// XYZ accelerBiasThreshold; // 加速度バイアスのしきい値(しきい値以下のデータがあれば加速度を代入しない)
// XYZ accelerBiasCancel[2]; //加速度バイアスを引いた加速度データ
// XYZ accelerFiltered[2];   //加速度をRCフィルタにかけたデータ

// XYZ velocity[2]; //速度変数(0が前のデータ、1が現在のデータ)

// void calVelocity();

// void setup(void) {
//     // LEDを光らせるタイマーを設定
//     Serial.begin(115200);
//     Serial.println("setup");
//     pinMode(LED_PIN, OUTPUT);
//     tick.attach_ms(650, []() {
//         digitalWrite(LED_PIN, !digitalRead(LED_PIN));
//     });

//     // センサの初期化
//     if (!bno.begin()) {
//         while (1) {
//             Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
//             delay(1000);
//         }
//     }
//     deltaT.reset();
//     delay(1000);

//     /* Display the current temperature */
//     int8_t temp = bno.getTemp();
//     Serial.print("Current Temperature: ");
//     Serial.print(temp);
//     Serial.println(" C");
//     Serial.println("");

//     bno.setExtCrystalUse(false);

//     Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
// }

// void calVelocity() {
//     imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER); //加速度センサーの値を取得
//     // 加速度のフィルタリング 1
//     if (abs(accelermetor.x()) > accelerBiasThreshold.x) {          // x軸の加速度がしきい値以上なら
//         accelerBiasCancel[1].x = accelermetor.x() - accelerBias.x; // x軸の加速度 - センサーの加速度バイアス
//     } else {
//         accelerBiasCancel[1].x = 0; // 閾値以下は0にする
//     }
//     if (abs(accelermetor.y()) > accelerBiasThreshold.y) {
//         accelerBiasCancel[1].y = accelermetor.y() - accelerBias.y;
//     } else {
//         accelerBiasCancel[1].y = 0;
//     }
//     if (abs(accelermetor.z()) > accelerBiasThreshold.z) {
//         accelerBiasCancel[1].z = accelermetor.z() - accelerBias.z;
//     } else {
//         accelerBiasCancel[1].z = 0;
//     }

//     // 加速度センサーRCフィルタ
//     accelerFiltered[1].x = FILTER_VALUE * accelerFiltered[0].x + (1.0 - FILTER_VALUE) * accelerBiasCancel[1].x;
//     accelerFiltered[1].y = FILTER_VALUE * accelerFiltered[0].y + (1.0 - FILTER_VALUE) * accelerBiasCancel[1].y;
//     accelerFiltered[1].z = FILTER_VALUE * accelerFiltered[0].z + (1.0 - FILTER_VALUE) * accelerBiasCancel[1].z;

//     // 時間を取得してmsから　ｓに変換する
//     float time = (float)deltaT.read_ms() * 0.001;

//     // 積分 (台形の面積を求める)
//     velocity[1].x = (accelerFiltered[1].x + accelerFiltered[0].x) / 2 * time;
//     velocity[1].y = (accelerFiltered[1].y + accelerFiltered[0].y) / 2 * time;
//     velocity[1].z = (accelerFiltered[1].z + accelerFiltered[0].z) / 2 * time;

//     // 三平方の定理で全ての速度ベクトルを合わせる
//     velocity_whole = sqrt(velocity[1].x * velocity[1].x + velocity[1].y * velocity[1].y + velocity[1].z * velocity[1].z);

//     //前の値を保持させる (0が古い方 1が新しい方)
//     velocity[0] = velocity[1];                   //速度
//     accelerBiasCancel[0] = accelerBiasCancel[1]; //加速度
//     deltaT.reset();                              //時間をリセット
// }

// void get_biase_acceler(void) {
//     // 定常誤差を計算する
//     for (size_t i = 0; i < 50; i++) { // 50回とりあえずデータ取ってみて様子見る
//         imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
//     }
//     // --------------------------------------------------------------------------------------
//     // 50回値を取って平均を取る
//     for (size_t i = 0; i < 50; i++) {
//         imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
//         accelerBias.x += accelermetor.x();
//         accelerBias.y += accelermetor.y();
//         accelerBias.z += accelermetor.z();
//     }
//     accelerBias.x /= 50;
//     accelerBias.y /= 50;
//     accelerBias.z /= 50;
//     // --------------------------------------------------------------------------------------

//     // 加速度センサーの閾値をバイアスから計算する
//     accelerBiasThreshold.x = abs(accelerBias.x * 1.2);
//     accelerBiasThreshold.y = abs(accelerBias.y * 1.2);
//     accelerBiasThreshold.z = abs(accelerBias.z * 1.2);
//     //バイアスの表示
//     Serial.printf("　Bias_xyz:%f,%f,%f,%f\r\n", accelerBias.x, accelerBias.y, accelerBias.z);
//     delay(2000);
// }

// void loop(void) {
//     get_biase_acceler();                        // バイアスを取得する
//     calVelocityTick.attach_ms(10, calVelocity); // 速度計算関数をタイマー割り込みで10msに設定
//     while (1) {
//         Serial.printf("velocity:%f,%f,%f \t accelerRC:%f,%f,%f \t VelWhole:%f\r\n", velocity[1].x, velocity[1].y, velocity[1].z, accelerFiltered[1].x, accelerFiltered[1].y, accelerFiltered[1].z, velocity_whole);
//         delay(10);
//     }
// }